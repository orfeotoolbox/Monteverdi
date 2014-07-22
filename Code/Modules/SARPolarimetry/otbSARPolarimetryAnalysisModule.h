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
#ifndef __otbSARPolarimetryAnalysisModule_h
#define __otbSARPolarimetryAnalysisModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbSARPolarimetryAnalysisModuleGUI.h"

// include the OTB/ITK elements
#include "otbMultiChannelsPolarimetricSynthesisFilter.h"
#include "otbReciprocalHAlphaImageFilter.h"

namespace otb
{
/** \class SARPolarimetryAnalysisModule
 *  \brief This is the SARPolarimetry module
 *
 * Description of the module.
 *
 */

class ITK_ABI_EXPORT SARPolarimetryAnalysisModule
  : public Module, public SARPolarimetryAnalysisModuleGUI
{
public:
  /** Standard class typedefs */
  typedef SARPolarimetryAnalysisModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SARPolarimetryAnalysisModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef TypeManager::Floating_Point_Image ImageType;
  typedef TypeManager::Floating_Point_Complex_VectorImage ComplexVectorImageType;

  /** Filters typedefs */
  typedef MultiChannelsPolarimetricSynthesisFilter<ComplexVectorImageType, ImageType> MultiChannelsPolarimetricSynthesisFilterType;
  typedef ReciprocalHAlphaImageFilter<ComplexVectorImageType, VectorImageType>        ReciprocalHAlphaImageFilterType;


  /** Show the Module GUI */
  virtual bool CanShow(){return true; }
  /** Show the Module GUI */
  virtual void Show()
  {
    wMainWindow->show();
  }

  virtual void Hide()
  {
    wMainWindow->hide();
   }

  virtual void Ok();

  virtual void Quit();


protected:
  /** Constructor */
  SARPolarimetryAnalysisModule();

  /** Destructor */
  virtual ~SARPolarimetryAnalysisModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Check/uncheck all */
  virtual void CheckAll(bool);

  /** Check inputs configuration. */
  void CheckInputs();

private:
  SARPolarimetryAnalysisModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ComplexVectorImageType::Pointer m_InputImage;

  MultiChannelsPolarimetricSynthesisFilterType::Pointer m_MultiChannelsPolarimetricSynthesisFilter;
  ReciprocalHAlphaImageFilterType::Pointer                   m_ReciprocalHAlphaImageFilter;

  };

} // End namespace otb

#endif
