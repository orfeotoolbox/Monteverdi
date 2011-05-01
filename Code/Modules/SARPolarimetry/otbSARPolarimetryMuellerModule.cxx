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
#include "otbSARPolarimetryMuellerModule.h"

#include "otbFltkFilterWatcher.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
  SARPolarimetryMuellerModule::SARPolarimetryMuellerModule()
{
  // Filters
  m_MuellerToReciprocalCovarianceImageFilter = MuellerToReciprocalCovarianceImageFilterType::New();
  m_MuellerToPolarisationDegreeAndPowerImageFilter = MuellerToPolarisationDegreeAndPowerImageFilterType::New();

  m_InputImage = VectorImageType::New();

  // Add inputs
  this->AddInputDescriptor<VectorImageType>("InputImage", otbGetTextMacro("Input Image (multi channel real image, 16 channels)"));
}

/** Destructor */
SARPolarimetryMuellerModule::~SARPolarimetryMuellerModule()
{}

/** PrintSelf method */
void SARPolarimetryMuellerModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SARPolarimetryMuellerModule::Run()
{
  // While the viewer is shown, it is busy
  this->BusyOn();

  this->BuildGUI();
  this->CheckInputs();

  this->Show();
  
}

/** Check inputs configuration */
void SARPolarimetryMuellerModule::CheckInputs()
{
  m_InputImage = this->GetInputData<VectorImageType>("InputImage");
  
  if( m_InputImage.IsNull() )
    {
      MsgReporter::GetInstance()->SendError("No input image detected.");
      this->Quit();
    }
  else
    {
      m_InputImage->UpdateOutputInformation();
      if( m_InputImage->GetNumberOfComponentsPerPixel() != 16)
        {
          itk::OStringStream oss;
          oss << "Invalid Input, must have 16 channels instead of " << m_InputImage->GetNumberOfComponentsPerPixel() << ".";
          MsgReporter::GetInstance()->SendError( oss.str() );
          this->Quit();
        }
    }
}


void SARPolarimetryMuellerModule::Ok()
{
  this->ClearOutputDescriptors();
  bool hasOutput = false;

  if( rb_MuePolDegPow->value() == true )
    {
      m_MuellerToPolarisationDegreeAndPowerImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_MuellerToPolarisationDegreeAndPowerImageFilter->GetOutput(), "MuellerToPolarisationDegreeAndPowerImageFilter",
                                otbGetTextMacro("Mueller to polarisation degree and power image"));
     hasOutput = true;
    }
  if( rb_MueRecCv->value() == true )
    {
      m_MuellerToReciprocalCovarianceImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_MuellerToReciprocalCovarianceImageFilter->GetOutput(), "MuellerToReciprocalCovarianceImageFilter",
                                otbGetTextMacro("Mueller to reciprocal covarianc image"));
     hasOutput = true;
    }
  
  if( hasOutput==true )
    {
      this->NotifyOutputsChange();
    }
  else
    {
      MsgReporter::GetInstance()->SendWarning( "No process selected." );
    }
  this->Hide();
  // Once module is closed, it is no longer busy
    this->BusyOff();
}
  
  
  
void SARPolarimetryMuellerModule::Quit()
{
  // 1First, clear any previous output
  this->ClearOutputDescriptors();
  this->Hide();
  // Once module is closed, it is no longer busy
  this->BusyOff();
}
  
  
void SARPolarimetryMuellerModule::CheckAll(bool val)
{
  rb_MuePolDegPow->value(val);
  rb_MueRecCv->value(val);
}


} // End namespace otb
