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

#include <cstdlib>
#include "otbDataDescriptor.h"
#include "otbDataObjectWrapper.h"
#include "otbImage.h"

int otbDataDescriptorTest(int argc, char * argv[])
{
  // Default constructor and call to the set method
  otb::DataDescriptor desc1;
  desc1.Set("Floating_Point_Image", "InputImage", "This is the input image");

  // Access parameters
  std::cout << "Desc1: DataType = " << desc1.GetDataType() << ", DataKey = " << desc1.GetDataKey() <<
  ", DataDescripion = " << desc1.GetDataDescription() << std::endl;

  // Constructor with parameters
  otb::DataDescriptor desc2("Floating_Point_Image", "InputImage", "This is the input image");

  // Access parameters
  std::cout << "Desc2: DataType = " << desc2.GetDataType() << ", DataKey = " << desc1.GetDataKey() <<
  ", DataDescripion = " << desc2.GetDataDescription() << std::endl;

  // Testing the << operator
  std::cout << "Testing << operator: " << desc2 << std::endl;

  // Checking type compatibility
  std::cout << "IsTypeCompatible(Floating_Point_Image): " << desc2.IsTypeCompatible("Floating_Point_Image") <<
  std::endl;

  // Create data
  typedef otb::TypeManager::Floating_Point_Image ImageType;
  otb::DataObjectWrapper wrapper = otb::DataObjectWrapper::Create(ImageType::New());

  // Add data
  desc2.AddData(wrapper);

  // Number of data
  std::cout << "Number of data: " << desc2.GetNumberOfData() << std::endl;

  // Retrieve nth data
  std::cout << "Data retrieved: " << desc2.GetNthData(0) << std::endl;

  // Clear data
  desc2.ClearData();

  // Number of data
  std::cout << "Number of data after clearing: " << desc2.GetNumberOfData() << std::endl;

  return EXIT_SUCCESS;
}
