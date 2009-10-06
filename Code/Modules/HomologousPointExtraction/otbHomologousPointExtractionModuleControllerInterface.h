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

#ifndef __otbHomologousPointExtractionModuleControllerInterface_h
#define __otbHomologousPointExtractionModuleControllerInterface_h

#include "itkObject.h"
#include "otbHomologousPointExtractionModuleModel.h"

namespace otb
{
class ITK_EXPORT HomologousPointExtractionModuleControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef HomologousPointExtractionModuleControllerInterface Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef HomologousPointExtractionModuleModel           ModelType;


  /** Standard type macros */
  itkTypeMacro(HomologousPointExtractionModuleControllerInterface,Superclass);

  /** Users actions */
  //virtual void GenerateLabeled(bool t) = 0;


protected:
  /** Constructor */
  HomologousPointExtractionModuleControllerInterface()
  {
  }
  /** Destructor */
  ~HomologousPointExtractionModuleControllerInterface() {};


private:
  HomologousPointExtractionModuleControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
