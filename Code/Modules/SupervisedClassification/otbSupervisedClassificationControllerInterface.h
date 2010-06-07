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
#ifndef __otbSupervisedClassificationControllerInterface_h
#define __otbSupervisedClassificationControllerInterface_h

#include "itkObject.h"
#include "itkContinuousIndex.h"
#include "otbSupervisedClassificationModel.h"

namespace otb
{
class ITK_EXPORT SupervisedClassificationControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef SupervisedClassificationControllerInterface Self;
  typedef itk::Object                                        Superclass;
  typedef itk::SmartPointer<Self>                            Pointer;
  typedef itk::SmartPointer<const Self>                      ConstPointer;

  typedef SupervisedClassificationModel               ModelType;
  typedef itk::ContinuousIndex<>              ContinuousIndexType;

  /** Standard type macros */
  itkTypeMacro(SupervisedClassificationControllerInterface,Superclass);

protected:
  /** Constructor */
  SupervisedClassificationControllerInterface() {}
  /** Destructor */
  ~SupervisedClassificationControllerInterface() {};


private:
  SupervisedClassificationControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};
} // end namespace otb


#endif
