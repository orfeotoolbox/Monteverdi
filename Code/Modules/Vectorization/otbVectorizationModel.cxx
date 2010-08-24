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
/** Initialize the singleton */
VectorizationModel::Pointer VectorizationModel::Instance = NULL;

/** Manage the singleton */
VectorizationModel::Pointer
VectorizationModel::
GetInstance()
{
  if (!Instance)
    {
    Instance = VectorizationModel::New();
    }
  return Instance;
}

void VectorizationModel::
Notify(ListenerBase * listener)
{
  listener->Notify();
}

VectorizationModel::
VectorizationModel() : m_VisualizationModel(),
  m_ImageGenerator(),
  m_InputImage(),
  m_VectorDataModel(),
  m_Output()
{
  // Visualization
  m_VisualizationModel  = VisualizationModelType::New();
  m_ImageGenerator      = LayerGeneratorType::New();

  // Input & Output
  m_InputImage = VectorImageType::New();

  // VectorData model
  m_VectorDataModel = VectorDataModelType::New();
  m_VectorDataModel->RegisterListener(this);
  m_OutputChanged = false;
}

VectorizationModel
::~VectorizationModel()
{}

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
  //if (!m_DEMDirectory.empty()) vdextract->SetDEMDirectory(m_DEMDirectory);

  // Reproject VectorData in image projection
  vproj = VectorDataProjectionFilterType::New();
  vproj->SetInput(vdextract->GetOutput());
  vproj->SetInputProjectionRef(vData->GetProjectionRef());
  vproj->SetOutputKeywordList(m_InputImage->GetImageKeywordlist());
  vproj->SetOutputProjectionRef(m_InputImage->GetProjectionRef());
  vproj->SetOutputOrigin(m_InputImage->GetOrigin());
  vproj->SetOutputSpacing(m_InputImage->GetSpacing());
  //if (!m_DEMDirectory.empty()) vproj->SetDEMDirectory(m_DEMDirectory);
  vproj->Update();

  std::cout<<"projected"<<std::endl;

  /*
  ProjectionFilterType::Pointer vectorDataProjection = ProjectionFilterType::New();
  vectorDataProjection->SetInput(vData);

  PointType lNewOrigin;
  // polygons are recorded with a 0.5 shift...
  lNewOrigin[0] = m_InputImage->GetOrigin()[0]+0.5;
  lNewOrigin[1] = m_InputImage->GetOrigin()[1]+0.5;

  vectorDataProjection->SetOutputOrigin(lNewOrigin);
  vectorDataProjection->SetOutputSpacing(m_InputImage->GetSpacing());

  std::string projectionRef;
  itk::ExposeMetaData<std::string>(m_InputImage->GetMetaDataDictionary(),
                                   otb::MetaDataKey::ProjectionRefKey, projectionRef );
  vectorDataProjection->SetOutputProjectionRef(projectionRef);
  vectorDataProjection->SetOutputKeywordList(m_InputImage->GetImageKeywordlist());
  vectorDataProjection->Update();
  */
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
  //VectorDataPointerType vData = m_VectorDataModel->GetVectorData();
  
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
  vectorDataProjection->Update();
  m_Output = vectorDataProjection->GetOutput();
  
  
  /*
  m_Output =  m_VectorDataModel->GetVectorData();
  m_Output->SetProjectionRef(m_InputImage->GetProjectionRef());
  m_Output->SetMetaDataDictionary(m_InputImage->GetMetaDataDictionary());
  */
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

} // namespace otb
