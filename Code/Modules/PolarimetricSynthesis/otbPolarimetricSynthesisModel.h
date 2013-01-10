/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbPolarimetricSynthesisModel_h
#define __otbPolarimetricSynthesisModel_h

#include "otbEventsSender.h"
#include "otbTypeManager.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbImageList.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbStreamingShrinkImageFilter.h"
#include "otbMultiChannelsPolarimetricSynthesisFilter.h"
#include "otbListenerBase.h"
#include "otbImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "itkListSample.h"
#include "itkListSampleToHistogramGenerator.h"

// Visu
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbPixelDescriptionModel.h"


namespace otb
{

typedef enum
{
  CO_POLAR,
  CROSS_POLAR,
  ANY_POLAR
} PolarizationMode;

typedef enum
{
  RED=0,
  GREEN=1,
  BLUE=2,
  GRAYSCALE=3
} Color;

class ITK_EXPORT PolarimetricSynthesisModel
: public  EventsSender<std::string>, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef PolarimetricSynthesisModel                         Self;
  typedef EventsSender<std::string>                          Superclass;
  typedef itk::SmartPointer<Self>                            Pointer;
  typedef itk::SmartPointer<const Self>                      ConstPointer;

  /** Standard type macro */
  itkTypeMacro(PolarimetricSynthesisModel, Object);

  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** New macro*/
  itkNewMacro(Self);

//  typedef TypeManager::Floating_Point_Precision     PixelType;
//  typedef TypeManager::Floating_Point_Image         ImageType;
//  typedef TypeManager::Floating_Point_Complex_Image ComplexImageType;

  /// Filters typedefs
//  typedef double                                                                   PixelType;
  typedef TypeManager::Floating_Point_Precision                                    PixelType;
  typedef std::complex<PixelType>                                                  ComplexPixelType;
  typedef otb::Image<ComplexPixelType, 2>                                           ComplexImageType;
  typedef ComplexImageType::SizeType                                               SizeType;
  typedef otb::Image<PixelType, 2>                                                  OutputImageType;
  typedef otb::VectorImage<PixelType, 2>                                            DoubleVectorImageType;
  typedef otb::VectorImage<ComplexPixelType, 2>                                     ComplexVectorImageType;
  typedef ComplexVectorImageType::PixelType                                        ComplexVectorPixelType;
  typedef otb::ImageFileReader<ComplexImageType>                                   ImageReaderType;
  typedef otb::ImageFileReader<ComplexVectorImageType>                             VectorImageReaderType;
  typedef otb::ImageList<ComplexImageType>                                         ComplexImageListType;
  typedef otb::ImageList<OutputImageType>                                          DoubleImageListType;
  typedef otb::ImageListToVectorImageFilter<ComplexImageListType, ComplexVectorImageType>  ComplexImageListToVectorImageFilterType;
  typedef otb::ImageListToVectorImageFilter<DoubleImageListType, DoubleVectorImageType> DoubleImageListToVectorImageFilterType;
  typedef otb::StreamingShrinkImageFilter<ComplexVectorImageType, ComplexVectorImageType> ShrinkFilterType;
  typedef otb::MultiChannelsPolarimetricSynthesisFilter<ComplexVectorImageType, OutputImageType> PolarimetricSynthesisFilterType;
  typedef itk::ImageRegionConstIterator< ComplexVectorImageType >                  ComplexIteratorType;
  typedef itk::ImageRegionConstIterator< OutputImageType >                         OutputIteratorType;
  typedef itk::Vector<itk::NumericTraits<PixelType>::RealType, 1>                   MeasurementVectorType;
  typedef itk::Statistics::ListSample<MeasurementVectorType>                       ListSampleType;
  typedef double                                                                   HistogramMeasurementType;
  typedef itk::Statistics::ListSampleToHistogramGenerator<ListSampleType,
  HistogramMeasurementType, itk::Statistics::DenseFrequencyContainer, 1>           HistogramGeneratorType;
  typedef  HistogramGeneratorType::HistogramType                                   HistogramType;
  typedef otb::ImageFileWriter<DoubleVectorImageType>                     WriterRGBType;
  typedef otb::ImageFileWriter<OutputImageType>                           WriterGrayType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                                   RGBPixelType;
  typedef Image<RGBPixelType, 2>                                          RGBImageType;
  typedef ImageLayer<DoubleVectorImageType, RGBImageType>                       LayerType;
  //typedef ImageLayer<OutputImageType, RGBImageType>                       GrayLayerType;
  typedef ImageLayerGenerator<LayerType>                                 LayerGeneratorType;
  typedef ImageLayerRenderingModel<RGBImageType>                         VisualizationModelType;
  typedef PixelDescriptionModel<RGBImageType>                            PixelDescriptionModelType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;
  typedef LayerGeneratorType::ImageLayerType::RenderingFunctionType      RenderingFunctionType;


