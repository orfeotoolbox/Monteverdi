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
#ifndef __otbMonteverdiControllerInterface_h
#define __otbMonteverdiControllerInterface_h

#include "otbMonteverdiModel.h"
#include "itkProcessObject.h"
#include "otbImageWidgetController.h"

namespace otb
{
class ITK_EXPORT  MonteverdiControllerInterface
: public itk::ProcessObject
{
public:
  /** Standard class typedefs */
  typedef MonteverdiControllerInterface        Self;
  typedef itk::ProcessObject                   Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;

  typedef MonteverdiModel                      ModelType;
  typedef ModelType::ModuleDescriptorMapType   ModuleDescriptorMapType;



  /** Standard type macros */
  itkTypeMacro(MonteverdiControllerInterface,Superclass);

  /** Users actions */
  virtual void CreateModuleByKey(const char * modulekey)=0;
  virtual const ModuleDescriptorMapType & GetRegisteredModuleDescriptors()=0;



protected:
  /** Constructor */
  MonteverdiControllerInterface()
  {
    m_Model = ModelType::GetInstance();
  }
  /** Destructor */
  ~MonteverdiControllerInterface() {};

  /** The instance of the model (for commodity) */
  ModelType * m_Model;

private:
  MonteverdiControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
