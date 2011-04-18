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
#include "otbSARPolarimetryAnalysisModule.h"

#include "otbFltkFilterWatcher.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
  SARPolarimetryAnalysisModule::SARPolarimetryAnalysisModule()
{
  // Filters
  m_MultiChannelsPolarimetricSynthesisFilter = MultiChannelsPolarimetricSynthesisFilterType::New();
  m_ReciprocalHAlphaImageFilter              = ReciprocalHAlphaImageFilterType::New();

  m_InputImage = ComplexVectorImageType::New();

  // Add inputs
  this->AddInputDescriptor<ComplexVectorImageType>("InputImage", otbGetTextMacro("Input Image (multi channel complex image, 4 or 6 channels)"));
}

/** Destructor */
SARPolarimetryAnalysisModule::~SARPolarimetryAnalysisModule()
{}

/** PrintSelf method */
void SARPolarimetryAnalysisModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SARPolarimetryAnalysisModule::Run()
{
  // While the viewer is shown, it is busy
  this->BusyOn();

  this->BuildGUI();
  this->CheckInputs();

  this->Show();
  
}

/** Check inputs configuration */
void SARPolarimetryAnalysisModule::CheckInputs()
{
  m_InputImage = this->GetInputData<ComplexVectorImageType>("InputImage");

  if( m_InputImage.IsNull() )
    {
      MsgReporter::GetInstance()->SendError("No input image detected.");
      this->Quit();
    }
  else
    {
      this->CheckAll(false);
      m_InputImage->UpdateOutputInformation();
      if( m_InputImage->GetNumberOfComponentsPerPixel() == 6 )
        {
          rb_HAlpha->value(1);
          rb_Syn->deactivate();
          gSynthParam->deactivate(); // activate parameter selection
        }
      else if( m_InputImage->GetNumberOfComponentsPerPixel() == 4 )
        {
          rb_HAlpha->deactivate();
          rb_Syn->value(1);
          gSynthParam->activate(); // activate parameter selection
        }
      else
        {
          itk::OStringStream oss;
          oss << "Invalid Input, must have 6 channels (for HAlpha process) or 4 channels (for synthesys process) instead of " << m_InputImage->GetNumberOfComponentsPerPixel() << ".";
          MsgReporter::GetInstance()->SendError( oss.str() );
          this->Quit();
        }
    }
}


void SARPolarimetryAnalysisModule::Ok()
{   
  this->ClearOutputDescriptors();
  bool hasOutput = false;  

  if( rb_HAlpha->value() == true )
    {
      m_ReciprocalHAlphaImageFilter->SetInput(m_InputImage);
      
      this->AddOutputDescriptor(m_ReciprocalHAlphaImageFilter->GetOutput(), "ReciprocalHAlphaImageFilter",
                                otbGetTextMacro("Reciprocal H-Alpha image"));
      hasOutput = true;
    }
  if( rb_Syn->value() == true )
    {
      m_MultiChannelsPolarimetricSynthesisFilter->SetInput(m_InputImage);
      m_MultiChannelsPolarimetricSynthesisFilter->SetKhiI( static_cast<double>(v_KhiI->value()) );
      m_MultiChannelsPolarimetricSynthesisFilter->SetKhiR( static_cast<double>(v_KhiR->value()) );
      m_MultiChannelsPolarimetricSynthesisFilter->SetPsiI( static_cast<double>(v_PsiI->value()) );
      m_MultiChannelsPolarimetricSynthesisFilter->SetPsiR( static_cast<double>(v_PsiR->value()) );
  
      this->AddOutputDescriptor(m_MultiChannelsPolarimetricSynthesisFilter->GetOutput(), "MultiChannelsPolarimetricSynthesisFilter", 
                                otbGetTextMacro("Polarimetric synthesis image"));
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
  
  
  
void SARPolarimetryAnalysisModule::Quit()
{
  // 1First, clear any previous output
  this->ClearOutputDescriptors();
  this->Hide();
  // Once module is closed, it is no longer busy
  this->BusyOff();
}
  
  
void SARPolarimetryAnalysisModule::CheckAll(bool val)
{
  rb_HAlpha->value(val);
  rb_Syn->value(val); 
}


} // End namespace otb
