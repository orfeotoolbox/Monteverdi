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
#include "otbKMeansModule.h"
#include <FLU/Flu_File_Chooser.h>
#include <FL/Fl.H>

#include "otbMsgReporter.h"
#include "itkImageRegionIterator.h"
#include "itkMutexLockHolder.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"

namespace otb
{
/** Constructor */
KMeansModule::KMeansModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  m_KMeansFilter = KMeansFilterType::New();
  m_ChangeLabelFilter = ChangeLabelFilterType::New();

  // Describe inputs
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage", "Image to cluster");
}

/** Destructor */
KMeansModule::~KMeansModule()
{}

/** PrintSelf method */
void KMeansModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void KMeansModule::Run()
{
  this->BuildGUI();
  pProgressBar->minimum(0);
  pProgressBar->maximum(1);
  wKMeansWindow->show();
  this->UpdateNumberOfSamplesFromAvailableRAM();
  this->UpdateNumberOfSamples();
}

void KMeansModule::Ok()
{
  this->StartProcess2();
  this->StartProcess1();
}

void KMeansModule::Cancel()
{
  wKMeansWindow->hide();
}

void KMeansModule::UpdateNumberOfSamples()
{
  vNumberOfSamples->show();

  FloatingVectorImageType::Pointer image = this->GetInputData<FloatingVectorImageType>("InputImage");

  if (image.IsNull())
    {
    itkExceptionMacro("InputImage is null");
    }

  image->UpdateOutputInformation();

  unsigned int nbSamples =  static_cast<unsigned int>(vNumberOfSamples->value() * image->GetLargestPossibleRegion().GetNumberOfPixels() / 100);

  std::ostringstream oss;
  oss << vNumberOfSamples->value() << "% of image (" << nbSamples << " samples)";
  oNumberOfSamples->copy_label(oss.str().c_str());
}

void KMeansModule::RegisterSampler(itk::ProcessObject* sampler)
{
  itk::MutexLockHolder<itk::SimpleFastMutexLock> lock(m_Mutex);
  m_ProcessObject = sampler;
}

void KMeansModule::ReleaseSampler()
{
  itk::MutexLockHolder<itk::SimpleFastMutexLock> lock(m_Mutex);
  m_ProcessObject = 0;
}

void KMeansModule::CreateEstimator()
{
  itk::MutexLockHolder<itk::SimpleFastMutexLock> lock(m_Mutex);
  m_Estimator = EstimatorType::New();
}

void KMeansModule::ReleaseEstimator()
{
  itk::MutexLockHolder<itk::SimpleFastMutexLock> lock(m_Mutex);
  m_Estimator = 0;
}

void KMeansModule::UpdateProgress()
{
  itk::MutexLockHolder<itk::SimpleFastMutexLock> lock(m_Mutex);

  std::ostringstream oss1;
  oss1.str("");

  pProgressBar->show();

  if (m_ProcessObject.IsNotNull())
    {
    double progress = m_ProcessObject->GetProgress();

    if (progress < 1.)
      {
      oss1 << "Sampling data  (" << std::floor(100 * progress) << "%)";
      pProgressBar->value(progress);
      pProgressBar->copy_label(oss1.str().c_str());
      }
    }

  if (m_Estimator)
    {
    if (m_Estimator->GetCurrentIteration() == 0)
      {
      pProgressBar->value(0.);
      pProgressBar->copy_label("Generating decision tree");
      }
    else
      {
      oss1 << "Estimating (res=" << m_Estimator->GetCentroidPositionChanges() << ")";
      pProgressBar->maximum(m_Estimator->GetMaximumIteration());
      pProgressBar->minimum(0);
      pProgressBar->value(m_Estimator->GetCurrentIteration());
      pProgressBar->copy_label(oss1.str().c_str());
      }
    }
}

void KMeansModule::UpdateProgressCallback(void * data)
{
  Self::Pointer pthis = static_cast<Self *>(data);

  if (pthis.IsNotNull())
    {
    pthis->UpdateProgress();
    }
}

