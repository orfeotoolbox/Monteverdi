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
#ifndef __otbVectorDataTransformModel_h
#define __otbVectorDataTransformModel_h

#include "otbEventsSender.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "otbVectorImage.h"
#include "otbVectorData.h"

// Visu
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbPixelDescriptionModel.h"

// Type manager
#include "otbTypeManager.h"

// For VectorData transform and projection purpose
#include "otbVectorDataTransformFilter.h"
#include "itkCenteredSimilarity2DTransform.h"
#include "otbVectorDataProjectionFilter.h"

namespace otb
{

class VectorDataTransformModel
  : public EventsSender<std::string>, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef VectorDataTransformModel                           Self;
  typedef itk::Object                                        Superclass;
  typedef itk::SmartPointer<Self>                            Pointer;
  typedef itk::SmartPointer<const Self>                      ConstPointer;

  /** Standard type macro */
  itkTypeMacro(VectorDataTransformModel,Object);

  /** Algorithms typedef */
  typedef      TypeManager::Floating_Point_VectorImage                   VectorImageType;
  typedef      TypeManager::Floating_Point_Image                         ImageType;
  typedef      VectorImageType::PixelType                                VectorPixelType;
  
  // VectorDataType 
  typedef TypeManager::Vector_Data                                       VectorDataType;
  typedef VectorDataType::DataNodeType                                   DataNodeType;

  /** Filter to project the VectorData generated from OSM */
  typedef otb::VectorDataProjectionFilter<VectorDataType,
                                          VectorDataType>                VectorDataProjectionFilterType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                                   RGBPixelType;
  typedef      Image<RGBPixelType,2>                                     RGBImageType;
  typedef      ImageLayer<VectorImageType,RGBImageType>                  LayerType;
  typedef      ImageLayerGenerator<LayerType>                            LayerGeneratorType;
  typedef      ImageLayerRenderingModel<RGBImageType>                    VisualizationModelType;
  typedef      PixelDescriptionModel<RGBImageType>                       PixelDescriptionModelType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;

  /** type for channels histogram */
  typedef std::vector<double>                                        ChannelsHistogramType;
  
  // Transform typedefs 
  typedef otb::VectorDataTransformFilter<VectorDataType, 
                                         VectorDataType>             VectorDataTransformType;
  typedef itk::CenteredSimilarity2DTransform<double>                 TransformType;
  typedef TransformType::ParametersType                              ParametersType;

  /** This is protected for the singleton. Use GetInstance() instead. */
  itkNewMacro(Self);

  /** Open an image with its associated label map */
  void OpenInputs(const VectorImageType * image, const VectorDataType * inputVd);
  
  /** Get the visualization models */
  itkGetObjectMacro(VisualizationModel,VisualizationModelType);
  itkGetObjectMacro(PixelDescriptionModel,PixelDescriptionModelType);
  
  /** Input Image Accessor*/
  itkGetConstObjectMacro(VectorImage,VectorImageType);

  /** Transformed VectorData accessor */ 
  itkGetObjectMacro(TransformedVectorData, VectorDataType);

  /** Transformed VectorData accessor */ 
  itkGetObjectMacro(OutputVectorData, VectorDataType);
  
  std::vector<unsigned int> GetChannels()
  {
    return m_Channels;
  }

  /** Apply Transform to Vector Data */
  void ApplyTransformToVectorData(double scale, double angle, 
                                  double translationX, 
                                  double translationY,
                                  double centerX, double centerY);

  /** Generate output VectorData */
  void GenerateOuputVectorData();

  /** Exit */
  void Exit();
  
protected:
  /** Constructor */
  VectorDataTransformModel();
  /** Destructor */
  ~VectorDataTransformModel(){};
  
private:
  VectorDataTransformModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Visualization model */
  VisualizationModelType::Pointer    m_VisualizationModel;

  /** Pixel description model */
  PixelDescriptionModelType::Pointer m_PixelDescriptionModel;

  /** Vector reader */
  VectorImageType::ConstPointer      m_VectorImage;

  /** Vector Image layer generator */
  LayerGeneratorType::Pointer        m_ImageGenerator;

  /** Input VectorData */
  VectorDataType::ConstPointer       m_InputVectorData;

  /** Transformed VectorData (in image coordinate system)*/
  VectorDataType::Pointer            m_TransformedVectorData;

  /** Transformed VectorData (in input vector data coordinate system)*/
  VectorDataType::Pointer            m_OutputVectorData;
  
  /** Vector for the channels organisation */
  std::vector<unsigned int>          m_Channels;

  /** string to store the input vector data projectionRef*/
  std::string                        m_InputVectorDataProjectionRef;
};
}
#endif
