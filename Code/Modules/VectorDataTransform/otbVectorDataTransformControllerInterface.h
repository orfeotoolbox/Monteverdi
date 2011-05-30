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
#ifndef __otbVectorDataTransformControllerInterface_h
#define __otbVectorDataTransformControllerInterface_h

#include "itkObject.h"
#include "otbVectorDataTransformModel.h"

namespace otb
{
class VectorDataTransformControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef VectorDataTransformControllerInterface       Self;
  typedef itk::Object                                  Superclass;
  typedef itk::SmartPointer<Self>                      Pointer;
  typedef itk::SmartPointer<const Self>                ConstPointer;
  
  /** Standard type macros */
  itkTypeMacro(VectorDataTransformControllerInterface, Superclass);

  /** Users actions */
  virtual void Exit() = 0;
  virtual void ApplyTransformToVectorData(double scale, double angle,
                                          double translationX,
                                          double translationY,
                                          double centerX, double centerY) = 0;

  virtual void SetModel(VectorDataTransformModel * model)
  {
    m_Model = model;
  }
  
protected:
  /** Constructor */
  VectorDataTransformControllerInterface()
  {}
  /** Destructor */
  ~VectorDataTransformControllerInterface() {}

  /** The instance of the model (for commodity) */
  VectorDataTransformModel * m_Model;

private:
  VectorDataTransformControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
