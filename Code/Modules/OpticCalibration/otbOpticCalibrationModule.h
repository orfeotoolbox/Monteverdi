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
#ifndef __otbOpticCalibration_h
#define __otbOpticCalibration_h

//#include "otbEventsListener.h"

// include the base class
#include "otbModule.h"

#include "otbOpticCalibrationViewGroup.h"

// Filters
#include "otbVectorImage.h"
#include "otbImageToLuminanceImageFilter.h"
#include "otbImageToReflectanceImageFilter.h"
#include "otbReflectanceToSurfaceReflectanceImageFilter.h"
#include "otbSurfaceAdjencyEffect6SCorrectionSchemeFilter.h"
#include "otbDifferenceImageFilter.h"
#include "otbMultiplyByScalarImageFilter.h"


namespace otb
{
/** \class OpticCalibrationModule
 *
 *  \brief This is the OpticCalibration module, which allows to perform
 *  OpticCalibration (Luminance, TOA, TOC) for Spot4, Spot5, and QuickBird.
 *
 *
 */

class ITK_EXPORT OpticCalibrationModule
  : public Module, public OpticCalibrationViewGroup
{
public:
  /** Standard class typedefs */
  typedef OpticCalibrationModule        Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(OpticCalibrationModule,Module);

  /** Data typedefs */
  typedef VectorImage<double,2>   ImageType;

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


  itkGetObjectMacro(DifferenceFilter, DifferenceImageFilterType);

  /** Check if the input file is a good one (QB, SPOT or Ikonos image + metadata availble). */
  bool CheckMetadata();

protected:
  /** Constructor */
  OpticCalibrationModule();

  /** Destructor */
  virtual ~OpticCalibrationModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  //void Notify(const std::string & event);

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run(); 
  /** Show the Module GUI */
  virtual bool CanShow(){return true;};
  /** Show the Module GUI */
  virtual void Show(){ wMainWindow->show(); };
  /** Hide the Module GUI */
  virtual void Hide() { wMainWindow->hide(); };

  /** GUI widget initialization. */
  void Init();
  /** Set the text displayed in the spectral sensitivity help window */
  void InitHelper();
  /** OK() */
  void OK();
  /** Update information displayed in the window title. */
  void UpdateInformation();
  /** Init correction parameters. */
  void UpdateCoefSetup();
  /** Init atmospheric radiative terms of the selected channel (channel index input choice)*/
  void UpdateRadiativeTermsCallback();
  /** Read filters attributs and display them in the text widgets. */
  void UpdateParamDisplay();
  /** Change the correction parameters values in filters. */
  void UpdateCorrectionParameters();

  /** GUI callbacks */
  /** Open Aeronet file*/
  virtual void OpenAeronetFileCallback();
  /** Open Fiter function values (Sensitivity coefficients) */
  virtual void OpenFFVFileCallback();
  /** Quit callback */
  virtual void QuitCallback();
  /** Change filters parameters when user change it through the GUI (Save/Quit button)*/
  virtual void SaveAndQuitCallback();
  /** Change the radiative terms of the selected band with the users value (Reload Radiative Terms button).
   * if updateIm==0 just update text display, else compute deducted parameters.
   */
  virtual void ReloadChannelTermsCallback( bool updateIm );
  /** Update parameters (Update button). */
  virtual void UpdateParametersCallback();

private:
  OpticCalibrationModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ImageType::Pointer m_InputImage;

  /** Filters */
  /** Image to Luminance filter*/
  ImageToLuminanceImageFilterPointerType                m_ImageToLuminanceFilter;
  /** Image to TOA filter*/
  ImageToReflectanceImageFilterPointerType              m_ImageToReflectanceFilter;
  /** TOA to TOC filter*/
  ReflectanceToSurfaceReflectanceImageFilterPointerType m_ReflectanceToSurfaceReflectanceFilter;
  /** Diff TOA-TOC filter*/
  DifferenceImageFilterPointerType                      m_DifferenceFilter;
  /** 1000* TOA filter*/
  MultiplyByScalarImageFilterPointerType m_TOAMultiplier;
  /** 1000* TOC filter*/
  MultiplyByScalarImageFilterPointerType m_TOCMultiplier;
  /** 1000* (TOA-TOC) filter*/
  MultiplyByScalarImageFilterPointerType m_DiffTOATOCMultiplier;
   
  /** String to store last opened file. */
  std::string m_LastPath;

  /** bool used to detected 2 consecutive click on Update while parameters haven't been changed. */
  bool m_CanUpdateParameters;

  /** Empty buffer for text helper display. */
  Fl_Text_Buffer * m_HelpTextBuffer;
};



} // End namespace otb

#endif
