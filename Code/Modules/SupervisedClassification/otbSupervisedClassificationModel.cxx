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
SupervisedClassificationModel() : m_MaxTrainingSize(100),
				  m_MaxValidationSize(100),
				  m_ValidationTrainingProportion(0.5),
				  m_NumberOfClasses(2),
				  m_Description(""),
				  m_OverallAccuracy(0.0),
				  m_KappaIndex(0.0)
{
  m_InputImage = ImageType::New();
  m_LabeledImage = LabeledImageType::New();
  m_VectorROIs = VectorDataType::New();
  m_SampleGenerator = ListSampleGeneratorType::New();
  m_ModelEstimator = ModelEstimatorType::New();
  m_ClassificationFilter = ClassificationFilterType::New();
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
  

  this->UpdateDescription();
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

  m_VectorROIs = vectorData;
  m_VectorROIs->Update();
  
  this->UpdateDescription();
}


void
SupervisedClassificationModel
::Ok()
{

  m_ClassificationFilter->SetInput(m_InputImage);
  m_ClassificationFilter->SetModel(m_ModelEstimator->GetModel());
  m_OutputChanged = true;
  this->NotifyAll();

}

void
SupervisedClassificationModel
::Quit()
{
  m_OutputChanged = false;
  this->NotifyAll();
}

void
SupervisedClassificationModel
::GenerateSamples()
{
  m_SampleGenerator->SetMaxTrainingSize(m_MaxTrainingSize);
  m_SampleGenerator->SetMaxValidationSize(m_MaxValidationSize);
  m_SampleGenerator->SetValidationTrainingProportion(m_ValidationTrainingProportion);

  otbGenericMsgDebugMacro(<<"Vector data "<< m_VectorROIs);
  otbGenericMsgDebugMacro(<<"Vector data size "<< m_VectorROIs->Size());
  otbGenericMsgDebugMacro(<<"Image "<< m_InputImage);
  m_SampleGenerator->SetInput(m_InputImage);
  m_SampleGenerator->SetInputVectorData(m_VectorROIs);

  m_SampleGenerator->Update();

  m_NumberOfClasses = m_SampleGenerator->GetNumberOfClasses();


  otbGenericMsgDebugMacro(<<"Samples generated. "<< m_NumberOfClasses
		   << " classes found ");
}


void
SupervisedClassificationModel
::Train()
{
  this->GenerateSamples();
  this->UpdateDescription();
  m_ModelEstimator->SetInputSampleList(m_SampleGenerator->GetTrainingListSample());
  m_ModelEstimator->SetTrainingSampleList(m_SampleGenerator->GetTrainingListLabel());
  m_ModelEstimator->SetNumberOfClasses(m_NumberOfClasses);
  if(m_NumberOfClasses == 1)
    m_ModelEstimator->SetSVMType(ONE_CLASS);
  m_ModelEstimator->Update();
}

void
SupervisedClassificationModel
::Validate()
{
  ClassifierType::Pointer validationClassifier = ClassifierType::New();
  validationClassifier->SetSample(m_SampleGenerator->GetValidationListSample());
  validationClassifier->SetNumberOfClasses(m_NumberOfClasses);
  validationClassifier->SetModel(m_ModelEstimator->GetModel());
  validationClassifier->Update();

  ConfusionMatrixCalculatorType::Pointer confMatCalc =
                                          ConfusionMatrixCalculatorType::New();

  confMatCalc->SetReferenceLabels( m_SampleGenerator->GetValidationListLabel() );
  confMatCalc->SetProducedLabels( validationClassifier->GetOutput() );

  confMatCalc->Update();

  m_ConfusionMatrix = confMatCalc->GetConfusionMatrix();
  m_OverallAccuracy = confMatCalc->GetOverallAccuracy();
  m_KappaIndex = confMatCalc->GetKappaIndex();

  otbGenericMsgDebugMacro(<<"Confusion matrix \n" << m_ConfusionMatrix);
  
}

void
SupervisedClassificationModel
::UpdateDescription()
{
  itk::OStringStream oss;
  oss << "Number of features: " <<  m_InputImage->GetNumberOfComponentsPerPixel() << "\n";
  oss << "Pixels to classify: " << m_InputImage->GetLargestPossibleRegion().GetSize() << "\n\n";
  oss << "Number of classes: " << m_NumberOfClasses << "\n\n";
  std::map<int, int> classesSamplesNumberTraining = m_SampleGenerator->GetClassesSamplesNumberTraining();
  std::map<int, int> classesSamplesNumberValidation = m_SampleGenerator->GetClassesSamplesNumberValidation();

  if (classesSamplesNumberTraining.size() != 0)
    {
    oss << "Training samples:\n";
    for (std::map<int, int>::const_iterator itmap = classesSamplesNumberTraining.begin();
        itmap != classesSamplesNumberTraining.end(); ++itmap)
      {
      oss << itmap->first << ":\t" << itmap->second << "\n";
      }
    }
  oss << "\n";
  if (classesSamplesNumberValidation.size() != 0)
    {
    oss << "Validation samples:\n";
    for (std::map<int, int>::const_iterator itmap = classesSamplesNumberValidation.begin();
        itmap != classesSamplesNumberValidation.end(); ++itmap)
      {
      oss << itmap->first << ":\t" << itmap->second << "\n";
      }
    }

  m_Description = oss.str();
  this->NotifyAll();
}

}// namespace otb
