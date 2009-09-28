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
#include "otbOGRVectorDataIO.h"
#include <FLU/Flu_File_Chooser.h>

namespace otb
{
/** Constructor */
WriterModule::WriterModule()
{
  m_FPVWriter = FPVWriterType::New();
  m_VectorWriter = VectorWriterType::New();
  
   // Describe inputs
  this->AddInputDescriptor<FPVImageType>("InputDataSet","Dataset to write.");
  this->AddTypeToInputDescriptor<VectorType>("InputDataSet");
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
  const Superclass::InputDataDescriptorMapType inMap = this->GetInputsMap();
  if(key == "InputDataSet")
  {
  if(data.GetDataType() == TypeManager::GetInstance()->GetTypeName<FPVImageType>())
    {

    FPVImageType * image = dynamic_cast<FPVImageType *>(data.GetDataObject());
    m_FPVWriter->SetInput(image);
    }
   else if (data.GetDataType() == TypeManager::GetInstance()->GetTypeName<FPVImageType>())
     {
     VectorType * vect = dynamic_cast<VectorType *>(data.GetDataObject());
     m_VectorWriter->SetInput(vect);  
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
  typedef OGRVectorDataIO<VectorType> OGRVectorDataIOType;
  OGRVectorDataIOType::Pointer test=OGRVectorDataIOType::New() ;
  
  if ( test->CanWriteFile(filepath.c_str()) ) 
  {
    m_VectorWriter->SetFileName(filepath);
    m_VectorWriter->Update();
  }
  else
  {
    m_FPVWriter->SetFileName(filepath);
    m_FPVWriter->Update();
  }
}

void WriterModule::Browse()
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

void WriterModule::Cancel()
{
  wFileChooserWindow->hide();
}


} // End namespace otb

#endif
