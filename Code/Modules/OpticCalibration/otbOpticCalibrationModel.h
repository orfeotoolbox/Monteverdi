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
#ifndef __otbOpticCalibrationModel_h
#define __otbOpticCalibrationModel_h

#include "otbMVCModel.h"
#include "otbEventsSender.h"
#include "otbListenerBase.h"
#include "otbOpticCalibrationEnum.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageMetadataInterfaceBase.h"

#include "itkRGBAPixel.h"
#include "otbImageLayer.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayerRenderingModel.h"
#include "otbPixelDescriptionModel.h"
#include "otbMultiplyByScalarImageFilter.h"

/* example used : AtmosphericCorrectionSequencement */
#include "otbImageToLuminanceImageFilter.h"
#include "otbImageToReflectanceImageFilter.h"
#include "otbReflectanceToSurfaceReflectanceImageFilter.h"
#include "otbSurfaceAdjencyEffect6SCorrectionSchemeFilter.h"
#include "otbDifferenceImageFilter.h"

namespace otb
{
/** \class OpticCalibrationModel
 *
 *
 *
 */
class ITK_EXPORT OpticCalibrationModel
  : public EventsSender<std::string>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef OpticCalibrationModel         Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(OpticCalibrationModel, MVCModel);

  /** Images typedefs */
  typedef double                                         PixelType;
  typedef VectorImage<PixelType,2>                       ImageType;
  typedef ImageType::Pointer                             ImagePointerType;
  typedef ImageType::PixelType                           ImagePixelType;
  typedef ImageType::IndexType                           IndexType;
  typedef ImageType::SizeType                            SizeType;
  typedef ImageType::OffsetType                          OffsetType;
  typedef ImageType::RegionType                          RegionType;
  typedef ImageType::SpacingType                         SpacingType;
  typedef ImageType::InternalPixelType                   InternalPixelType;

  /** Viewer Image Type */
  typedef itk::RGBAPixel<unsigned char>                  RGBPixelType;
  typedef Image<RGBPixelType,2>                          ViewerImageType;

  /** Image layer type */
  typedef ImageLayer<ImageType,ViewerImageType>          ImageLayerType;
  typedef ImageLayerType::Pointer                        ImageLayerPointerType;
  typedef ObjectList<ImageLayerType>                     ImageLayerListType;
  typedef ImageLayerListType::Pointer                    ImageLayerListPointerType;
  typedef ImageLayerType::HistogramType                  HistogramType;

  /** Image layer generator type */
  typedef ImageLayerGenerator<ImageLayerType> ImageLayerGeneratorType;
  typedef ImageLayerGeneratorType::Pointer    ImageLayerGeneratorPointerType;

  /** Rendering functions */
  typedef Function::RenderingFunction<ImageType::PixelType,RGBPixelType> RenderingFunctionType;
  typedef RenderingFunctionType::Pointer                                 RenderingFunctionPointerType;

  typedef Function::StandardRenderingFunction<ImageType::PixelType,RGBPixelType> StandardRenderingFunctionType;
  typedef std::vector<unsigned int>                                              ChannelListType;

  /** Visu */
  typedef otb::ImageLayerRenderingModel<ViewerImageType> VisuModelType;
  typedef VisuModelType::Pointer                         VisuModelPointerType;

  /** Filters typedef */
  typedef ImageToLuminanceImageFilter<ImageType,ImageType>                ImageToLuminanceImageFilterType;
  typedef ImageToLuminanceImageFilterType::Pointer                        ImageToLuminanceImageFilterPointerType;
  typedef ImageToReflectanceImageFilter<ImageType,ImageType>              ImageToReflectanceImageFilterType;
  typedef ImageToReflectanceImageFilterType::Pointer                      ImageToReflectanceImageFilterPointerType;
  typedef ReflectanceToSurfaceReflectanceImageFilter<ImageType,ImageType> ReflectanceToSurfaceReflectanceImageFilterType;
  typedef ReflectanceToSurfaceReflectanceImageFilterType::Pointer         ReflectanceToSurfaceReflectanceImageFilterPointerType;
  typedef ReflectanceToSurfaceReflectanceImageFilterType::FilterFunctionCoefVectorType FilterFunctionCoefVectorType;
  typedef ReflectanceToSurfaceReflectanceImageFilterType::CoefVectorType               CoefVectorType;
  typedef AtmosphericCorrectionParameters::AerosolModelType                            AerosolModelType;

  typedef DifferenceImageFilter<ImageType, ImageType>      DifferenceImageFilterType;
  typedef DifferenceImageFilterType::Pointer               DifferenceImageFilterPointerType;

  typedef MultiplyByScalarImageFilter<ImageType,ImageType> MultiplyByScalarImageFilterType;
  typedef MultiplyByScalarImageFilterType::Pointer         MultiplyByScalarImageFilterPointerType;

  /** Metadatas typedef */
  typedef ImageMetadataInterfaceBase          ImageMetadataInterfaceType;
  typedef ImageMetadataInterfaceBase::Pointer ImageMetadataInterfacePointerType;

  /** Get/Set Input Image Pointer */
  itkSetObjectMacro(InputImage,ImageType);
  itkGetObjectMacro(InputImage,ImageType);

