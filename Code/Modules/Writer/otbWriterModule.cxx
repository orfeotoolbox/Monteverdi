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
  this->StartProcess2();
  this->StartProcess1();
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
  // Deactivate window buttons
  Fl::lock();
  bBrowse->deactivate();
  bCancel->deactivate();
  bOk->deactivate();
  vFilePath->deactivate();
  Fl::unlock();

  float progress = 0;
  float progressOld = -1;

  while( progress != 1)
    {
      sleep(0.5);
      if(m_ProcessObject.IsNotNull())
	{
	progress = m_ProcessObject->GetProgress();
	}

      float diffProg = progress - progressOld;

       if(diffProg > 0.01)
 	{
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
      itkExceptionMacro(<<"Input data are NULL.");
    } 
}


} // End namespace otb


