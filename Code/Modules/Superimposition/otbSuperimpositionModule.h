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

// include the OTB elements
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbGenericRSTransform.h"
#include "otbStreamingResampleImageFilter.h"
#include "otbPerBandVectorImageFilter.h"

namespace otb
{
/** \class SuperimpositionModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT SuperimpositionModule
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
  itkTypeMacro(SuperimpositionModule,Module);

  /** OTB typedefs */
  /// Dataset
  typedef VectorImage<double,2>         VectorImageType;
  typedef Image<double,2>               ImageType;
  typedef GenericRSTransform<>          TransformType; 
  typedef StreamingResampleImageFilter<ImageType,ImageType,double>                      ResampleFilterType;
  typedef PerBandVectorImageFilter<VectorImageType,VectorImageType, ResampleFilterType> PerBandFilterType;
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

private:
  SuperimpositionModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ResampleFilterType::Pointer m_Resampler;
  PerBandFilterType::Pointer  m_PerBanderFilter;
  TransformType::Pointer      m_Transform;
};


} // End namespace otb

#endif
