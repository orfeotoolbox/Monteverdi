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
#ifndef __otbReaderModule_cxx
#define __otbReaderModule_cxx

#include "otbReaderModule.h"
#include <FLU/Flu_File_Chooser.h>
#include "base/ossimFilename.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
ReaderModule::ReaderModule()
{
  // Build filters
  m_FPVReader = FPVReaderType::New();
  m_VectorReader = VectorReaderType::New();
  m_ComplexReader = ComplexImageReaderType::New();
  m_LabeledVectorReader = LabeledVectorReaderType::New();
  m_AmplitudeFilter = AmplitudeFilterType::New();
  m_ExtractROIFilterList = ExtractROIImageFilterListType::New();
  m_RealFilter = RealFilterType::New();
  m_ImaginaryFilter = ImaginaryFilterType::New();
  m_ModulusFilter = ModulusFilterType::New();
  m_PhaseFilter = PhaseFilterType::New();

  this->BuildGUI();

  // Expose supported data type:
  vType->add("Unknown");
  vType->add("Optical image");
  vType->add("SAR image");
  vType->add("Vector");
  vType->value(0);
  
  // Deactivate ok for now
  bOk->deactivate();

  // No name for now
  vName->value("");
}

/** Destructor */
ReaderModule::~ReaderModule()
{}

/** PrintSelf method */
void ReaderModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void ReaderModule::Run()
{
  wFileChooserWindow->show();
}

void ReaderModule::Analyse()
{
  // Get the path
  std::string filepath = vFilePath->value();

  // Is type found ?
  bool typeFound = false;

  // Get the filename from the filepath
  ossimFilename lFile = ossimFilename(filepath);

  // Try different types
  try
    {
    // Read the image
    m_FPVReader->SetFileName(filepath);
    m_FPVReader->GenerateOutputInformation();

    switch(m_FPVReader->GetImageIO()->GetPixelType())     
      {
      // handle the radar case
      case itk::ImageIOBase::COMPLEX:
	vType->value(2);
	// If we are still here, this is a readable image
	typeFound = true;
	break;

	// handle the optical case
      default:
	vType->value(1);
	// If we are still here, this is a readable image
	typeFound = true;
	break;
      }
    }
  catch(itk::ExceptionObject & err)
    {
    // Silent catch
    }

  if(!typeFound)
    {
    try
      {
      m_LabeledVectorReader->SetFileName(filepath);
      m_LabeledVectorReader->GenerateOutputInformation();
      vType->value(3);
      typeFound = true;
      }
    catch(itk::ExceptionObject & err)
      {
      // Silent catch
      vType->value(0);
      }
    }
  
  // Activate/ deactivate ok
  if(!typeFound)
    {
    vType->value(0);
    bOk->deactivate();
    }
  else
    {
    bOk->activate();
    }

  std::string name = vName->value();

  if(name.empty())
    {
    ossimFilename fname (vFilePath->value());
    vName->value(fname.fileNoExtension());
    }
}