void KMeansModule::ThreadedWatch()
{
  // Deactivate window buttons
  Fl::lock();
  bCancel->deactivate();
  bOk->deactivate();
  vNumberOfSamples->deactivate();
  vNumberOfClasses->deactivate();
  vConvergenceThreshold->deactivate();
  vNumberOfIterations->deactivate();
  vAvailableRAM->deactivate();
  Fl::unlock();

  // Wait for the module to be busy
  while (!this->IsBusy())
    {
    Sleep(500);
    }

  while (this->IsBusy())
    {
    Fl::awake(&UpdateProgressCallback, this);
    // Sleep for a while
    Sleep(500);
    }

  // Update progress one last time
  Fl::awake(&UpdateProgressCallback, this);

  Fl::lock();
  // Reactivate window buttons
  bCancel->activate();
  bOk->activate();
  vNumberOfSamples->activate();
  vNumberOfClasses->activate();
  vConvergenceThreshold->activate();
  vNumberOfIterations->activate();
  vAvailableRAM->activate();
  Fl::unlock();

  Fl::awake(&HideWindowCallback, this);
}

void KMeansModule::ThreadedRun()
{
  this->BusyOn();

  FloatingVectorImageType::Pointer image  = this->GetInputData<FloatingVectorImageType>("InputImage");
  unsigned int                     nbComp = image->GetNumberOfComponentsPerPixel();
  
  if (image.IsNull())
    {
    m_ErrorMsg = "InputImage is null";
    Fl::awake(&SendErrorCallback, &m_ErrorMsg);
    this->BusyOff();
    return;
    }

  // First, sample data
  SamplingFilterType::Pointer sampler = SamplingFilterType::New();
  sampler->SetInput(image);

  unsigned int nbClasses = static_cast<unsigned int>(vNumberOfClasses->value());
  const double percentageOfTotalNbSamples = vNumberOfSamples->value() / 100;
  const double actualNBSamplesForKMeans = image->GetLargestPossibleRegion().GetNumberOfPixels()
                                         * percentageOfTotalNbSamples;
  const double shrinkFactor
    = vcl_ceil(vcl_sqrt(image->GetLargestPossibleRegion().GetNumberOfPixels()
                         / actualNBSamplesForKMeans ));
  sampler->SetShrinkFactor(shrinkFactor);
  this->RegisterSampler(sampler->GetStreamer());
  sampler->Update();

  itk::ImageRegionIterator<FloatingVectorImageType> it(sampler->GetOutput(),
                                                       sampler->GetOutput()->GetLargestPossibleRegion());
  it.GoToBegin();
  ListSampleType::Pointer listSample = ListSampleType::New();
  listSample->SetMeasurementVectorSize(it.Get().GetSize());

  FloatingVectorImageType::PixelType pixel = it.Get();
  SampleType mini(pixel);
  SampleType maxi(pixel);

  listSample->PushBack(it.Get());
  ++it;

  while (!it.IsAtEnd())
    {
    SampleType sample = it.Get();
    listSample->PushBack(sample);

    for (unsigned int i = 0; i < nbComp; ++i)
      {
      if (mini[i] > sample[i])
        {
        mini[i] = sample[i];
        }
      if (maxi[i] < sample[i])
        {
        maxi[i] = sample[i];
        }
      }
    ++it;
    }

  // We don't need the QL anymore
  ReleaseSampler();
  sampler = 0;

  // Next, initialiaze centroids
  EstimatorType::ParametersType initialCentroids(nbComp * nbClasses);

  itk::Statistics::MersenneTwisterRandomVariateGenerator::Pointer generator
    = itk::Statistics::MersenneTwisterRandomVariateGenerator::GetInstance();

  for (unsigned int classIndex = 0; classIndex < nbClasses; ++classIndex)
    {
    for (unsigned int compIndex = 0; compIndex < nbComp; ++compIndex)
      {
      initialCentroids[compIndex + classIndex * nbComp] =
          generator->GetUniformVariate(mini[compIndex], maxi[compIndex]);
      }
    }

  this->CreateEstimator();

  // Now, build the kdTree
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
  treeGenerator->SetSample(listSample);
  treeGenerator->SetBucketSize(static_cast<unsigned int>(vNumberOfSamples->value() / (10 * nbClasses)));
  treeGenerator->Update();

  otbGenericMsgDebugMacro(<< "Tree generated");

  // Estimate the centroids
  m_Estimator->SetKdTree(treeGenerator->GetOutput());
  m_Estimator->SetParameters(initialCentroids);
  m_Estimator->SetMaximumIteration(static_cast<unsigned int>(vNumberOfIterations->value()));
  m_Estimator->SetCentroidPositionChangesThreshold(vConvergenceThreshold->value());
  m_Estimator->StartOptimization();

  otbGenericMsgDebugMacro(<< "Optimization ended");

  // Finally, update the KMeans filter
  KMeansFunctorType functor;

  EstimatorType::ParametersType finalCentroids = m_Estimator->GetParameters();

  this->ReleaseEstimator();

  for (unsigned int classIndex = 0; classIndex < nbClasses; ++classIndex)
    {
    SampleType centroid(nbComp);

    for (unsigned int compIndex = 0; compIndex < nbComp; ++compIndex)
      {
      centroid[compIndex] = finalCentroids[compIndex + classIndex * nbComp];
      }
    functor.AddCentroid(classIndex, centroid);
    m_ChangeLabelFilter->SetChange(classIndex, centroid);
    }

  m_KMeansFilter->SetFunctor(functor);
  m_KMeansFilter->SetInput(image);
  m_ChangeLabelFilter->SetInput(m_KMeansFilter->GetOutput());
  m_ChangeLabelFilter->SetNumberOfComponentsPerPixel(nbComp);

  Fl::lock();
  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_KMeansFilter->GetOutput(), "KMeans labeled image",
                            "The labeled image from kmeans classification");
  this->AddOutputDescriptor(m_ChangeLabelFilter->GetOutput(), "KMeans clustered image",
                            "The clustered image from kmeans classification");
  this->NotifyOutputsChange();
  Fl::unlock();

  this->BusyOff();
}

