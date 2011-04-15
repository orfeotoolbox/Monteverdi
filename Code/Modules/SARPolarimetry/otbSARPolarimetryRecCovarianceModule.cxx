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
#include "otbSARPolarimetryRecCovarianceModule.h"

#include "otbFltkFilterWatcher.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
  SARPolarimetryRecCovarianceModule::SARPolarimetryRecCovarianceModule()
{
  // Filters
  m_ReciprocalCovarianceToReciprocalCoherencyImageFilter = ReciprocalCovarianceToReciprocalCoherencyImageFilterType::New();
  m_ReciprocalCovarianceToCoherencyDegreeImageFilter = ReciprocalCovarianceToCoherencyDegreeImageFilterType::New();

  m_InputImage = ComplexVectorImageType::New();

  // Add inputs
  this->AddInputDescriptor<ComplexVectorImageType>("InputImage", otbGetTextMacro("Input Image"));
}

/** Destructor */
SARPolarimetryRecCovarianceModule::~SARPolarimetryRecCovarianceModule()
{}

/** PrintSelf method */
void SARPolarimetryRecCovarianceModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SARPolarimetryRecCovarianceModule::Run()
{
  // While the viewer is shown, it is busy
  this->BusyOn();

  this->CheckInputs();
  this->BuildGUI();

  this->Show();
  
}

/** Check inputs configuration */
void SARPolarimetryRecCovarianceModule::CheckInputs()
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
      if( m_InputImage->GetNumberOfComponentsPerPixel() != 6)
        {
          itk::OStringStream oss;
          oss << "Invalid Input, must have 6 channels instead of " << m_InputImage->GetNumberOfComponentsPerPixel() << ".";
          MsgReporter::GetInstance()->SendError( oss.str() );
          this->Quit();
        }
    }
}


void SARPolarimetryRecCovarianceModule::Ok()
{
  this->ClearOutputDescriptors();
  
  if( rb_RecCoh->value() == true )
    {
      m_ReciprocalCovarianceToReciprocalCoherencyImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_ReciprocalCovarianceToReciprocalCoherencyImageFilter->GetOutput(), "ReciprocalCovarianceToReciprocalCoherencyImageFilter", otbGetTextMacro("RecCovariance to rec. coherency image"));
    }
  if( rb_CohDeg->value() == true )
    {
      m_ReciprocalCovarianceToCoherencyDegreeImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_ReciprocalCovarianceToCoherencyDegreeImageFilter->GetOutput(), "ReciprocalCovarianceToCoherencyDegreeImageFilter", otbGetTextMacro("RecCovariance to cohenrency degree image"));
    }
  
  this->NotifyOutputsChange();
  this->Hide();
  // Once module is closed, it is no longer busy
    this->BusyOff();
}
  
  
  
void SARPolarimetryRecCovarianceModule::Quit()
{
  // 1First, clear any previous output
  this->ClearOutputDescriptors();
  this->Hide();
  // Once module is closed, it is no longer busy
  this->BusyOff();
}
  
  
void SARPolarimetryRecCovarianceModule::CheckAll(bool val)
{
  rb_CohDeg->value(val);
  rb_RecCoh->value(val); 
}


} // End namespace otb
