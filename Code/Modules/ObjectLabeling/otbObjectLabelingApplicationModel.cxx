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
#ifndef __otbObjectLabelingApplicationModel_cxx
#define __otbObjectLabelingApplicationModel_cxx

#include "otbObjectLabelingApplicationModel.h"
#include "otbImageFileWriter.h"
#include "base/ossimFilename.h"

#include "otbPostGISConnectionImplementation.h"
#include "otbPostGISTable.h"
#include <cstdlib>
namespace otb
{


struct SQLStringFunctor
{
  std::string operator()(std::string& in)
    {
    std::string out = in;
    std::replace(out.begin(), out.end(), ':', '_');
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
    }
};


typedef otb::PostGISConnectionImplementation BdConnection;
typedef otb::PostGISConnectionImplementation::Pointer BdConnectionPointer;
typedef otb::PostGISTable<BdConnection, double, 2> PostGISTableType;
typedef otb::PostGISTable<BdConnection, double, 2>::Pointer PostGISTableTypePointer;


class PostGISCreateTableWithAttributesAndClassTransactor
: public PostGISCreateTableTransactor
{
public:

  void SetAttributes(std::vector<std::string> attributes)
  {
    m_Attributes = attributes;
  }

  void operator()(pqxx::nontransaction &T)
  {
    // Drop old table if necessarry
    if(m_RemoveExistingTable)
      {
      std::stringstream dropCommand;
      dropCommand << "DROP TABLE IF EXISTS " << m_TableName;
      otbGenericMsgDebugMacro(<<"Drop Command : " << std::endl << dropCommand.str());
      m_Result = T.exec(dropCommand.str());
      }

    // Create new table
    std::stringstream createCommand;
    createCommand << "CREATE TABLE " << m_TableName
                  << " (id serial PRIMARY KEY, class_id INTEGER";
    std::vector<std::string>::const_iterator it;
    for (it =  m_Attributes.begin(); it !=  m_Attributes.end(); ++it)
      {
      createCommand << ", " << (*it) << " NUMERIC";
      }
    createCommand << ");";

    otbGenericMsgDebugMacro(<<"Create Command : " << std::endl << createCommand.str());
    m_Result = T.exec(createCommand.str());

    // Add geometry column
    std::stringstream addGeometryCommand;
    addGeometryCommand << "SELECT AddGeometryColumn( '"<< m_TableName <<
      "', 'the_geom', "<< m_SRID <<", 'GEOMETRY',"<< m_Dimension <<" );";
    m_Result = T.exec(addGeometryCommand.str());

    // Create GIST index
    std::stringstream addGISTIndexCommand;
    addGISTIndexCommand << "CREATE INDEX idx_" << m_TableName << "_the_geom ON " << m_TableName << " USING gist( the_geom );";
    otbGenericMsgDebugMacro(<<"Create Command " << addGISTIndexCommand.str());
    m_Result = T.exec(addGISTIndexCommand.str());
  }
private:
  std::vector<std::string> m_Attributes;
};


class ITK_EXPORT PostGISTableWithAttributesAndClass
: public otb::PostGISTable<BdConnection, double, 2>
{
public:
  typedef PostGISTableWithAttributesAndClass Self;
  typedef otb::PostGISTable<BdConnection, double, 2> Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkNewMacro(Self);
  itkTypeMacro(PostGISTableWithAttributesAndClass,GISTable);
  itkStaticConstMacro(Dimension, unsigned int, 2);

  /** Some convenient typedefs */
  typedef Superclass::ConnectionType ConnectionType;
  typedef Superclass::ConnectionPointerType ConnectionPointerType;
  typedef Superclass::ConnectionPointerType PointType;
  typedef Superclass::LineType LineType;
  typedef Superclass::LineType LinePointerType;
  typedef Superclass::LineType PolygonType;
  typedef Superclass::PolygonPointerType PolygonPointerType;
  typedef Superclass::PolygonConstPointerType PolygonConstPointerType;
  typedef Superclass::PolygonListType PolygonListType;
  typedef Superclass::PolygonListPointerType PolygonListPointerType;
  typedef Superclass::PolygonListPointerType PolygonListConstPointerType;

  typedef ObjectLabelingApplicationModel::LabelObjectType LabelObjectType;
  typedef ObjectLabelingApplicationModel::LabelMapType LabelMapType;

public:

  const std::vector<std::string>& GetAttributes()
  {
    return m_Attributes;
  }
  void SetAttributes( const std::vector<std::string>& attributes )
  {
    m_Attributes = attributes;
    std::transform(m_Attributes.begin(), m_Attributes.end(), std::back_inserter(m_SQLAttributes), SQLStringFunctor());
  }

  // Create the table in the db. Attributes must be set already
  void CreateTable(bool dropExisting = true)
  {
    typedef PostGISCreateTableWithAttributesAndClassTransactor TransactorType;

    TransactorType myTransactor;
    myTransactor.SetAttributes(m_SQLAttributes);
    myTransactor.SetDimension( Dimension );
    myTransactor.SetTableName( this->GetTableName() );
    myTransactor.SetSRID( this->GetSrid() );
    myTransactor.SetRemoveExistingTable( dropExisting );

    this->GetConnection()->GetConnection()->perform( myTransactor );
  }

  void InsertLabelObject( std::stringstream& sqlCmd, const LabelObjectType* lo )
  {
    sqlCmd << "INSERT INTO "
           << "\"" << this->GetTableName() << "\" "
           << "( "
           << "\"the_geom\""
           << ", \"class_id\"";

    std::vector<std::string>::const_iterator it;
    for (it = m_SQLAttributes.begin(); it != m_SQLAttributes.end(); ++it)
      {
      sqlCmd << " ," << "\"" << *it << "\"";
      }

    sqlCmd << ") VALUES ";

    // Add geometry
    sqlCmd << "(GeometryFromText('";
    sqlCmd << "POLYGON( ( ";


    PolygonType::Pointer polygon = m_PolygonExtractor(lo);
    PolygonType::Pointer correctPolygon = m_CorrectFunctor(polygon);

    typedef PolygonType::VertexListConstIteratorType VertexIterator;
    VertexIterator itVertex = correctPolygon->GetVertexList()->Begin();

    while( itVertex != correctPolygon->GetVertexList()->End() )
      {
      sqlCmd << itVertex.Value()[0] << " " << itVertex.Value()[1];
      ++itVertex;

      if (itVertex != correctPolygon->GetVertexList()->End())
        {
        sqlCmd << ",";
        }
      }
    sqlCmd << ") )'";


    sqlCmd << " ," << this->GetSrid() << ") "; // end "GeometryFromText("

    sqlCmd << " ," << lo->GetClassLabel();
    //sqlCmd << " ," << lo->GetLabel();


    for (it = m_Attributes.begin(); it != m_Attributes.end(); ++it)
      {
      sqlCmd << " ," << lo->GetAttribute( (*it).c_str() );
      }

    sqlCmd << ");" << std::endl;

  }

  void InsertLabelMap( LabelMapType& labelMap )
  {
    typedef otb::PostGISFromStringTransactor TransactorType;
    TransactorType transactor;


    LabelMapType::LabelObjectContainerType::const_iterator it, beg, end;
    beg = labelMap.GetLabelObjectContainer().begin();
    end = labelMap.GetLabelObjectContainer().end();

    unsigned long nbLabelObjects = labelMap.GetNumberOfLabelObjects();
    unsigned long progressEach = nbLabelObjects / 20;
    unsigned long i = 0;

    otbGenericMsgDebugMacro( << "Inserting " << nbLabelObjects << " label objects..." );
    for( it = beg; it != end; ++it)
      {
      std::stringstream sqlCmd;
      this->InsertLabelObject( sqlCmd, it->second );
      transactor.SetTransactionString( sqlCmd.str() );
      this->GetConnection()->GetConnection()->perform( transactor );

      i++;
      if (i % progressEach == 0)
        {
        otbGenericMsgDebugMacro( <<  static_cast<int>( ::round(i * 100.0 / nbLabelObjects) )  << " % done" );
        }

      }

  }

protected:
  PostGISTableWithAttributesAndClass() {};
  virtual ~PostGISTableWithAttributesAndClass() {};

private:
  PostGISTableWithAttributesAndClass(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typedef otb::Functor::LabelObjectToPolygonFunctor<LabelObjectType,PolygonType> FunctorType;
  typedef otb::CorrectPolygonFunctor <PolygonType> CorrectFunctorType;

  FunctorType m_PolygonExtractor;
  CorrectFunctorType m_CorrectFunctor;

  std::vector<std::string> m_Attributes;
  std::vector<std::string> m_SQLAttributes; // m_Attributes with SQL regularized name
};






/** Initialize the singleton */
ObjectLabelingApplicationModel::Pointer ObjectLabelingApplicationModel::Instance = NULL;

/** Manage the singleton */
ObjectLabelingApplicationModel::Pointer
ObjectLabelingApplicationModel::GetInstance()
{
  if (!Instance)
  {
    Instance = ObjectLabelingApplicationModel::New();
  }
  return Instance;
}

void ObjectLabelingApplicationModel::Notify(ObjectLabelingApplicationEventsListener * listener)
{
  listener->Notify();
}

ObjectLabelingApplicationModel::ObjectLabelingApplicationModel() : m_VisualizationModel(), m_PixelDescriptionModel(), m_Classes(), m_LabeledImage(), m_VectorImage(), m_LabelMap(), m_SelectedLabel(itk::NumericTraits<LabelType>::max()), m_SelectedClass(0),m_HasSelectedClass(false), m_SelectedPolygon(), m_SelectedPolygonNode(), m_AvailableFeatures()
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
  m_NeighborsPolygon = VectorDataType::New();
  document = DataNodeType::New();
  m_NeighborsFolder= DataNodeType::New();
  
  document->SetNodeType(otb::DOCUMENT);
  m_NeighborsFolder->SetNodeType(otb::FOLDER);
  
  root = m_NeighborsPolygon->GetDataTree()->GetRoot()->Get();

  m_NeighborsPolygon->GetDataTree()->Add(document,root);
  m_NeighborsPolygon->GetDataTree()->Add(m_NeighborsFolder,document);


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

  // Do we use context ?
  m_UseContext = true;
}

ObjectLabelingApplicationModel::~ObjectLabelingApplicationModel()
{}


/** Open an image with its associated label map */
void ObjectLabelingApplicationModel::OpenImage(VectorImageType* vimage, LabeledImageType* limage)
{
  // Size checking
  if(limage->GetLargestPossibleRegion() 
     != vimage->GetLargestPossibleRegion())
    {
    itkExceptionMacro(<<"Image and label map size are different, can not load data into the application.");
    }

  m_LabeledImage = limage;
  m_VectorImage  = vimage;

//   m_LabeledImageFile = labels;
//   m_VectorImageFile = image;

  // Update origin and spacing
  m_Origin[0] = -m_VectorImage->GetOrigin()[0];
  m_Origin[1] = -m_VectorImage->GetOrigin()[1];
  m_Spacing[0] = 1/m_VectorImage->GetSpacing()[0];
  m_Spacing[1] = 1/m_VectorImage->GetSpacing()[1];

  // Generate the layer
  LayerGeneratorType::Pointer imageGenerator = LayerGeneratorType::New();
  imageGenerator->SetImage(m_VectorImage);
//   imageGenerator->GenerateQuicklookOff();
//   imageGenerator->SetQuicklook(m_VectorImage);
//   imageGenerator->SetSubsamplingRate(1);
  imageGenerator->GenerateLayer();

  m_ImageLayerRenderingFunction = imageGenerator->GetLayer()->GetRenderingFunction();

  m_Channels.clear();
  if(m_VectorImage->GetNumberOfComponentsPerPixel()==3)
    {
      m_Channels.push_back(0);
      m_Channels.push_back(1);
      m_Channels.push_back(2);
      std::cout<< "3 bands" << std::endl;
    }
  else
    {
      m_Channels.push_back(2);
      m_Channels.push_back(1);
      m_Channels.push_back(0);
      std::cout<< "4 bands" << std::endl;
    }

  m_ImageLayerRenderingFunction->SetChannelList(m_Channels);
  imageGenerator->GetLayer()->SetName("Image");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();
  m_PixelDescriptionModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(imageGenerator->GetLayer());
  m_PixelDescriptionModel->AddLayer(imageGenerator->GetLayer());

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
  if(m_VectorImage->GetNumberOfComponentsPerPixel()==3)
    {
    radiometricLabelMapFilter->SetRedChannelIndex(2);
    radiometricLabelMapFilter->SetGreenChannelIndex(1);
    radiometricLabelMapFilter->SetBlueChannelIndex(0);
    radiometricLabelMapFilter->SetNIRChannelIndex(2);
    }
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
   std::cout<<"Exporting to class label image ..."<<std::endl;
   m_ClassLabelFilter->SetInput(m_LabelMap);
   m_ClassLabelFilter->Update();
   std::cout<<"Done."<<std::endl;

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
     std::cout<<"New value: "<<newValue<<std::endl;
     m_ColorMapper->SetChange(oit->m_Label,newValue);
     }