void ReaderModule::OpenDataSet()
{
  try
    {
    switch(vType->value())
      {
      case 1:
	this->OpenOpticalImage();
	break;
      case 2:
	this->OpenSarImage();
	break;
      case 3:
	this->OpenVector();
	break;
      default:
	itkExceptionMacro(<<"Unknow dataset type.");
	break;
      }
 
    wFileChooserWindow->hide();
  
    // Notify all listener
    // TODO: this should not be done by the user
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ReaderModule::TypeChanged()
{
  if(vType->value() >0)
    {
    bOk->activate();
    }
  else
    {
    bOk->deactivate();
    }
}

void ReaderModule::OpenOpticalImage()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  ostringstream oss,ossId; 
  std::string filepath = vFilePath->value();
  ossimFilename lFile = ossimFilename(filepath);

  // Add the full data set as a descriptor
  oss << "Image read from file: " << lFile.file();
  ossId<<vName->value()<<" (Whole dataset)";
  this->AddOutputDescriptor(m_FPVReader->GetOutput(),ossId.str(),oss.str());
  // Add sub-bands
  for(unsigned int band = 0; band<m_FPVReader->GetOutput()->GetNumberOfComponentsPerPixel();++band)
    {
    // Extract band
    ExtractROIImageFilterType::Pointer extract = ExtractROIImageFilterType::New();
    extract->SetInput(m_FPVReader->GetOutput());
    extract->SetChannel(band+1);
    m_ExtractROIFilterList->PushBack(extract);
    
    // Description
    oss.str("");
    oss << "Band "<<band+1<<" read from file: " << lFile.file();
    // Output ID
    ossId.str("");
    ossId<<vName->value()<<" (band "<<band+1<<")";
    this->AddOutputDescriptor(extract->GetOutput(),ossId.str(),oss.str());
    }
  m_AmplitudeFilter->SetInput(m_FPVReader->GetOutput());
  oss.str("");
  oss <<"Image amplitude read from file: "<<vName->value();
  ossId.str("");
  ossId<<vName->value()<<" (amplitude)";
  this->AddOutputDescriptor(m_AmplitudeFilter->GetOutput(),ossId.str(),oss.str());
}


void ReaderModule::OpenSarImage()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  ostringstream oss,ossId; 
  std::string filepath = vFilePath->value();
  ossimFilename lFile = ossimFilename(filepath);

  m_ComplexReader->SetFileName(filepath);
  m_ComplexReader->GenerateOutputInformation();
  oss <<"Complex image read from file: "<<lFile.file();
  ossId<<vName->value()<<" (whole dataset)";
  this->AddOutputDescriptor(m_ComplexReader->GetOutput(),ossId.str(),oss.str());
   
  m_RealFilter->SetInput(m_ComplexReader->GetOutput());
  oss.str("");
  oss <<"Image real part read from file: "<<lFile.file();
  ossId.str("");
  ossId<<vName->value()<<" (real part)";
  this->AddOutputDescriptor(m_RealFilter->GetOutput(),ossId.str(),oss.str());
   
  m_ImaginaryFilter->SetInput(m_ComplexReader->GetOutput());
  oss.str("");
  oss <<"Image Imaginary part read from file: "<<lFile.file();
  ossId.str("");
  ossId<<vName->value()<<" (imaginary part)";
  this->AddOutputDescriptor(m_ImaginaryFilter->GetOutput(),ossId.str(),oss.str());
  m_ModulusFilter->SetInput(m_ComplexReader->GetOutput());
  oss.str("");
  oss <<"Image modulus read from file: "<<lFile.file();
  ossId.str("");
  ossId<<vName->value()<<" (modulus)";
  this->AddOutputDescriptor(m_ModulusFilter->GetOutput(),ossId.str(),oss.str());
   
  m_PhaseFilter->SetInput(m_ComplexReader->GetOutput());
  oss.str("");
  oss <<"Image phase part read from file: "<<lFile.file();
  ossId.str("");
  ossId<<vName->value()<<" (phase)";
  this->AddOutputDescriptor(m_PhaseFilter->GetOutput(),ossId.str(),oss.str());
}

void ReaderModule::OpenVector()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  ostringstream oss,ossId; 
  std::string filepath = vFilePath->value();
  ossimFilename lFile = ossimFilename(filepath);

  m_VectorReader->SetFileName(filepath);
  oss << "Vector read from file : " << lFile.file();
  ossId<<vName->value()<<" (whole dataset)";
  this->AddOutputDescriptor(m_VectorReader->GetOutput(),ossId.str(),oss.str());
}


void ReaderModule::Browse()
{
  const char * filename = NULL;

  filename = flu_file_chooser("Choose the dataset file ...", "*.*",".");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }
  vFilePath->value(filename);
  this->Analyse();
}

void ReaderModule::Cancel()
{
  wFileChooserWindow->hide();
}


} // End namespace otb

#endif
