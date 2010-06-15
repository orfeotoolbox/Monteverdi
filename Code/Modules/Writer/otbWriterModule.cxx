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

static const char* InputDataSetID = "InputDataSet";

/** Constructor */
WriterModule::WriterModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Describe inputs
  // Image inputs
  this->AddInputDescriptor<UCharImageType>(InputDataSetID,otbGetTextMacro("Dataset to write"));
  this->AddTypeToInputDescriptor<UShortImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<UIntImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<FloatImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<DoubleImageType>(InputDataSetID);

  // Vector Image inputs
  this->AddTypeToInputDescriptor<UCharVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<UShortVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<UIntVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<FloatVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<DoubleVectorImageType>(InputDataSetID);

  // Vector inputs
  this->AddTypeToInputDescriptor<VectorType>(InputDataSetID);
  this->AddTypeToInputDescriptor<LabeledVectorType>(InputDataSetID);

  m_OutputTypesChoices[UCHAR]  = "unsigned char";
  m_OutputTypesChoices[USHORT] = "unsigned short";
  m_OutputTypesChoices[UINT]   = "unsigned int";
  m_OutputTypesChoices[FLOAT]  = "float";
  m_OutputTypesChoices[DOUBLE] = "double";
}

/** Destructor */
WriterModule::~WriterModule()
{
}

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

  // TODO hide this widget until we know what we do with it
  bAutoScale->hide();

  cOutDataType->add(m_OutputTypesChoices[UCHAR].c_str());
  cOutDataType->add(m_OutputTypesChoices[USHORT].c_str());
  cOutDataType->add(m_OutputTypesChoices[UINT].c_str());
  cOutDataType->add(m_OutputTypesChoices[FLOAT].c_str());
  cOutDataType->add(m_OutputTypesChoices[DOUBLE].c_str());

  if ( this->GetInputData<UCharImageType>(InputDataSetID) != 0
       || this->GetInputData<UCharVectorImageType>(InputDataSetID) != 0 )
    {
    cOutDataType->value(UCHAR);
    }
  else if ( this->GetInputData<UShortImageType>(InputDataSetID) != 0
      || this->GetInputData<UShortVectorImageType>(InputDataSetID) != 0 )
    {
    cOutDataType->value(USHORT);
    }
  else if ( this->GetInputData<UShortImageType>(InputDataSetID) != 0
      || this->GetInputData<UShortVectorImageType>(InputDataSetID) != 0 )
    {
    cOutDataType->value(UINT);
    }
  else if ( this->GetInputData<FloatImageType>(InputDataSetID) != 0
      || this->GetInputData<FloatVectorImageType>(InputDataSetID) != 0 )
    {
    cOutDataType->value(FLOAT);
    }
  else if ( this->GetInputData<DoubleImageType>(InputDataSetID) != 0
      || this->GetInputData<DoubleVectorImageType>(InputDataSetID) != 0 )
    {
    cOutDataType->value(DOUBLE);
    }
  else
    {
    // Vector Data
    cOutDataType->hide();
    bAutoScale->hide();
    }

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

  filename = flu_file_chooser(otbGetTextMacro("Choose the dataset file..."), "*.*","");

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
  oss1 << otbGetTextMacro("Writing dataset") << "  ("<<std::floor(100*progress)<<"%)";
  oss2.str("");
  oss2 << std::floor(100*progress);
  oss2 << "%";
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

  while ((m_ProcessObject.IsNull() || this->IsBusy()))
    {
    if (m_ProcessObject.IsNotNull())
      {
      current = m_ProcessObject->GetProgress();
      if (current - last > updateThres)
        {
        // Make the main fltk loop update progress fields
        Fl::awake(&UpdateProgressCallback, this);
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

  m_Filename = vFilePath->value();
  m_AutoScale = (bAutoScale->value() == 1);
  OutputFormat outFormat = static_cast<OutputFormat> (cOutDataType->value());

  try
    {
    if (this->GetInputData<UCharImageType> (InputDataSetID))
      {
        UCharImageType::Pointer image = this->GetInputData<UCharImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<UCharImageType, UCharImageType> (image);
          break;
        case USHORT:
          this->DoWrite<UCharImageType, UShortImageType> (image);
          break;
        case UINT:
          this->DoWrite<UCharImageType, UIntImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<UCharImageType, FloatImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<UCharImageType, DoubleImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<UShortImageType> (InputDataSetID))
      {
        UShortImageType::Pointer image = this->GetInputData<UShortImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<UShortImageType, UCharImageType> (image);
          break;
        case USHORT:
          this->DoWrite<UShortImageType, UShortImageType> (image);
          break;
        case UINT:
          this->DoWrite<UShortImageType, UIntImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<UShortImageType, FloatImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<UShortImageType, DoubleImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<UIntImageType> (InputDataSetID))
      {
        UIntImageType::Pointer image = this->GetInputData<UIntImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<UIntImageType, UCharImageType> (image);
          break;
        case USHORT:
          this->DoWrite<UIntImageType, UShortImageType> (image);
          break;
        case UINT:
          this->DoWrite<UIntImageType, UIntImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<UIntImageType, FloatImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<UIntImageType, DoubleImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<FloatImageType> (InputDataSetID))
      {
        FloatImageType::Pointer image = this->GetInputData<FloatImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<FloatImageType, UCharImageType> (image);
          break;
        case USHORT:
          this->DoWrite<FloatImageType, UShortImageType> (image);
          break;
        case UINT:
          this->DoWrite<FloatImageType, UIntImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<FloatImageType, FloatImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<FloatImageType, DoubleImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<DoubleImageType> (InputDataSetID))
      {
        DoubleImageType::Pointer image = this->GetInputData<DoubleImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<DoubleImageType, UCharImageType> (image);
          break;
        case USHORT:
          this->DoWrite<DoubleImageType, UShortImageType> (image);
          break;
        case UINT:
          this->DoWrite<DoubleImageType, UIntImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<DoubleImageType, FloatImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<DoubleImageType, DoubleImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<UCharVectorImageType> (InputDataSetID))
      {
        UCharVectorImageType::Pointer image = this->GetInputData<UCharVectorImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<UCharVectorImageType, UCharVectorImageType> (image);
          break;
        case USHORT:
          this->DoWrite<UCharVectorImageType, UShortVectorImageType> (image);
          break;
        case UINT:
          this->DoWrite<UCharVectorImageType, UIntVectorImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<UCharVectorImageType, FloatVectorImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<UCharVectorImageType, DoubleVectorImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<UShortVectorImageType> (InputDataSetID))
      {
        UShortVectorImageType::Pointer image = this->GetInputData<UShortVectorImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<UShortVectorImageType, UCharVectorImageType> (image);
          break;
        case USHORT:
          this->DoWrite<UShortVectorImageType, UShortVectorImageType> (image);
          break;
        case UINT:
          this->DoWrite<UShortVectorImageType, UIntVectorImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<UShortVectorImageType, FloatVectorImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<UShortVectorImageType, DoubleVectorImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<UIntVectorImageType> (InputDataSetID))
      {
        UIntVectorImageType::Pointer image = this->GetInputData<UIntVectorImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<UIntVectorImageType, UCharVectorImageType> (image);
          break;
        case USHORT:
          this->DoWrite<UIntVectorImageType, UShortVectorImageType> (image);
          break;
        case UINT:
          this->DoWrite<UIntVectorImageType, UIntVectorImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<UIntVectorImageType, FloatVectorImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<UIntVectorImageType, DoubleVectorImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<FloatVectorImageType> (InputDataSetID))
      {
        FloatVectorImageType::Pointer image = this->GetInputData<FloatVectorImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<FloatVectorImageType, UCharVectorImageType> (image);
          break;
        case USHORT:
          this->DoWrite<FloatVectorImageType, UShortVectorImageType> (image);
          break;
        case UINT:
          this->DoWrite<FloatVectorImageType, UIntVectorImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<FloatVectorImageType, FloatVectorImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<FloatVectorImageType, DoubleVectorImageType> (image);
          break;
        default:
          break;
        }
      }
    else if (this->GetInputData<DoubleVectorImageType> (InputDataSetID))
      {
        DoubleVectorImageType::Pointer image = this->GetInputData<DoubleVectorImageType> (InputDataSetID);
        switch (outFormat) {
        case UCHAR:
          this->DoWrite<DoubleVectorImageType, UCharVectorImageType> (image);
          break;
        case USHORT:
          this->DoWrite<DoubleVectorImageType, UShortVectorImageType> (image);
          break;
        case UINT:
          this->DoWrite<DoubleVectorImageType, UIntVectorImageType> (image);
          break;
        case FLOAT:
          this->DoWrite<DoubleVectorImageType, FloatVectorImageType> (image);
          break;
        case DOUBLE:
          this->DoWrite<DoubleVectorImageType, DoubleVectorImageType> (image);
          break;
        default:
          break;
        }
      }
    else if( this->GetInputData<VectorType> (InputDataSetID) )
      {
      VectorWriterType::Pointer writer = VectorWriterType::New();
      writer->SetInput(this->GetInputData<VectorType> (InputDataSetID));
      writer->SetFileName(m_Filename);
      m_ProcessObject = writer;
      writer->Update();
      }
    else if( this->GetInputData<LabeledVectorType> (InputDataSetID) )
      {
      LabeledVectorWriterType::Pointer writer = LabeledVectorWriterType::New();
      writer->SetInput( this->GetInputData<LabeledVectorType> (InputDataSetID));
      writer->SetFileName(m_Filename);
      m_ProcessObject = writer;
      writer->Update();
      }
    }
  catch (itk::ExceptionObject & err)
    {
      m_ErrorMsg = err.GetDescription();
      Fl::awake(&SendErrorCallback, &m_ErrorMsg);
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