   // Generate the layer
   LayerGeneratorType::Pointer labeledImageGenerator = LayerGeneratorType::New();
   labeledImageGenerator->SetImage(m_ColorMapper->GetOutput());
//    labeledImageGenerator->GenerateQuicklookOff();
//    labeledImageGenerator->SetQuicklook(m_ColorMapper->GetOutput());
//    labeledImageGenerator->SetSubsamplingRate(1);
   labeledImageGenerator->GenerateLayer();

   labeledImageGenerator->GetLayer()->GetRenderingFunction()->SetAutoMinMax(false);


   LayerGeneratorType::ImageLayerType::RenderingFunctionType::ParametersType parameters(6);
   parameters.Fill(0);
   parameters[1] = 255;
   parameters[3] = 255;
   parameters[5] = 255;

   labeledImageGenerator->GetLayer()->GetRenderingFunction()->SetParameters(parameters);

   BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
   blender->SetAlpha(0);

   labeledImageGenerator->GetLayer()->SetBlendingFunction(blender);
   labeledImageGenerator->GetLayer()->SetName("Classification");
   labeledImageGenerator->GetLayer()->SetVisible(false);

   // Add the layer to the models
   m_VisualizationModel->AddLayer(labeledImageGenerator->GetLayer());
   m_PixelDescriptionModel->AddLayer(labeledImageGenerator->GetLayer());

