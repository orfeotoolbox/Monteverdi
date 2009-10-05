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
  m_CharVWriter = CharVWriterType::New();
  m_FPWriter = FPWriterType::New();
  m_VectorWriter = VectorWriterType::New();
  m_LabeledVectorWriter = LabeledVectorWriterType::New();
  
   // Describe inputs
  this->AddInputDescriptor<FloatingVectorImageType>("InputDataSet","Dataset to write.");
  this->AddTypeToInputDescriptor<FloatingImageType>("InputDataSet");
  this->AddTypeToInputDescriptor<CharVectorImageType>("InputDataSet");
  this->AddTypeToInputDescriptor<VectorType>("InputDataSet");
  this->AddTypeToInputDescriptor<LabeledVectorType>("InputDataSet");
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
  pBar->minimum(0);
  pBar->maximum(1);
  wFileChooserWindow->show();
}

void WriterModule::SaveDataSet()
{
  this->StartProcess1();
  this->StartProcess2();
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


void WriterModule::UpdateProgressBar( float progress )
{
  itk::OStringStream oss1, oss2;
  oss1.str("");
  oss1<<"Writing dataset  ("<<std::floor(100*progress)<<"%)";
  oss2.str("");
  oss2<<std::floor(100*progress);
  oss2<<"%";
  Fl::lock();
  pBar->value( progress );
  wFileChooserWindow->copy_label(oss1.str().c_str());
  pBar->copy_label( oss2.str().c_str() );
  Fl::awake();
  Fl::unlock();
}


void WriterModule::ThreadedWatch()
{
  std::cout<<"RunProcess1: Deactivate interface"<<std::endl;
  // Deactivate window buttons
  Fl::lock();
  bBrowse->deactivate();
  bCancel->deactivate();
  bOk->deactivate();
  vFilePath->deactivate();
  Fl::unlock();

  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputDataSet");
  CharVectorImageType::Pointer charVectorImage = this->GetInputData<CharVectorImageType>("InputDataSet");
  FloatingImageType::Pointer singleImage = this->GetInputData<FloatingImageType>("InputDataSet");
  VectorType::Pointer vectorData = this->GetInputData<VectorType>("InputDataSet");
  LabeledVectorType::Pointer labeledVectorData = this->GetInputData<LabeledVectorType>("InputDataSet");

  itk::ProcessObject::Pointer myObject;
				   
  if ( charVectorImage.IsNotNull() ) 
    {
    myObject = m_CharVWriter;
    }
  else if ( vectorImage.IsNotNull() ) 
    {
    myObject = m_FPVWriter;
    }
  else if( singleImage.IsNotNull() )
    {
    myObject = m_FPWriter;
    }
  else if( vectorData.IsNotNull() )
    {
    myObject = m_VectorWriter;
    }
  else if( labeledVectorData.IsNotNull() )
    {
    myObject = m_LabeledVectorWriter;
    }
  else
    {
    itkExceptionMacro(<<"Input data are NULL.");
    }
	
  float progress = 0;
  float progressOld = -1;

  std::cout<<"RunProcess1: Starting the watch loop."<<std::endl;


  while( progress != 1)
    {
      sleep(0.5);
      progress = myObject->GetProgress();
      float diffProg = progress - progressOld;

       if(diffProg > 0.01)
 	{
	  std::cout<<"RunProcess1: Updating progress ..."<<std::endl;
	  this->UpdateProgressBar( progress );
 	  progressOld = progress;
 	}
    }

  Fl::lock();
  this->UpdateProgressBar( 1. );
  this->UpdateProgressBar( 0. );
   
  // Activate window buttons
  bBrowse->activate();
  bCancel->activate();
  bOk->activate();
  vFilePath->activate();

  // Changing back label
  wFileChooserWindow->copy_label("Save dataset ...");

  // Close the window
  wFileChooserWindow->hide();
  Fl::unlock();
  
  std::cout<<"RunProcess1: End"<<std::endl;
}




void WriterModule::ThreadedRun()
{
  std::string filepath = vFilePath->value();
  
  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputDataSet");
  FloatingImageType::Pointer singleImage = this->GetInputData<FloatingImageType>("InputDataSet");
  VectorType::Pointer vectorData = this->GetInputData<VectorType>("InputDataSet");
  CharVectorImageType::Pointer charVectorImage = this->GetInputData<CharVectorImageType>("InputDataSet");
  LabeledVectorType::Pointer labeledVectorData = this->GetInputData<LabeledVectorType>("InputDataSet");

  if ( charVectorImage.IsNotNull() ) 
    {
      m_CharVWriter->SetInput(charVectorImage);
      m_CharVWriter->SetFileName(filepath);
      m_CharVWriter->Update();
    }
  else if ( vectorImage.IsNotNull() ) 
    {
    std::cout<<"RunProcess2: Writing floating point vector image"<<std::endl;
      m_FPVWriter->SetInput(vectorImage);
      m_FPVWriter->SetFileName(filepath);
      m_FPVWriter->Update();
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
  else if( labeledVectorData.IsNotNull() )
    {
      m_LabeledVectorWriter->SetInput(labeledVectorData);
      m_LabeledVectorWriter->SetFileName(filepath);
      m_LabeledVectorWriter->Update();
    }
  else
    {
      itkExceptionMacro(<<"Input data are NULL.");
    } 
}


} // End namespace otb


