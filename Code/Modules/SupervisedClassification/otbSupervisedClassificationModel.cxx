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
#include "otbVectorDataIntoImageProjectionFilter.h"

namespace otb
{

SupervisedClassificationModel::
SupervisedClassificationModel() : m_MaxTrainingSize(100),
  m_MaxValidationSize(100),
  m_ValidationTrainingProportion(0.5),
  m_ClassKey("Class"),
  m_NumberOfClasses(0),
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
  m_Caster = CasterType::New();
}

SupervisedClassificationModel
::~SupervisedClassificationModel()
{
}

void SupervisedClassificationModel::
Notify(ListenerBase * listener)
{
  listener->Notify();
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

  this->UpdateDescription();
}

void
SupervisedClassificationModel
::SetVectorROIs(VectorDataPointerType vectorData)
{
  m_OutputChanged = false;

  m_VectorROIs = vectorData;
  m_VectorROIs->Update();

  this->ReprojectVectorData();

  this->UpdateVectorDataInformation();
  this->UpdateDescription();
}

void
SupervisedClassificationModel
::ReprojectVectorData()
{
  if(m_InputImage.IsNull())
    {
      itkExceptionMacro("Invalid input image.");
    }

  typedef otb::VectorDataIntoImageProjectionFilter<VectorDataType, ImageType>
    VectorDataReprojectionType;

  VectorDataReprojectionType::Pointer reproj = VectorDataReprojectionType::New();
  reproj->SetInputImage(m_InputImage);
  reproj->SetInputVectorData(m_VectorROIs.GetPointer());

  // We want to transform into image physical coordinates
  reproj->SetUseOutputSpacingAndOriginFromImage(false);
  reproj->Update();
  m_VectorROIs = reproj->GetOutput();
}

void
SupervisedClassificationModel
::Ok()
{

  m_ClassificationFilter->SetInput(m_InputImage);
  m_ClassificationFilter->SetModel(m_ModelEstimator->GetModel());
  m_Caster->SetInput(m_ClassificationFilter->GetOutput());
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
  m_SampleGenerator->SetClassKey(m_ClassKey);

  otbGenericMsgDebugMacro(<< "Vector data " << m_VectorROIs);
  otbGenericMsgDebugMacro(<< "Vector data size " << m_VectorROIs->Size());
  otbGenericMsgDebugMacro(<< "Image " << m_InputImage);
  m_SampleGenerator->SetInput(m_InputImage);
  m_SampleGenerator->SetInputVectorData(m_VectorROIs);

  m_SampleGenerator->Update();

  m_NumberOfClasses = m_SampleGenerator->GetNumberOfClasses();

  otbGenericMsgDebugMacro(<< "Samples generated. " << m_NumberOfClasses
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
  if (m_NumberOfClasses == 1) m_ModelEstimator->SetSVMType(ONE_CLASS);
  m_ModelEstimator->Update();
}

void
SupervisedClassificationModel
::Validate()
{
  ClassifierType::Pointer validationClassifier = ClassifierType::New();
  validationClassifier->SetInput(m_SampleGenerator->GetValidationListSample());
  validationClassifier->SetNumberOfClasses(m_NumberOfClasses);
  validationClassifier->SetModel(m_ModelEstimator->GetModel());
  validationClassifier->Update();

// Verify outputs

  ValidationListSampleType::ConstIterator it = validationClassifier->GetOutput()->Begin();
  ValidationListSampleType::ConstIterator itEnd = validationClassifier->GetOutput()->End();

  TrainingListSampleType::Pointer validationList = TrainingListSampleType::New();

  while (it != itEnd)
    {
    validationList->PushBack(it.GetClassLabel());
    ++it;
    }

  ConfusionMatrixCalculatorType::Pointer confMatCalc =
    ConfusionMatrixCalculatorType::New();

  confMatCalc->SetReferenceLabels(m_SampleGenerator->GetValidationListLabel());
  confMatCalc->SetProducedLabels(validationList);

  confMatCalc->Compute();

  m_ConfusionMatrix = confMatCalc->GetConfusionMatrix();
  m_OverallAccuracy = confMatCalc->GetOverallAccuracy();
  m_KappaIndex = confMatCalc->GetKappaIndex();
  m_MapOfClasses = confMatCalc->GetMapOfClasses();

  otbGenericMsgDebugMacro(<< "Confusion matrix \n" << m_ConfusionMatrix);

  this->UpdateMatrixString();

}

void
SupervisedClassificationModel
::UpdateVectorDataInformation()
{

  m_SampleGenerator->SetMaxTrainingSize(m_MaxTrainingSize);
  m_SampleGenerator->SetMaxValidationSize(m_MaxValidationSize);
  m_SampleGenerator->SetValidationTrainingProportion(m_ValidationTrainingProportion);

  VectorDataType::DataTreeType::TreeNodeType::ConstPointer node = m_VectorROIs->GetDataTree()->GetRoot();
  //Go down the vector data tree until we reach a leaf
  while (node->HasChildren())
    {
    node = node->GetChild(0);
    }
  m_ClassKeyList = node->Get()->GetFieldList();
  if ((!node->Get()->HasField(m_ClassKey)) && (m_ClassKeyList.size() > 0))
    {
    m_ClassKey = m_ClassKeyList[0];
    }

  m_SampleGenerator->SetClassKey(m_ClassKey);

  otbGenericMsgDebugMacro(<< "Vector data " << m_VectorROIs);
  otbGenericMsgDebugMacro(<< "Vector data size " << m_VectorROIs->Size());
  otbGenericMsgDebugMacro(<< "Image " << m_InputImage);
  m_SampleGenerator->SetInput(m_InputImage);
  m_SampleGenerator->SetInputVectorData(m_VectorROIs);

 //m_SampleGenerator->GenerateClassStatistics();
  m_SampleGenerator->Update();
  m_NumberOfClasses = m_SampleGenerator->GetNumberOfClasses();

  m_ClassMinSize = vcl_ceil(m_SampleGenerator->GetClassMinSize());

  this->NotifyAll();
}

void
SupervisedClassificationModel
::UpdateDescription()
{
  std::ostringstream oss;
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

void
SupervisedClassificationModel
::UpdateMatrixString()
{
  // m_MapOfClasses contains the mapping of class name (or class number) => class index in the matrix
  std::ostringstream oss;
  oss << "Confusion matrix:\n\n";
  // write the first line
  oss << "\t";
  for (std::map<ClassLabelType, int>::const_iterator itmap = m_MapOfClasses.begin();
       itmap != m_MapOfClasses.end(); ++itmap)
    {
    oss << itmap->first << "\t";
    }
  oss << "\n";
  for (std::map<ClassLabelType, int>::const_iterator itmap = m_MapOfClasses.begin();
       itmap != m_MapOfClasses.end(); ++itmap)
    {
    oss << itmap->first << "\t";
    for (std::map<ClassLabelType, int>::const_iterator itmap2 = m_MapOfClasses.begin();
         itmap2 != m_MapOfClasses.end(); ++itmap2)
      {
      oss << m_ConfusionMatrix(itmap->second, itmap2->second) << "\t";
      }
    oss << "\n";
    }

  //Also output the accuracy info
  oss << "\nOverall accuracy:\t" << m_OverallAccuracy;
  oss << "\nKappa:\t" << m_KappaIndex;

  m_MatrixString = oss.str();
  this->NotifyAll();
}

} // namespace otb
