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
  if(key == "OutputImage")
    {
    wrapper.Set("Floating_Point_VectorImage",m_FPVReader->GetOutput());
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
  m_FPVReader->SetFileName(filepath);
  m_FPVReader->GenerateOutputInformation();
  this->AddOutputDescriptor("Floating_Point_VectorImage","OutputImage","Image read from file");
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
