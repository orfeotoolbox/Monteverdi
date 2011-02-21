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

// Interpolators
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "otbBCOInterpolateImageFunction.h"

namespace otb
{
/** Constructor */
SuperimpositionModule::SuperimpositionModule()
{
  m_Resampler        = ResampleFilterType::New();
  m_CastFixedFilter  = CastImageFilterType::New();
  m_CastMovingFilter = CastImageFilterType::New();
    
  // Describe inputs
  this->AddInputDescriptor<VectorImageType>("ReferenceImage",
                                            otbGetTextMacro("Reference image for reprojection"));
  this->AddTypeToInputDescriptor<ImageType>("ReferenceImage");
  this->AddInputDescriptor<VectorImageType>("InputImage", 
                                            otbGetTextMacro("Image to reproject"));
  this->AddTypeToInputDescriptor<ImageType>("InputImage");

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
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SuperimpositionModule::Run()
{
  wFileChooserWindow->show();
}

void SuperimpositionModule::Ok()
{
  VectorImageType::Pointer vfixed = this->GetInputData<VectorImageType>("ReferenceImage");
  VectorImageType::Pointer vmoving = this->GetInputData<VectorImageType>("InputImage");

  ImageType::Pointer fixed = this->GetInputData<ImageType>("ReferenceImage");
  ImageType::Pointer moving = this->GetInputData<ImageType>("InputImage");

  // Get the inputs and cast if otb::Image
  if (fixed.IsNotNull() && vfixed.IsNull())
    {
    // Cast Image into VectorImage
    m_CastFixedFilter->SetInput(fixed);
    vfixed = m_CastFixedFilter->GetOutput();
    }
  
  if (moving.IsNotNull() && vmoving.IsNull())
    {
    // Cast Image into VectorImage
    m_CastMovingFilter->SetInput(moving);
    vmoving = m_CastMovingFilter->GetOutput();
    }

  if (vfixed.IsNull() || vmoving.IsNull() )
    {
    itkExceptionMacro(<< "Input is null");
    }

  // Update input output informations
  vfixed->UpdateOutputInformation();
  vmoving->UpdateOutputInformation();
  
  // Resampler 
  m_Resampler->SetInput(vmoving);
  m_Resampler->SetOutputParametersFromImage(vfixed);
  
  if (choiceDEM->value() == 1)
    {
    m_Resampler->SetDEMDirectory(vDEMPath->value());
    }
  else
    {
    m_Resampler->SetAverageElevation(vAverageElevation->value());
    }
  
  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_Resampler->GetOutput(), "Reprojected image", 
                            otbGetTextMacro("Image superimposable to reference"));
  this->NotifyOutputsChange();

  // Close the GUI
  wFileChooserWindow->hide();
}

void SuperimpositionModule::Browse()
{
  const char * filename = NULL;

  filename = flu_dir_chooser(otbGetTextMacro("Choose the dataset dir..."), "*.*", "");

  if (filename == NULL)
    {
    otbMsgDebugMacro(<< "Empty file name!");
    return;
    }
  vDEMPath->value(filename);
}

void SuperimpositionModule::Cancel()
{
  wFileChooserWindow->hide();
}

void
SuperimpositionModule::SetInterpolatorType(InterpolatorType interp)
{
  m_InterpType = interp;
  this->UpdateInterpolator();
  this->Modified();
}

InterpolatorType
SuperimpositionModule::GetInterpolatorType()
{
  return m_InterpType;
}

void
SuperimpositionModule::UpdateInterpolator()
{
  switch (this->GetInterpolatorType())
    {
    case MAP_LINEAR_SUPERIMPOSITION:
    {
    typedef itk::LinearInterpolateImageFunction<VectorImageType, double> LinearType;
    LinearType::Pointer interp = LinearType::New();
    m_Resampler->SetInterpolator(interp);
    break;
    }
    case MAP_NEAREST_SUPERIMPOSITION:
    {
    typedef itk::NearestNeighborInterpolateImageFunction<VectorImageType, double> NearestType;
    NearestType::Pointer interp = NearestType::New();
    m_Resampler->SetInterpolator(interp);
    break;
    }
    case MAP_BCO_SUPERIMPOSITION:
    {
    typedef BCOInterpolateImageFunction<VectorImageType, double> BCOType;
    BCOType::Pointer interp = BCOType::New();
    interp->SetRadius(static_cast<unsigned int>(guiBCORadius->value()));
    m_Resampler->SetInterpolator(interp);
    break;
    }
    default:
    {
    std::ostringstream oss;
    oss<<"Problem with interpolator, cannot use the selected interpolator";
    MsgReporter::GetInstance()->SendError(oss.str());
    break;
    }
    }
}

} // End namespace otb

#endif