   m_VisualizationModel->Update();

 
  this->NotifyAll("Update");
}

unsigned int ObjectLabelingApplicationModel::GetNumberOfClasses()
{
  return m_Classes.size();
}

/** Add sample */
void ObjectLabelingApplicationModel::AddSampleToClass(const IndexType & sampleIndex, unsigned int classIndex)
{
  // Check for a valid label image
  if(!m_LabeledImage)
    {
    return;
    }
  
  LabelType label = m_LabeledImage->GetPixel(sampleIndex);

  this->AddSampleToClass(label,classIndex);
  this->NotifyAll();
}


void ObjectLabelingApplicationModel::AddSampleToClass(const LabelType & label, unsigned int classIndex)
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
void ObjectLabelingApplicationModel::RemoveSampleFromClass(const LabelType & label, unsigned int classIndex)
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

    this->NotifyAll();
    }
}

/** Clear samples from class */
void ObjectLabelingApplicationModel::ClearSamplesFromClass(unsigned int classIndex)
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
    this->NotifyAll();
    }
}

/** Focus on sample */
void ObjectLabelingApplicationModel::FocusOnSample(const LabelType & label)
{
  // Convert sample to polygon
  SimplifyPolygonFunctorType sfunctor;
  PolygonType::Pointer polygon = /**sfunctor(*/m_LabelMap->GetLabelObject(label)->GetPolygon()/**)*/;
  VectorImageType::RegionType pregion = polygon->GetBoundingRegion().GetImageRegion();

  VectorImageType::IndexType pindex = pregion.GetIndex();

  pindex[0]+=pregion.GetSize()[0]/2;
  pindex[1]+=pregion.GetSize()[1]/2;

  m_VisualizationModel->SetExtractRegionCenter(pindex);
  m_VisualizationModel->SetScaledExtractRegionCenter(pindex);

  m_VisualizationModel->Update();
}

