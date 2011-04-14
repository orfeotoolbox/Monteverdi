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
#include "otbSARPolarimetryConversionModule.h"

#include "otbFltkFilterWatcher.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
  SARPolarimetryConversionModule::SARPolarimetryConversionModule()
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

  m_SinclairToCovarianceMatrixFilter = SinclairToCovarianceMatrixFilterType::New();
  m_SinclairToCircularCovarianceMatrixFilter = SinclairToCircularCovarianceMatrixFilterType::New();
  m_SinclairToCoherencyMatrixFilter = SinclairToCoherencyMatrixFilterType::New();
  m_SinclairToMuellerMatrixFilter = SinclairToMuellerMatrixFilterType::New();

  // Add inputs
  this->AddInputDescriptor<ComplexImageType>("HHImage", otbGetTextMacro("HH Image"));
  this->AddInputDescriptor<ComplexImageType>("HVImage", otbGetTextMacro("HV Image (used for reciprocal process)"));
  this->AddInputDescriptor<ComplexImageType>("VHImage", otbGetTextMacro("VH Image( optional)"), true, false);
  this->AddInputDescriptor<ComplexImageType>("VVImage", otbGetTextMacro("VV Image"));
}

/** Destructor */
SARPolarimetryConversionModule::~SARPolarimetryConversionModule()
{}

/** PrintSelf method */
void SARPolarimetryConversionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void SARPolarimetryConversionModule::Run()
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

  void SARPolarimetryConversionModule::CheckInputs()
  {
    m_HHImage = this->GetInputData<ComplexImageType>("HHImage");
    m_HVImage = this->GetInputData<ComplexImageType>("HVImage");
    m_VHImage = this->GetInputData<ComplexImageType>("VHImage");
    m_VVImage = this->GetInputData<ComplexImageType>("VVImage");

    if( m_HHImage.IsNull() || m_HVImage.IsNull() || m_VVImage.IsNull() )
      {
        MsgReporter::GetInstance()->SendError("Invalid Inputs. HH, HV, VH and VV has all to be set.");
        this->Quit();
      }
  }


void SARPolarimetryConversionModule::InitChecks()
{
  // The CheckInputs methods insures that both HH, VH, HV and VV are set or not
  if(m_VHImage.IsNull())
    {
      rb_CirCov->deactivate();
      rb_Coh->deactivate();
      rb_Cov->deactivate();
      rb_Mue->deactivate();
    }
}
  
void SARPolarimetryConversionModule::Ok()
{
  this->ClearOutputDescriptors();
  if( rb_CirCov->value() == true )
    {
      m_SinclairToCircularCovarianceMatrixFilter->SetInputHH(m_HHImage);
      m_SinclairToCircularCovarianceMatrixFilter->SetInputHV(m_HVImage);
      m_SinclairToCircularCovarianceMatrixFilter->SetInputVH(m_VHImage);
      m_SinclairToCircularCovarianceMatrixFilter->SetInputVV(m_VVImage);
    this->AddOutputDescriptor(m_SinclairToCircularCovarianceMatrixFilter->GetOutput(), "SinclairToCircularCovarianceMatrixFilter",
                                  otbGetTextMacro("Sinclair to circular covariance image"));
    }
  if( rb_Coh->value() == true )
    {
      m_SinclairToCoherencyMatrixFilter->SetInputHH(m_HHImage);
      m_SinclairToCoherencyMatrixFilter->SetInputHV(m_HVImage);
      m_SinclairToCoherencyMatrixFilter->SetInputVH(m_VHImage);
      m_SinclairToCoherencyMatrixFilter->SetInputVV(m_VVImage);
   this->AddOutputDescriptor(m_SinclairToCoherencyMatrixFilter->GetOutput(), "SinclairToCoherencyMatrixFilter",
                                  otbGetTextMacro("Sinclair to coherency image"));
    }
  if( rb_Cov->value() == true )
    {
      m_SinclairToCovarianceMatrixFilter->SetInputHH(m_HHImage);
      m_SinclairToCovarianceMatrixFilter->SetInputHV(m_HVImage);
      m_SinclairToCovarianceMatrixFilter->SetInputVH(m_VHImage);
      m_SinclairToCovarianceMatrixFilter->SetInputVV(m_VVImage);
   this->AddOutputDescriptor(m_SinclairToCovarianceMatrixFilter->GetOutput(), "SinclairToCovarianceMatrixFilter",
                                  otbGetTextMacro("Sinclair to covariance image"));
    }
  if( rb_Mue->value() == true )
    {
      m_SinclairToMuellerMatrixFilter->SetInputHH(m_HHImage);
      m_SinclairToMuellerMatrixFilter->SetInputHV(m_HVImage);
      m_SinclairToMuellerMatrixFilter->SetInputVH(m_VHImage);
      m_SinclairToMuellerMatrixFilter->SetInputVV(m_VVImage);
   this->AddOutputDescriptor(m_SinclairToMuellerMatrixFilter->GetOutput(), "SinclairToMuellerMatrixFilter",
                                  otbGetTextMacro("Sinclair to Mueller image"));
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

     
     


  void SARPolarimetryConversionModule::Quit()
  {
    // 1First, clear any previous output
    this->ClearOutputDescriptors();
    this->Hide();
    // Once module is closed, it is no longer busy
    this->BusyOff();
  }
  

  void SARPolarimetryConversionModule::CheckAll(bool val)
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
 
}


} // End namespace otb
