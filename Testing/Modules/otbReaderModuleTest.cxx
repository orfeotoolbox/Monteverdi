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

#include "otbReaderModule.h"

#include "otbVectorImage.h"
#include "otbImageFileWriter.h"

int main(int argc, char* argv[])
{
  otb::ReaderModule::Pointer specificModule = otb::ReaderModule::New();
  otb::Module::Pointer module = specificModule.GetPointer();
  
  std::cout<<"Module: "<<module<<std::endl;

  // Put in the tests
  typedef otb::VectorImage<double,2>      ImageType;
  typedef otb::ImageFileWriter<ImageType> WriterType;
  
  module->Start();
  Fl::check();

  // Simulate file chooser and ok callback
  specificModule->vFilePath->value(argv[1]);
  specificModule->bOk->do_callback();

  otb::DataObjectWrapper wrapperOut = module->GetDataByKey("OutputDataSet");

  std::cout<<"Output wrapper: "<<wrapperOut<<std::endl;

  if(wrapperOut.GetDataType()=="Floating_Point_VectorImage")
    {

    ImageType::Pointer outImage = dynamic_cast<ImageType *>(wrapperOut.GetDataObject());

    //Write the image
    WriterType::Pointer  writer = WriterType::New();
    writer->SetFileName(argv[2]);
    writer->SetInput(outImage);
    writer->Update();
    }

  return EXIT_SUCCESS;

}

