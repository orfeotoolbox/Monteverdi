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
#include "otbVectorizationModel.h"
#include "itkPreOrderTreeIterator.h"


namespace otb
{

VectorizationModel::
VectorizationModel(): m_DEMPath(""),m_UseDEM(false),
                      m_ExtractRegionUpdated(false),
                      m_ActualLayerNumber(0)
{
  // Visualization
  m_VisualizationModel  = VisualizationModelType::New();
  m_ImageGenerator      = LayerGeneratorType::New();

  // Input & Output
  m_InputImage = VectorImageType::New();

  // VectorData model
  m_VectorDataModel = VectorDataModelType::New();
  m_VectorDataModel->RegisterListener(this);
  
  // Output changed flag
  m_OutputChanged = false;
  
  // Extract Filter 
  m_ExtractImageFilter = ExtractImageFilterType::New();
  
  // Extract region flag
  m_ExtractRegionUpdated = false;

  // Selected Polygon on right click in automatic mode 
  m_SelectedPolygon     = PolygonType::New();
  m_SelectedPolygonNode = DataNodeType::New();

  // Build the automatic vectordata vector for each polygon selected
  m_SelectedVectorDataType       = VectorDataType::New();
  DataNodeType::Pointer root     = m_SelectedVectorDataType->GetDataTree()->GetRoot()->Get();
  DataNodeType::Pointer document = DataNodeType::New();
  DataNodeType::Pointer folder   = DataNodeType::New();
  
  document->SetNodeType(otb::DOCUMENT);
  folder->SetNodeType(otb::FOLDER);

  m_SelectedVectorDataType->GetDataTree()->Add(document,root);
  m_SelectedVectorDataType->GetDataTree()->Add(folder,document);
  m_SelectedVectorDataType->GetDataTree()->Add(m_SelectedPolygonNode,folder);
}

VectorizationModel
::~VectorizationModel()
{}

void VectorizationModel::
Notify(ListenerBase * listener)
{
  listener->Notify();
}

void
VectorizationModel
::SetImage(VectorImagePointerType image)
{
  image->UpdateOutputInformation();
  m_InputImage = image;

  // Generate the layer
  m_ImageGenerator->SetImage(image);
  m_ImageGenerator->GenerateQuicklookOn();
  m_ImageGenerator->GenerateLayer();

  std::vector<unsigned int> channels;
  if (image->GetNumberOfComponentsPerPixel() == 3)
    {
    channels.push_back(0);
    channels.push_back(1);
    channels.push_back(2);
    }
  else if (image->GetNumberOfComponentsPerPixel() > 3)
    {
    channels.push_back(2);
    channels.push_back(1);
    channels.push_back(0);
    }
  if (channels.size() == 3) m_ImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

  m_ImageGenerator->GetLayer()->SetName("InputImage");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());
  m_VisualizationModel->Update();

//  m_VectorDataModel->SetOrigin(m_InputImage->GetOrigin());
//  m_VectorDataModel->SetSpacing(m_InputImage->GetSpacing());
}

