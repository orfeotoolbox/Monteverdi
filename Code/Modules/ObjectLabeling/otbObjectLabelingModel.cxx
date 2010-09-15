/*=========================================================================

Program:   ORFEO Toolbox
Language:  C++
Date:      $Date$
Version:   $Revision$


Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbObjectLabelingModel_cxx
#define __otbObjectLabelingModel_cxx

#include "otbObjectLabelingModel.h"
#include "otbImageFileWriter.h"
#include "base/ossimFilename.h"
#include "otbFltkFilterWatcher.h"
#include "otbImageMetadataInterfaceBase.h"
#include "otbImageMetadataInterfaceFactory.h"

#include <cstdlib>
namespace otb
{
  ObjectLabelingModel::ObjectLabelingModel() :
      m_SelectedLabel(itk::NumericTraits<LabelType>::max()),
      m_SelectedClass(0),
      m_HasSelectedClass(false)
  {
    m_VisualizationModel    = VisualizationModelType::New();
    m_PixelDescriptionModel = PixelDescriptionModelType::New();

    // Build the selected polygon vector data
    m_SelectedPolygon = VectorDataType::New();
    DataNodeType::Pointer document = DataNodeType::New();
    DataNodeType::Pointer folder = DataNodeType::New();
    m_SelectedPolygonNode = DataNodeType::New();
    m_SelectedPolygonNode->SetPolygonExteriorRing(PolygonType::New());

    document->SetNodeType(otb::DOCUMENT);
    folder->SetNodeType(otb::FOLDER);

    // Build the selected polygon vector data
    DataNodeType::Pointer root = m_SelectedPolygon->GetDataTree()->GetRoot()->Get();

    m_SelectedPolygon->GetDataTree()->Add(document,root);
    m_SelectedPolygon->GetDataTree()->Add(folder,document);
    m_SelectedPolygon->GetDataTree()->Add(m_SelectedPolygonNode,folder);

    // Building the margin sampling vector data
    m_MarginSampledPolygon = VectorDataType::New();
    document = DataNodeType::New();
    m_MarginSampledFolder= DataNodeType::New();

    document->SetNodeType(otb::DOCUMENT);
    m_MarginSampledFolder->SetNodeType(otb::FOLDER);

    root = m_MarginSampledPolygon->GetDataTree()->GetRoot()->Get();

    m_MarginSampledPolygon->GetDataTree()->Add(document,root);
    m_MarginSampledPolygon->GetDataTree()->Add(m_MarginSampledFolder,document);

    // Build the SVM objects
    m_SVMEstimator = SVMEstimatorType::New();
    m_ListSample = ListSampleType::New();
    m_TrainingListSample = ListSampleType::New();
    m_LabelsListSample = TrainingListSampleType::New();
    m_MarginSampler = MarginSamplerType::New();

    m_Accuracy = 0.;

    // Classification
    m_ClassLabelFilter = ClassLabelFilterType::New();
    m_ColorMapper = ChangeLabelFilterType::New();
  }

  ObjectLabelingModel::~ObjectLabelingModel()
  {}

  std::vector<unsigned int>
  ObjectLabelingModel::GetChannels()
  {
    return m_ImageLayerRenderingFunction->GetChannelList();
  }

  /** Open an image with its associated label map */
  void ObjectLabelingModel::OpenImage(VectorImageType* vimage, ImageType* limage)
  {
    // Size checking
    if(limage->GetLargestPossibleRegion() != vimage->GetLargestPossibleRegion())
      {
      itkExceptionMacro(<<"Image and label map size are different, can not load data into the application.");
      }

    m_LabeledImage = limage;
    m_VectorImage  = vimage;

    // Update origin and spacing
    m_Origin[0] = m_VectorImage->GetOrigin()[0];
    m_Origin[1] = m_VectorImage->GetOrigin()[1];
    m_Spacing[0] =m_VectorImage->GetSpacing()[0];
    m_Spacing[1] =m_VectorImage->GetSpacing()[1];


    // Generate the layer
    m_ImageGenerator = LayerGeneratorType::New();
    m_ImageGenerator->SetImage(m_VectorImage);
    m_ImageGenerator->GenerateQuicklookOff();
    m_ImageGenerator->SetQuicklook(m_VectorImage);
    m_ImageGenerator->SetSubsamplingRate(1);
    FltkFilterWatcher qlwatcher(m_ImageGenerator->GetResampler(), 0, 0, 200, 20, otbGetTextMacro("Generating QuickLook ..."));
    m_ImageGenerator->GenerateLayer();

    m_ImageLayerRenderingFunction = m_ImageGenerator->GetLayer()->GetRenderingFunction();
    m_ImageGenerator->GetLayer()->SetName("Image");

    this->Link();
  }

  bool ObjectLabelingModel::CheckLabelImage(ImageType* limage)
  {
    // Convert to label map
    LabelMapFilterType::Pointer lfilter = LabelMapFilterType::New();
    lfilter->SetBackgroundValue(itk::NumericTraits<LabelType>::max());
    lfilter->SetInput(limage);

    lfilter->Update();
    unsigned int nbPix = limage->GetLargestPossibleRegion().GetSize()[0]*limage->GetLargestPossibleRegion().GetSize()[1];
    bool res = true;
    otbMsgDevMacro(<<"map label size: "<<lfilter->GetOutput()->GetLabelObjectContainer().size());
    if(lfilter->GetOutput()->GetLabelObjectContainer().size() > nbPix*0.1)
      {
      res = false;
      }

    return res;
  }

  void ObjectLabelingModel::Link()
  {
    // Clear previous layers
    m_VisualizationModel->ClearLayers();
    m_PixelDescriptionModel->ClearLayers();

    // Add the layer to the models
    m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());
    m_PixelDescriptionModel->AddLayer(m_ImageGenerator->GetLayer());

    m_VisualizationModel->Update();

    // Convert to label map
    LabelMapFilterType::Pointer lfilter = LabelMapFilterType::New();
    lfilter->SetBackgroundValue(itk::NumericTraits<LabelType>::max());
    lfilter->SetInput(m_LabeledImage);


    // Compute shape attributes
    ShapeLabelMapFilterType::Pointer shapeLabelMapFilter = ShapeLabelMapFilterType::New();
    shapeLabelMapFilter->SetInput(lfilter->GetOutput());

    // Compute radiometric attributes
    RadiometricLabelMapFilterType::Pointer radiometricLabelMapFilter = RadiometricLabelMapFilterType::New();
    radiometricLabelMapFilter->SetInput1(shapeLabelMapFilter->GetOutput());
    radiometricLabelMapFilter->SetInput2(m_VectorImage);
    radiometricLabelMapFilter->Update();

    // Get the label map
    m_LabelMap = radiometricLabelMapFilter->GetOutput();
    m_LabelMap->SetAdjacencyMap(lfilter->GetOutput()->GetAdjacencyMap());

    // Populates the features list
    m_AvailableFeatures.clear();

    std::vector<std::string> features = m_LabelMap->GetNthLabelObject(0)->GetAvailableAttributes();

    for(std::vector<std::string>::const_iterator fit = features.begin();fit!=features.end();++fit)
      {
      m_AvailableFeatures[*fit]=true;
      }

    // Computes the features statistics
    this->ComputeFeaturesStatistics();

    m_SelectedPolygon->SetProjectionRef(m_VectorImage->GetProjectionRef());
    m_MarginSampledPolygon->SetProjectionRef(m_VectorImage->GetProjectionRef());

    // Export to class label image
    otbMsgDevMacro(<<"Exporting to class label image ...");
    m_ClassLabelFilter = ClassLabelFilterType::New();
    m_ClassLabelFilter->SetInput(m_LabelMap);
    m_ClassLabelFilter->Update();
    otbMsgDevMacro(<<"Done.");

    // Coloring classes
    m_ColorMapper = ChangeLabelFilterType::New();
    m_ColorMapper->SetInput(m_ClassLabelFilter->GetOutput());
    m_ColorMapper->SetNumberOfComponentsPerPixel(3);
    m_ColorMapper->ClearChangeMap();

    // For each classes
    for(ObjectClassVectorType::const_iterator oit = m_Classes.begin(); oit != m_Classes.end();++oit)
      {
      VectorPixelType newValue(3);
      newValue[0]=oit->m_Color[0];
      newValue[1]=oit->m_Color[1];
      newValue[2]=oit->m_Color[2];
      otbMsgDevMacro(<<"New value: "<<newValue);
      m_ColorMapper->SetChange(oit->m_Label,newValue);
      }


    // Generate the layer
    m_LabeledImageGenerator = LayerGeneratorType::New();
    m_LabeledImageGenerator->SetImage(m_ColorMapper->GetOutput());
    m_LabeledImageGenerator->GenerateQuicklookOff();
    m_LabeledImageGenerator->SetQuicklook(m_ColorMapper->GetOutput());
    m_LabeledImageGenerator->SetSubsamplingRate(1);
    m_LabeledImageGenerator->GenerateLayer();
    m_LabeledImageGenerator->GetLayer()->GetRenderingFunction()->SetAutoMinMax(false);

    LayerGeneratorType::ImageLayerType::RenderingFunctionType::ParametersType parameters(6);
    parameters.Fill(0);
    parameters[1] = 255;
    parameters[3] = 255;
    parameters[5] = 255;

    m_LabeledImageGenerator->GetLayer()->GetRenderingFunction()->SetParameters(parameters);

    BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
    blender->SetAlpha(0);

    m_LabeledImageGenerator->GetLayer()->SetBlendingFunction(blender);
    m_LabeledImageGenerator->GetLayer()->SetName("Classification");
    m_LabeledImageGenerator->GetLayer()->SetVisible(false);

    // Add the layer to the models
    m_VisualizationModel->AddLayer(m_LabeledImageGenerator->GetLayer());
    m_PixelDescriptionModel->AddLayer(m_LabeledImageGenerator->GetLayer());

    m_VisualizationModel->Update();
  }

  /** Open an image with its associated label map */
  void ObjectLabelingModel::Init(VectorImageType* vimage, ImageType* limage)
  {
    // Update origin and spacing
    m_Origin[0] = vimage->GetOrigin()[0];
    m_Origin[1] = vimage->GetOrigin()[1];
    m_Spacing[0] =vimage->GetSpacing()[0];
    m_Spacing[1] =vimage->GetSpacing()[1];

    // Convert to label map
    LabelMapFilterType::Pointer lfilter = LabelMapFilterType::New();
    lfilter->SetBackgroundValue(itk::NumericTraits<LabelType>::max());
    lfilter->SetInput(m_LabeledImage);

    // Compute shape attributes
    ShapeLabelMapFilterType::Pointer shapeLabelMapFilter = ShapeLabelMapFilterType::New();
    shapeLabelMapFilter->SetInput(lfilter->GetOutput());

    // Compute radiometric attributes
    RadiometricLabelMapFilterType::Pointer radiometricLabelMapFilter = RadiometricLabelMapFilterType::New();
    radiometricLabelMapFilter->SetInput1(shapeLabelMapFilter->GetOutput());
    radiometricLabelMapFilter->SetInput2(vimage);
    radiometricLabelMapFilter->Update();

    // Get the label map
    m_LabelMap = radiometricLabelMapFilter->GetOutput();
    m_LabelMap->SetAdjacencyMap(lfilter->GetOutput()->GetAdjacencyMap());

    // Populates the features list
    m_AvailableFeatures.clear();

    std::vector<std::string> features = m_LabelMap->GetNthLabelObject(0)->GetAvailableAttributes();

    for(std::vector<std::string>::const_iterator fit = features.begin();fit!=features.end();++fit)
      {
      m_AvailableFeatures[*fit]=true;
      }

    // Computes the features statistics
    this->ComputeFeaturesStatistics();

    m_SelectedPolygon->SetProjectionRef(vimage->GetProjectionRef());
    m_MarginSampledPolygon->SetProjectionRef(vimage->GetProjectionRef());

    // Export to class label image
    otbMsgDevMacro(<<"Exporting to class label image ...");
    m_ClassLabelFilter->SetInput(m_LabelMap);
    m_ClassLabelFilter->Update();
    otbMsgDevMacro(<<"Done.");

    // Coloring classes */
    m_ColorMapper->SetInput(m_ClassLabelFilter->GetOutput());
    m_ColorMapper->SetNumberOfComponentsPerPixel(3);
    m_ColorMapper->ClearChangeMap();

    // For each classes
    for(ObjectClassVectorType::const_iterator oit = m_Classes.begin(); oit != m_Classes.end();++oit)
      {
      VectorPixelType newValue(3);
      newValue[0]=oit->m_Color[0];
      newValue[1]=oit->m_Color[1];
      newValue[2]=oit->m_Color[2];
      otbMsgDevMacro(<<"New value: "<<newValue);
      m_ColorMapper->SetChange(oit->m_Label,newValue);
      }
  }

  unsigned int ObjectLabelingModel::GetNumberOfClasses()
  {
    return m_Classes.size();
  }

  /** Add sample */
  void ObjectLabelingModel::AddSampleToClass(const IndexType & sampleIndex, unsigned int classIndex)
  {
    // Check for a valid label image
    if(!m_LabeledImage)
      {
      return;
      }

    LabelType label = m_LabeledImage->GetPixel(sampleIndex);

    this->AddSampleToClass(label,classIndex);
    this->NotifyAll("Update");
  }


  void ObjectLabelingModel::AddSampleToClass(const LabelType & label, unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      // Add The sample to the class
      m_Classes[classIndex].m_Samples.push_back(label);
      // Add the polygon to the VectorData
      SimplifyPolygonFunctorType sfunctor;
      DataNodeType::Pointer polygonNode = DataNodeType::New();
      PolygonType::Pointer polygon = /**sfunctor(*/m_LabelMap->GetLabelObject(label)->GetPolygon()/**)*/;
      polygonNode->SetPolygonExteriorRing(polygon);
      m_Classes[classIndex].m_VectorData->GetDataTree()->Add(polygonNode,m_Classes[classIndex].m_Folder);
      }
  }

  /** Remove label */
  void ObjectLabelingModel::RemoveSampleFromClass(const LabelType & label, unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      ObjectClassType::LabelVectorType::iterator lit = find(m_Classes[classIndex].m_Samples.begin(),m_Classes[classIndex].m_Samples.end(),label);

      if(lit!=m_Classes[classIndex].m_Samples.end())
        {
        // Erase the polygon in the VectorData
        TreeNodeType * node = const_cast<TreeNodeType *> (m_Classes[classIndex].m_VectorData->GetDataTree()->GetNode(m_Classes[classIndex].m_Folder));
        unsigned int childrenIndex = std::distance(m_Classes[classIndex].m_Samples.begin(),lit);
        node->Remove(node->GetChild(childrenIndex));
        // Erase the sample from the list
        m_Classes[classIndex].m_Samples.erase(lit);
        }

      this->NotifyAll("Update");
      }
  }

  /** Clear samples from class */
  void ObjectLabelingModel::ClearSamplesFromClass(unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      // Erase the polygons in the VectorData
      TreeNodeType * node = const_cast<TreeNodeType *> (m_Classes[classIndex].m_VectorData->GetDataTree()->GetNode(m_Classes[classIndex].m_Folder));
      unsigned int nbChildren = node->CountChildren();
      for(unsigned int i = 0; i<nbChildren;++i)
        {
        node->Remove(node->GetChild(nbChildren-i-1));
        }
      m_Classes[classIndex].m_Samples.clear();
      this->NotifyAll("Update");
      }
  }

  /** Focus on sample */
  void ObjectLabelingModel::FocusOnSample(const LabelType & label)
  {
    // Convert sample to polygon
    SimplifyPolygonFunctorType sfunctor;
    PolygonType::Pointer polygon = /**sfunctor(*/m_LabelMap->GetLabelObject(label)->GetPolygon()/**)*/;
    PolygonType::RegionType rsRegion = polygon->GetBoundingRegion();
    PolygonType::PointType center;

    for(unsigned int dim = 0; dim<VectorImageType::ImageDimension;++dim)
      {
      center[dim]= polygon->GetBoundingRegion().GetOrigin()[dim]
                                                            +polygon->GetBoundingRegion().GetSize()[dim]/2;
      }

    VectorImageType::IndexType pindex;

    m_VectorImage->TransformPhysicalPointToIndex(center,pindex);
    m_VisualizationModel->SetExtractRegionCenter(pindex);
    m_VisualizationModel->SetScaledExtractRegionCenter(pindex);

    m_VisualizationModel->Update();
  }

  /** Get a class by its index */
  const ObjectLabelingModel::ObjectClassType & ObjectLabelingModel::GetClass(unsigned int classIndex) const
  {
    if(classIndex < m_Classes.size())
      {
      return m_Classes[classIndex];
      }
    else
      {
      itkExceptionMacro(<<"Class index out of bounds");
      }
  }

  /** Get a class by its label */
  unsigned int ObjectLabelingModel::GetClassIndex(LabelType label) const
  {
    ObjectClassVectorType::const_iterator beg = m_Classes.begin();
    ObjectClassVectorType::const_iterator it  = beg;
    ObjectClassVectorType::const_iterator end = m_Classes.end();

    while(it != end && it->m_Label != label)
      {
      ++it;
      }

    if (it == end)
      {
      itkExceptionMacro(<<"Class label does not exist");
      }

    return (it - beg);
  }

  /** Add a new class (label, color, and name auto-generated)*/
  void ObjectLabelingModel::AddClass()
  {
    LabelType label = GetNextAvailableClassLabel();

    // TODO: Automatic color generation here
    ColorType color;

    // srand(newLabel+123456);
    color[0]= rand()/(RAND_MAX+1.0);
    color[1]= rand()/(RAND_MAX+1.0);
    color[2]= rand()/(RAND_MAX+1.0);
    color[3]=1.0;

    itk::OStringStream oss;
    oss << "New Class " << label;
    std::string name = oss.str().c_str();

    AddClass( label, name, color );

    this->NotifyAll("Update");
  }

  /** Add a new class (label, color, and name already known)*/
  void ObjectLabelingModel::AddClass(const LabelType& label, const std::string& name, const ColorType& color)
  {
    ObjectClassType newClass;
    newClass.m_Label = label;
    newClass.m_Name = name;
    newClass.m_Color = color;

    // Building the vector data
    newClass.m_VectorData = VectorDataType::New();
    DataNodeType::Pointer document = DataNodeType::New();
    DataNodeType::Pointer folder1 = DataNodeType::New();

    newClass.m_Folder = folder1;

    document->SetNodeType(otb::DOCUMENT);
    folder1->SetNodeType(otb::FOLDER);

    DataNodeType::Pointer root = newClass.m_VectorData->GetDataTree()->GetRoot()->Get();

    if(!m_VectorImage)
      {
      itkExceptionMacro(<<"No image loaded, please load an image first.");
      }

    newClass.m_VectorData->GetDataTree()->Add(document,root);
    newClass.m_VectorData->GetDataTree()->Add(folder1,document);
    newClass.m_VectorData->SetProjectionRef(m_VectorImage->GetProjectionRef());

    // Push back the new class
    m_Classes.push_back(newClass);
  }


  ObjectLabelingModel::LabelType ObjectLabelingModel::GetNextAvailableClassLabel()
  {
    // Check for an available label
    ObjectClassVectorType::const_iterator it = m_Classes.begin();
    LabelType newLabel = 0;
    bool      available = false;

    while(!available && newLabel != itk::NumericTraits<LabelType>::max())
      {
      ObjectClassVectorType::const_iterator it = m_Classes.begin();
      bool found = false;

      while(it!=m_Classes.end() && !found)
        {
        if(it->m_Label == newLabel)
          {
          found = true;
          }
        ++it;
        }
      available = !found;

      if(!available)
        {
        ++newLabel;
        }
      }

    if(newLabel ==  itk::NumericTraits<LabelType>::max())
      {
      itkExceptionMacro(<<"No more label available for new class.");
      }

    return newLabel;
  }

  /** Remove the given class */
  void ObjectLabelingModel::RemoveClass(unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      ObjectClassVectorType::iterator it = m_Classes.begin() + classIndex;
      m_Classes.erase(it);
      if(m_SelectedClass>=m_Classes.size())
        {
        this->ClearSelectedClass();
        }
      this->NotifyAll("Update");
      }

  }

  /** Clear all classes */
  void ObjectLabelingModel::ClearClasses()
  {
    m_Classes.clear();
    this->ClearSelectedClass();
    this->NotifyAll("Update");
  }

  /** Get a const reference on the classes vector */
  const ObjectLabelingModel::ObjectClassVectorType & ObjectLabelingModel::GetClasses() const
  {
    return m_Classes;
  }

  /** Is there any classes available ? */
  bool ObjectLabelingModel::HasClasses() const
  {
    return !m_Classes.empty();
  }

  /** Set class name */
  void ObjectLabelingModel::SetClassName(const char * name, unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      m_Classes[classIndex].m_Name = name;
      this->NotifyAll("Update");
      }
  }

  /** Set class label */
  void ObjectLabelingModel::SetClassLabel(const LabelType & label, unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      ObjectClassVectorType::const_iterator it = m_Classes.begin();
      bool found = false;

      while(it!=m_Classes.end() && !found)
        {
        if(it->m_Label == label)
          {
          found = true;
          }
        ++it;
        }

      if(!found)
        {
        m_Classes[classIndex].m_Label = label;
        this->NotifyAll("Update");
        }
      else
        {
        itkExceptionMacro(<<"Label "<<label<<" is already in use.");
        }
      }
  }

  /** Set class color */
  void ObjectLabelingModel::SetClassColor(const ColorType & color, unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      m_Classes[classIndex].m_Color = color;
      this->NotifyAll("Update");
      }
  }

  /** Select sample */
  void ObjectLabelingModel::SelectSample(const IndexType & index)
  {
    // Check for a valid label image
    if(!m_LabeledImage)
      {
      return;
      }

    LabelType label = m_LabeledImage->GetPixel(index);

    this->SelectSample(label);
  }
  void ObjectLabelingModel::SelectSample(const LabelType & label)
  {
    m_SelectedLabel = label;

    // Add the polygon to the VectorData
    PolygonType::Pointer polygon = m_LabelMap->GetLabelObject(label)->GetPolygon();
    m_SelectedPolygonNode->SetPolygonExteriorRing(polygon);

    this->NotifyAll("Update");
  }

  /** Is the given sample selected */
  bool ObjectLabelingModel::IsSampleSelected(const IndexType & index)
  {
    // Check for a valid label image
    if(!m_LabeledImage)
      {
      return false;
      }

    LabelType label = m_LabeledImage->GetPixel(index);

    return this->IsSampleSelected(label);

  }
  bool ObjectLabelingModel::IsSampleSelected(const LabelType & label)
  {
    return (m_SelectedLabel == label);
  }

  bool ObjectLabelingModel::IsSampleSelected()
  {
    return (m_SelectedLabel != itk::NumericTraits<LabelType>::max());
  }

  void ObjectLabelingModel::ClearSelectedSample()
  {
    m_SelectedLabel = itk::NumericTraits<LabelType>::max();
    m_SelectedPolygonNode->SetPolygonExteriorRing(PolygonType::New());
    this->NotifyAll("Update");
  }

  void ObjectLabelingModel::IndexClicked(const IndexType & index)
  {
    // If there is a class selected
    if(this->HasSelectedClass())
      {
      LabelType label = m_LabeledImage->GetPixel(index);

      // If sample is already selected
      if(this->IsSampleSelected(label))
        {
        this->AddSampleToClass(label,this->GetSelectedClass());
        this->ClearSelectedSample();
        }
      else
        {
        // Select it
        this->SelectSample(label);
        }

      this->NotifyAll("Update");
      }
  }

  void ObjectLabelingModel::SelectClass(unsigned int classIndex)
  {
    if(classIndex < m_Classes.size())
      {
      m_SelectedClass = classIndex;
      m_HasSelectedClass = true;
      this->NotifyAll("Update");
      }
    else
      {
      this->ClearSelectedClass();
      }
  }

  void ObjectLabelingModel::ClearSelectedClass()
  {
    m_SelectedClass = 0;
    m_HasSelectedClass = false;
    this->NotifyAll("Update");
  }

  bool ObjectLabelingModel::HasSelectedClass()
  {
    return m_HasSelectedClass;
  }

  unsigned int ObjectLabelingModel::GetSelectedClass()
  {
    if(!m_HasSelectedClass)
      {
      itkExceptionMacro(<<"No selected class.");
      }
    return m_SelectedClass;
  }

  void ObjectLabelingModel::SaveSamplesToXMLFile(const char * fname)
  {
    TiXmlDocument doc;

    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "ObjectLabelingClasses" );
    doc.LinkEndChild( root );

    TiXmlElement * sources = new TiXmlElement( "Sources" );
    root->LinkEndChild( sources );

    TiXmlElement* srcImage = new TiXmlElement("SourceImage");
    srcImage->LinkEndChild(new TiXmlText( m_VectorImageFile.c_str() ));
    sources->LinkEndChild(srcImage);

    TiXmlElement* labelImage = new TiXmlElement("LabeledImage");
    //labelImage->LinkEndChild(new TiXmlText( m_LabeledImageFile.c_str() ));
    sources->LinkEndChild(labelImage);

    TiXmlElement * classes = new TiXmlElement( "Classes" );
    root->LinkEndChild( classes );

    for(ObjectClassVectorType::const_iterator oit = m_Classes.begin(); oit != m_Classes.end();++oit)
      {
      TiXmlElement * currentClass = new TiXmlElement( "Class" );
      currentClass->SetAttribute("label", oit->m_Label);
      classes->LinkEndChild( currentClass );

      TiXmlElement * name = new TiXmlElement( "Name" );
      name->LinkEndChild( new TiXmlText( oit->m_Name.c_str() ) );
      currentClass->LinkEndChild(name);

      TiXmlElement * color = new TiXmlElement( "Color" );
      color->SetAttribute("r", static_cast<int>(oit->m_Color[0] * 255));
      color->SetAttribute("g", static_cast<int>(oit->m_Color[1] * 255));
      color->SetAttribute("b", static_cast<int>(oit->m_Color[2] * 255));
      color->SetAttribute("a", static_cast<int>(oit->m_Color[3] * 255));
      currentClass->LinkEndChild(color);

      TiXmlElement * samples = new TiXmlElement( "Samples" );
      currentClass->LinkEndChild( samples );

      for(ObjectClassType::LabelVectorType::const_iterator lit = oit->m_Samples.begin(); lit != oit->m_Samples.end();++lit)
        {
        TiXmlElement * sample = new TiXmlElement( "Sample" );
        sample->SetAttribute("lbl", *lit);
        samples->LinkEndChild(sample);
        }
      }

    doc.SaveFile( fname );
  }


  void ObjectLabelingModel::SaveClassificationParametersToXMLFile(const char * fname)
  {
    itk::OStringStream oss;

    // Build the model name
    ossimFilename tmpfname(fname);
    ossimFilename svmExt = tmpfname.noExtension().setExtension("svm");

    // Save the model to the svm file
    m_SVMEstimator->GetModel()->SaveModel(svmExt);

    TiXmlDocument doc;

    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    TiXmlElement * root = new TiXmlElement( "ObjectLabelingClassificationParameters" );
    doc.LinkEndChild( root );

    // The svm model file
    TiXmlElement * svmModelFile = new TiXmlElement( "SVMModelFile" );
    svmModelFile->LinkEndChild(new TiXmlText(svmExt));
    root->LinkEndChild( svmModelFile );

    // The classes
    TiXmlElement * classes = new TiXmlElement( "Classes" );
    root->LinkEndChild( classes );

    for(ObjectClassVectorType::const_iterator oit = m_Classes.begin(); oit != m_Classes.end();++oit)
      {
      TiXmlElement * currentClass = new TiXmlElement( "Class" );
      currentClass->SetAttribute("label", oit->m_Label);
      classes->LinkEndChild( currentClass );

      TiXmlElement * name = new TiXmlElement( "Name" );
      name->LinkEndChild( new TiXmlText( oit->m_Name.c_str() ) );
      currentClass->LinkEndChild(name);
      }

    // The features
    TiXmlElement * features = new TiXmlElement( "Features" );
    root->LinkEndChild(features);

    // For each feature
    AvailableFeaturesMapType::const_iterator fit;

    for(fit = m_AvailableFeatures.begin(); fit!=m_AvailableFeatures.end();++fit)
      {
      // If it is selected
      if(fit->second)
        {
        // Generate the feature node
        TiXmlElement * feature = new TiXmlElement( "Feature" );
        features->LinkEndChild(feature);

        // Name of the feature
        TiXmlElement * name = new TiXmlElement( "Name" );
        name->LinkEndChild(new TiXmlText(fit->first.c_str()));
        feature->LinkEndChild(name);

        // Min of the feature
        TiXmlElement * min = new TiXmlElement( "MinValue" );
        oss.str("");
        oss<<m_FeaturesMinimum[fit->first];
        min->LinkEndChild(new TiXmlText(oss.str().c_str()));
        feature->LinkEndChild(min);

        // Max of the feature
        TiXmlElement * max = new TiXmlElement( "MaxValue" );
        oss.str("");
        oss<<m_FeaturesMaximum[fit->first];
        max->LinkEndChild(new TiXmlText(oss.str().c_str()));
        feature->LinkEndChild(max);
        }
      }

    // Finally, write the file
    doc.SaveFile( fname );
  }

  void ObjectLabelingModel::LoadSamplesFromXMLFile(const char * fname)
  {
    // Currently tested only from a fresh new class list (just after importing the images)
    if (m_Classes.size() > 0)
      {
      itkExceptionMacro(<<"The classes list must be empty before loading the file"<<fname);
      }

    TiXmlDocument doc(fname);
    if (!doc.LoadFile())
      {
      itkExceptionMacro(<<"Can't open file "<<fname);
      }

    TiXmlHandle hDoc(&doc);
    TiXmlHandle classes = hDoc.FirstChildElement("ObjectLabelingClasses").FirstChildElement("Classes");

    for( TiXmlElement* currentClass = classes.FirstChildElement("Class").ToElement();
        currentClass != NULL;
        currentClass = currentClass->NextSiblingElement() )
      {
      ObjectClassType newClass;
      int readLabel;
      LabelType classLabel;
      currentClass->QueryIntAttribute("label", &readLabel);
      classLabel = static_cast<LabelType>(readLabel);

      TiXmlElement* nameElement = currentClass->FirstChildElement("Name");
      if (!nameElement)
        {
        itkExceptionMacro(<<"Bad XML file : "<<fname);
        }
      std::string name = nameElement->GetText();

      TiXmlElement* colorElement = currentClass->FirstChildElement("Color");
      if (!colorElement)
        {
        itkExceptionMacro(<<"Bad XML file : "<<fname);
        }
      int readColor[4];
      colorElement->Attribute("r", &readColor[0]);
      colorElement->Attribute("g", &readColor[1]);
      colorElement->Attribute("b", &readColor[2]);
      colorElement->Attribute("a", &readColor[3]);

      ColorType color;
      color[0] = readColor[0] / 255.;
      color[1] = readColor[1] / 255.;
      color[2] = readColor[2] / 255.;
      color[3] = readColor[3] / 255.;

      AddClass(classLabel, name, color);
      unsigned int classIndex = GetClassIndex(classLabel);

      TiXmlElement* samplesElement = currentClass->FirstChildElement("Samples");
      if (samplesElement)
        {
        for( TiXmlElement* sample = samplesElement->FirstChildElement("Sample");
            sample != NULL;
            sample = sample->NextSiblingElement() )
          {
          int sampleLabel;
          sample->QueryIntAttribute("lbl", &sampleLabel);
          LabelType label = static_cast<LabelType>(sampleLabel);
          this->AddSampleToClass(label, classIndex);
          }
        }
      else
        {
        itkExceptionMacro(<<"Bad XML file : "<<fname);
        }
      }

    this->NotifyAll("Update");
  }

  void ObjectLabelingModel::ComputeFeaturesStatistics()
  {
    otbMsgDevMacro(<<"Computing statistics ...");
    m_FeaturesMeans.clear();
    m_FeaturesVariances.clear();

    std::map<std::string,double> sum;
    std::map<std::string,double> sumOfSquares;

    LabelMapType::LabelObjectContainerType::const_iterator it
    = m_LabelMap->GetLabelObjectContainer().begin();

    for(AvailableFeaturesMapType::const_iterator fit = m_AvailableFeatures.begin();
        fit != m_AvailableFeatures.end();++fit)
      {
      sum[fit->first] = 0.;
      sumOfSquares[fit->first] = 0.;
      m_FeaturesMinimum[fit->first] = it->second->GetAttribute((fit->first).c_str());
      m_FeaturesMaximum[fit->first] = it->second->GetAttribute((fit->first).c_str());
      }

    unsigned int nbSamples = 0;

    // iterate on label objects
    while(it != m_LabelMap->GetLabelObjectContainer().end())
      {
      for(AvailableFeaturesMapType::const_iterator fit = m_AvailableFeatures.begin();
          fit != m_AvailableFeatures.end();++fit)
        {
        // Update sums and sums of squares
        double value = it->second->GetAttribute((fit->first).c_str());
        sum[fit->first]+= value;
        sumOfSquares[fit->first]+= vcl_pow(value,2);

        if(m_FeaturesMinimum[fit->first] > value)
          {
          m_FeaturesMinimum[fit->first] = value;
          }
        if(m_FeaturesMaximum[fit->first] < value)
          {
          m_FeaturesMaximum[fit->first] = value;
          }

        }
      ++nbSamples;
      ++it;
      }

    // Compute mean and variance from sums and sums of squares
    for(AvailableFeaturesMapType::const_iterator fit = m_AvailableFeatures.begin();
        fit != m_AvailableFeatures.end();++fit)
      {
      m_FeaturesMeans[fit->first]=sum[fit->first]/static_cast<double>(nbSamples);
      m_FeaturesVariances[fit->first]=vcl_sqrt(sumOfSquares[fit->first]/static_cast<double>(nbSamples)-vcl_pow(m_FeaturesMeans[fit->first],2));
      }
    otbMsgDevMacro(<<"Done.");
  }

  void ObjectLabelingModel::SaveClassification()
  {

    //this->Classify();

    //   typedef otb::ImageFileWriter<VectorImageType> WriterType;
    //   WriterType::Pointer writer = WriterType::New();
    //   writer->SetFileName(fname);
    //   writer->SetInput(m_ColorMapper->GetOutput());
    //   writer->Update();

    m_LabeledOutput = m_ClassLabelFilter->GetOutput();
    m_ColoredOutput = m_ColorMapper->GetOutput();
    this->NotifyAll("OutputsUpdated");
  }

  void ObjectLabelingModel::EstimateCentroids()
  {
    otbMsgDevMacro(<<"Computing Kmeans centroids ..." );
    otbMsgDevMacro(<<"Number of classes: "<<m_Classes.size());
    unsigned int numberOfCentroids = m_Classes.size();

    if(numberOfCentroids <= 1)
      {
      otbMsgDevMacro(<<"ONE CLASS svm mode, forcing number of centroids to 10");
      numberOfCentroids = 10;
      }
    otbMsgDevMacro(<<"Number of centroids: "<<numberOfCentroids);


    ListSampleType::Pointer ls = ListSampleType::New();

    LabelMapType::LabelObjectContainerType::const_iterator it = m_LabelMap->GetLabelObjectContainer().begin();

    VectorType newSample = this->BuildSample(it->second);
    unsigned int sampleSize = newSample.GetSize();

    // Add the new sample
    ls->PushBack(newSample);
    ++it;

    // Iterate on label objects
    while(it!=m_LabelMap->GetLabelObjectContainer().end())
      {
      VectorType newSample = this->BuildSample(it->second);

      // Add the new sample
      ls->PushBack(newSample);
      ++it;
      }

    // Build the Kd Tree
    TreeGeneratorType::Pointer kdTreeGenerator = TreeGeneratorType::New();
    kdTreeGenerator->SetSample(ls);
    kdTreeGenerator->SetBucketSize(100);
    kdTreeGenerator->Update();

    // Randomly pick the initial means among the classes
    EstimatorType::ParametersType initialMeans(sampleSize*numberOfCentroids);
    initialMeans.Fill(0.);

    // TODO: Do we need random intialization here ?
    srand(0);
    if(m_Classes.size() > 1)
      {
      for(unsigned int cId = 0; cId<m_Classes.size();++cId)
        {
        // Identify one centroid and get the sample
        unsigned int centroidId = static_cast<unsigned int>(static_cast<double>(m_Classes[cId].m_Samples.size())*rand()/RAND_MAX);
        assert(centroidId<m_Classes[cId].m_Samples.size());
        LabelType centroidLabel = m_Classes[cId].m_Samples[centroidId];
        VectorType centroidSample = this->BuildSample(m_LabelMap->GetLabelObject(centroidLabel));
        // Fill the initial means
        for(unsigned int idx = 0; idx < sampleSize; ++idx)
          {
          assert(idx<centroidSample.Size());
          assert(cId*sampleSize+idx<initialMeans.Size());
          initialMeans[cId*sampleSize+idx]=centroidSample[idx];
          }
        }
      }
    else
      {
      // ONE CLASS case
      for(unsigned int cId = 0; cId<numberOfCentroids;++cId)
        {
        unsigned int centroidId = static_cast<unsigned int>(static_cast<double>(m_LabelMap->GetNumberOfLabelObjects())*rand()/RAND_MAX);
        assert(centroidId<m_LabelMap->GetNumberOfLabelObjects());
        LabelType centroidLabel = m_LabelMap->GetNthLabelObject(centroidId)->GetLabel();
        VectorType centroidSample = this->BuildSample(m_LabelMap->GetLabelObject(centroidLabel));
        // Fill the initial means
        for(unsigned int idx = 0; idx < sampleSize; ++idx)
          {
          assert(idx<centroidSample.Size());
          assert(cId*sampleSize+idx<initialMeans.Size());
          initialMeans[cId*sampleSize+idx]=centroidSample[idx];
          }

        }
      }

    // Do KMeans estimation
    EstimatorType::Pointer estimator = EstimatorType::New();
    estimator->SetParameters(initialMeans);
    estimator->SetKdTree(kdTreeGenerator->GetOutput());
    estimator->SetMaximumIteration(10000);
    estimator->SetCentroidPositionChangesThreshold(0.00001);
    estimator->StartOptimization();

    // Retrieve final centroids
    m_CentroidsVector.clear();

    for(unsigned int cId = 0; cId<numberOfCentroids;++cId)
      {
      VectorType newCenter(sampleSize);

      for(unsigned int i = 0; i < sampleSize; ++i)
        {
        assert(cId * sampleSize + i<estimator->GetParameters().Size());
        newCenter[i]=estimator->GetParameters()[cId * sampleSize + i];
        }
      otbMsgDevMacro(<<"New centroid: "<<newCenter);
      m_CentroidsVector.push_back(newCenter);
      }

    otbMsgDevMacro(<<"Done.");
  }

  void ObjectLabelingModel::BuildSampleList()
  {
    otbMsgDevMacro(<<"Building samples list ...");

    // Clear previous samples
    m_ListSample->Clear();

    LabelMapType::LabelObjectContainerType::const_iterator it = m_LabelMap->GetLabelObjectContainer().begin();

    // Iterate on label objects
    while(it!=m_LabelMap->GetLabelObjectContainer().end())
      {
      VectorType newSample = this->BuildSample(it->second);

      // Add the new sample
      m_ListSample->PushBack(newSample);

      ++it;
      }
    otbMsgDevMacro(<<"Done.");
  }

  void ObjectLabelingModel::BuildTrainingSampleList()
  {
    otbMsgDevMacro(<<"Building training samples list ...");
    // Clear previous samples
    m_TrainingListSample->Clear();
    m_LabelsListSample->Clear();

    // For each classes
    for(ObjectClassVectorType::const_iterator oit = m_Classes.begin(); oit != m_Classes.end();++oit)
      {
      // For each sample in class
      for(ObjectClassType::LabelVectorType::const_iterator lit = oit->m_Samples.begin(); lit != oit->m_Samples.end();++lit)
        {

        if(m_LabelMap->HasLabel(*lit))
          {

          // Create new sample
          VectorType newSample = this->BuildSample(m_LabelMap->GetLabelObject(*lit));

          TrainingVectorType label;
          label[0]=oit->m_Label;

          // Add the new samples
          m_TrainingListSample->PushBack(newSample);
          m_LabelsListSample->PushBack(label);

          //std::cout<<"Training: "<<label<<" "<<newSample<<std::endl;
          }
        }
      }
    otbMsgDevMacro(<<"Done.");
  }

  void ObjectLabelingModel::ClearMarginSamples()
  {
    otbMsgDevMacro(<<"Erase any previous margin sampled polygons ...");

    // Clear margin samples
    m_MarginSamples.clear();

    // Erase the previous margin sampled polygons
    TreeNodeType * node = const_cast<TreeNodeType *> (m_MarginSampledPolygon->GetDataTree()->GetNode(m_MarginSampledFolder));
    unsigned int nbChildren = node->CountChildren();
    for(unsigned int i = 0; i<nbChildren;++i)
      {
      node->Remove(node->GetChild(nbChildren-i-1));
      }
    this->NotifyAll("Update");
    otbMsgDevMacro(<<"Done.");
  }


  void ObjectLabelingModel::Train()
  {
    // Build the sample lists
    this->BuildSampleList();
    this->BuildTrainingSampleList();

    otbMsgDevMacro(<<"Estimating model ...");
    // Model estimation
    //m_SVMEstimator = SVMEstimatorType::New();
    m_SVMEstimator->SetInputSampleList(m_TrainingListSample);
    m_SVMEstimator->SetTrainingSampleList(m_LabelsListSample);
    m_SVMEstimator->Modified();
    otbMsgDevMacro(<<"Number of classes: "<<m_Classes.size());
    m_SVMEstimator->SetNumberOfClasses(m_Classes.size());

    if(m_Classes.size() == 1)
      {
      otbMsgDevMacro(<<"ONE_CLASS svm mode.");
      m_SVMEstimator->SetSVMType(ONE_CLASS);
      }

    m_SVMEstimator->Update();

    m_Accuracy = m_SVMEstimator->GetFinalCrossValidationAccuracy();

    otbMsgDevMacro(<<"Done.");
  }

  void ObjectLabelingModel::SampleMargin()
  {
    this->ClearMarginSamples();

    // Triggers training
    this->Train();

    otbMsgDevMacro(<<"Sampling margin ...");

    // Margin sampling
    // m_MarginSampler = MarginSamplerType::New();
    m_MarginSampler->SetSample(m_ListSample);
    m_MarginSampler->SetModel(m_SVMEstimator->GetModel());
    m_MarginSampler->Modified();
    m_MarginSampler->Update();

    otbMsgDevMacro(<<"Done.");

    otbMsgDevMacro(<<"Add the margin sampled polygons to the displayed vector layers ...");

    // For each margin sample
    for(MarginSamplerType::IndexVectorType::const_iterator sit = m_MarginSampler->GetMarginSamples().begin();
        sit!=m_MarginSampler->GetMarginSamples().end();++sit)
      {
      // Add sample to margin samples vector
      m_MarginSamples.push_back(m_LabelMap->GetNthLabelObject(*sit)->GetLabel());

      // Add the polygon to the VectorData
      SimplifyPolygonFunctorType sfunctor;
      DataNodeType::Pointer polygonNode = DataNodeType::New();
      PolygonType::Pointer polygon = /**sfunctor(*/m_LabelMap->GetNthLabelObject(*sit)->GetPolygon()/**)*/;
      polygonNode->SetPolygonExteriorRing(polygon);
      m_MarginSampledPolygon->GetDataTree()->Add(polygonNode,m_MarginSampledFolder);
      }

    otbMsgDevMacro(<<"Done.");

    this->NotifyAll("Update");
  }

  void ObjectLabelingModel::Classify()
  {
    // Triggers training
    this->Train();

    // SVM Classification
    otbMsgDevMacro(<<"Classifying ...");
    SVMClassifierType::Pointer svmClassifier = SVMClassifierType::New();
    svmClassifier->SetSample(m_ListSample);
    svmClassifier->SetModel(m_SVMEstimator->GetModel());
    if(m_Classes.size() == 1)
      {
      svmClassifier->SetNumberOfClasses(2);
      }
    else
      {
      svmClassifier->SetNumberOfClasses(m_Classes.size());
      }

    svmClassifier->Update();
    otbMsgDevMacro(<<"Done.");

    // Class labeling the label object
    otbMsgDevMacro(<<"Class labeling the objects ...");
    LabelMapType::LabelObjectContainerType::const_iterator lit
    = m_LabelMap->GetLabelObjectContainer().begin();
    SVMClassifierType::OutputType::ConstIterator sit = svmClassifier->GetOutput()->Begin();


    while(lit != m_LabelMap->GetLabelObjectContainer().end()
        && sit!=svmClassifier->GetOutput()->End())
      {
      lit->second->SetClassLabel(sit.GetClassLabel());
      ++lit;
      ++sit;
      }
    otbMsgDevMacro(<<"Done.");

    m_ClassLabelFilter = ClassLabelFilterType::New();
    m_ClassLabelFilter->SetInput(m_LabelMap);
    m_ClassLabelFilter->Update();

    // Coloring classes */
    m_ColorMapper->SetInput(m_ClassLabelFilter->GetOutput());
    m_ColorMapper->SetNumberOfComponentsPerPixel(3);
    m_ColorMapper->ClearChangeMap();

    VectorPixelType defaultValue(3);
    defaultValue.Fill(0);

    // If one class SVM
    if(m_Classes.size() == 1)
      {
      VectorPixelType newValue(3);
      newValue[0]=255*m_Classes[0].m_Color[0];
      newValue[1]=255*m_Classes[0].m_Color[1];
      newValue[2]=255*m_Classes[0].m_Color[2];
      otbMsgDevMacro(<<"New value: "<<newValue);
      m_ColorMapper->SetChange(1,newValue);
      m_ColorMapper->SetChange(2,defaultValue);

      }

    // For each classes
    for(ObjectClassVectorType::const_iterator oit = m_Classes.begin(); oit != m_Classes.end();++oit)
      {
      VectorPixelType newValue(3);
      newValue[0]=255*oit->m_Color[0];
      newValue[1]=255*oit->m_Color[1];
      newValue[2]=255*oit->m_Color[2];
      otbMsgDevMacro(<<"New value: "<<oit->m_Label<<" "<<newValue);
      m_ColorMapper->SetChange(oit->m_Label,newValue);
      }

    m_ColorMapper->Update();
    otbMsgDevMacro(<<"Color Mapper update.");
    m_VisualizationModel->GetLayerByName("Classification")->SetVisible(true);
    otbMsgDevMacro(<<"Get layer by name.");

    m_VisualizationModel->Update();
    otbMsgDevMacro(<<"Refresh done");

    this->NotifyAll("Update");

    //   this->SaveClassification();
  }

  void ObjectLabelingModel::ClearClassification()
  {
    m_VisualizationModel->GetLayerByName("Classification")->SetVisible(false);
    m_VisualizationModel->Update();
  }

  void ObjectLabelingModel::ChangeFeatureState(const std::string& fname, bool state)
  {
    //   otbMsgDevMacro(<<"Trying "<<fname);
    if(m_AvailableFeatures.count(fname)>0)
      {
      //     otbMsgDevMacro(<<"Changing state of feature "<<fname<<" to "<<state);
      m_AvailableFeatures[fname]=state;
      }
    else
      {
      itkExceptionMacro(<<"Feature "<<fname<<" could not be found in the available features.");
      }
  }

  void ObjectLabelingModel::ChangeClassificationOpacity(double opacity)
  {
    BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
    blender->SetAlpha(opacity);
    m_VisualizationModel->GetLayerByName("Classification")->SetBlendingFunction(blender);
    m_VisualizationModel->Update();
  }

  void ObjectLabelingModel::UpdateViewerDisplay(std::vector<unsigned int> ch)
  {
    if(m_VisualizationModel->GetNumberOfLayers() != 2 )
      {
      itkExceptionMacro("Invalid number of layers");
      }

    m_ImageLayerRenderingFunction->SetChannelList(ch);
    m_VisualizationModel->Update();
  }

  ObjectLabelingModel::VectorType ObjectLabelingModel::BuildSample(const LabelObjectType * lo) const
  {

    // Compute the sample size
    unsigned int sampleSize = 0;
    AvailableFeaturesMapType::const_iterator fit;

    for(fit = m_AvailableFeatures.begin(); fit!=m_AvailableFeatures.end();++fit)
      {
      if(fit->second)
        {
        sampleSize++;
        }
      }

    // Create new sample
    VectorType newSample(sampleSize);
    newSample.Fill(0);

    unsigned int index = 0;

    // Iterate on available features
    for(fit = m_AvailableFeatures.begin(); fit!=m_AvailableFeatures.end();++fit)
      {
      if(fit->second)
        {
        // Centered and reduced
        //double mean = m_FeaturesMeans.find(fit->first)->second;
        //double var = m_FeaturesVariances.find(fit->first)->second;
        //newSample[index] = (lo->GetAttribute(fit->first.c_str())-mean)/var;

        // Linear
        double min = m_FeaturesMinimum.find(fit->first)->second;
        double max = m_FeaturesMaximum.find(fit->first)->second;

        // TODO: Check assertion
        assert(index<sampleSize);

        newSample[index] =(lo->GetAttribute(fit->first.c_str())-min)/(max-min);
        ++index;
        }
      }

    //otbMsgDevMacro(<<"NewSample: "<<newSample);

    // Return the sample
    return newSample;
  }

}
#endif
