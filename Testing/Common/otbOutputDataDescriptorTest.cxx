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
#include "otbOutputDataDescriptor.h"


int main(int argc, char * argv[])
{
  // Default constructor and call to the set method
  otb::OutputDataDescriptor desc1;
  desc1.Set("Floating_Point_Image","OutputImage","This is the output image");
  desc1.SetNumberOfData(1);

  // Access parameters
  std::cout<<"Desc1: Number of data = "<<desc1.GetNumberOfData()<<std::endl;

  // Constructor with parameters
  otb::OutputDataDescriptor desc2("Floating_Point_Image","OutputImage","This is the output image");

  // Access parameters
   std::cout<<"Desc2: number of data = "<<desc2.GetNumberOfData()<<std::endl;
   desc1.SetNumberOfData(5);

  // Testing the << operator
  std::cout<<"Testing << operator: "<<desc2<<std::endl;

  return EXIT_SUCCESS;
}
