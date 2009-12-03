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
#include "otbI18n.h"

namespace otb
{
/** Constructor */
ReaderModule::ReaderModule()
{
  // Build filters
  m_FPVReader = FPVReaderType::New();
  m_VectorReader = VectorReaderType::New();
  m_LabeledVectorReader = LabeledVectorReaderType::New();
  m_ComplexReader = ComplexImageReaderType::New();

  this->BuildGUI();

  // Expose supported data type:
  vType->add(otbGetTextMacro("Unknown"));
  vType->add(otbGetTextMacro("Optical image"));
  vType->add(otbGetTextMacro("SAR image"));
  vType->add(otbGetTextMacro("Vector"));
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
  catch(itk::ExceptionObject)
    {
    // Silent catch
    }

  if(!typeFound)
    {
    try
      {
      VectorReaderType::Pointer vectorReader = VectorReaderType::New();
      vectorReader->SetFileName(filepath);
      vectorReader->GenerateOutputInformation();
      vType->value(3);
      typeFound = true;
      }
    catch(itk::ExceptionObject)
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

  m_FPVReader->SetFileName(filepath);
  m_FPVReader->GenerateOutputInformation();

  // Add the full data set as a descriptor
  oss << "Image read from file: " << lFile.file();
  ossId<<vName->value();
  this->AddOutputDescriptor(m_FPVReader->GetOutput(),ossId.str(),oss.str(),true);
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
  ossId<<vName->value();
  this->AddOutputDescriptor(m_ComplexReader->GetOutput(),ossId.str(),oss.str(),true);
}

void ReaderModule::OpenVector()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  ostringstream oss,ossId;
  std::string filepath = vFilePath->value();
  ossimFilename lFile = ossimFilename(filepath);

  m_VectorReader->SetFileName(filepath);
  m_VectorReader->GenerateOutputInformation();

  oss << "Vector read from file : " << lFile.file();
  ossId<<vName->value()<<" (whole dataset)";
  this->AddOutputDescriptor(m_VectorReader->GetOutput(),ossId.str(),oss.str());
}


void ReaderModule::Browse()
{
  const char * filename = NULL;

  filename = flu_file_chooser("Choose the dataset file...", "*.*",".");
  
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
  this->Hide();
}

void ReaderModule::Hide()
{
  wFileChooserWindow->hide();
}
} // End namespace otb

#endif
