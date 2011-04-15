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
#ifndef __otbSARPolarimetryRecCovarianceModule_h
#define __otbSARPolarimetryRecCovarianceModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbSARPolarimetryRecCovarianceModuleGUI.h"

// include the OTB/ITK elements
#include "otbReciprocalCovarianceToReciprocalCoherencyImageFilter.h"
#include "otbReciprocalCovarianceToCoherencyDegreeImageFilter.h"

namespace otb
{
/** \class SARPolarimetryRecCovarianceModule
 *  \brief This is the SARPolarimetry module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT SARPolarimetryRecCovarianceModule
  : public Module, public SARPolarimetryRecCovarianceModuleGUI
{
public:
  /** Standard class typedefs */
  typedef SARPolarimetryRecCovarianceModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SARPolarimetryRecCovarianceModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Complex_VectorImage ComplexVectorImageType;

  /** Filters typedefs */
  typedef ReciprocalCovarianceToReciprocalCoherencyImageFilter<ComplexVectorImageType, ComplexVectorImageType> ReciprocalCovarianceToReciprocalCoherencyImageFilterType;
  typedef ReciprocalCovarianceToCoherencyDegreeImageFilter<ComplexVectorImageType, ComplexVectorImageType>     ReciprocalCovarianceToCoherencyDegreeImageFilterType;


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
  SARPolarimetryRecCovarianceModule();

  /** Destructor */
  virtual ~SARPolarimetryRecCovarianceModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Check/uncheck all */
  virtual void CheckAll(bool);

  /** Check inputs configuration. */
  void CheckInputs();

private:
  SARPolarimetryRecCovarianceModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ComplexVectorImageType::Pointer m_InputImage;

  ReciprocalCovarianceToReciprocalCoherencyImageFilterType::Pointer       m_ReciprocalCovarianceToReciprocalCoherencyImageFilter;
  ReciprocalCovarianceToCoherencyDegreeImageFilterType::Pointer m_ReciprocalCovarianceToCoherencyDegreeImageFilter;

  };

} // End namespace otb

#endif