/** Get a class by its index */
const ObjectLabelingApplicationModel::ObjectClassType & ObjectLabelingApplicationModel::GetClass(unsigned int classIndex) const 
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
unsigned int ObjectLabelingApplicationModel::GetClassIndex(LabelType label) const 
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
void ObjectLabelingApplicationModel::AddClass()
{
  LabelType label = GetNextAvailableClassLabel();

  // TODO: Automatic color generation here
  ColorType color;

  // srand(newLabel+123456);
  color[0]=1 /**rand()/(RAND_MAX+1.0)*/;
  color[1]=0 /**rand()/(RAND_MAX+1.0)*/;
  color[2]=0 /**rand()/(RAND_MAX+1.0)*/;
  color[3]=0.75;

  itk::OStringStream oss;
  oss << "New Class " << label;
  std::string name = oss.str().c_str();

  AddClass( label, name, color );

  this->NotifyAll();
}

/** Add a new class (label, color, and name already known)*/
void ObjectLabelingApplicationModel::AddClass(const LabelType& label, const std::string& name, const ColorType& color)
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


ObjectLabelingApplicationModel::LabelType ObjectLabelingApplicationModel::GetNextAvailableClassLabel()
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
void ObjectLabelingApplicationModel::RemoveClass(unsigned int classIndex)
{
  if(classIndex < m_Classes.size())
    {
    ObjectClassVectorType::iterator it = m_Classes.begin() + classIndex;
    m_Classes.erase(it);
    if(m_SelectedClass>=m_Classes.size())
      {
      this->ClearSelectedClass();
      }
    this->NotifyAll();
    }

}

/** Clear all classes */
void ObjectLabelingApplicationModel::ClearClasses()
{
  m_Classes.clear();
  this->ClearSelectedClass();
  this->NotifyAll();
}

/** Get a const reference on the classes vector */
const ObjectLabelingApplicationModel::ObjectClassVectorType & ObjectLabelingApplicationModel::GetClasses() const
{
  return m_Classes;
}

/** Is there any classes available ? */
bool ObjectLabelingApplicationModel::HasClasses() const
{
  return !m_Classes.empty();
}

/** Set class name */
void ObjectLabelingApplicationModel::SetClassName(const char * name, unsigned int classIndex)
{
  if(classIndex < m_Classes.size())
    {
    m_Classes[classIndex].m_Name = name;
    this->NotifyAll();
    }
}

/** Set class label */
void ObjectLabelingApplicationModel::SetClassLabel(const LabelType & label, unsigned int classIndex)
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
      this->NotifyAll();
      }
    else
      {
      itkExceptionMacro(<<"Label "<<label<<" is already in use.");
      }
    }
}

/** Set class color */
void ObjectLabelingApplicationModel::SetClassColor(const ColorType & color, unsigned int classIndex)
{
  if(classIndex < m_Classes.size())
    {
    m_Classes[classIndex].m_Color = color;
    this->NotifyAll();
    }  
}

/** Select sample */
void ObjectLabelingApplicationModel::SelectSample(const IndexType & index)
{
  // Check for a valid label image
  if(!m_LabeledImage)
    {
    return;
    }
  
  LabelType label = m_LabeledImage->GetPixel(index);

  this->SelectSample(label);
}
void ObjectLabelingApplicationModel::SelectSample(const LabelType & label)
{
  m_SelectedLabel = label;

  // Add the polygon to the VectorData
  SimplifyPolygonFunctorType sfunctor;
  PolygonType::Pointer polygon = /**sfunctor(*/m_LabelMap->GetLabelObject(label)->GetPolygon()/**)*/;
  m_SelectedPolygonNode->SetPolygonExteriorRing(polygon);

  // Erase the previous neighboring polygons
  TreeNodeType * node = const_cast<TreeNodeType *> (m_NeighborsPolygon->GetDataTree()->GetNode(m_NeighborsFolder));
  unsigned int nbChildren = node->CountChildren();
  for(unsigned int i = 0; i<nbChildren;++i)
    {
    node->Remove(node->GetChild(nbChildren-i-1));
    }

  try
    {
    LabelMapType::AdjacentLabelsContainerType neighbors = m_LabelMap->GetAdjacentLabels(m_SelectedLabel);
    
    LabelMapType::AdjacentLabelsContainerType::const_iterator nit;
  
    std::cout<<"Add the "<<neighbors.size() <<" neighboring polygons ..."<<std::endl;
    
    // For each neighbor
    for(nit = neighbors.begin();nit!= neighbors.end();++nit)
      {
      // Add the polygon to the VectorData
      DataNodeType::Pointer polygonNode = DataNodeType::New();
      PolygonType::Pointer polygon = /**sfunctor(*/m_LabelMap->GetLabelObject(*nit)->GetPolygon()/**)*/;
      polygonNode->SetPolygonExteriorRing(polygon);
      m_NeighborsPolygon->GetDataTree()->Add(polygonNode,m_NeighborsFolder);
      }
    std::cout<<"Done."<<std::endl;
    }
  catch(itk::ExceptionObject & err)
    {
    std::cout<<"Neighbors not found for label "<<m_SelectedLabel<<std::endl;
    }
  this->NotifyAll();
}

