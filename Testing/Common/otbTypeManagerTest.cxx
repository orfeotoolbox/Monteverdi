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
#include "otbTypeManager.h"
#include "itkPointSet.h"
#include "itkExceptionObject.h"

int otbTypeManagerTest(int argc, char * argv[])
{
  // Retrieve the TypeManger instance
  otb::TypeManager::Pointer typeManager = otb::TypeManager::GetInstance();

  // Print the already registered types
  std::cout << typeManager << std::endl;

  // Register a new type
  typeManager->RegisterType<itk::PointSet<double, 2> >("Point_Set");

  // Print types again
  std::cout << typeManager << std::endl;

  // Try to register twice the same type

  typeManager->RegisterType<itk::PointSet<double, 2> >("Point_Set");

  // Try to get the name of a type
  std::cout << "The name of the pointSet type is: " <<
  typeManager->GetTypeName<itk::PointSet<double, 2> >() << std::endl;

  // Try to get the name of an unregistered type
  std::string name = typeManager->GetTypeName<itk::PointSet<float, 2> >();

  // Try to register a type with an already existing name
  typeManager->RegisterType<itk::PointSet<float, 2> >("Point_Set");

  return EXIT_SUCCESS;
}
