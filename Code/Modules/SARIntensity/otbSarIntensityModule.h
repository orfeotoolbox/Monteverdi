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
#ifndef __otbSarIntensityModule_h
#define __otbSarIntensityModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbImage.h"
#include "itkComplexToModulusImageFilter.h"
#include "itkSquareImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

namespace otb
{
/** \class SarIntensityModule
 *  \brief This is the SarIntensity module
 *
 * Description of the module.
 *
 */

namespace Functor
{
template <class TInput, class TOutput> class Log10Functor
{
public:
  TOutput operator()(const TInput & input) const
  {
    TOutput resp = static_cast<TOutput>(10. * vcl_log10(input));
   
    return resp;
  }
};
}


class ITK_EXPORT SarIntensityModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef SarIntensityModule           Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SarIntensityModule,Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Precision      FloatingPrecision;
  typedef TypeManager::Floating_Point_Complex_Image  ComplexImageType;
  typedef TypeManager::Floating_Point_Image          FloatingImageType;
  
  typedef itk::ComplexToModulusImageFilter<ComplexImageType,FloatingImageType> ModulusFilterType;
  typedef itk::SquareImageFilter<FloatingImageType,FloatingImageType>          SquareFilterType;
  typedef Functor::Log10Functor<FloatingPrecision,FloatingPrecision>           LogFunctorType;
  typedef itk::UnaryFunctorImageFilter<FloatingImageType,
                                       FloatingImageType,
                                       LogFunctorType>                         LogFilterType;

protected:
  /** Constructor */
  SarIntensityModule();

  /** Destructor */
  virtual ~SarIntensityModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  SarIntensityModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ModulusFilterType::Pointer m_ModulusFilter;
  SquareFilterType::Pointer  m_SquareFilter;
  LogFilterType::Pointer     m_LogFilter;
};


} // End namespace otb

#endif
