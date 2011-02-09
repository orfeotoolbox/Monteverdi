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
#ifndef __otbSarCalibrationModule_h
#define __otbSarCalibrationModule_h

#include "otbModule.h"
#include "otbSarCalibrationModuleGUI.h"

// include the OTB elements
#include "otbImage.h"
#include "otbSarRadiometricCalibrationToImageFilter.h"
#include "otbSarBrightnessToImageFilter.h"
#include "otbLog10ThresholdedImageFilter.h"


namespace otb
{
/** \class SarCalibrationModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT SarCalibrationModule
  : public Module, public SarCalibrationModuleGUI
{
public:
  /** Standard class typedefs */
  typedef SarCalibrationModule          Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SarCalibrationModule, Module);

  // Convenient typedefs
  typedef TypeManager::Floating_Point_Precision     Floating_Point_PrecisionType;
  typedef TypeManager::Floating_Point_Image         ImageType;
  typedef TypeManager::Floating_Point_Complex_Image ComplexImageType;


  // SarCalibration Class typedefs
  typedef SarRadiometricCalibrationToImageFilter<ImageType, ImageType>               CalibrationFilterType;
  typedef SarRadiometricCalibrationToImageFilter<ComplexImageType,ImageType>         CalibrationComplexFilterType;

  // SarBrightness Class typedefs
  typedef SarBrightnessToImageFilter<ImageType, ImageType>               BrightnessFilterType;
  typedef SarBrightnessToImageFilter<ComplexImageType,ImageType>         BrightnessComplexFilterType;

  typedef otb::Log10ThresholdedImageFilter<ImageType,ImageType>          Log10TImageFilterType;
  /** Check metadat validity */
  bool CheckMetadata();


protected:
  /** Constructor */
  SarCalibrationModule();
  /** Destructor */
  virtual ~SarCalibrationModule();

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    wMainWindow->show();
  }

  /** Show the Module GUI */
  virtual void Hide()
  {
    wMainWindow->hide();
  }


  /** Callbacks */
  virtual void OK();
  virtual void Cancel();

  /** SarCalibration Methods*/

  /** Calibration Process */
  void CalibrationProcess();

  /** Complex Calibration Process */
  void ComplexCalibrationProcess();

  static const double Epsilon;

private:
  SarCalibrationModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Class attributes
  ImageType::Pointer        m_InputImage;
  ComplexImageType::Pointer m_ComplexInputImage;
  ImageType::Pointer        m_OutputImage;

  CalibrationFilterType::Pointer        m_CalibFilter;
  CalibrationComplexFilterType::Pointer m_ComplexCalibFilter;

  BrightnessFilterType::Pointer        m_BrightnessCalibFilter;
  BrightnessComplexFilterType::Pointer m_ComplexBrightnessFilter;

  Log10TImageFilterType::Pointer m_Log10TImageFilter;

  /** Modulus input image or complex one */
  bool m_WorkWithCplx;
};

} // End namespace otb

#endif
