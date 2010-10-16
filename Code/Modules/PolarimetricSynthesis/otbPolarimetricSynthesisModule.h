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
#ifndef __otbPolarimetricSynthesisModule_h
#define __otbPolarimetricSynthesisModule_h

// include the base class
#include "otbModule.h"
#include "otbEventsListener.h"
#include "otbTypeManager.h"


#include "otbPolarimetricSynthesisController.h"
#include "otbPolarimetricSynthesisModel.h"
#include "otbPolarimetricSynthesisView.h"

#include "otbRealAndImaginaryImageToComplexImageFilter.h"
#include "otbRealImageToComplexImageFilter.h"
#include "otbImaginaryImageToComplexImageFilter.h"

namespace otb
{
/** \class PolarimetricSynthesisModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT PolarimetricSynthesisModule
  : public Module, public EventsListener<std::string>
{
public:
  /** Standard class typedefs */
  typedef PolarimetricSynthesisModule      Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(PolarimetricSynthesisModule, Module);


  /** MVC typedefs */
  typedef otb::PolarimetricSynthesisController ControllerType;
  typedef otb::PolarimetricSynthesisModel      ModelType;
  typedef otb::PolarimetricSynthesisView       ViewType;


  /** Input typedef */
  /** Data types */
  // Convenient typedefs
  typedef TypeManager::Floating_Point_Precision     Floating_Point_PrecisionType;
  typedef TypeManager::Floating_Point_Image         ImageType;
  typedef TypeManager::Floating_Point_Complex_Image ComplexImageType;

  typedef otb::RealAndImaginaryImageToComplexImageFilter<ImageType,ImageType,
              ComplexImageType>  RealAndImaginaryImageToComplexImageFilterType;
  typedef otb::RealImageToComplexImageFilter<ImageType,
              ComplexImageType>  RealImageToComplexImageFilterType;
  typedef otb::ImaginaryImageToComplexImageFilter<ImageType,
              ComplexImageType>  ImaginaryImageToComplexImageFilterType;

protected:
  /** Constructor */
  PolarimetricSynthesisModule();

  /** Destructor */
  virtual ~PolarimetricSynthesisModule();

  /** Notify Monteverdi application that PolarimetricSynthesis has a result */
  void Notify(const std::string & event);

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  PolarimetricSynthesisModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /**Combine real and imaginary image to create a complex image */
  ComplexImageType::Pointer CombineData(unsigned int i,
                                        ImageType::Pointer & realImage,
                                        ImageType::Pointer & imagImage);


  ControllerType::Pointer m_Controller;
  ViewType::Pointer m_View;
  ModelType::Pointer m_Model;

  std::vector<RealImageToComplexImageFilterType::Pointer> realToComplexFilter;
  std::vector<ImaginaryImageToComplexImageFilterType::Pointer> imaginaryToComplexFilter;
  std::vector<RealAndImaginaryImageToComplexImageFilterType::Pointer> realAndImaginaryToComplexFilter;
};

} // End namespace otb

#endif