void KMeansModule::HideWindow()
{
  wKMeansWindow->hide();
}

void KMeansModule::HideWindowCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if (writer.IsNotNull())
    {
    writer->HideWindow();
    }
}

void KMeansModule::SendErrorCallback(void * data)
{
  std::string * error = static_cast<std::string *>(data);
  //TODO test if error is null
  if (error == NULL)
    {
    MsgReporter::GetInstance()->SendError("Unknown error during update");
    }
  else
    {
    MsgReporter::GetInstance()->SendError(error->c_str());
    }
}

void KMeansModule::UpdateNumberOfSamplesFromAvailableRAM()
{
  FloatingVectorImageType::Pointer image  = this->GetInputData<FloatingVectorImageType>("InputImage");
  image->UpdateOutputInformation();

  // Necessary informations
  unsigned int nbComp = image->GetNumberOfComponentsPerPixel();
  unsigned int nbClasses = static_cast<unsigned int>(vNumberOfClasses->value());

  // Compute the number of maximum samples allowed by the RAM
  double numberOfSampleKnowingRAM = static_cast<unsigned int>(vAvailableRAM->value()) * 1024 *1024 /(sizeof(PrecisionType) * nbComp);
  double percentageOfImageUsed = 100 * static_cast<double>(numberOfSampleKnowingRAM / image->GetLargestPossibleRegion().GetNumberOfPixels());

  // maximum of the image to be used is 100%
  percentageOfImageUsed = std::min(percentageOfImageUsed, 100.);
  
  // Update the GUI with the new value
  vNumberOfSamples->value(percentageOfImageUsed);
  this->UpdateNumberOfSamples();
}

} // End namespace otb
