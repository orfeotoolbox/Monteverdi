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
#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"
#include "otbSupervisedClassificationModel.h"


int otbSupervisedClassificationModelTest(int argc, char* argv[])
{
  otb::SupervisedClassificationModel::Pointer spclModel = otb::SupervisedClassificationModel::New();
  otb::SupervisedClassificationModel::Pointer model = spclModel.GetPointer();
  
  std::cout<<"Model: "<< model <<std::endl;

  return EXIT_SUCCESS;
  
}


int otbSupervisedClassificationModelSetImageTest(int argc, char* argv[])
{
  otb::SupervisedClassificationModel::Pointer spclModel = otb::SupervisedClassificationModel::New();
  otb::SupervisedClassificationModel::Pointer model = spclModel.GetPointer();

  std::string infname = std::string(argv[1]);

  typedef float                                             PixelType;

  typedef otb::VectorImage<PixelType,2>                     ImageType;

  typedef otb::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();

  reader->SetFileName(infname);
  reader->UpdateOutputInformation();

  spclModel->SetImage( reader->GetOutput() );

  return EXIT_SUCCESS;

}

int otbSupervisedClassificationModelSetLabeledImageTest(int argc, char* argv[])
{
  otb::SupervisedClassificationModel::Pointer spclModel = otb::SupervisedClassificationModel::New();
  otb::SupervisedClassificationModel::Pointer model = spclModel.GetPointer();


  std::string labeledfname = std::string(argv[1]);

  typedef int                                               LabeledPixelType;
  typedef otb::Image<LabeledPixelType,2>                    LabeledImageType;
  typedef otb::ImageFileReader<LabeledImageType>            LabeledImageReaderType;
  LabeledImageReaderType::Pointer labeledReader = LabeledImageReaderType::New();

  labeledReader->SetFileName(labeledfname);
  labeledReader->UpdateOutputInformation();

  spclModel->SetLabeledImage( labeledReader->GetOutput() );

  return EXIT_SUCCESS;

}

int otbSupervisedClassificationModelSetVectorDataTest(int argc, char* argv[])
{
  otb::SupervisedClassificationModel::Pointer spclModel = otb::SupervisedClassificationModel::New();
  otb::SupervisedClassificationModel::Pointer model = spclModel.GetPointer();

    std::string infname = std::string(argv[1]);

  typedef float                                             PixelType;

  typedef otb::VectorImage<PixelType,2>                     ImageType;

  typedef otb::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();

  reader->SetFileName(infname);
  reader->UpdateOutputInformation();

  spclModel->SetImage( reader->GetOutput() );
  
  std::string vectorfname = std::string(argv[2]);
  
  typedef otb::VectorData<float, 2>                 VectorDataType;
  typedef otb::VectorDataFileReader<VectorDataType> VectorDataReaderType;

  VectorDataReaderType::Pointer vectorReader = VectorDataReaderType::New();
  vectorReader->SetFileName(vectorfname);
  vectorReader->Update();

  spclModel->SetVectorROIs( vectorReader->GetOutput() );

  return EXIT_SUCCESS;
}

int otbSupervisedClassificationModelTrainTest(int argc, char* argv[])
{
  otb::SupervisedClassificationModel::Pointer spclModel = otb::SupervisedClassificationModel::New();
  otb::SupervisedClassificationModel::Pointer model = spclModel.GetPointer();

    std::string infname = std::string(argv[1]);

  typedef float                                             PixelType;

  typedef otb::VectorImage<PixelType,2>                     ImageType;

  typedef otb::ImageFileReader<ImageType> ImageReaderType;
  ImageReaderType::Pointer reader = ImageReaderType::New();

  reader->SetFileName(infname);
  reader->UpdateOutputInformation();

  spclModel->SetImage( reader->GetOutput() );
  
  std::string vectorfname = std::string(argv[2]);
  
  typedef otb::VectorData<float, 2>                 VectorDataType;
  typedef otb::VectorDataFileReader<VectorDataType> VectorDataReaderType;

  VectorDataReaderType::Pointer vectorReader = VectorDataReaderType::New();
  vectorReader->SetFileName(vectorfname);
  vectorReader->Update();

  spclModel->SetVectorROIs( vectorReader->GetOutput() );

  spclModel->Train();

  return EXIT_SUCCESS;
}
