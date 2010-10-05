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
#ifndef __otbVectorizationModel_h
#define __otbVectorizationModel_h

#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbEventsSender.h"
#include "otbTypeManager.h"

#include "otbVectorDataProjectionFilter.h"
#include "otbVectorDataExtractROI.h"

//Visu
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"

// Vectorization
#include "otbVectorDataModel.h"

namespace otb {

/** \class VectorizationModel
 *
 */
class ITK_EXPORT VectorizationModel
  : public  MVCModel<ListenerBase>, public itk::Object, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef VectorizationModel            Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(VectorizationModel, MVCModel);

  /** New macro */
  itkNewMacro(Self);

  typedef TypeManager
  ::Floating_Point_VectorImage VectorImageType;
  typedef VectorImageType::Pointer   VectorImagePointerType;
  typedef VectorImageType::IndexType IndexType;
  typedef VectorImageType::SizeType  SizeType;
  typedef VectorImageType::PointType ImagePointType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char> RGBPixelType;
  typedef Image<RGBPixelType, 2>       RGBImageType;
  typedef ImageLayer<VectorImageType,
      RGBImageType>            LayerType;
  typedef ImageLayerGenerator<LayerType> LayerGeneratorType;
  typedef LayerGeneratorType::Pointer    LayerGeneratorPointerType;
  typedef ImageLayerRenderingModel
  <RGBImageType>                            VisualizationModelType;
  typedef VisualizationModelType::Pointer VisualizationModelPointerType;
  typedef LayerGeneratorType::ImageLayerType
  ::OutputPixelType OutputPixelType;
  typedef VisualizationModelType::RegionType RegionType;

  typedef TypeManager::Vector_Data     VectorDataType;
  typedef VectorDataType::Pointer      VectorDataPointerType;
  typedef VectorDataModel              VectorDataModelType;
  typedef VectorDataModelType::Pointer VectorDataModelPointerType;
  typedef VectorDataType::DataNodeType DataNodeType;
  typedef DataNodeType::PointType      PointType;
  typedef DataNodeType::PolygonType::VertexType
  VertexType; 
  
  // Reprojection filter
  typedef VectorDataProjectionFilter<VectorDataType,VectorDataType> 
    VectorDataProjectionFilterType;
  typedef VectorDataExtractROI<VectorDataType>  VectorDataExtractROIType;
  typedef VectorDataExtractROIType::RegionType  RemoteSensingRegionType;

  /** Get the visualization model */
  itkGetObjectMacro(VisualizationModel, VisualizationModelType);

  /** Get the vector data model */
  itkGetObjectMacro(VectorDataModel, VectorDataModelType);

  /** Input Image Pointer */
  itkGetConstObjectMacro(InputImage, VectorImageType);
  void SetImage(VectorImagePointerType image);
  /** Load a vector data */
  void AddVectorData(VectorDataPointerType vData);

  void RemoveDataNode(DataNodeType * node);
  void SetDataNodeFieldAsInt(DataNodeType * node, const std::string& name, int value);
  void SetDataNodeFieldAsFloat(DataNodeType * node, const std::string& name, float value);
  void SetDataNodeFieldAsString(DataNodeType* node, const std::string& name, const std::string& value);
  void RemoveFieldFromDataNode(DataNodeType * node, const std::string& name);
  void RemovePointFromDataNode(DataNodeType * node,
                               const unsigned long& index,
                               bool interiorRing,
                               const unsigned int& interiorRingIndex = 0);
  void UpdatePointFromDataNode(DataNodeType * node,
                               const unsigned long& index,
                               const PointType& point,
                               bool interiorRing,
                               const unsigned int& interiorRingIndex = 0);
  void UpdateAlpha(double alpha);
  void OK();
  void FocusOnDataNode(const PointType& index);

  /** Methods necessary for the automatic mode*/

  void RightIndexClicked(const IndexType & index, RegionType ExtRegion)
  {
    std::cout<< "RightIndexClicked:: Not Implemented Yet"<< std::endl;
    std::cout<< "LeftIndexClicked:: index" << index<< std::endl;
    std::cout<< "LeftIndexClicked:: ExtRegion" << ExtRegion<< std::endl;
  }
  void LeftIndexClicked(const IndexType & index, RegionType ExtRegion)
  {
    std::cout<< "LeftIndexClicked:: Not Implemented Yet"<< std::endl;
    std::cout<< "LeftIndexClicked:: index" << index<< std::endl;
    std::cout<< "LeftIndexClicked:: ExtRegion" << ExtRegion<< std::endl;
  }
  
  void DeleteGeometry(void){}

  
  /** Output accessor. */
  itkGetObjectMacro(Output, VectorDataType );
  
  /** Output changed accessor. */
  itkGetMacro( OutputChanged, bool );
  
  /** DEM directory accessors. */
  itkGetConstMacro(DEMPath, std::string);
  itkSetMacro(DEMPath, std::string);

  /** Use DEM or not accessor. */
  itkGetMacro(UseDEM, bool);
  itkSetMacro(UseDEM, bool);

  /** Receive notifications */
  virtual void Notify();

protected:
  /** Constructor */
  VectorizationModel();
  /** Destructor */
  virtual ~VectorizationModel();

private:
  VectorizationModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Visualization */
  VisualizationModelPointerType m_VisualizationModel;
  LayerGeneratorPointerType     m_ImageGenerator;

  /** Input Images */
  VectorImagePointerType m_InputImage;

  /** VectorData model */
  VectorDataModelPointerType m_VectorDataModel;

  /**Output vector data. */
  VectorDataPointerType  m_Output;
  
  /** Has a new output or not. */
  bool m_OutputChanged;

  /** DEM directoryu path. */
  std::string m_DEMPath;

  /** Use DEM or not. */
  bool m_UseDEM;
};

} //end namespace otb
#endif
