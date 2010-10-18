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
#ifndef __otbPolarimetricSynthesisControllerInterface_h
#define __otbPolarimetricSynthesisControllerInterface_h

#include "itkObject.h"
#include "otbPolarimetricSynthesisModel.h"
#include "itkIndex.h"

namespace otb
{
class PolarimetricSynthesisControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef PolarimetricSynthesisControllerInterface Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef itk::Index<2>                    IndexType;

  /** Standard type macros */
  itkTypeMacro(PolarimetricSynthesisControllerInterface,Superclass);

  /** Users actions */
  virtual void LoadImages() = 0;
  virtual void Save() = 0;
  virtual void ChangeRGB(bool value) = 0;
  virtual void ChangeGain(double value) = 0;
  virtual void ChangeGrayPsiI(double value) = 0;
  virtual void ChangeGrayPsiR(double value) = 0;
  virtual void ChangeGrayKhiI(double value) = 0;
  virtual void ChangeGrayKhiR(double value) = 0;
  virtual void ChangeGrayPolarizationMode(PolarizationMode mode) = 0;
  virtual void ChangeRedPsiI(double value) = 0;
  virtual void ChangeRedPsiR(double value) = 0;
  virtual void ChangeRedKhiI(double value) = 0;
  virtual void ChangeRedKhiR(double value) = 0;
  virtual void ChangeRedPolarizationMode(PolarizationMode mode) = 0;
  virtual void ChangeGreenPsiI(double value) = 0;
  virtual void ChangeGreenPsiR(double value) = 0;
  virtual void ChangeGreenKhiI(double value) = 0;
  virtual void ChangeGreenKhiR(double value) = 0;
  virtual void ChangeGreenPolarizationMode(PolarizationMode mode) = 0;
  virtual void ChangeBluePsiI(double value) = 0;
  virtual void ChangeBluePsiR(double value) = 0;
  virtual void ChangeBlueKhiI(double value) = 0;
  virtual void ChangeBlueKhiR(double value) = 0;
  virtual void ChangeBluePolarizationMode(PolarizationMode mode) = 0;
  virtual void IndexClicked(const IndexType& index) = 0;
  virtual void MouseIn() = 0;
  virtual void MouseOut() = 0;
  virtual void PixelHovered(const IndexType & index) = 0;
  virtual void SetModel(PolarimetricSynthesisModel * model)
  {
    m_Model = model;
  }

  virtual bool IsHEmissionMode() = 0;
  virtual bool IsVEmissionMode() = 0;


protected:
  /** Constructor */
  PolarimetricSynthesisControllerInterface() {}

  /** Destructor */
  ~PolarimetricSynthesisControllerInterface() {}

  /** The instance of the model (for commodity) */
  PolarimetricSynthesisModel::Pointer m_Model;

private:
  PolarimetricSynthesisControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