/** Is the given sample selected */
bool ObjectLabelingApplicationModel::IsSampleSelected(const IndexType & index)
{
// Check for a valid label image
  if(!m_LabeledImage)
    {
    return false;
    }
  
  LabelType label = m_LabeledImage->GetPixel(index);
  
  return this->IsSampleSelected(label);

}
bool ObjectLabelingApplicationModel::IsSampleSelected(const LabelType & label)
{
  return (m_SelectedLabel == label);
}

bool ObjectLabelingApplicationModel::IsSampleSelected()
{
  return (m_SelectedLabel != itk::NumericTraits<LabelType>::max());
}

void ObjectLabelingApplicationModel::ClearSelectedSample()
{
  m_SelectedLabel = itk::NumericTraits<LabelType>::max();
  m_SelectedPolygonNode->SetPolygonExteriorRing(PolygonType::New());
  this->NotifyAll();
}

void ObjectLabelingApplicationModel::IndexClicked(const IndexType & index)
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

    this->NotifyAll();
    }
}

void ObjectLabelingApplicationModel::SelectClass(unsigned int classIndex)
{
  if(classIndex < m_Classes.size())
    {
    m_SelectedClass = classIndex;
    m_HasSelectedClass = true;
    this->NotifyAll();
    }
  else
    {
    this->ClearSelectedClass();
    }
}

void ObjectLabelingApplicationModel::ClearSelectedClass()
{
  m_SelectedClass = 0;
  m_HasSelectedClass = false;
  this->NotifyAll();
}

bool ObjectLabelingApplicationModel::HasSelectedClass()
{
  return m_HasSelectedClass;
}

unsigned int ObjectLabelingApplicationModel::GetSelectedClass()
{
  if(!m_HasSelectedClass)
    {
    itkExceptionMacro(<<"No selected class.");
    }
  return m_SelectedClass;
}

void ObjectLabelingApplicationModel::SaveSamplesToXMLFile(const char * fname)
{
  TiXmlDocument doc;

  TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
  doc.LinkEndChild( decl );

  TiXmlElement * root = new TiXmlElement( "ObjectLabelingApplicationClasses" );
  doc.LinkEndChild( root );

  TiXmlElement * sources = new TiXmlElement( "Sources" );
  root->LinkEndChild( sources );

  TiXmlElement* srcImage = new TiXmlElement("SourceImage");
  srcImage->LinkEndChild(new TiXmlText( m_VectorImageFile.c_str() ));
  sources->LinkEndChild(srcImage);

  TiXmlElement* labelImage = new TiXmlElement("LabeledImage");
  labelImage->LinkEndChild(new TiXmlText( m_LabeledImageFile.c_str() ));
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


void ObjectLabelingApplicationModel::SaveClassificationParametersToXMLFile(const char * fname)
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

  TiXmlElement * root = new TiXmlElement( "ObjectLabelingApplicationClassificationParameters" );
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
  
  // If context description
  if(m_UseContext)
    {
    // Export centroids as well
    TiXmlElement * centroids = new TiXmlElement( "ContextCentroids" );
    root->LinkEndChild( centroids );
    
    // For each centroid
    for(unsigned int cindex = 0; cindex<m_CentroidsVector.size();++cindex)
      {
      TiXmlElement * centroid = new TiXmlElement( "Centroid" );
      centroids->LinkEndChild( centroid );

      // For each value in centroid
      unsigned int findex = 0;
      
      for(fit = m_AvailableFeatures.begin(); fit!=m_AvailableFeatures.end();++fit)
	{
	// If it is selected
	if(fit->second)
	  {
	  TiXmlElement * position = new TiXmlElement( "CentroidFeature" );
	  centroid->LinkEndChild( position );

	  // Name of the feature
	  TiXmlElement * name = new TiXmlElement( "Name" );
	  name->LinkEndChild(new TiXmlText(fit->first.c_str()));
	  position->LinkEndChild(name);

	  // Value of the feature
	  TiXmlElement * value = new TiXmlElement( "Value" );
	  oss.str("");
	  oss<<m_CentroidsVector[cindex][findex];
	  value->LinkEndChild(new TiXmlText(oss.str().c_str()));
	  position->LinkEndChild(value);
	  ++findex;
	  }
	}
      }
    }

  // Finally, write the file
  doc.SaveFile( fname );
}

