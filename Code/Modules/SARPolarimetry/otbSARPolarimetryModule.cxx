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
#include "otbSARPolarimetryModule.h"

#include "otbFltkFilterWatcher.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
  SARPolarimetryModule::SARPolarimetryModule()
{
  // First, do constructor stuffs 
  // Input images
  m_HHImage = ComplexImageType::New();
  m_HVImage = ComplexImageType::New();
  m_VHImage = ComplexImageType::New();
  m_VVImage = ComplexImageType::New();

  // Filters
  m_SinclairToReciprocalCovarianceMatrixFilter = SinclairToReciprocalCovarianceMatrixFilterType::New();
  m_SinclairToReciprocalCircularCovarianceMatrixFilter = SinclairToReciprocalCircularCovarianceMatrixFilterType::New();
  m_SinclairToReciprocalCoherencyMatrixFilter = SinclairToReciprocalCoherencyMatrixFilterType::New();

  // Add inputs
  this->AddInputDescriptor<VectorImageType>("InputImage", otbGetTextMacro("Statistics for image"), true, false);
  this->AddTypeToInputDescriptor<ImageType>("InputImage");
  this->AddTypeToInputDescriptor<ComplexImageType>("InputImage");
  this->AddTypeToInputDescriptor<ComplexVectorImageType>("InputImage");
  this->AddInputDescriptor<ComplexImageType>("TheHHImage", otbGetTextMacro("HH Image"), true, false);
  this->AddInputDescriptor<ComplexImageType>("TheHVImage", otbGetTextMacro("HV Image"), true, false);
  this->AddInputDescriptor<ComplexImageType>("TheVHImage", otbGetTextMacro("VH Image"), true, false);
  this->AddInputDescriptor<ComplexImageType>("TheVVImage", otbGetTextMacro("VV Image"), true, false);
}

/** Destructor */
SARPolarimetryModule::~SARPolarimetryModule()
{}

/** PrintSelf method */
void SARPolarimetryModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SARPolarimetryModule::Run()
{
  // While the viewer is shown, it is busy
  this->BusyOn();
  
  this->BuildGUI();
  this->CheckInputs();
  this->InitChecks();

  unsigned int nbOfChannels(1);
  
  // Init the list of inputs image
  /*
  m_InputImage = this->GetInputData<VectorImageType>("InputImage");
  if (m_InputImage.IsNull())
    {
      ImageType::Pointer image = this->GetInputData<ImageType>("InputImage");
      
      if(image.IsNotNull() == true)
        {
          m_ImageToVectorImageFilter = ImageToVectorImageCastFilterType::New();
          // Cast image into vectorImage
          m_ImageToVectorImageFilter->SetInput( image );
          m_ImageToVectorImageFilter->UpdateOutputInformation();
          
          m_InputImage = m_ImageToVectorImageFilter->GetOutput();
          
          // Compute nb of channels
          nbOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();
          // Compute image size
          imSize = m_InputImage->GetLargestPossibleRegion().GetSize();
        }
      else
        {
          this->Quit();
          itkExceptionMacro(<< "The input image number " << 0 << " is Null.");
        }
    }
  */  
  //m_InputImage->UpdateOutputInformation();
  // Compute nb of channels
  //nbOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();
  
  this->Show();
  
}

  void SARPolarimetryModule::CheckInputs()
  {
    m_HHImage = this->GetInputData<ComplexImageType>("TheHHImage");
    m_HVImage = this->GetInputData<ComplexImageType>("TheHVImage");
    m_VHImage = this->GetInputData<ComplexImageType>("TheVHImage");
    m_VVImage = this->GetInputData<ComplexImageType>("TheVVImage");

    bool isOK = false;
    // User doesn't use HH, HV, VH, VV
    if( m_HHImage.IsNull() && m_HVImage.IsNull() && m_VHImage.IsNull() && m_VVImage.IsNull() ) 
      {
        isOK = true;
      }
    //  User uses HH, HV, VH, VV
    else if( m_HHImage.IsNotNull() && m_HVImage.IsNotNull() && m_VHImage.IsNotNull() && m_VVImage.IsNotNull() )
      {
        isOK = true;
      }
    
    if(isOK == false)
      {
        MsgReporter::GetInstance()->SendError("Invalid Inputs. HH, HV, VH and VV has all to be set.");
        this->Quit();
      }

  }


