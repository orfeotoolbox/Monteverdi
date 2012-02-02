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

#ifndef __otbCompareImagesModel_h
#define __otbCompareImagesModel_h

#include "otbMVCModel.h"
#include "otbTypeManager.h"
#include "otbListenerBase.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "otbEventsSender.h"

#include "otbImage.h"
#include "otbStreamingCompareImageFilter.h"

#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbPixelDescriptionModel.h"

namespace otb
{
class ITK_EXPORT CompareImagesModel
  : public EventsSender<std::string>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef CompareImagesModel         Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** typedefs */
  typedef TypeManager::Floating_Point_Image InputImageType;
  typedef InputImageType::Pointer           InputImagePointer;
  typedef InputImageType::IndexType         IndexType;
  typedef itk::ContinuousIndex<>            ContinuousIndexType;
  
  
  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                                 RGBPixelType;
  typedef Image<RGBPixelType, 2>                                       RGBImageType;
  typedef ImageLayer<InputImageType, RGBImageType>                     LayerType;
  typedef ImageLayerGenerator<LayerType>                               LayerGeneratorType;
  typedef ImageLayerRenderingModel<RGBImageType>                       VisuModelType;
  typedef LayerGeneratorType::ImageLayerType::OutputPixelType          LayerOutputPixelType;
  typedef Function::UniformAlphaBlendingFunction<LayerOutputPixelType> BlendingFunctionType;

  /** Pixel description model */
  typedef PixelDescriptionModel<RGBImageType> PixelDescriptionModelType;
  typedef PixelDescriptionModelType::Pointer  PixelDescriptionModelPointerType;
  
  /** Standard type macro */
  itkTypeMacro(CompareImagesModel, MVCModel);
  itkNewMacro(Self);

  typedef otb::StreamingCompareImageFilter<InputImageType>               CompareImagessFilterType;

  /** Get the rendering models */
  itkGetObjectMacro(LeftVisuModel, VisuModelType);
  itkGetObjectMacro(RightVisuModel, VisuModelType);
  itkGetObjectMacro(LeftPixelDescriptionModel, PixelDescriptionModelType);
  itkGetObjectMacro(RightPixelDescriptionModel, PixelDescriptionModelType);

  /** SetInputImage */
  void SetInputLeftImage(InputImagePointer leftImage);
  itkGetObjectMacro(InputLeftImage, InputImageType);
  
  void SetInputRightImage(InputImagePointer rightImage);
  itkGetObjectMacro(InputRightImage, InputImageType);

  /** Get the output changed flag */
  itkGetMacro(OutputChanged, bool);

  void ComputeCompareImages();
  void SaveQuit();
  
  itkGetMacro(RMSE,double);
  itkGetMacro(PSNR,double);
  itkGetMacro(MaxError,double);
  

protected:
  /** Constructor */
  CompareImagesModel();

  /** Destructor */
  virtual ~CompareImagesModel();
  
  itkSetMacro(RMSE,double);
  itkSetMacro(PSNR,double);
  itkSetMacro(MaxError,double);

private:
  CompareImagesModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);

  InputImageType::Pointer m_InputLeftImage;
  InputImageType::Pointer m_InputRightImage;

  CompareImagessFilterType::Pointer         m_CompareImagessFilter;
  
  double m_RMSE;
  double m_PSNR;
  double m_MaxError;
  
  bool m_OutputChanged;

  /** Rendering model */
  VisuModelType::Pointer m_LeftVisuModel;
  VisuModelType::Pointer m_RightVisuModel;

  /** The pixel description model */
  PixelDescriptionModelPointerType m_LeftPixelDescriptionModel;
  PixelDescriptionModelPointerType m_RightPixelDescriptionModel;
  
  LayerGeneratorType::Pointer m_LeftImageGenerator;

};

}
#endif
