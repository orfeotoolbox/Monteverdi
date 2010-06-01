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

#include "otbImageFileReader.h"
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbSupervisedClassificationModel.h"


int otbSupervisedClassificationModelTest(int argc, char* argv[])
{
  otb::SupervisedClassificationModel::Pointer spclModel = otb::SupervisedClassificationModel::New();
  otb::SupervisedClassificationModel::Pointer model = spclModel.GetPointer();
  
  std::cout<<"Model: "<< model <<std::endl;

  if(argc == 1)
    {
    return EXIT_SUCCESS;
    }

    
  std::string infname = std::string(argv[1]);

  typedef float PixelType;
  typedef unsigned short LabeledPixelType;

  typedef otb::VectorImage<PixelType,2> ImageType;

  typedef otb::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();

  reader->SetFileName(infname);
  reader->UpdateOutputInformation();

  spclModel->SetImage( reader->GetOutput() );

  if( argc == 2 )
    {
    return EXIT_SUCCESS;
    }

  

  return EXIT_SUCCESS;

}

