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
WriterModule::WriterModule() : m_FPVWriter(), m_FPWriter(), m_VectorWriter()
{
  m_FPVWriter    = FPVWriterType::New();
  m_FPWriter     = FPWriterType::New();
  m_VectorWriter = VectorWriterType::New();
 
   // Describe inputs
  this->AddInputDescriptor<FPVImageType>("InputDataSet","Dataset to write.");
  this->AddTypeToInputDescriptor<FPImageType>("InputDataSet");
  this->AddTypeToInputDescriptor<VectorType>("InputDataSet");
     std::cout<<"WriterModule::WriterModule()"<<std::endl;
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

  /*
  */
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
  FPImageType::Pointer  singleImage = this->GetInputData<FPImageType>("InputDataSet");
  VectorType::Pointer   vectorData  = this->GetInputData<VectorType>("InputDataSet");
  

  if ( vectorImage.IsNotNull() ) 
    {
      std::cout<<"On y va, on record VectorImage"<<std::endl;

      FPVWriterType::Pointer writer = FPVWriterType::New();
      writer->SetInput(vectorImage);
      writer->SetFileName("localWriter.tif");
      writer->Update();
      //std::cout<<"writer:"<<std::endl;
      //std::cout<<writer<<std::endl;
      
      //m_FPVWriter = writer;//FPVWriterType::New();
      m_FPVWriter->SetInput(vectorImage);
      m_FPVWriter->SetFileName(filepath);
      m_FPVWriter->Update();
      //std::cout<<" ******************************************************************* "<<std::endl;
      
      //std::cout<<"m_FPVWriter:"<<std::endl;
      //std::cout<<m_FPVWriter<<std::endl;

      /*
      this->GetFPVWriter()->SetInput(vectorImage);
      this->GetFPVWriter()->SetFileName(filepath);
      this->GetFPVWriter()->Update();
      */

    }
  else if( singleImage.IsNotNull() )
    {
      std::cout<<"On y va, on record SingleImage"<<std::endl;
      m_FPWriter->SetInput(singleImage);
      m_FPWriter->SetFileName(filepath);
      m_FPWriter->Update();
    }
  else if( vectorData.IsNotNull() )
    {
      std::cout<<"On y va, on record VectorData"<<std::endl;
      m_VectorWriter->SetInput(vectorData);
      m_VectorWriter->SetFileName(filepath);
      m_VectorWriter->Update();
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
