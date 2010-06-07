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
#ifndef __otbVectorizationControllerInterface_h
#define __otbVectorizationControllerInterface_h

#include "itkObject.h"
#include "otbVectorizationModel.h"

namespace otb
{
class ITK_EXPORT VectorizationControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef VectorizationControllerInterface Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef VectorizationModel               ModelType;

  /** Standard type macros */
  itkTypeMacro(VectorizationControllerInterface,Superclass);

  /** Users actions */

protected:
  /** Constructor */
  VectorizationControllerInterface() {}
  /** Destructor */
  ~VectorizationControllerInterface() {};


private:
  VectorizationControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
