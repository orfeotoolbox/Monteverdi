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
#include <FL/Fl.H>

#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
WriterModule::WriterModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

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
  pBar->copy_label("0%");
}

void WriterModule::SaveDataSet()
{
  this->StartProcess2();
  this->StartProcess1();
}

void WriterModule::Browse()
{
  const char * filename = NULL;

  filename = flu_file_chooser("Choose the dataset file...", "*.*",".");
  
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


void WriterModule::UpdateProgress()
{
  double progress = m_ProcessObject->GetProgress();

  itk::OStringStream oss1, oss2;
  oss1.str("");
  oss1<<"Writing dataset  ("<<std::floor(100*progress)<<"%)";
  oss2.str("");
  oss2<<std::floor(100*progress);
  oss2<<"%";
  pBar->value( progress );
  wFileChooserWindow->copy_label(oss1.str().c_str());
  pBar->copy_label( oss2.str().c_str() );
}

void WriterModule::UpdateProgressCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if(writer.IsNotNull())
    {
    writer->UpdateProgress();
    }
}

void WriterModule::ThreadedWatch()
{
  // Deactivate window buttons
  Fl::lock();
  bBrowse->deactivate();
  bCancel->deactivate();
  bOk->deactivate();
  vFilePath->deactivate();
  Fl::unlock();

  double last = 0;
  double updateThres = 0.01;
  double current = 0;

  while( (m_ProcessObject.IsNull() || this->IsBusy()) )
    {
    if(m_ProcessObject.IsNotNull())
         {
        current = m_ProcessObject->GetProgress();
         if(current - last > updateThres)
           {
        // Make the main fltk loop update progress fields
          Fl::awake(&UpdateProgressCallback,this);
           last = current;
           }
         }
       // Sleep for a while
    Sleep(500);
    }

  // Update progress one last time
  Fl::awake(&UpdateProgressCallback,this);
  
  Fl::lock();
  // Reactivate window buttons
  bBrowse->activate();
  bCancel->activate();
  bOk->activate();
  vFilePath->activate();
  Fl::unlock();

  Fl::awake(&HideWindowCallback,this);
  }

void WriterModule::ThreadedRun()
{
  this->BusyOn();

  std::string filepath = vFilePath->value();
  
  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputDataSet");
  FloatingImageType::Pointer singleImage = this->GetInputData<FloatingImageType>("InputDataSet");
  VectorType::Pointer vectorData = this->GetInputData<VectorType>("InputDataSet");
  CharVectorImageType::Pointer charVectorImage = this->GetInputData<CharVectorImageType>("InputDataSet");
  LabeledVectorType::Pointer labeledVectorData = this->GetInputData<LabeledVectorType>("InputDataSet");

  try 
  {
    if ( charVectorImage.IsNotNull() )
      {
      CharVWriterType::Pointer charVWriter = CharVWriterType::New();
      charVWriter->SetInput(charVectorImage);
      charVWriter->SetFileName(filepath);
      m_ProcessObject = charVWriter;
      charVWriter->Update();
      }
    else if ( vectorImage.IsNotNull() )
      {
      FPVWriterType::Pointer fPVWriter = FPVWriterType::New();
      fPVWriter->SetInput(vectorImage);
      fPVWriter->SetFileName(filepath);
      m_ProcessObject = fPVWriter;
      fPVWriter->Update();
      }
    else if( singleImage.IsNotNull() )
      {
      FPWriterType::Pointer fPWriter = FPWriterType::New();
      fPWriter->SetInput(singleImage);
      fPWriter->SetFileName(filepath);
      m_ProcessObject = fPWriter;
      fPWriter->Update();
      }
    else if( vectorData.IsNotNull() )
      {
      VectorWriterType::Pointer vectorWriter = VectorWriterType::New();
      vectorWriter->SetInput(vectorData);
      vectorWriter->SetFileName(filepath);
      m_ProcessObject = vectorWriter;
      vectorWriter->Update();
      }
    else if( labeledVectorData.IsNotNull() )
      {
      LabeledVectorWriterType::Pointer labeledVectorWriter = LabeledVectorWriterType::New();
      labeledVectorWriter->SetInput(labeledVectorData);
      labeledVectorWriter->SetFileName(filepath);
      m_ProcessObject = labeledVectorWriter;
      labeledVectorWriter->Update();
      }
    else
      {
      this->BusyOff();
      itkExceptionMacro(<<"Input data are NULL.");
      }
  }
  catch (itk::ExceptionObject & err)
  {
    // Make the main fltk loop update Msg reporter
    m_ErrorMsg = err.GetDescription();
    Fl::awake(&SendErrorCallback,&m_ErrorMsg);
    this->BusyOff();    
  }
  this->BusyOff();
}

void WriterModule::HideWindow()
{
  wFileChooserWindow->hide();
}

void WriterModule::HideWindowCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if(writer.IsNotNull())
    {
    writer->HideWindow();
    }
}

void WriterModule::SendErrorCallback(void * data)
{
  std::string *  error = static_cast<std::string *>(data);
  //TODO test if error is null
  if ( error == NULL )
  {
    MsgReporter::GetInstance()->SendError("Unknown error during update");
  }
  else 
  {
    MsgReporter::GetInstance()->SendError(error->c_str());
  }
}
} // End namespace otb