  /** Setters */
  void SetGain(double value);
  void SetGrayPsiI(double value);
  void SetGrayPsiR(double value);
  void SetGrayKhiI(double value);
  void SetGrayKhiR(double value);
  void SetRedPsiI(double value);
  void SetRedPsiR(double value);
  void SetRedKhiI(double value);
  void SetRedKhiR(double value);
  void SetGreenPsiI(double value);
  void SetGreenPsiR(double value);
  void SetGreenKhiI(double value);
  void SetGreenKhiR(double value);
  void SetBluePsiI(double value);
  void SetBluePsiR(double value);
  void SetBlueKhiI(double value);
  void SetBlueKhiR(double value);
  void SetGrayPolarizationMode(PolarizationMode mode);
  void SetRedPolarizationMode(PolarizationMode mode);
  void SetGreenPolarizationMode(PolarizationMode mode);
  void SetBluePolarizationMode(PolarizationMode mode);
  itkSetMacro(RGB, bool);
  itkSetMacro(HEmissionMode, bool);
  itkSetMacro(VEmissionMode, bool);
  itkSetMacro(QuicklookSize, SizeType);

  /** Getters */
  itkGetMacro(GrayPsiI, double);
  itkGetMacro(GrayPsiR, double);
  itkGetMacro(GrayKhiI, double);
  itkGetMacro(GrayKhiR, double);
  itkGetMacro(RedPsiI, double);
  itkGetMacro(RedPsiR, double);
  itkGetMacro(RedKhiI, double);
  itkGetMacro(RedKhiR, double);
  itkGetMacro(GreenPsiI, double);
  itkGetMacro(GreenPsiR, double);
  itkGetMacro(GreenKhiI, double);
  itkGetMacro(GreenKhiR, double);
  itkGetMacro(BluePsiI, double);
  itkGetMacro(BluePsiR, double);
  itkGetMacro(BlueKhiI, double);
  itkGetMacro(BlueKhiR, double);

  itkGetMacro(RGB, bool);
  itkGetMacro(HEmissionMode, bool);
  itkGetMacro(VEmissionMode, bool);
  itkGetMacro(GrayPolarizationMode, PolarizationMode);
  itkGetMacro(RedPolarizationMode, PolarizationMode);
  itkGetMacro(GreenPolarizationMode, PolarizationMode);
  itkGetMacro(BluePolarizationMode, PolarizationMode);
  itkGetMacro(QuicklookSize, SizeType);
  itkGetMacro(Valid, bool);
  itkGetObjectMacro(ShrinkFilter, ShrinkFilterType);
  itkGetMacro(Min, double);
  itkGetMacro(Max, double);
  itkGetMacro(ShrinkFactor, unsigned int);

  /** Get the output images */
  DoubleVectorImageType * GetOutputVectorImage();
  OutputImageType * GetOutputImage();
  DoubleVectorImageType * GetQuicklookOutputVectorImage();
  OutputImageType * GetQuicklookOutputImage();
  const HistogramType * GetOutputHistogram();

  /** Load a single image */
  void InitPointers();
  void LoadImages();
  void Save();
  void WirePipeline();
  void SetupPolarization(Color value);
  void SetupEmission(Color value);
  void ComputeNormalizationFactors();
  void GenerateOutputHistogram(Color value);
  void GenerateQuicklook();

