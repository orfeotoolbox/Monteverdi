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
#ifndef __otbSARPolarimetrySinclairModule_h
#define __otbSARPolarimetrySinclairModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbSARPolarimetrySinclairModuleGUI.h"

// include the OTB/ITK elements
#include "otbSinclairReciprocalImageFilter.h"
#include "otbSinclairToReciprocalCovarianceMatrixFunctor.h"
#include "otbSinclairToReciprocalCircularCovarianceMatrixFunctor.h"
#include "otbSinclairToReciprocalCoherencyMatrixFunctor.h"

#include "otbSinclairImageFilter.h"
#include "otbSinclairToCovarianceMatrixFunctor.h"
#include "otbSinclairToCircularCovarianceMatrixFunctor.h"
#include "otbSinclairToCoherencyMatrixFunctor.h"
#include "otbSinclairToMuellerMatrixFunctor.h"

namespace otb
{
/** \class SARPolarimetrySinclairModule
 *  \brief This is the SARPolarimetry module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT SARPolarimetrySinclairModule
  : public Module, public SARPolarimetrySinclairModuleGUI
{
public:
  /** Standard class typedefs */
  typedef SARPolarimetrySinclairModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SARPolarimetrySinclairModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Image ImageType;
  typedef TypeManager::Floating_Point_Complex_Image ComplexImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef TypeManager::Floating_Point_Complex_VectorImage ComplexVectorImageType;

  typedef ComplexImageType::PixelType ComplexImagePixelType;
  typedef VectorImageType::PixelType VectorImagePixelType;
  typedef ComplexVectorImageType::PixelType ComplexVectorImagePixelType;

  /** Functors definitions */
  typedef Functor::SinclairToReciprocalCovarianceMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType>         SinclairToReciprocalCovarianceMatrixFunctorType;
  typedef Functor::SinclairToReciprocalCircularCovarianceMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType> SinclairToReciprocalCircularCovarianceMatrixFunctorType;
  typedef Functor::SinclairToReciprocalCoherencyMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType>          SinclairToReciprocalCoherencyMatrixFunctorType;
   
  typedef Functor::SinclairToCovarianceMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType>         SinclairToCovarianceMatrixFunctorType;
  typedef Functor::SinclairToCircularCovarianceMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType> SinclairToCircularCovarianceMatrixFunctorType;
  typedef Functor::SinclairToCoherencyMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexVectorImagePixelType>          SinclairToCoherencyMatrixFunctorType;
  typedef Functor::SinclairToMuellerMatrixFunctor<ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, ComplexImagePixelType, VectorImagePixelType>                   SinclairToMuellerMatrixFunctorType;

  /** Filters typedefs */
  typedef SinclairReciprocalImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToReciprocalCovarianceMatrixFunctorType>         SinclairToReciprocalCovarianceMatrixFilterType; 
  typedef SinclairReciprocalImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToReciprocalCircularCovarianceMatrixFunctorType> SinclairToReciprocalCircularCovarianceMatrixFilterType;
  typedef SinclairReciprocalImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToReciprocalCoherencyMatrixFunctorType>          SinclairToReciprocalCoherencyMatrixFilterType;

  typedef otb::SinclairImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToCovarianceMatrixFunctorType>         SinclairToCovarianceMatrixFilterType;
  typedef otb::SinclairImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToCircularCovarianceMatrixFunctorType> SinclairToCircularCovarianceMatrixFilterType;
  typedef otb::SinclairImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexImageType, ComplexVectorImageType, SinclairToCoherencyMatrixFunctorType>          SinclairToCoherencyMatrixFilterType;
  typedef otb::SinclairImageFilter<ComplexImageType, ComplexImageType, ComplexImageType, ComplexImageType, VectorImageType, SinclairToMuellerMatrixFunctorType>                   SinclairToMuellerMatrixFilterType;

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
  SARPolarimetrySinclairModule();

  /** Destructor */
  virtual ~SARPolarimetrySinclairModule();

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
  SARPolarimetrySinclairModule(const Self&); //purposely not implemented
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

  SinclairToCovarianceMatrixFilterType::Pointer m_SinclairToCovarianceMatrixFilter;
  SinclairToCircularCovarianceMatrixFilterType::Pointer m_SinclairToCircularCovarianceMatrixFilter ;
  SinclairToCoherencyMatrixFilterType::Pointer m_SinclairToCoherencyMatrixFilter;
  SinclairToMuellerMatrixFilterType::Pointer m_SinclairToMuellerMatrixFilter;
  };

} // End namespace otb

#endif
