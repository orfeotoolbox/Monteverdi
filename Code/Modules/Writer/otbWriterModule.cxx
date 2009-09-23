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
#ifndef __otbWriterModule_cxx
#define __otbWriterModule_cxx

#include "otbWriterModule.h"
#include <FL/Fl_File_Chooser.H>

namespace otb
{
/** Constructor */
WriterModule::WriterModule()
{
  m_FPVWriter = FPVWriterType::New();
  m_VectorWriter = VectorWriterType::New();
}

/** Destructor */
WriterModule::~WriterModule()
{}

/** PrintSelf method */
void WriterModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** Assign input by key. This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void WriterModule::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  if(key == "InputDataSet")
  {
    const Superclass::OutputDataDescriptorMapType outMap = this->GetOutputsMap();
    
    if(outMap.find(key)->second.GetDataType() == "Floating_Point_VectorImage")
    {                                          
      FPVImageType * image = dynamic_cast<FPVImageType *>(data.GetDataObject());
      m_FPVWriter->SetInput(image);  
    }
    else if(outMap.find(key)->second.GetDataType() == "VectorData")
    {
      VectorType * vector = dynamic_cast<VectorType *>(data.GetDataObject());
      m_VectorWriter->SetInput(vector);  
    }
  }
}

/** The custom run command */
void WriterModule::Run()
{
  this->BuildGUI();
  wFileChooserWindow->show();
}

void WriterModule::OpenDataSet()
{
  std::string filepath = vFilePath->value();

  bool typeFound = false;

  try
    {
    m_FPVWriter->SetFileName(filepath);
//     m_FPVWriter->GenerateOutputInformation();
    // If we are still here, this is a readable image
    typeFound = true;
    this->AddOutputDescriptor("Floating_Point_VectorImage","InputDataSet","Write image to file");
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
      m_VectorWriter->SetFileName(filepath);
      m_VectorWriter->Update();
      // If we are still here, this is a readable image
      typeFound = true;
      this->AddOutputDescriptor("VectorData","InputDataSet","Write vector to file");
      }
    catch(itk::ExceptionObject & err)
      {
      // Silent catch
      }
    }

  wFileChooserWindow->hide();
}

void WriterModule::Browse()
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

void WriterModule::Cancel()
{
  wFileChooserWindow->hide();
}


} // End namespace otb

#endif