  /** Setter / getter polarimetric Image*/
  itkSetObjectMacro(ImageHH, ComplexImageType);
  itkSetObjectMacro(ImageHV, ComplexImageType);
  itkSetObjectMacro(ImageVH, ComplexImageType);
  itkSetObjectMacro(ImageVV, ComplexImageType);
  itkGetObjectMacro(ImageHH, ComplexImageType);
  itkGetObjectMacro(ImageHV, ComplexImageType);
  itkGetObjectMacro(ImageVH, ComplexImageType);
  itkGetObjectMacro(ImageVV, ComplexImageType);

protected:

  /** Constructor */
  PolarimetricSynthesisModel();
  /** Destructor */
  ~PolarimetricSynthesisModel();

private:
  PolarimetricSynthesisModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);

  ComplexImageListType::Pointer  m_ImageList;
  // For RGB scroll
  DoubleImageListType::Pointer   m_ScrollImageList;
  // For RGB full
  DoubleImageListType::Pointer   m_FullImageList;

  ComplexImageListToVectorImageFilterType::Pointer m_ImageListToVectorImageFilter;
  DoubleImageListToVectorImageFilterType::Pointer m_ImageListToVectorImageFilterForFull;
  DoubleImageListToVectorImageFilterType::Pointer m_ImageListToVectorImageFilterForScroll;

  ShrinkFilterType::Pointer m_ShrinkFilter;
  PolarimetricSynthesisFilterType::Pointer m_GrayPolarimetricSynthesisFilter;
  PolarimetricSynthesisFilterType::Pointer m_RedPolarimetricSynthesisFilter;
  PolarimetricSynthesisFilterType::Pointer m_GreenPolarimetricSynthesisFilter;
  PolarimetricSynthesisFilterType::Pointer m_BluePolarimetricSynthesisFilter;
  PolarimetricSynthesisFilterType::Pointer m_GrayQuicklookPolarimetricSynthesisFilter;
  PolarimetricSynthesisFilterType::Pointer m_RedQuicklookPolarimetricSynthesisFilter;
  PolarimetricSynthesisFilterType::Pointer m_GreenQuicklookPolarimetricSynthesisFilter;
  PolarimetricSynthesisFilterType::Pointer m_BlueQuicklookPolarimetricSynthesisFilter;

  HistogramGeneratorType::Pointer          m_HistogramGenerator;

  /** Flags to activate/deactivate the preprocessings */
  bool m_Valid;

  /** The instance singleton */
  static Pointer Instance;

  /** Parameters */
  ComplexImageType::Pointer m_ImageHH;
  ComplexImageType::Pointer m_ImageHV;
  ComplexImageType::Pointer m_ImageVH;
  ComplexImageType::Pointer m_ImageVV;

  // Histogram and normalization
  double                 m_Min;
  double                 m_Max;
  double                 m_AbsoluteMin;
  double                 m_AbsoluteMax;

  // Configuration
  bool m_HEmissionMode;
  bool m_VEmissionMode;
  PolarizationMode m_GrayPolarizationMode;
  PolarizationMode m_RedPolarizationMode;
  PolarizationMode m_GreenPolarizationMode;
  PolarizationMode m_BluePolarizationMode;
  SizeType m_QuicklookSize;
  unsigned int m_ShrinkFactor;
  bool m_RGB;


  // Parameters
  double m_GrayPsiI;
  double m_GrayKhiI;
  double m_GrayPsiR;
  double m_GrayKhiR;
  double m_RedPsiI;
  double m_RedKhiI;
  double m_RedPsiR;
  double m_RedKhiR;
  double m_GreenPsiI;
  double m_GreenKhiI;
  double m_GreenPsiR;
  double m_GreenKhiR;
  double m_BluePsiI;
  double m_BlueKhiI;
  double m_BluePsiR;
  double m_BlueKhiR;

  ComplexVectorImageType::Pointer m_Quicklook;
};
} // end namespace otb


#endif