void VectorizationModel
::AddVectorData(VectorDataPointerType vData)
{
  if(m_InputImage.IsNull())
    {
      itkExceptionMacro("Invalid input image.");
    }
  
  // Vector data reprojection
  VectorDataProjectionFilterType::Pointer vproj;
  VectorDataExtractROIType::Pointer       vdextract;

  // Extract The part of the VectorData that actually overlaps with
  // the image extent
  vdextract = VectorDataExtractROIType::New();
  vdextract->SetInput(vData);

  // Find the geographic region of interest

  // Ge the index of the corner of the image
  IndexType ul, ur, ll, lr;
  PointType pul, pur, pll, plr;
  ul = m_InputImage->GetLargestPossibleRegion().GetIndex();
  ur = ul;
  ll = ul;
  lr = ul;
  ur[0] += m_InputImage->GetLargestPossibleRegion().GetSize()[0];
  lr[0] += m_InputImage->GetLargestPossibleRegion().GetSize()[0];
  lr[1] += m_InputImage->GetLargestPossibleRegion().GetSize()[1];
  ll[1] += m_InputImage->GetLargestPossibleRegion().GetSize()[1];

  // Transform to physical point
  m_InputImage->TransformIndexToPhysicalPoint(ul, pul);
  m_InputImage->TransformIndexToPhysicalPoint(ur, pur);
  m_InputImage->TransformIndexToPhysicalPoint(ll, pll);
  m_InputImage->TransformIndexToPhysicalPoint(lr, plr);

  // Build the cartographic region
  RemoteSensingRegionType            rsRegion;
  RemoteSensingRegionType::IndexType rsOrigin;
  RemoteSensingRegionType::SizeType  rsSize;
  rsOrigin[0] = min(pul[0], plr[0]);
  rsOrigin[1] = min(pul[1], plr[1]);
  rsSize[0] = vcl_abs(pul[0] - plr[0]);
  rsSize[1] = vcl_abs(pul[1] - plr[1]);

  rsRegion.SetOrigin(rsOrigin);
  rsRegion.SetSize(rsSize);
  rsRegion.SetRegionProjection(m_InputImage->GetProjectionRef());
  rsRegion.SetKeywordList(m_InputImage->GetImageKeywordlist());

  // Set the cartographic region to the extract roi filter
  vdextract->SetRegion(rsRegion);
  if(m_UseDEM==true)
    {    
      if (!m_DEMPath.empty()) 
	{
	  vdextract->SetDEMDirectory(m_DEMPath);
	}
      else
	{
	  itkExceptionMacro("Invalid DEM directory: "<<m_DEMPath<<".");
	}
    }
  // Reproject VectorData in image projection
  vproj = VectorDataProjectionFilterType::New();
  vproj->SetInput(vdextract->GetOutput());
  vproj->SetInputProjectionRef(vData->GetProjectionRef());
  vproj->SetOutputKeywordList(m_InputImage->GetImageKeywordlist());
  vproj->SetOutputProjectionRef(m_InputImage->GetProjectionRef());
  vproj->SetOutputOrigin(m_InputImage->GetOrigin());
  vproj->SetOutputSpacing(m_InputImage->GetSpacing());
  if(m_UseDEM==true)
    {    
      if (!m_DEMPath.empty()) 
	{
	  vproj->SetDEMDirectory(m_DEMPath);
	}
      else
	{
	  itkExceptionMacro("Invalid DEM directory: "<<m_DEMPath<<".");
	}
    }

  vproj->Update();


  //m_VectorDataModel->AddVectorData(tihs->ReprojectedVectorData(vData, false));
  m_VectorDataModel->AddVectorData(vproj->GetOutput());
}


void VectorizationModel
::RemoveDataNode(DataNodeType * node)
{
  // Look-up node in the graph
  itk::PreOrderTreeIterator<VectorDataType::DataTreeType>
  it(m_VectorDataModel->GetVectorData()->GetDataTree());
  it.GoToBegin();

  while (!it.IsAtEnd() && it.Get() != node)
    {
    ++it;
    }
  // If node is found, remove it
  if (!it.IsAtEnd())
    {
    it.Remove();
     this->NotifyAll();
    }
}

