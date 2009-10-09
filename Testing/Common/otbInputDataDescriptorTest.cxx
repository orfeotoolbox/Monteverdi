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
#include "otbInputDataDescriptor.h"


int otbInputDataDescriptorTest(int argc, char * argv[])
{
  // Case single, mandatory
  otb::InputDataDescriptor desc1;
  desc1.Set("Floating_Point_VectorImage","InputImage","This is the input image");
  desc1.AddSupportedType("Vector_Data");
  desc1.AddSupportedType("Vector_Data");

  // Check type compatibility
  std::cout<<"IsTypeCompatible: Vector_Data "<<desc1.IsTypeCompatible("Vector_Data")<<std::endl;
  std::cout<<"IsTypeCompatible: Data "<<desc1.IsTypeCompatible("Data")<<std::endl;

  std::cout<<"Desc1 consistent:" << desc1.IsConsistent()<<std::endl;
  desc1.AddData(otb::DataObjectWrapper::Create(otb::VectorImage<double,2>::New()));
  desc1.AddData(otb::DataObjectWrapper::Create(otb::VectorImage<double,2>::New()));
  std::cout<<"Desc1 consistent with data:" << desc1.IsConsistent()<<std::endl;

  // Access parameters
  std::cout<<"Desc1: Optional = "<<desc1.IsOptional()<<", Multiple = "<<desc1.IsMultiple()<<std::endl;

  // Case single, optional
  // Constructor with parameters
  otb::InputDataDescriptor desc2("Floating_Point_VectorImage","InputImage","This is the input image");
  desc2.SetOptional(true);
  std::cout<<"Desc2 consistent:" << desc2.IsConsistent()<<std::endl;

  // Testing the << operator
  std::cout<<"Testing << operator: "<<desc2<<std::endl;

  // Case multiple, mandatory
  otb::InputDataDescriptor desc3("Floating_Point_VectorImage","InputImage","This is the input image");
  desc3.SetMultiple(true);
  std::cout<<"Desc3 consistent:" << desc3.IsConsistent()<<std::endl;
  desc3.AddData(otb::DataObjectWrapper::Create(otb::VectorImage<double,2>::New()));
  desc3.AddData(otb::DataObjectWrapper::Create(otb::VectorImage<double,2>::New()));
  std::cout<<"Desc3 consistent with data:" << desc3.IsConsistent()<<std::endl;

  // Testing the << operator
  std::cout<<"Testing << operator: "<<desc3<<std::endl;
  
  // Case multiple, optional
  otb::InputDataDescriptor desc4("Floating_Point_VectorImage","InputImage","This is the input image");
  desc4.SetMultiple(true);
  desc4.SetOptional(true);
  std::cout<<"Desc4 consistent:" << desc4.IsConsistent()<<std::endl;
  desc4.AddData(otb::DataObjectWrapper::Create(otb::VectorImage<double,2>::New()));
  desc4.AddData(otb::DataObjectWrapper::Create(otb::VectorImage<double,2>::New()));
  std::cout<<"Desc4 consistent with data:" << desc4.IsConsistent()<<std::endl;

  // Testing the << operator
  std::cout<<"Testing << operator: "<<desc4<<std::endl;
  
  return EXIT_SUCCESS;
}
