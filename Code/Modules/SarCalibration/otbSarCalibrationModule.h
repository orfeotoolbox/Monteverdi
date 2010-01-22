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
#include "otbTerraSarBrightnessImageFilter.h"
#include "otbTerraSarCalibrationImageFilter.h"


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
  itkTypeMacro(SarCalibrationModule,Module);
  
  // Convenient typedefs
  typedef Image<double,2>                ImageType;
  typedef Image<std::complex<double>, 2> ComplexImageType;

  // SarCalibration Class typedefs
  typedef TerraSarBrightnessImageFilter<ImageType, ImageType>                BrightnessFilterType;
  typedef TerraSarBrightnessImageFilter<ComplexImageType, ComplexImageType>  BrightnessComplexFilterType;

  typedef TerraSarCalibrationImageFilter<ImageType, ImageType>               CalibrationFilterType;
  typedef TerraSarCalibrationImageFilter<ComplexImageType, ComplexImageType> CalibrationComplexFilterType;
  
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
  virtual bool CanShow(){return true;};
  
  /** Show the Module GUI */
  virtual void Show()
  {
    wMainWindow->show();
  };

  /** Show the Module GUI */
  virtual void Hide()
  {
    wMainWindow->hide();
  };

  /** SarCalibration Methods*/
                            
  /** OK callback*/
  virtual void OK();

private:
  SarCalibrationModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Class attributes
  ImageType::Pointer         m_InputImage;
  ComplexImageType::Pointer  m_ComplexInputImage;

  BrightnessFilterType::Pointer         m_BrighFilter;
  BrightnessComplexFilterType::Pointer  m_BrighComplexFilter;
  CalibrationFilterType::Pointer        m_CalibFilter;
  CalibrationComplexFilterType::Pointer m_ComplexCalibFilter;


  /** Modulus input image or complex one */
  bool                       m_WorkWithCplx;
};


} // End namespace otb

#endif