void VectorizationModel
::SetDataNodeFieldAsInt(DataNodeType * node, const std::string& name, int value)
{
  node->SetFieldAsInt(name, value);
  this->NotifyAll();
}
void VectorizationModel
::SetDataNodeFieldAsFloat(DataNodeType * node, const std::string& name, float value)
{
  this->NotifyAll();
}
void VectorizationModel
::SetDataNodeFieldAsString(DataNodeType* node, const std::string& name, const std::string& value)
{
  std::cout << "Setting field " << name << " with value " << value << " to node " << node << std::endl;
  node->SetFieldAsString(name, value);
  this->NotifyAll();
}
void VectorizationModel
::RemoveFieldFromDataNode(DataNodeType * node, const std::string& name)
{

  // TODO: Not implemented yet, requires new methods in DataNode class.
}
void VectorizationModel
::RemovePointFromDataNode(DataNodeType * node,
                          const unsigned long& index,
                          bool interiorRing,
                          const unsigned int& interiorRingIndex)
{
  switch (node->GetNodeType())
    {
    case FEATURE_POINT:
      {
      // If the geometry is a point, remove it
      this->RemoveDataNode(node);
      break;
      }
    case FEATURE_LINE:
      {
      if (node->GetLine()->GetVertexList()->Size() < 3)
        {
        this->RemoveDataNode(node);
        }
      else
        {
        // Since PolylineParametricPath does not provide read-write access to the vertex list, nor
        // a method to remove a given vertex, we must use a const_cast here.
        DataNodeType::LineType::VertexListType * pointContainer
          = const_cast<DataNodeType::LineType::VertexListType *>(node->GetLine()->GetVertexList());
        pointContainer->DeleteIndex(index);
        }
      break;
      }
    case FEATURE_POLYGON:
      {
      if (!interiorRing)
        {
        if (node->GetPolygonExteriorRing()->GetVertexList()->Size() < 4)
          {
          this->RemoveDataNode(node);
          }
        else
          {
          // Since PolylineParametricPath does not provide read-write access to the vertex list, nor
          // a method to remove a given vertex, we must use a const_cast here.
          DataNodeType::PolygonType::VertexListType * pointContainer
            = const_cast<DataNodeType::PolygonType::VertexListType *>(node->GetPolygonExteriorRing()->GetVertexList());
          pointContainer->DeleteIndex(index);
          }
        }
      else
        {
        if (interiorRingIndex < node->GetPolygonInteriorRings()->Size())
          {
          // Since PolylineParametricPath does not provide read-write access to the vertex list, nor
          // a method to remove a given vertex, we must use a const_cast here.
          DataNodeType::PolygonType::VertexListType * pointContainer
            = const_cast<DataNodeType::PolygonType::VertexListType *>(node->GetPolygonInteriorRings()
                                                                      ->GetNthElement(interiorRingIndex)->GetVertexList());
          pointContainer->DeleteIndex(index);
          }
        }
      }
    default:
      {
      // Not supported yet
      break;
      }
    }
  this->NotifyAll();
}

void VectorizationModel
::UpdatePointFromDataNode(DataNodeType * node,
                          const unsigned long& index,
                          const PointType& point,
                          bool interiorRing,
                          const unsigned int& interiorRingIndex)
{
  // Cast PointType to VertexType
  VertexType vertex;
  vertex[0] = point[0];
  vertex[1] = point[1];

  switch (node->GetNodeType())
    {
    case FEATURE_POINT:
      {
      // If the geometry is a point, remove it
      node->SetPoint(point);
      break;
      }
    case FEATURE_LINE:
      {
      if (index < node->GetLine()->GetVertexList()->Size())
        {
        // Since PolylineParametricPath does not provide read-write access to the vertex list, nor
        // a method to set a given vertex, we must use a const_cast here.
        DataNodeType::LineType::VertexListType * pointContainer
          = const_cast<DataNodeType::LineType::VertexListType *>(node->GetLine()->GetVertexList());
        pointContainer->SetElement(index, vertex);
        }
      break;
      }
    case FEATURE_POLYGON:
      {
      if (!interiorRing)
        {
        if (index < node->GetPolygonExteriorRing()->GetVertexList()->Size())
          {
          // Since PolylineParametricPath does not provide read-write access to the vertex list, nor
          // a method to set a given vertex, we must use a const_cast here.
          DataNodeType::PolygonType::VertexListType * pointContainer
            = const_cast<DataNodeType::PolygonType::VertexListType *>(node->GetPolygonExteriorRing()->GetVertexList());
          pointContainer->SetElement(index, vertex);
          }
        }
      else
        {
        if (interiorRingIndex < node->GetPolygonInteriorRings()->Size()
            && index < node->GetPolygonInteriorRings()->GetNthElement(interiorRingIndex)->GetVertexList()->Size())
          {
          // Since PolylineParametricPath does not provide read-write access to the vertex list, nor
          // a method to set a given vertex, we must use a const_cast here.
          DataNodeType::PolygonType::VertexListType * pointContainer
            = const_cast<DataNodeType::PolygonType::VertexListType *>(node->GetPolygonInteriorRings()
                                                                      ->GetNthElement(interiorRingIndex)->GetVertexList());
          pointContainer->SetElement(index, vertex);
          }
        }
      }
    default:
      {
      // Not supported yet
      break;
      }
    }
  this->NotifyAll();
}


