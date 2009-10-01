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
#include "otbWriterModule.h"
#include <FLU/Flu_File_Chooser.h>

namespace otb
{
/** Constructor */
WriterModule::WriterModule()
{
  m_FPVWriter = FPVWriterType::New();
  m_FPWriter = FPWriterType::New();
  m_VectorWriter = VectorWriterType::New();
  
   // Describe inputs
  this->AddInputDescriptor<FPVImageType>("InputDataSet","Dataset to write.");
  this->AddTypeToInputDescriptor<FPImageType>("InputDataSet");
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

/** The custom run command */
void WriterModule::Run()
{
  this->BuildGUI();
  wFileChooserWindow->show();
}

void WriterModule::SaveDataSet()
{
  std::string filepath = vFilePath->value();
  
  FPVImageType::Pointer vectorImage = this->GetInputData<FPVImageType>("InputDataSet");
  FPImageType::Pointer singleImage = this->GetInputData<FPImageType>("InputDataSet");
  VectorType::Pointer vectorData = this->GetInputData<VectorType>("InputDataSet");
					   
  if ( vectorImage.IsNotNull() ) 
    {
    m_FPVWriter->SetInput(vectorImage);
    m_FPVWriter->SetFileName(filepath);
    m_FPVWriter->Update();
    //m_FPVWriter = FPVWriterType::New();
    }
  else if( singleImage.IsNotNull() )
    {
    m_FPWriter->SetInput(singleImage);
    m_FPWriter->SetFileName(filepath);
    m_FPWriter->Update();
    }
  else if( vectorData.IsNotNull() )
    {
    m_VectorWriter->SetInput(vectorData);
    m_VectorWriter->SetFileName(filepath);
    m_VectorWriter->Update();
    }
  else
    {
    itkExceptionMacro(<<"Input data are NULL.");
    }

  wFileChooserWindow->hide();
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

