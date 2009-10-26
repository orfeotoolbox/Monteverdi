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

namespace otb
{
/** Constructor */
KMeansModule::KMeansModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  m_KMeansFilter = KMeansFilterType::New();

  // Describe inputs
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage","Image to cluster");
}

/** Destructor */
KMeansModule::~KMeansModule()
{}

/** PrintSelf method */
void KMeansModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void KMeansModule::Run()
{
  this->BuildGUI();
  pProgressBar->minimum(0);
  pProgressBar->maximum(1);
  this->UpdateNumberOfSamples();
  wKMeansWindow->show();
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
  int squareRatio = 100/vNumberOfSamples->value();
  
  FloatingVectorImageType::Pointer image = this->GetInputData<FloatingVectorImageType>("InputImage");
  
  if(image.IsNull())
    {
    itkExceptionMacro("InputImage is null");
    }

  image->UpdateOutputInformation();

  unsigned int nbSamples = image->GetLargestPossibleRegion().GetNumberOfPixels()/squareRatio;

  itk::OStringStream oss;
  oss.str("");
  oss<<vNumberOfSamples->value()<<"% of image ("<<nbSamples<<" samples)";
  oNumberOfSamples->copy_label(oss.str().c_str());
}


void KMeansModule::UpdateProgress()
{
  double progress = m_ProcessObject->GetProgress();

  itk::OStringStream oss1;
  oss1.str("");
  oss1<<"Sampling data  ("<<std::floor(100*progress)<<"%)"; 
  pProgressBar->value( progress );
  wKMeansWindow->copy_label(oss1.str().c_str());
  pProgressBar->copy_label( oss1.str().c_str() );
}

void KMeansModule::UpdateProgressCallback(void * data)
{
  Self::Pointer pthis = static_cast<Self *>(data);

  if(pthis.IsNotNull())
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
  Fl::unlock();

  double last = 0;
  double updateThres = 0.01;
  double current = 0;

  while( (m_ProcessObject.IsNull() || this->IsBusy()) )
    {
    if(m_ProcessObject.IsNotNull())
         {
        current = m_ProcessObject->GetProgress();
         if(current - last > updateThres)
           {
        // Make the main fltk loop update progress fields
          Fl::awake(&UpdateProgressCallback,this);
           last = current;
           }
         }
       // Sleep for a while
    Sleep(500);
    }

  // Update progress one last time
  Fl::awake(&UpdateProgressCallback,this);
  
  Fl::lock();
  // Reactivate window buttons
  bCancel->activate();
  bOk->activate();
  vNumberOfSamples->activate();
  vNumberOfClasses->activate();
  Fl::unlock();

  Fl::awake(&HideWindowCallback,this);
  }

void KMeansModule::ThreadedRun()
{
  this->BusyOn();
  
  FloatingVectorImageType::Pointer image = this->GetInputData<FloatingVectorImageType>("InputImage");

  if(image.IsNull())
    {
    m_ErrorMsg = "InputImage is null";
    Fl::awake(&SendErrorCallback,&m_ErrorMsg);
    this->BusyOff();
    return;
    }
  
  // First, sample data
  SamplingFilterType::Pointer sampler = SamplingFilterType::New();
  sampler->SetInput(image);
  sampler->SetShrinkFactor(vcl_floor(vcl_sqrt(100/vNumberOfSamples->value())));
  m_ProcessObject = sampler;
  sampler->Update();
  
  // Then, build the sample list
  unsigned int nbComp = sampler->GetOutput()->GetNumberOfComponentsPerPixel();
  unsigned int nbClasses = vNumberOfClasses->value();

  itk::ImageRegionIterator<FloatingVectorImageType> it(sampler->GetOutput(),sampler->GetOutput()->GetLargestPossibleRegion());
  it.GoToBegin();
  ListSampleType::Pointer listSample = ListSampleType::New();

  SampleType min = it.Get();
  SampleType max = it.Get();
  listSample->PushBack(it.Get());

  ++it;

  while(!it.IsAtEnd())
    {
    SampleType sample = it.Get();
    listSample->PushBack(sample);
    
    for(unsigned int i = 0; i<nbComp;++i)
      {
      if(min[i]>sample[i])
       {
       min[i] = sample[i];
       }
      if(max[i]<sample[i])
       {
       max[i] = sample[i];
       }
      }
    ++it;
    }

  // Next, intialiaze centroids
  EstimatorType::ParametersType initialCentroids(nbComp*nbClasses);

  for(unsigned int classIndex = 0; classIndex < nbClasses;++classIndex)
    {
    for(unsigned int compIndex = 0; compIndex < nbComp;++compIndex)
      {
      initialCentroids[compIndex + classIndex * nbComp] = min[compIndex] 
       + (max[compIndex]-min[compIndex])*rand()/(RAND_MAX+1);
      }
    }
  
  // Now, build the kdTree 
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();
  treeGenerator->SetSample(listSample);
  treeGenerator->SetBucketSize(100);
  treeGenerator->Update();
  
  // Estimate the centroids
  EstimatorType::Pointer estimator = EstimatorType::New();
  estimator->SetKdTree(treeGenerator->GetOutput());
  estimator->SetParameters(initialCentroids);
  estimator->SetMaximumIteration(1000/**vNumberOfIterations->value()*/);
  estimator->SetCentroidPositionChangesThreshold(0.001 /**vCentroidsUpdateThreshold->value()*/);
  estimator->StartOptimization();

  // Finally, update the KMeans filter
  KMeansFunctorType functor;
  
  EstimatorType::ParametersType finalCentroids = estimator->GetParameters();

 for(unsigned int classIndex = 0; classIndex < nbClasses;++classIndex)
    {
    SampleType centroid(nbComp);

    for(unsigned int compIndex = 0; compIndex < nbComp;++compIndex)
      {
      centroid[compIndex] = finalCentroids[compIndex + classIndex * nbComp];
      }
    functor.AddCentroid(classIndex,centroid);
    }

   m_KMeansFilter->SetFunctor(functor);
   m_KMeansFilter->SetInput(image);

   Fl::lock();
   this->ClearOutputDescriptors();
   this->AddOutputDescriptor(m_KMeansFilter->GetOutput(),"KMeans classification","The image kmeans classification.");
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

  if(writer.IsNotNull())
    {
    writer->HideWindow();
    }
}

void KMeansModule::SendErrorCallback(void * data)
{
  std::string *  error = static_cast<std::string *>(data);
  //TODO test if error is null
  if ( error == NULL )
  {
    MsgReporter::GetInstance()->SendError("Unknown error during update");
  }
  else 
  {
    MsgReporter::GetInstance()->SendError(error->c_str());
  }
}
} // End namespace otb