void ObjectLabelingApplicationModel::LoadSamplesFromXMLFile(const char * fname)
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
  TiXmlHandle classes = hDoc.FirstChildElement("ObjectLabelingApplicationClasses").FirstChildElement("Classes");

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

    this->NotifyAll();
}

void ObjectLabelingApplicationModel::SaveColorsToAsciiFile(const char * fname)
{
  std::ofstream ofs;
  ofs.open(fname);

  if(!ofs)
    {
    itkExceptionMacro(<<"Can not open file "<<fname);
    }

  for(ObjectClassVectorType::const_iterator oit = m_Classes.begin(); oit != m_Classes.end();++oit)
    {
    ofs<<"# "<<oit->m_Name<<std::endl;

    ofs<<oit->m_Label;
    ofs<<" "<<static_cast<unsigned int>(oit->m_Color[0]*255);
    ofs<<" "<<static_cast<unsigned int>(oit->m_Color[1]*255);
    ofs<<" "<<static_cast<unsigned int>(oit->m_Color[2]*255);
    ofs<<std::endl;
    }
  ofs.close();
}

void ObjectLabelingApplicationModel::SaveClassificationGraph(const char * fname)
{
  std::ofstream ofs;
  ofs.open(fname);

  if(!ofs)
    {
    itkExceptionMacro(<<"Can not open file "<<fname);
    }

  LabelMapType::LabelObjectContainerType::const_iterator lit;
  LabelMapType::LabelObjectContainerType::const_iterator beg = m_LabelMap->GetLabelObjectContainer().begin();
  LabelMapType::LabelObjectContainerType::const_iterator end = m_LabelMap->GetLabelObjectContainer().end();

  // Store all the vertices
  // SUBDUE format imposes :
  // - increasing vertices IDs
  // - IDs begin with 1 (our LabelMap are 0-based)
  std::map<LabelType,LabelType> labelCorrespondance;
  LabelType targetLabel = 1;
  for( lit = beg; lit != end; ++lit )
    {
    LabelType l = lit->second->GetLabel();
    labelCorrespondance[l] = targetLabel++;
    }

  for( lit = beg; lit != end; ++lit )
    {
    LabelType label = lit->first;
    LabelType classLabel = lit->second->GetClassLabel();
    const ObjectClassType& objectClass = GetClass( GetClassIndex(classLabel) );
    ofs << "v " << labelCorrespondance[label] << " \"" << objectClass.m_Name << "\"" << std::endl;
    }

  // Store the edges : one undirected edge for each neighboring relation between 2 LabelObject
  for( lit = beg; lit != end; ++lit )
    {
    LabelType label = lit->first;

    try
      {
      LabelMapType::AdjacentLabelsContainerType neighbors = m_LabelMap->GetAdjacentLabels(label);
      LabelMapType::AdjacentLabelsContainerType::const_iterator nit;
      for(nit = neighbors.begin(); nit!= neighbors.end(); ++nit)
        {
        if (*nit < label)
          {
          // "u" declares an undirected edge
          // "-" is an arbitrary edge label (subdue format needs it)
          ofs << "u " << labelCorrespondance[label] << " " << labelCorrespondance[*nit] << " -" << std::endl;
          }
        }
      }
    catch(itk::ExceptionObject & err)
      {
      std::cout << "Neighbors not found for label " << label << std::endl;
      }
    }
  ofs.close();
}

void ObjectLabelingApplicationModel::ExportClassificationToGIS(const GISExportInfo& exportInfo)
{
  typedef otb::PostGISConnectionImplementation BdConnection;
  typedef otb::PostGISConnectionImplementation::Pointer BdConnectionPointer;

  BdConnectionPointer connection = BdConnection::New();
  PostGISTableWithAttributesAndClass::Pointer gisTable = PostGISTableWithAttributesAndClass::New();

  // Create the DB connection
  connection->SetHost     (exportInfo.host);
  connection->SetPort     (exportInfo.port);
  connection->SetDBName   (exportInfo.db);
  connection->SetUser     (exportInfo.user);
  connection->SetPassword (exportInfo.passwd);

  connection->ConnectToDB();
  gisTable->SetConnection(connection);
  gisTable->SetTableName(exportInfo.table_name);


  AvailableFeaturesMapType::const_iterator fit;
  std::vector<std::string> attributes;
  for(fit = m_AvailableFeatures.begin(); fit != m_AvailableFeatures.end(); ++fit)
    {
    if(fit->second)
      {
      attributes.push_back(fit->first);
      }
    }
  gisTable->SetAttributes(attributes);

  // Create the table in db
  gisTable->CreateTable( true );

  gisTable->InsertLabelMap( *m_LabelMap );

}



void ObjectLabelingApplicationModel::ComputeFeaturesStatistics()
{
  std::cout<<"Computing statistics ..."<<std::endl;
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
  std::cout<<"Done."<<std::endl;
}

void ObjectLabelingApplicationModel::SaveClassification(const char * fname)
{

  Classify();
  typedef otb::ImageFileWriter<VectorImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(fname);
  writer->SetInput(m_ColorMapper->GetOutput());
  writer->Update();
  
}

