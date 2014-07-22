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
#ifndef __otbSuperimpositionModule_h
#define __otbSuperimpositionModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbSuperimpositionModuleGUI.h"
#include "otbGenericRSResampleImageFilter.h"
#include "otbImageToVectorImageCastFilter.h"

#include "otbSuperimpositionEnum.h"

#include "otbDEMHandler.h"

namespace otb
{
/** \class SuperimpositionModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT SuperimpositionModule
  : public Module, public SuperimpositionModuleGUI
{
public:
  /** Standard class typedefs */
  typedef SuperimpositionModule         Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SuperimpositionModule, Module);

  /** OTB typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Image       ImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  
  /** Resampler */
  typedef GenericRSResampleImageFilter<VectorImageType,
                                       VectorImageType>    ResampleFilterType;
  /** Cast otb::Image types*/
  typedef ImageToVectorImageCastFilter<ImageType,
                                       VectorImageType>    CastImageFilterType;
  
protected:
  /** Constructor */
  SuperimpositionModule();
  /** Destructor */
  virtual ~SuperimpositionModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void Ok();
  virtual void Browse();
  virtual void Cancel();

  /** Interpolator selection callbacks*/
  virtual void SetInterpolatorType(InterpolatorType interp);
  virtual InterpolatorType GetInterpolatorType();
  virtual void UpdateInterpolator();

private:
  SuperimpositionModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ResampleFilterType::Pointer  m_Resampler;
  CastImageFilterType::Pointer m_CastFixedFilter;
  CastImageFilterType::Pointer m_CastMovingFilter;

  InterpolatorType     m_InterpType;
};

} // End namespace otb

#endif
