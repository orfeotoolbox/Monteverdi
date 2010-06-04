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

#include "otbSupervisedClassificationModel.h"
#include "otbFltkFilterWatcher.h"


namespace otb
{
/** Initialize the singleton */
SupervisedClassificationModel::Pointer SupervisedClassificationModel::Instance = NULL;

/** Manage the singleton */
SupervisedClassificationModel::Pointer
SupervisedClassificationModel::
GetInstance()
{
  if (!Instance)
  {
    Instance = SupervisedClassificationModel::New();
  }
  return Instance;
}

void SupervisedClassificationModel::
Notify(ListenerBase * listener)
{
  listener->Notify();
}

SupervisedClassificationModel::
SupervisedClassificationModel() : m_MaxTrainingSize(100), m_MaxValidationSize(100), m_ValidationTrainingRatio(1.0), m_NumberOfClasses(2), m_CValue(4)
{
  m_InputImage = ImageType::New();
  m_LabeledImage = LabeledImageType::New();
  m_VectorROIs = VectorDataType::New();
  m_SampleGenerator = ListSampleGeneratorType::New();
  m_ModelEstimator = ModelEstimatorType::New();
}

SupervisedClassificationModel
::~SupervisedClassificationModel()
{
}


void
SupervisedClassificationModel
::SetImage(ImagePointerType image)
{
  m_OutputChanged = false;

  image->UpdateOutputInformation();
  m_InputImage = image;
  
  // Add input image to the filter

}

void
SupervisedClassificationModel
::SetLabeledImage(LabeledImagePointerType image)
{
  m_OutputChanged = false;

  image->UpdateOutputInformation();
  m_LabeledImage = image;

  //build ROIs from labeled image
}

void
SupervisedClassificationModel
::SetVectorROIs(VectorDataPointerType vectorData)
{
  m_OutputChanged = false;

  vectorData->UpdateOutputInformation();
  m_VectorROIs = vectorData;

  this->GenerateSamples();
  
}


void
SupervisedClassificationModel
::OK()
{
  // Set Output
  //m_Output->UpdateOutputInformation();
  
  m_OutputChanged = true;
  this->NotifyAll();
}

void
SupervisedClassificationModel
::GenerateSamples()
{
  m_SampleGenerator->SetMaxTrainingSize(m_MaxTrainingSize);
  m_SampleGenerator->SetMaxValidationSize(m_MaxValidationSize);
  m_SampleGenerator->SetValidationTrainingRatio(m_ValidationTrainingRatio);

  m_SampleGenerator->SetInput(m_InputImage);
  m_SampleGenerator->SetInputVectorData(m_VectorROIs);

  m_SampleGenerator->Update();

  m_NumberOfClasses = m_SampleGenerator->GetNumberOfClasses();
}


void
SupervisedClassificationModel
::Train()
{
  m_ModelEstimator->SetInputSampleList(m_SampleGenerator->GetTrainingListSample());
  m_ModelEstimator->SetTrainingSampleList(m_SampleGenerator->GetTrainingListLabel());
  m_ModelEstimator->SetNumberOfClasses(m_NumberOfClasses);
  m_ModelEstimator->SetC( m_CValue );
  m_ModelEstimator->Update();
}

}// namespace otb
