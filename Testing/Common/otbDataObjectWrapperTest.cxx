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

#include "otbDataObjectWrapper.h"
#include "otbImage.h"

int main(int argc, char * argv[])
{
  // DataObject typedef
  typedef otb::Image<double,2> FloatImageType;

  // Default constructor and call to the set method
  otb::DataObjectWrapper wrapper1;
  wrapper1.Set(FloatImageType::New());

  // Access parameters
  std::cout<<"Wrapper1: DataObject = "<<wrapper1.GetDataObject()<<", DataType = "<<wrapper1.GetDataType()<<std::endl;

  // Constructor with parameters
  otb::DataObjectWrapper wrapper2 = otb::DataObjectWrapper::Create(FloatImageType::New());

  // Access parameters
  std::cout<<"Wrapper2: DataObject = "<<wrapper2.GetDataObject()<<", DataType = "<<wrapper2.GetDataType()<<std::endl;

  // Testing the << operator
  std::cout<<"Testing << operator: "<<wrapper2<<std::endl;

  return EXIT_SUCCESS;
}