void ObjectLabelingApplicationModel::EstimateCentroids()
{
  std::cout<<"Computing Kmeans centroids ..." <<std::endl;
  std::cout<<"Number of classes: "<<m_Classes.size()<<std::endl;
  unsigned int numberOfCentroids = m_Classes.size();

  if(numberOfCentroids <= 1)
    {
    std::cout<<"ONE CLASS svm mode, forcing number of centroids to 10"<<std::endl;
    numberOfCentroids = 10;
    }
  std::cout<<"Number of centroids: "<<numberOfCentroids<<std::endl;


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

  srand(time(0));
  if(m_Classes.size() > 1)
    {
    for(unsigned int cId = 0; cId<m_Classes.size();++cId)
      {
      // Identify one centroid and get the sample
      unsigned int centroidId = static_cast<unsigned int>(static_cast<double>(m_Classes[cId].m_Samples.size())*rand()/RAND_MAX);
      LabelType centroidLabel = m_Classes[cId].m_Samples[centroidId];
      VectorType centroidSample = this->BuildSample(m_LabelMap->GetLabelObject(centroidLabel));
      // Fill the initial means
      for(unsigned int idx = 0; idx < sampleSize; ++idx)
	{
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
      LabelType centroidLabel = m_LabelMap->GetNthLabelObject(centroidId)->GetLabel();
      VectorType centroidSample = this->BuildSample(m_LabelMap->GetLabelObject(centroidLabel));
      // Fill the initial means
      for(unsigned int idx = 0; idx < sampleSize; ++idx)
	{
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
      newCenter[i]=estimator->GetParameters()[cId * sampleSize + i];
      }
    std::cout<<"New centroid: "<<newCenter<<std::endl;
    m_CentroidsVector.push_back(newCenter);
    }
  
  std::cout<<"Done."<<std::endl;
}

void ObjectLabelingApplicationModel::BuildSampleList()
{
  std::cout<<"Building samples list ..."<<std::endl;

  // Clear previous samples
  m_ListSample->Clear();

  LabelMapType::LabelObjectContainerType::const_iterator it = m_LabelMap->GetLabelObjectContainer().begin();

  // Iterate on label objects
  while(it!=m_LabelMap->GetLabelObjectContainer().end())
    {
    VectorType newSample = this->BuildSample(it->second);
    
    if(m_UseContext)
      {
      this->FillContextDescription(newSample,it->first);
      }
    // Add the new sample
    m_ListSample->PushBack(newSample);

    ++it;
    }
  std::cout<<"Done."<<std::endl;
}

void ObjectLabelingApplicationModel::BuildTrainingSampleList()
{
  std::cout<<"Building training samples list ..."<<std::endl;
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
	
     
        // Eventually add context description
	if(m_UseContext)
	  {
	  this->FillContextDescription(newSample,*lit);
	  }

	// Add the new samples
	m_TrainingListSample->PushBack(newSample);
	m_LabelsListSample->PushBack(label);
	}  
      }
    }
  std::cout<<"Done."<<std::endl;
}

void ObjectLabelingApplicationModel::ClearMarginSamples()
{
  std::cout<<"Erase any previous margin sampled polygons ..."<<std::endl;

  // Clear margin samples
  m_MarginSamples.clear();

  // Erase the previous margin sampled polygons
  TreeNodeType * node = const_cast<TreeNodeType *> (m_MarginSampledPolygon->GetDataTree()->GetNode(m_MarginSampledFolder));
  unsigned int nbChildren = node->CountChildren();
  for(unsigned int i = 0; i<nbChildren;++i)
    {
    node->Remove(node->GetChild(nbChildren-i-1));
    }
  this->NotifyAll();
  std::cout<<"Done."<<std::endl;
}


void ObjectLabelingApplicationModel::SampleMargin()
{
 
  this->ClearMarginSamples();

  // Build the sample lists
  if(m_UseContext)
    {
    this->EstimateCentroids();
    }

  this->BuildSampleList();
  this->BuildTrainingSampleList();

  std::cout<<"Estimating model ..."<<std::endl;
  // Model estimation
  m_SVMEstimator->SetInputSampleList(m_TrainingListSample);
  m_SVMEstimator->SetTrainingSampleList(m_LabelsListSample);
  m_SVMEstimator->Modified();
  std::cout<<"Number of classes: "<<m_Classes.size()<<std::endl;
  m_SVMEstimator->SetNumberOfClasses(m_Classes.size());

  if(m_Classes.size() == 1)
    {
    std::cout<<"ONE_CLASS svm mode."<<std::endl;
    m_SVMEstimator->SetSVMType(ONE_CLASS);
    }


  m_SVMEstimator->Update();
  
  m_Accuracy = m_SVMEstimator->GetFinalCrossValidationAccuracy();

  std::cout<<"Done."<<std::endl;

  std::cout<<"Sampling margin ..."<<std::endl;

  // Margin sampling
  // m_MarginSampler = MarginSamplerType::New();
  m_MarginSampler->SetSample(m_ListSample);
  m_MarginSampler->SetModel(m_SVMEstimator->GetModel());
  m_MarginSampler->Update();
  
  std::cout<<"Done."<<std::endl;

  std::cout<<"Add the margin sampled polygons to the displayed vector layers ..."<<std::endl;

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

  std::cout<<"Done."<<std::endl;

  this->NotifyAll();
}

