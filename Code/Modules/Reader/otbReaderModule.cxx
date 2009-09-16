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
#include <FL/Fl_File_Chooser.H>

namespace otb
{
/** Constructor */
ReaderModule::ReaderModule()
{
  m_FPVReader = FPVReaderType::New();
  m_VectorReader = VectorReaderType::New();
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

/** Retrieve output by key  This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
const DataObjectWrapper ReaderModule::RetrieveOutputByKey(const std::string & key) const
{
  DataObjectWrapper wrapper;
  if(key == "OutputDataSet")
    {
    const Superclass::OutputDataDescriptorMapType outMap = this->GetOutputsMap();

    if(outMap.find(key)->second.GetDataType() == "Floating_Point_VectorImage")
      {                                          
      wrapper.Set("Floating_Point_VectorImage",m_FPVReader->GetOutput());
      }
    else if(outMap.find(key)->second.GetDataType() == "VectorData")
      {
      wrapper.Set("VectorData",m_VectorReader->GetOutput());
      }
    }
  return wrapper;
}

/** The custom run command */
void ReaderModule::Run()
{
  this->BuildGUI();
  wFileChooserWindow->show();
}

void ReaderModule::OpenDataSet()
{
  std::string filepath = vFilePath->value();

  bool typeFound = false;

  try
    {
    m_FPVReader->SetFileName(filepath);
    m_FPVReader->GenerateOutputInformation();
    // If we are still here, this is a readable image
    typeFound = true;
    this->AddOutputDescriptor("Floating_Point_VectorImage","OutputDataSet","Image read from file");
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
      this->AddOutputDescriptor("VectorData","OutputDataSet","Vector read from file");
      }
    catch(itk::ExceptionObject & err)
      {
      // Silent catch
      }
    }

  wFileChooserWindow->hide();
}

void ReaderModule::Browse()
{
  const char * filename = NULL;

  filename = fl_file_chooser("Choose the dataset file ...", "*.*",".");
  
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
