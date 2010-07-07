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
#include "otbTypeManager.h"

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
  : public MVCModel<ListenerBase>, public itk::Object, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef VectorizationModel            Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(VectorizationModel, Object);

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
  typedef Function::UniformAlphaBlendingFunction
  <OutputPixelType>                         BlendingFunctionType;
  typedef BlendingFunctionType::Pointer      BlendingFunctionPointerType;
  typedef VisualizationModelType::RegionType RegionType;

  typedef TypeManager
  ::Vector_Data VectorDataType;
  typedef VectorDataModel              VectorDataModelType;
  typedef VectorDataModelType::Pointer VectorDataModelPointerType;
  typedef VectorDataType::DataNodeType DataNodeType;
  typedef DataNodeType::PointType      PointType;
  typedef DataNodeType::PolygonType::VertexType
  VertexType;
  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** Get the visualization model */
  itkGetObjectMacro(VisualizationModel, VisualizationModelType);

  /** Get the vector data model */
  itkGetObjectMacro(VectorDataModel, VectorDataModelType);

  /** Input Image Pointer */
  itkGetConstObjectMacro(InputImage, VectorImageType);
  void SetImage(VectorImagePointerType image);

  void RemoveDataNode(DataNodeType * node);
  void SetDataNodeFieldAsInt(DataNodeType * node, const std::string& name, int value);
  void SetDataNodeFieldAsFloat(DataNodeType * node, const std::string& name, float value);
  void SetDataNodeFieldAsString(DataNodeType* node, const std::string& name, const std::string& value);
  void RemoveFieldFromDataNode(DataNodeType * node, const std::string& name);
  void RemovePointFromDataNode(DataNodeType * node,
                               const long& index,
                               bool interiorRing,
                               const unsigned int& interiorRingIndex = 0);
  void UpdatePointFromDataNode(DataNodeType * node,
                               const long& index,
                               const PointType& point,
                               bool interiorRing,
                               const unsigned int& interiorRingIndex = 0);

protected:
  /** Constructor */
  VectorizationModel();
  /** Destructor */
  ~VectorizationModel();

private:
  VectorizationModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Receive notifications */
  virtual void Notify();

  /** Singleton instance */
  static Pointer Instance;

  /** Visualization */
  VisualizationModelPointerType m_VisualizationModel;
  LayerGeneratorPointerType     m_ImageGenerator;
  BlendingFunctionPointerType   m_BlendingFunction;

  /** Input Images */
  VectorImagePointerType m_InputImage;

  /** VectorData model */
  VectorDataModelPointerType m_VectorDataModel;
};

} //end namespace otb
#endif
