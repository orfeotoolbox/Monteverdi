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
#ifndef __otbSARPolarimetryMuellerModule_h
#define __otbSARPolarimetryMuellerModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbSARPolarimetryMuellerModuleGUI.h"

// include the OTB/ITK elements
#include "otbMuellerToReciprocalCovarianceImageFilter.h"
#include "otbMuellerToPolarisationDegreeAndPowerImageFilter.h"

namespace otb
{
/** \class SARPolarimetryMuellerModule
 *  \brief This is the SARPolarimetry module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT SARPolarimetryMuellerModule
  : public Module, public SARPolarimetryMuellerModuleGUI
{
public:
  /** Standard class typedefs */
  typedef SARPolarimetryMuellerModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SARPolarimetryMuellerModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef TypeManager::Floating_Point_Complex_VectorImage ComplexVectorImageType;

  typedef VectorImageType::PixelType VectorImagePixelType;
  typedef ComplexVectorImageType::PixelType ComplexVectorImagePixelType;

  /** Filters typedefs */
  typedef MuellerToReciprocalCovarianceImageFilter<VectorImageType, ComplexVectorImageType> MuellerToReciprocalCovarianceImageFilterType;
   typedef MuellerToPolarisationDegreeAndPowerImageFilter<VectorImageType, VectorImageType>  MuellerToPolarisationDegreeAndPowerImageFilterType;


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
  SARPolarimetryMuellerModule();

  /** Destructor */
  virtual ~SARPolarimetryMuellerModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Check/uncheck all */
  virtual void CheckAll(bool);

  /** Check inputs configuration. */
  void CheckInputs();

private:
  SARPolarimetryMuellerModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  VectorImageType::Pointer m_InputImage;

  MuellerToReciprocalCovarianceImageFilterType::Pointer       m_MuellerToReciprocalCovarianceImageFilter;
  MuellerToPolarisationDegreeAndPowerImageFilterType::Pointer m_MuellerToPolarisationDegreeAndPowerImageFilter;

  };

} // End namespace otb

#endif