  /** Get/Set the Image Layer */
  itkSetObjectMacro(ImageLayerList,ImageLayerListType);
  itkGetObjectMacro(ImageLayerList,ImageLayerListType);

  /** Get has new input */
  itkNewMacro(Self);
  itkGetMacro(HasNewInput,bool);

  /** VisuModel */
  itkGetMacro(VisuModel,VisuModelPointerType );
  itkGetMacro(RVisuModel,VisuModelPointerType );

  /** Get ReflectanceToSurfaceReflectanceFilter */
  itkGetObjectMacro(ReflectanceToSurfaceReflectanceFilter, ReflectanceToSurfaceReflectanceImageFilterType);

  /** Get AtmosphericCorrectionParameters */
  AtmosphericCorrectionParameters::Pointer GetAtmosphericCorrectionParameters()
  {
    return m_ReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters();
  }

  /** Get AtmosphericRadiativeTerms */
  AtmosphericRadiativeTerms::Pointer GetAtmosphericRadiativeTerms()
  {
    return m_ReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms();
  }

  /** Generate output image */
  //void SaveImage(EnumImageType pType);

  /** Open Input Image */
  void OpenImage(ImagePointerType img);//const std::string & strfilename);

  /* Initialization of the model */
  void Initialization();

  /** Display Input Image */
  void DisplayImage();

  /** Update the displayed layer according to the choosen result */
  void UpdateDisplay(EnumImageType pType);

  /** Calibrate the extract */
  void CalibrateExtract();

  /** Quit the model : set the output and call notyfication. */
  void Quit();

  /** Change reflectance scale (multiply reflectance by 1000 or not)*/
  itkSetMacro(ChangeReflectanceScale, bool);
  itkGetMacro(ChangeReflectanceScale, bool);

  /** Update view channel order */
  void UpdateRGBChannelOrder(int redChoice , int greenChoice, int BlueChoice);
  void UpdateGrayscaleChannel(int choice);
  void CheckMetadata(ImagePointerType img) const;//const std::string & filename)const;
  void UpdateCorrectionParameters(AerosolModelType aeroMod, double ozAmount, double atmoPres);
  void UpdateCorrectionParameters(AerosolModelType aeroMod, double ozAmount, double atmoPres, double aeroTh, double waterAm);
  void UpdateRadiativeTerm(unsigned int ch, double intRef, double albebo, double gasT, double dT, double uT, double uDiffT, double uDirT, double uDTR, double uDTA );
  
  /** If reachable, load the spectral sensitivity */
  bool LoadSpectralSensitivity();

  /** Get luminance image */
  itkGetObjectMacro(LuminanceImage, ImageType);
  /** Get TOA image */
  itkGetObjectMacro(ReflectanceImage, ImageType);
  /** Get TOC image */
  itkGetObjectMacro(SurfaceReflectanceImage, ImageType);
  /** Get Difference TOA-TOC image */
  itkGetObjectMacro(DifferenceImage, ImageType);


protected:
  /** Constructor */
  OpticCalibrationModel();
  /** Destructor */
  virtual ~OpticCalibrationModel();

private:
  OpticCalibrationModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Visu Model */
  VisuModelPointerType        m_VisuModel;
  VisuModelPointerType        m_RVisuModel;

  /** Image layer list */
  ImageLayerListPointerType   m_ImageLayerList;

  /** Filters */
  ImageToLuminanceImageFilterPointerType                m_ImageToLuminanceFilter;
  ImageToReflectanceImageFilterPointerType              m_ImageToReflectanceFilter;
  ReflectanceToSurfaceReflectanceImageFilterPointerType m_ReflectanceToSurfaceReflectanceFilter;
  DifferenceImageFilterPointerType                      m_DifferenceFilter;
  
  /** Filter for QL pipeline */
  ImageToLuminanceImageFilterPointerType                m_QLImageToLuminanceFilter;
  ImageToReflectanceImageFilterPointerType              m_QLImageToReflectanceFilter;
  ReflectanceToSurfaceReflectanceImageFilterPointerType m_QLReflectanceToSurfaceReflectanceFilter;
  DifferenceImageFilterPointerType                      m_QLDifferenceFilter;

  /** Imput Image Pointer */
  ImagePointerType m_InputImage;

  /** Store luminance image */
  ImagePointerType m_LuminanceImage;
  /** Store TOA image */
  ImagePointerType m_ReflectanceImage;
  /** Store TOC image */
  ImagePointerType m_SurfaceReflectanceImage;
  /** Store Difference TOA-TOC image */
  ImagePointerType m_DifferenceImage;

  /** TOA Multiplier*/
  MultiplyByScalarImageFilterPointerType m_TOAMultiplier;
    /** TOC Multiplier*/
  MultiplyByScalarImageFilterPointerType m_TOCMultiplier;
  /** Diff TOA-TOC Multiplier*/
  MultiplyByScalarImageFilterPointerType m_DiffTOATOCMultiplier;
  
  /** Flags to activate/deactivate the preprocessings */
  bool m_HasNewInput;

  /** Change reflectance scale (multiply reflectance by 1000 or not)*/
  bool m_ChangeReflectanceScale;
};

}//end namespace otb
#endif
