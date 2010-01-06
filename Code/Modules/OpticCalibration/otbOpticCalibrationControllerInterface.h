/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbOpticCalibrationControllerInterface_h
#define __otbOpticCalibrationControllerInterface_h

#include "otbOpticCalibrationModel.h"
#include "itkProcessObject.h"
#include "otbImageWidgetController.h"

namespace otb
{
class ITK_EXPORT  OpticCalibrationControllerInterface
: public itk::ProcessObject
{
public:
  /** Standard class typedefs */
  typedef OpticCalibrationControllerInterface Self;
  typedef itk::ProcessObject                  Superclass;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::SmartPointer<const Self>       ConstPointer;

  typedef OpticCalibrationModel               ModelType;
  typedef ModelType::ImageType                ImageType;
  typedef ModelType::ImagePointerType         ImagePointerType;
  typedef ModelType::IndexType                IndexType;
  typedef ModelType::SizeType                 SizeType;

  /** NewVisu */
  typedef ImageWidgetController               VisuControllerType;
  typedef VisuControllerType::Pointer         VisuControllerPointerType;


  /** Standard type macros */
  itkTypeMacro(OpticCalibrationControllerInterface,Superclass);

  /** Users actions */
  virtual void LinkPixelDescriptors() =0;
  virtual void UpdateRGBChannelOrder(unsigned int red,unsigned int green,unsigned int blue) =0;
  virtual void UpdateGrayscaleChannel(unsigned int channel) =0;
  virtual void UpdateDisplay(EnumImageType pType) =0;
  virtual void UpdateCorrectionParameters() =0;
  virtual void UpdateRadiativeTerms( bool updateIm ) =0;
  virtual VisuControllerPointerType GetVisuController() =0;
  virtual VisuControllerPointerType GetRVisuController() =0;
  virtual void QuitModel() =0;

protected:
  /** Constructor */
  OpticCalibrationControllerInterface() {};
  ~OpticCalibrationControllerInterface() {};

private:
  OpticCalibrationControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
