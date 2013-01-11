/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbSpectrumModuleModel_h
#define __otbSpectrumModuleModel_h

#include <itkObject.h>

#include "otbEventsSender.h"

#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"

#include "otbImageLayerRenderingModel.h"
#include "otbPixelSpectrumModel.h"
#include "otbSpectralAngleDistanceImageFilter.h"

#include "otbImageFileWriter.h"
#include "itkShiftScaleImageFilter.h"
#include "otbImageToVectorImageCastFilter.h"

#include "itkScalarToRGBColormapImageFilter.h"
#include "otbScalarToReverseRainbowRGBPixelFunctor.h"

#include "otbRGBImageToVectorImageCastFilter.h"

#include "otbTypeManager.h"

#include <fstream>
#include <iostream>
#include <time.h>

namespace otb {

class SpectrumModuleModel
: public itk::Object, public EventsSender<std::string>
{
public:
  /** Standard class typedefs */
  typedef SpectrumModuleModel           Self;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** General Types */
  typedef TypeManager::Floating_Point_Precision   PixelType;
  typedef TypeManager::Floating_Point_VectorImage ImageType;
  typedef TypeManager::Floating_Point_Image       SingleImageType;
  typedef TypeManager::Labeled_Short_VectorImage  LabelImageType;

  typedef itk::RGBPixel<unsigned char> RGBPixelType;
  typedef otb::Image<RGBPixelType, 2>  RGBImage;

  typedef itk::VariableLengthVector<PixelType>                PixelVectorType;
  typedef ImageType::Pointer                                  ImagePointerType;
  typedef otb::ImageLayer<ImageType, RGBImage>                LayerType;
  typedef otb::ImageLayerGenerator<LayerType>                 LayerGeneratorType;
  typedef LayerGeneratorType::ImageLayerType::OutputPixelType LayerOutputPixelType;
  typedef ImageType::IndexType                                IndexType;

  /** Filters */
  typedef otb::SpectralAngleDistanceImageFilter<ImageType, SingleImageType> SpectralFilterType;
  typedef Function::UniformAlphaBlendingFunction<LayerOutputPixelType>      BlendingFunctionType;
  typedef itk::ShiftScaleImageFilter<SingleImageType, SingleImageType>      rescaleFiltertype;
  typedef otb::ImageToVectorImageCastFilter<SingleImageType, ImageType>     ImageToVectorImageFilterType;

  typedef itk::ScalarToRGBColormapImageFilter<SingleImageType, RGBImage>               ColorMapFilterType;
  typedef otb::Functor::ScalarToReverseRainbowRGBPixelFunctor<PixelType, RGBPixelType> ColorMapFunctorType;
  typedef otb::RGBImageToVectorImageCastFilter<RGBImage, ImageType>                    RGBtoVectorImageCastFilterType;

  /** Models */
  typedef otb::ImageLayerRenderingModel<RGBImage> ImageModelType;
  typedef ImageModelType::Pointer                 ImageModelPointerType;

  typedef otb::PixelSpectrumModel         PixelSpectrumModelType;
  typedef PixelSpectrumModelType::Pointer PixelSpectrumModelPointerType;

  /** New macro */
  itkNewMacro(Self);

  /** Methods */
  itkGetObjectMacro(ImageModel, ImageModelType);

  itkSetObjectMacro(ImageModel, ImageModelType);

  itkGetObjectMacro(PixelSpectrumModel, PixelSpectrumModelType);
  itkSetObjectMacro(PixelSpectrumModel, PixelSpectrumModelType);

  itkGetObjectMacro(spectralImage, ImageType);

  itkGetMacro(IsImageReady, bool);
  itkSetMacro(IsImageReady, bool);

  itkGetMacro(UseColorMap, bool);
  itkSetMacro(UseColorMap, bool);

  itkGetMacro(OriginalDynamic, double);
  itkSetMacro(OriginalDynamic, double);

  void SetInputImage(ImagePointerType im);
  itkGetObjectMacro(InputImage, ImageType);

  std::vector<unsigned int> GetChannels()
  {
    return m_Channels;
  }

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Determine 2 vectors composed of the lowest/biggest intensities per channel*/
  void DetermineMinMaxValues();

  void UpdateViewerDisplay(std::vector<unsigned int> ch);
  void GenerateSpectralAngle(unsigned int Id);

  void SetSpectralLayerOpacity(float f);
  void WriteSpreadsheetFile(int id, const char *path);
  void ForceSpectralContrast(float c);
  void ClearSpectralAngle();
  void SaveAndQuit();
  void Quit();

protected:
  SpectrumModuleModel();
  ~SpectrumModuleModel();

private:
  /** Members */
  ImageModelPointerType         m_ImageModel;
  PixelSpectrumModelPointerType m_PixelSpectrumModel;

  ImagePointerType            m_InputImage;
  LayerGeneratorType::Pointer m_LayerGenerator;
  LayerGeneratorType::Pointer m_SpectralLayerGenerator;

  bool                      m_IsImageReady;
  std::vector<unsigned int> m_Channels;
  ImageType::Pointer        m_spectralImage;
  ImageType::Pointer        m_spectralQL;

  SpectralFilterType::Pointer           m_SpectralAnglefilter;
  SpectralFilterType::Pointer           m_SpectralAnglefilterQL;
  rescaleFiltertype::Pointer            m_rescaleFilterImage;
  rescaleFiltertype::Pointer            m_rescaleFilterQL;
  ImageToVectorImageFilterType::Pointer m_CastImage;
  ImageToVectorImageFilterType::Pointer m_CastQL;

  ColorMapFilterType::Pointer             m_Colormapper;
  ColorMapFilterType::Pointer             m_ColormapperQL;
  ColorMapFunctorType::Pointer            m_Colormap;
  RGBtoVectorImageCastFilterType::Pointer m_RgbToVectorFilter;
  RGBtoVectorImageCastFilterType::Pointer m_RgbToVectorFilterQL;

  bool                          m_SpectralAngleLayerAvailable;
  bool                          m_SecondLayerToAdd;
  bool                          m_UseColorMap;
  BlendingFunctionType::Pointer m_BlendingFunction;
  double                        m_OriginalDynamic;
  double                        m_LastBWContrast;
};

}       // end namespace otb

#endif