void
VectorizationModel
::OK()
{
  typedef otb::VectorDataProjectionFilter<VectorDataType,VectorDataType> ProjectionFilterType;
  ProjectionFilterType::Pointer vectorDataProjection = ProjectionFilterType::New();
  vectorDataProjection->SetInput(m_VectorDataModel->GetVectorData());

  PointType lNewOrigin;
  // polygons are recorded with a 0.5 shift...
  lNewOrigin[0] = m_InputImage->GetOrigin()[0]+0.5;
  lNewOrigin[1] = m_InputImage->GetOrigin()[1]+0.5;

  vectorDataProjection->SetInputOrigin(lNewOrigin);
  vectorDataProjection->SetInputSpacing(m_InputImage->GetSpacing());

  std::string projectionRef;
  itk::ExposeMetaData<std::string>(m_InputImage->GetMetaDataDictionary(),
                                   MetaDataKey::ProjectionRefKey, projectionRef );
  vectorDataProjection->SetInputProjectionRef(projectionRef);
  vectorDataProjection->SetInputKeywordList(m_InputImage->GetImageKeywordlist());
//   if(m_UseDEM==true)
//     {    
//       if (!m_DEMPath.empty()) 
// 	{
// 	  vectorDataProjection->SetDEMDirectory(m_DEMDirectory);
// 	}
//       else
// 	{
// 	  itkExceptionMacro("Invalid DEM directory: "<<m_DEMPath<<".");
// 	}
//     }

  vectorDataProjection->Update();
  m_Output = vectorDataProjection->GetOutput();
  
  m_OutputChanged = true;
  this->NotifyAll();
  m_OutputChanged = false;
}


void
VectorizationModel
::Notify()
{
  this->NotifyAll();
}

void
VectorizationModel
::FocusOnDataNode(const PointType& point)
{
  IndexType index;
  index[0] = static_cast<IndexType::IndexValueType>(point[0]);
  index[1] = static_cast<IndexType::IndexValueType>(point[1]);

  // Center the view around the defined index 
  m_VisualizationModel->SetScaledExtractRegionCenter(index);
  m_VisualizationModel->SetExtractRegionCenter(index);
  m_VisualizationModel->Update();
}

void VectorizationModel
::ExtractRegionOfImage(RegionType ExtRegion)
{
  if (m_LastRegionSelected == ExtRegion)
    {
    m_ExtractRegionUpdated = false;
    std::cout<<"Same Region"<<std::endl;
    }
  else
    {
    m_ExtractImageFilter->SetInput(m_InputImage);
    m_ExtractImageFilter->SetExtractionRegion(ExtRegion);
    m_LastRegionSelected = ExtRegion;
    m_ExtractRegionUpdated = true;
    std::cout<<"Different Region"<<std::endl;
    }
  std::cout<<"Model:ExtractRegionOfImage  -----> ok"<<std::endl;
}

/** 
 * Add the polygon to the tree browser.
 * 
 */
