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
#ifndef __otbSARPolarimetryModule_h
#define __otbSARPolarimetryModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbSARPolarimetryModuleGUI.h"

// include the OTB/ITK elements
#include "otbSinclairReciprocalImageFilter.h"
#include "otbSinclairToReciprocalCovarianceMatrixFunctor.h"
#include "otbSinclairToReciprocalCircularCovarianceMatrixFunctor.h"
#include "otbSinclairToReciprocalCoherencyMatrixFunctor.h"

namespace otb
{
/** \class SARPolarimetryModule
 *  \brief This is the SARPolarimetry module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT SARPolarimetryModule
  : public Module, public SARPolarimetryModuleGUI
{
public:
  /** Standard class typedefs */
  typedef SARPolarimetryModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SARPolarimetryModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Image ImageType;
  typedef TypeManager::Floating_Point_Complex_Image ComplexImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef TypeManager::Floating_Point_Complex_VectorImage ComplexVectorImageType;

  typedef ComplexImageType::PixelType ComplexImagePixelType;
  typedef ComplexVectorImageType::PixelType ComplexVectorImagePixelType;

  /** Functors definitions */
  typedef Functor::SinclairToReciprocalCovarianceMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType> SinclairToReciprocalCovarianceMatrixFunctorType;
  typedef Functor::SinclairToReciprocalCircularCovarianceMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType> SinclairToReciprocalCircularCovarianceMatrixFunctorType;
  typedef Functor::SinclairToReciprocalCoherencyMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType> SinclairToReciprocalCoherencyMatrixFunctorType;
   
  /** Filters typedefs */
  typedef SinclairReciprocalImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToReciprocalCovarianceMatrixFunctorType>         SinclairToReciprocalCovarianceMatrixFilterType; 
  typedef SinclairReciprocalImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToReciprocalCircularCovarianceMatrixFunctorType> SinclairToReciprocalCircularCovarianceMatrixFilterType;
  typedef SinclairReciprocalImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToReciprocalCoherencyMatrixFunctorType>          SinclairToReciprocalCoherencyMatrixFilterType;

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
  SARPolarimetryModule();

  /** Destructor */
  virtual ~SARPolarimetryModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Check/uncheck all */
  virtual void CheckAll(bool);

  /** Check inputs configuration. */
  void CheckInputs();

  /** Init the GUI accroding to the available inputs. */
  void InitChecks();

private:
  SARPolarimetryModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Input images */
  ComplexImageType::Pointer m_HHImage;
  ComplexImageType::Pointer m_HVImage;
  ComplexImageType::Pointer m_VHImage;
  ComplexImageType::Pointer m_VVImage;

  /** Filters */
  SinclairToReciprocalCovarianceMatrixFilterType::Pointer m_SinclairToReciprocalCovarianceMatrixFilter;
  SinclairToReciprocalCircularCovarianceMatrixFilterType::Pointer m_SinclairToReciprocalCircularCovarianceMatrixFilter;
  SinclairToReciprocalCoherencyMatrixFilterType::Pointer m_SinclairToReciprocalCoherencyMatrixFilter;
   
  };

} // End namespace otb

#endif
