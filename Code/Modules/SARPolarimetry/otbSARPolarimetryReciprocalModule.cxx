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
#include "otbSARPolarimetryReciprocalModule.h"

#include "otbFltkFilterWatcher.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
  SARPolarimetryReciprocalModule::SARPolarimetryReciprocalModule()
{
  // Filters
  m_ReciprocalCovarianceToReciprocalCoherencyImageFilter = ReciprocalCovarianceToReciprocalCoherencyImageFilterType::New();
  m_ReciprocalCovarianceToCoherencyDegreeImageFilter = ReciprocalCovarianceToCoherencyDegreeImageFilterType::New();
  m_ReciprocalCoherencyToReciprocalMuellerImageFilter = ReciprocalCoherencyToReciprocalMuellerImageFilterType::New();

  m_InputImage = ComplexVectorImageType::New();

  // Add inputs
  this->AddInputDescriptor<ComplexVectorImageType>("InputImage", otbGetTextMacro("Input Image (multi channel complex image, 6 or 10 channels)"));
}

/** Destructor */
SARPolarimetryReciprocalModule::~SARPolarimetryReciprocalModule()
{}

/** PrintSelf method */
void SARPolarimetryReciprocalModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SARPolarimetryReciprocalModule::Run()
{
  // While the viewer is shown, it is busy
  this->BusyOn();

  this->BuildGUI();
  this->CheckInputs();
 
  this->Show();
  
}

/** Check inputs configuration */
void SARPolarimetryReciprocalModule::CheckInputs()
{
  m_InputImage = this->GetInputData<ComplexVectorImageType>("InputImage");
  
  if( m_InputImage.IsNull() )
    {
      MsgReporter::GetInstance()->SendError("No input image detected.");
      this->Quit();
    }
  else
    {
      m_InputImage->UpdateOutputInformation();
      if( m_InputImage->GetNumberOfComponentsPerPixel() == 6)
        {
          rb_Mue->deactivate();
        }
      else if( m_InputImage->GetNumberOfComponentsPerPixel() == 10)
        {
          rb_CohDeg->deactivate();
          rb_RecCoh->deactivate();
        }
      else
        {
          std::ostringstream oss;
          oss << "Invalid Input, must have 6 channels instead of " << m_InputImage->GetNumberOfComponentsPerPixel() << ".";
          MsgReporter::GetInstance()->SendError( oss.str() );
          this->Quit();
        }
    }
}


void SARPolarimetryReciprocalModule::Ok()
{
  this->ClearOutputDescriptors();
  bool hasOutput = false;

  if( rb_RecCoh->value() )
    {
      m_ReciprocalCovarianceToReciprocalCoherencyImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_ReciprocalCovarianceToReciprocalCoherencyImageFilter->GetOutput(), "ReciprocalCovarianceToReciprocalCoherencyImageFilter",
                                otbGetTextMacro("Reciprocal to rec. coherency image"));
      hasOutput = true;
    }
  if( rb_CohDeg->value() )
    {
      m_ReciprocalCovarianceToCoherencyDegreeImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_ReciprocalCovarianceToCoherencyDegreeImageFilter->GetOutput(), "ReciprocalCovarianceToCoherencyDegreeImageFilter",
                                otbGetTextMacro("Reciprocal to cohenrency degree image"));
      hasOutput = true;
    }
  if( rb_Mue->value() )
    {
      m_ReciprocalCoherencyToReciprocalMuellerImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_ReciprocalCoherencyToReciprocalMuellerImageFilter->GetOutput(), "ReciprocalCoherencyToMuellerImageFilter",
                                otbGetTextMacro("RecCoherency to Mueller image"));
      hasOutput = true;
    }

  if( hasOutput )
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
  
  
  
void SARPolarimetryReciprocalModule::Quit()
{
  // 1First, clear any previous output
  this->ClearOutputDescriptors();
  this->Hide();
  // Once module is closed, it is no longer busy
  this->BusyOff();
}
  
  
void SARPolarimetryReciprocalModule::CheckAll(bool val)
{
  if( m_InputImage->GetNumberOfComponentsPerPixel() == 10)
    {
      rb_Mue->value(val);
    }
  else if( m_InputImage->GetNumberOfComponentsPerPixel() == 6)
    {
      rb_CohDeg->value(val);
      rb_RecCoh->value(val);
    }
}


} // End namespace otb