void VectorizationModel
::RightIndexClicked(const IndexType & index, RegionType ExtRegion)
{
  DataNodeType::Pointer PolygonNode = DataNodeType::New();
  PolygonNode->SetNodeType(otb::FEATURE_POLYGON);
  PolygonNode->SetNodeId("FEATURE_POLYGON");
  PolygonNode->SetPolygonExteriorRing( m_SelectedPolygon);
  m_VectorDataModel->GetVectorData()->GetDataTree()->Add(PolygonNode,
                                                         m_VectorDataModel->GetVectorData()->GetDataTree()->GetRoot()->Get());
  this->NotifyAll();
  m_ActualLayerNumber = 0;
}

/**
 * show the different polygon that the point belongs to.
 */
void VectorizationModel
::LeftIndexClicked(const IndexType & index, 
                   RegionType ExtRegion)
{
  if(m_LastRegionSelected == ExtRegion)
    {
    LabelObject2PolygonFunctorType Functor;
    LabelType label = m_LabelImageVector[m_ActualLayerNumber]->GetPixel(index);
    m_SelectedPolygon = Functor(m_LabelMapVector[m_ActualLayerNumber]->GetLabelObject(label));
    m_SelectedPolygonNode->SetPolygonExteriorRing(m_SelectedPolygon);

    std::cout <<"The layer selected is "<<m_ActualLayerNumber   << std::endl;
    
    if(m_ActualLayerNumber == m_LabelImageVector.size()-1)
      m_ActualLayerNumber = 0;
    else
      m_ActualLayerNumber++;
    }

  this->NotifyAll();
}

void
VectorizationModel::GenerateLayers()
{
  if (m_ExtractRegionUpdated)
    {
    //this->DeleteLayers();
    m_LabelImageVector.push_back(GenerateMeanshiftClustering(10,30,100));
    m_LabelImageVector.push_back(GenerateMeanshiftClustering(3,50,150));
    m_LabelImageVector.push_back(GenerateMeanshiftClustering(2,5,10));
    for(unsigned int i=0; i<m_LabelImageVector.size(); i++)
      m_LabelMapVector.push_back(ConvertLabelImageToLabelMap(m_LabelImageVector[i]));
    }
}


VectorizationModel
::LabeledImagePointerType
VectorizationModel
::GenerateMeanshiftClustering(int SpatialRadius, 
                              double RangeRadius, 
                              int MinRegionSize)
{
  MeanShiftVectorImageFilterType::Pointer 
    MSImageFilter = MeanShiftVectorImageFilterType::New();
  
  MSImageFilter->SetInput(m_ExtractImageFilter->GetOutput());
  MSImageFilter->SetSpatialRadius(SpatialRadius);
  MSImageFilter->SetRangeRadius(RangeRadius);
  MSImageFilter->SetMinimumRegionSize(MinRegionSize);
  MSImageFilter->Update();
  std::ostringstream os;
  os <<"MeanShift. Spatial Radius : "<<SpatialRadius<<"; Range Radius : "<<RangeRadius<<"; MinRegionSize : "<<MinRegionSize<<std::endl;
  m_AlgorithmNameList.push_back(os.str());
  std::cout<<"Model: Meanshift clustering: spatial radius = "<<SpatialRadius<<", rangeradius = "<<RangeRadius<<", minimum region size = "<<MinRegionSize<<std::endl;
  return MSImageFilter->GetLabeledClusteredOutput();
}

VectorizationModel::LabelMapPointerType
VectorizationModel
::ConvertLabelImageToLabelMap(LabeledImagePointerType inputImage)
{
  LabelImageToLabelMapFilterType::Pointer  
    LI2LM = LabelImageToLabelMapFilterType::New();
  
  LI2LM->SetBackgroundValue(itk::NumericTraits<LabelType>::max());
  LI2LM->SetInput(inputImage);
  LI2LM->Update();
  std::cout<<"Model: Labelimage to label map ------> OK"<<std::endl;
  return LI2LM->GetOutput();
}

} // namespace otb

