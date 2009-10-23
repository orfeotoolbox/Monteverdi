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
#ifndef __otbChangeDetectionModule_h
#define __otbChangeDetectionModule_h

#include "otbModule.h"

// ChangeDetection Application
#include "otbInteractiveChangeDetection.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbListenerBase.h"

namespace otb
{
/** \class ChangeDetectionModule
 *  \brief
 *
 */

class ITK_EXPORT ChangeDetectionModule
  : public Module, public ListenerBase
  {
  public:
  /** Standard class typedefs */
  typedef ChangeDetectionModule         Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ChangeDetectionModule,Module);
  
  /** Input typedef */
  typedef InteractiveChangeDetection<double>         ChangeDetectionType;
  typedef ChangeDetectionType::ImageType             ImageType;
  typedef ChangeDetectionType::SingleImageType       SingleImageType;

  /** Cast SingleImage to ImageType (VectorImage) */
  typedef ImageToVectorImageCastFilter<SingleImageType,ImageType>     CastSingleImageFilter;

 /*  /\** Accessor to the ChangeDetection Instance *\/ */
  itkGetObjectMacro(ChangeDetection,ChangeDetectionType); 

protected:
  /** Constructor */
  ChangeDetectionModule();

  /** Destructor */
  virtual ~ChangeDetectionModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Notify Monteverdi application that orthorectification has a result */
  virtual void Notify();

private:
  ChangeDetectionModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The view
  ChangeDetectionType::Pointer        m_ChangeDetection;

  // Cast Filter
  CastSingleImageFilter::Pointer     m_CastFilter;
};


} // End namespace otb

#endif
