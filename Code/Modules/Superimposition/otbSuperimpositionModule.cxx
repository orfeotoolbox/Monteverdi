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
#ifndef __otbSuperimpositionModule_cxx
#define __otbSuperimpositionModule_cxx

#include "otbSuperimpositionModule.h"
#include <FLU/Flu_File_Chooser.h>
#include "base/ossimFilename.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
SuperimpositionModule::SuperimpositionModule()
{
  m_Resampler       = ResampleFilterType::New();
  m_PerBanderFilter = PerBandFilterType::New();
  m_Transform       = TransformType::New();

  // Describe inputs
  this->AddInputDescriptor<VectorImageType>("ReferenceImage","Reference image for reprojection.");
  this->AddInputDescriptor<VectorImageType>("InputImage","Image to reproject.");
  
  this->BuildGUI();
}

/** Destructor */
SuperimpositionModule::~SuperimpositionModule()
{
  wFileChooserWindow->hide();
}

/** PrintSelf method */
void SuperimpositionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void SuperimpositionModule::Run()
{
  wFileChooserWindow->show();
}



void SuperimpositionModule::Ok()
{
  VectorImageType::Pointer fixed = this->GetInputData<VectorImageType>("ReferenceImage");
  VectorImageType::Pointer moving = this->GetInputData<VectorImageType>("InputImage");
  
  if(fixed.IsNull() || moving.IsNull())
    {
    itkExceptionMacro(<<"One of the input filter is null");
    }

  // Update input information
  fixed->UpdateOutputInformation();
  moving->UpdateOutputInformation();

  // Build the transform
  m_Transform->SetInputProjectionRef(fixed->GetProjectionRef());
  m_Transform->SetInputDictionary(fixed->GetMetaDataDictionary());
  m_Transform->SetInputKeywordList(fixed->GetImageKeywordlist());
  m_Transform->SetOutputProjectionRef(moving->GetProjectionRef());
  m_Transform->SetOutputDictionary(moving->GetMetaDataDictionary());
  m_Transform->SetOutputKeywordList(moving->GetImageKeywordlist());

  if(choiceDEM->value() == 1)
    {
    m_Transform->SetDEMDirectory(vDEMPath->value());
    }
  else
    {
    m_Transform->SetAverageElevation(vAverageElevation->value());
    }

  m_Transform->InstanciateTransform();

  // Copy parameters from reference image
  m_Resampler->SetTransform(m_Transform);
  m_Resampler->SetSize(fixed->GetLargestPossibleRegion().GetSize());
  m_Resampler->SetOutputStartIndex(fixed->GetLargestPossibleRegion().GetIndex());
  m_Resampler->SetOutputSpacing(fixed->GetSpacing());
  m_Resampler->SetOutputOrigin(fixed->GetOrigin());

  m_PerBanderFilter->SetInput(moving);
  m_PerBanderFilter->SetFilter(m_Resampler);

  VectorImageType::Pointer output = m_PerBanderFilter->GetOutput();
  
  output->UpdateOutputInformation();
  
  // Report projection ref (not done by the resample filter)
  output->CopyInformation(fixed);

  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(output,"Reprojected image","Image superimposable to reference");
  this->NotifyOutputsChange();

  wFileChooserWindow->hide();
}

void SuperimpositionModule::Browse()
{
  const char * filename = NULL;

  filename = flu_dir_chooser("Choose the dataset dir...", "*.*",".");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }
  vDEMPath->value(filename);
}

void SuperimpositionModule::Cancel()
{
  wFileChooserWindow->hide();
}


} // End namespace otb

#endif
