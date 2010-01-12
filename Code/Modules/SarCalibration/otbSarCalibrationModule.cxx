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
#ifndef __otbSarCalibrationModule_cxx
#define __otbSarCalibrationModule_cxx

#include "otbSarCalibrationModule.h"

#include "otbTerraSarImageMetadataInterface.h"
#include "itkMetaDataDictionary.h"
#include "otbMsgReporter.h"

namespace otb
{
/**
 * Constructor
 */
SarCalibrationModule
::SarCalibrationModule()
{
  // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Input image"));
  this->AddTypeToInputDescriptor<ComplexImageType>("InputImage");

  m_WorkWithCplx = false;
  // Build the GUI
  this->BuildGUI();
}

/**
 * Destructor
 */
SarCalibrationModule
::~SarCalibrationModule()
{}


/** The custom run command */
void
SarCalibrationModule
::Run()
{
  this->BusyOn();

  m_InputImage = this->GetInputData<ImageType>("InputImage");
  if(m_InputImage.IsNull())
    {
      m_ComplexInputImage = this->GetInputData<ComplexImageType>("InputImage");
      if( m_ComplexInputImage.IsNull() )
	{
	  this->BusyOff();
	  itkExceptionMacro(<<"Input image is NULL");
	}
      else
	m_WorkWithCplx = true;
    }

  if( this->CheckMetadata() )
    {
      // open the GUI
      this->Show();
    }
  else
    {
      this->BusyOff();
    }
}


bool 
SarCalibrationModule
::CheckMetadata()
{
  TerraSarImageMetadataInterface::Pointer lImageMetadata = TerraSarImageMetadataInterface::New();
  itk::MetaDataDictionary dict;
  
  if(!m_WorkWithCplx)
    dict = m_InputImage->GetMetaDataDictionary();
  else
    dict = m_ComplexInputImage->GetMetaDataDictionary();
  
  if( !lImageMetadata->CanRead(dict) )
    {
      MsgReporter::GetInstance()->SendError("Invalid Image : No TerraSarX metadata detected");
      return false;
    }

  try
    {
      // Test if all used metadatadatas are available...
      lImageMetadata->GetCalibrationFactor(dict);
      lImageMetadata->GetNoiseValidityRangeMinList(dict);
      lImageMetadata->GetNoiseValidityRangeMaxList(dict);
      lImageMetadata->GetNoiseReferencePointList(dict);
      lImageMetadata->GetNoisePolynomialCoefficientsList(dict);
      lImageMetadata->GetRadarFrequency(dict);
      lImageMetadata->GetNoiseTimeUTCList(dict);	  
      lImageMetadata->GetMeanIncidenceAngles(dict);
    }
  catch(itk::ExceptionObject & err)
    {
      itk::OStringStream oss;
      oss.str("");
      oss << "Invalid input image medtadata. The parsing returns the following error:\n";
      oss << err.GetDescription();
      MsgReporter::GetInstance()->SendError(oss.str().c_str());
      return false;
    }

  return true;
  
}


void 
SarCalibrationModule
::OK()
{
  this->ClearOutputDescriptors();
  if(!m_WorkWithCplx)
    {
      if(bCalib->value() == 1)
	{
	  m_CalibFilter = CalibrationFilterType::New();
	  m_CalibFilter->SetInput(m_InputImage);
	  if( bFastMethodYes->value()==1 && bFastMethodNo->value()==0 )
	    m_CalibFilter->SetUseFastCalibrationMethod( true );
	  else if(  bFastMethodYes->value()==0 && bFastMethodNo->value()==1 )
	    m_CalibFilter->SetUseFastCalibrationMethod( false );
    
	  this->AddOutputDescriptor(m_CalibFilter->GetOutput(),"CalibOutputImage",otbGetTextMacro("Calibrated image"));
	}
      if(bBrightness->value() == 1)
	{
	  m_BrighFilter = BrightnessFilterType::New();
	  m_BrighFilter->SetInput(m_InputImage);
	  this->AddOutputDescriptor(m_BrighFilter->GetOutput(),"BrightnessOutputImage",otbGetTextMacro("Brightness image"));
	}
    }
  else
    {
      if(bCalib->value() == 1)
	{
	  m_ComplexCalibFilter = CalibrationComplexFilterType::New();
	  m_ComplexCalibFilter->SetInput(m_ComplexInputImage);
	  if( bFastMethodYes->value()==1 && bFastMethodNo->value()==0 )
	    m_ComplexCalibFilter->SetUseFastCalibrationMethod( true );
	  else if(  bFastMethodYes->value()==0 && bFastMethodNo->value()==1 )
	    m_ComplexCalibFilter->SetUseFastCalibrationMethod( false );
	  
	  this->AddOutputDescriptor(m_ComplexCalibFilter->GetOutput(),"CalibOutputImage",otbGetTextMacro("Calibrated image"));
	}
      if(bBrightness->value() == 1)
	{
	  m_BrighComplexFilter = BrightnessComplexFilterType::New();
	  m_BrighComplexFilter->SetInput(m_ComplexInputImage);
	  this->AddOutputDescriptor(m_BrighComplexFilter->GetOutput(),"BrightnessOutputImage",otbGetTextMacro("Brightness image"));
	}
    }


  this->NotifyOutputsChange();

  // close the GUI
  this->Hide();
  this->BusyOff();
}

} // End namespace otb

#endif
