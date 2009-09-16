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

int main(int argc, char * argv[])
{
  // Default constructor and call to the set method
  otb::DataDescriptor desc1;
  desc1.Set("Floating_Point_Image","InputImage","This is the input image");

  // Access parameters
  std::cout<<"Desc1: DataType = "<<desc1.GetDataType()<<", DataKey = "<<desc1.GetDataKey()<<", DataDescripion = "<<desc1.GetDataDescription()<<std::endl;

  // Constructor with parameters
  otb::DataDescriptor desc2("Floating_Point_Image","InputImage","This is the input image");

  // Access parameters
  std::cout<<"Desc2: DataType = "<<desc2.GetDataType()<<", DataKey = "<<desc1.GetDataKey()<<", DataDescripion = "<<desc2.GetDataDescription()<<std::endl;

  // Testing the << operator
  std::cout<<"Testing << operator: "<<desc2<<std::endl;

  return EXIT_SUCCESS;
}