void SARPolarimetryModule::InitChecks()
{
  // The CheckInputs methods insures that both HH, VH, HV and VV are set or not
  if(m_VVImage.IsNull())
    {
      rb_CirCov->deactivate();
      rb_Coh->deactivate();
      rb_Cov->deactivate();
      rb_RecCoh->deactivate();
      rb_Mue->deactivate();
      rb_RecCirCoh->deactivate();
      rb_RecCov->deactivate();
    }
}

  void SARPolarimetryModule::Ok()
  {
    this->ClearOutputDescriptors();
    if( rb_CirCov->value() == true )
      {
      }
    if( rb_Coh->value() == true )
      {
      }
    if( rb_Cov->value() == true )
      {
      }
    // SinclairToReciprocalCoherencyMatrixFilter
    if( rb_RecCoh->value() == true )
      {
        m_SinclairToReciprocalCoherencyMatrixFilter->SetInputHH( m_HHImage );
        m_SinclairToReciprocalCoherencyMatrixFilter->SetInputHV_VH( m_HVImage );
        m_SinclairToReciprocalCoherencyMatrixFilter-> SetInputVV( m_VVImage );
        this->AddOutputDescriptor(m_SinclairToReciprocalCoherencyMatrixFilter->GetOutput(), "SinclairToReciprocalCoherencyImage",
                                  otbGetTextMacro("Sinclair to reciprocal coherency image"));
      }
    if( rb_Mue->value() == true )
      {
      }
    // SinclairToReciprocalCircularCovarianceMatrixFilter
    if( rb_RecCirCoh->value() == true )
      {
        m_SinclairToReciprocalCircularCovarianceMatrixFilter->SetInputHH( m_HHImage );
        m_SinclairToReciprocalCircularCovarianceMatrixFilter->SetInputHV_VH( m_HVImage );
        m_SinclairToReciprocalCircularCovarianceMatrixFilter->SetInputVV( m_VVImage );
        this->AddOutputDescriptor(m_SinclairToReciprocalCircularCovarianceMatrixFilter->GetOutput(), "SinclairToReciprocalCircularCovariance",
                                  otbGetTextMacro("Sinclair to reciprocal circular covariance"));
      }
    // SinclairToReciprocalCovarianceMatrixFilter
    if( rb_RecCov->value() == true )
      {
        m_SinclairToReciprocalCovarianceMatrixFilter->SetInputHH( m_HHImage );
        m_SinclairToReciprocalCovarianceMatrixFilter->SetInputHV_VH( m_HVImage );
        m_SinclairToReciprocalCovarianceMatrixFilter->SetInputVV( m_VVImage );
        this->AddOutputDescriptor(m_SinclairToReciprocalCovarianceMatrixFilter->GetOutput(), "SinclairToReciprocalCovariance",
                                  otbGetTextMacro("Sinclair to reciprocal covariance"));
      }

    this->NotifyOutputsChange();
    this->Hide();
    // Once module is closed, it is no longer busy
    this->BusyOff();
  }

     
     


  void SARPolarimetryModule::Quit()
  {
    // 1First, clear any previous output
    this->ClearOutputDescriptors();
    this->Hide();
    // Once module is closed, it is no longer busy
    this->BusyOff();
  }
  

  void SARPolarimetryModule::CheckAll(bool val)
{
  rb_CirCov->value(val);
  rb_Coh->value(val);
  rb_Cov->value(val);
  rb_RecCoh->value(val);
  rb_Mue->value(val);
  rb_RecCirCoh->value(val);
  rb_RecCov->value(val);
  rb_MuePolDegPow->value(val);
  rb_MueRecCv->value(val);
  rb_RecCohMue->value(val);
  rb_RecCovCohDeg->value(val);
  rb_RecCovRecCoh->value(val);
  rb_RecHAlp->value(val);
  rb_RecLinCov->value(val);
  
}


} // End namespace otb
