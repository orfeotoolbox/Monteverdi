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

namespace otb
{
/** Constructor */
ReaderModule::ReaderModule()
{
  m_FPVReader = FPVReaderType::New();
  m_VectorReader = VectorReaderType::New();
  m_AmplitudeFilter = AmplitudeFilterType::New();
  m_ExtractROIFilterList = ExtractROIImageFilterListType::New();
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
  this->BuildGUI();
  wFileChooserWindow->show();
}

void ReaderModule::OpenDataSet()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();

  std::string filepath = vFilePath->value();

  bool typeFound = false;

  // Get the filename from the filepath
  ossimFilename lFile = ossimFilename(filepath);
  ostringstream oss; 

  try
    {
    // Read the image
    m_FPVReader->SetFileName(filepath);
    m_FPVReader->GenerateOutputInformation();

    // Add the full data set as a descriptor
    oss << "Image read from file: " << lFile.file();
    this->AddOutputDescriptor(m_FPVReader->GetOutput(),"OutputImage",oss.str());

    // Extract first band
    ExtractROIImageFilterType::Pointer extract = ExtractROIImageFilterType::New();
    extract->SetInput(m_FPVReader->GetOutput());
    extract->SetChannel(1);
    m_ExtractROIFilterList->PushBack(extract);

    // Add the first sub band
    oss.str("");
    oss << "Image bands read from file: " << lFile.file();
    this->AddOutputDescriptor(extract->GetOutput(),"OutputImageBand",oss.str());

    // Add sub-bands
    for(unsigned int band = 1; band<m_FPVReader->GetOutput()->GetNumberOfComponentsPerPixel();++band)
      {
      ExtractROIImageFilterType::Pointer extract = ExtractROIImageFilterType::New();
      extract->SetInput(m_FPVReader->GetOutput());
      extract->SetChannel(band+1);
      m_ExtractROIFilterList->PushBack(extract);
      this->AddDataToOutputDescriptor(extract->GetOutput(),"OutputImageBand");
      }

    m_AmplitudeFilter->SetInput(m_FPVReader->GetOutput());

    oss.str("");
    oss <<" Image amplitude read from file: "<<lFile.file();
    this->AddOutputDescriptor(m_AmplitudeFilter->GetOutput(),"OutputImageAmplitude",oss.str());


    // If we are still here, this is a readable image
    typeFound = true;
    }
  catch(itk::ExceptionObject & err)
    {
    // Silent catch
    } 
  
  // If it is not an image, try to open a VectorData
  if(!typeFound)
    {
    try
      {
      m_VectorReader->SetFileName(filepath);
      m_VectorReader->Update();
      // If we are still here, this is a readable image
      typeFound = true;
      // Get the filename from the filepath
      oss << "Vector read from file : " << lFile.file();
      this->AddOutputDescriptor(m_VectorReader->GetOutput(),"OutputVector",oss.str());
      }
    catch(itk::ExceptionObject & err)
      {
      // Silent catch
      }
    }

  wFileChooserWindow->hide();
  
  // Notify all listener
  // TODO: this should not be done by the user
  this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
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
  
  
}

void ReaderModule::Cancel()
{
  wFileChooserWindow->hide();
}


} // End namespace otb

#endif