void ObjectLabelingApplicationModel::Classify()
{
  // Build the sample lists
  if(m_UseContext)
    {
    this->EstimateCentroids();
    }

  this->BuildSampleList();
  this->BuildTrainingSampleList();

  // Model estimation
  std::cout<<"Estimating model ..."<<std::endl;
  m_SVMEstimator->SetInputSampleList(m_TrainingListSample);
  m_SVMEstimator->SetTrainingSampleList(m_LabelsListSample);
  m_SVMEstimator->Modified();
  std::cout<<"Number of classes: "<<m_Classes.size()<<std::endl;
  m_SVMEstimator->SetNumberOfClasses(m_Classes.size());

   if(m_Classes.size() == 1)
     {
     std::cout<<"ONE_CLASS svm mode."<<std::endl;
     m_SVMEstimator->SetSVMType(ONE_CLASS);
     }

  m_SVMEstimator->Update();

  m_Accuracy = m_SVMEstimator->GetFinalCrossValidationAccuracy();


  m_SVMEstimator->GetModel()->SaveModel("model.svm");

  // SVM Classification
  std::cout<<"Classifying ..."<<std::endl;
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
  std::cout<<"Done."<<std::endl;

   // Class labeling the label object
   std::cout<<"Class labeling the objects ..."<<std::endl;
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
   std::cout<<"Done."<<std::endl;
   
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
      std::cout<<"New value: "<<newValue<<std::endl;
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
     std::cout<<"New value: "<<oit->m_Label<<" "<<newValue<<std::endl;
     m_ColorMapper->SetChange(oit->m_Label,newValue);
     }

   m_ColorMapper->Update();
   std::cout<<"Color Mapper update."<<std::endl;
   m_VisualizationModel->GetLayerByName("Classification")->SetVisible(true);
   std::cout<<"Get layer by name."<<std::endl;

   m_VisualizationModel->Update();
   std::cout<<"Refresh done"<<std::endl;
}

void ObjectLabelingApplicationModel::ClearClassification()
{
   m_VisualizationModel->GetLayerByName("Classification")->SetVisible(false);
   m_VisualizationModel->Update();
}

void ObjectLabelingApplicationModel::ChangeFeatureState(const std::string& fname, bool state)
{
//   std::cout<<"Trying "<<fname<<std::endl;
  if(m_AvailableFeatures.count(fname)>0)
    {
//     std::cout<<"Changing state of feature "<<fname<<" to "<<state<<std::endl;
    m_AvailableFeatures[fname]=state;
    }
  else
    {
    itkExceptionMacro(<<"Feature "<<fname<<" could not be found in the available features.");
    }
}

void ObjectLabelingApplicationModel::FillContextDescription(VectorType & vect, const LabelType & label)
{
  DistanceType::Pointer distCalculator = DistanceType::New();
  unsigned int sampleSize = vect.Size() + m_CentroidsVector.size();
  unsigned int contextStartIndex = vect.Size();


  // Resize the vector to handle context
  vect.SetSize(sampleSize,false);

  // Initialise
  for(unsigned int i = 0; i<m_CentroidsVector.size();++i)
    {
    vect[contextStartIndex+i] = 0.;
    }

  // Get the adjacent labels map
  AdjacentLabelsContainerType adjMap = m_LabelMap->GetAdjacentLabels(label);
  
  // Neighborhood frequency
  double frequency = 1/static_cast<double>(adjMap.size());

  // Iterate on adjacent labels
  for(AdjacentLabelsContainerType::const_iterator lit = adjMap.begin();lit!=adjMap.end();++lit)
    {
    // Build the sample
    VectorType newSample = this->BuildSample(m_LabelMap->GetLabelObject(*lit));

    double minDist = itk::NumericTraits<double>::max();
    int minCId = -1;

    for(unsigned int cId = 0; cId < m_CentroidsVector.size();++cId)
      {
      double dist = distCalculator->Evaluate(newSample,m_CentroidsVector[cId]);
      if(dist < minDist)
	{
	minDist = dist;
	minCId = cId;
	}
      }
    // TODO: Check for sigma here
    vect[contextStartIndex+minCId]+=frequency;
    }
}

void ObjectLabelingApplicationModel::ChangeClassificationOpacity(double opacity)
{
  BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
  blender->SetAlpha(opacity);
  m_VisualizationModel->GetLayerByName("Classification")->SetBlendingFunction(blender);
  m_VisualizationModel->Update();
}

void ObjectLabelingApplicationModel::UpdateViewerDisplay(std::vector<unsigned int> ch)
{
  if(m_VisualizationModel->GetNumberOfLayers() != 2 )
    {
      itkExceptionMacro("Invalid number of layers");
    }

  m_Channels = ch; 
  m_ImageLayerRenderingFunction->SetChannelList(m_Channels);    
  m_VisualizationModel->Update();
}


ObjectLabelingApplicationModel::VectorType ObjectLabelingApplicationModel::BuildSample(const LabelObjectType * lo) const
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
      newSample[index] =(lo->GetAttribute(fit->first.c_str())-min)/(max-min); 
      ++index;
      }
    }

  //std::cout<<"NewSample: "<<newSample<<std::endl;

  // Return the sample
  return newSample;
}
}
#endif
