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
  this->AddInputDescriptor<UCharImageType>(InputDataSetID, otbGetTextMacro("Dataset to write"));
  this->AddTypeToInputDescriptor<UShortImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<ShortImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<UIntImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<IntImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<FloatImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<DoubleImageType>(InputDataSetID);

  // Vector Image inputs
  this->AddTypeToInputDescriptor<UCharVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<UShortVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<ShortVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<UIntVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<IntVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<FloatVectorImageType>(InputDataSetID);
  this->AddTypeToInputDescriptor<DoubleVectorImageType>(InputDataSetID);

  // Vector inputs
  this->AddTypeToInputDescriptor<VectorType>(InputDataSetID);
  this->AddTypeToInputDescriptor<LabeledVectorType>(InputDataSetID);

  m_OutputTypesChoices[otb::UNSIGNEDCHAR]     = "unsigned char (8 bits)";
  m_OutputTypesChoices[otb::SHORTINT]         = "short (16 bits)";
  m_OutputTypesChoices[otb::INT]              = "int (32 bits)";
  m_OutputTypesChoices[otb::FLOAT]            = "float (32 bits)";
  m_OutputTypesChoices[otb::DOUBLE]           = "double (64 bits)";
  m_OutputTypesChoices[otb::UNSIGNEDSHORTINT] = "unsigned short (16 bits)";
  m_OutputTypesChoices[otb::UNSIGNEDINT]      = "unsigned int (32 bits)";

  m_WriteGeomFile = false;
}

/** Destructor */
WriterModule::~WriterModule()
{
}

/** PrintSelf method */
void WriterModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void WriterModule::Run()
{
  this->BuildGUI();

  cOutDataType->add(m_OutputTypesChoices[otb::UNSIGNEDCHAR].c_str());
  cOutDataType->add(m_OutputTypesChoices[otb::SHORTINT].c_str());
  cOutDataType->add(m_OutputTypesChoices[otb::INT].c_str());
  cOutDataType->add(m_OutputTypesChoices[otb::FLOAT].c_str());
  cOutDataType->add(m_OutputTypesChoices[otb::DOUBLE].c_str());
  cOutDataType->add(m_OutputTypesChoices[otb::UNSIGNEDSHORTINT].c_str());
  cOutDataType->add(m_OutputTypesChoices[otb::UNSIGNEDINT].c_str());

  if (this->GetInputData<UCharImageType>(InputDataSetID) != 0
      || this->GetInputData<UCharVectorImageType>(InputDataSetID) != 0)
    {
    cOutDataType->value(otb::UNSIGNEDCHAR);
    }
  else if (this->GetInputData<ShortImageType>(InputDataSetID) != 0
           || this->GetInputData<ShortVectorImageType>(InputDataSetID) != 0)
    {
    cOutDataType->value(otb::SHORTINT);
    }
  else if (this->GetInputData<IntImageType>(InputDataSetID) != 0
           || this->GetInputData<IntVectorImageType>(InputDataSetID) != 0)
    {
    cOutDataType->value(otb::INT);
    }
  else if (this->GetInputData<FloatImageType>(InputDataSetID) != 0
           || this->GetInputData<FloatVectorImageType>(InputDataSetID) != 0)
    {
    cOutDataType->value(otb::FLOAT);
    }
  else if (this->GetInputData<DoubleImageType>(InputDataSetID) != 0
           || this->GetInputData<DoubleVectorImageType>(InputDataSetID) != 0)
    {
    cOutDataType->value(otb::DOUBLE);
    }
  else if (this->GetInputData<UShortImageType>(InputDataSetID) != 0
           || this->GetInputData<UShortVectorImageType>(InputDataSetID) != 0)
    {
    cOutDataType->value(otb::UNSIGNEDSHORTINT);
    }
  else if (this->GetInputData<UIntImageType>(InputDataSetID) != 0
           || this->GetInputData<UIntVectorImageType>(InputDataSetID) != 0)
    {
    cOutDataType->value(otb::UNSIGNEDINT);
    }
  else
    {
    // Vector Data
    cOutDataType->hide();
    }

  bSaveMetadata->value(m_WriteGeomFile);

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

  filename = flu_file_chooser(otbGetTextMacro("Choose the dataset file..."), "*.*", "");

  if (filename == NULL)
    {
    otbMsgDebugMacro(<< "Empty file name!");
    return;
    }
  vFilePath->value(filename);

}

void WriterModule::Cancel()
{
  wFileChooserWindow->hide();
}

void WriterModule::CheckButtonSaveMetadata()
{
  m_WriteGeomFile = bSaveMetadata->value();
}


void WriterModule::UpdateProgress()
{
  double progress = m_ProcessObject->GetProgress();

  itk::OStringStream oss1, oss2;
  oss1.str("");
  oss1 << otbGetTextMacro("Writing dataset") << "  (" << std::floor(100 * progress) << "%)";
  oss2.str("");
  oss2 << std::floor(100 * progress);
  oss2 << "%";
  pBar->value(progress);
  wFileChooserWindow->copy_label(oss1.str().c_str());
  pBar->copy_label(oss2.str().c_str());
}

void WriterModule::UpdateProgressCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if (writer.IsNotNull())
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
  cOutDataType->deactivate();
  bSaveMetadata->deactivate();
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
  Fl::awake(&UpdateProgressCallback, this);

  Fl::lock();
  // Reactivate window buttons
  bBrowse->activate();
  bCancel->activate();
  bOk->activate();
  vFilePath->activate();
  cOutDataType->activate();
  bSaveMetadata->activate();
  Fl::unlock();

  Fl::awake(&HideWindowCallback, this);
}

void WriterModule::ThreadedRun()
{
  this->BusyOn();

  m_Filename = vFilePath->value();
  PixelType outFormat = static_cast<PixelType> (cOutDataType->value());

  try
    {
    // Mono band images
    if (this->GetInputData<UCharImageType> (InputDataSetID))
      {
      this->DoWriteSingleBand(this->GetInputData<UCharImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<UShortImageType> (InputDataSetID))
      {
      this->DoWriteSingleBand(this->GetInputData<UShortImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<UIntImageType> (InputDataSetID))
      {
      this->DoWriteSingleBand(this->GetInputData<UIntImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<FloatImageType> (InputDataSetID))
      {
      this->DoWriteSingleBand(this->GetInputData<FloatImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<DoubleImageType> (InputDataSetID))
      {
      this->DoWriteSingleBand(this->GetInputData<DoubleImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<ShortImageType> (InputDataSetID))
      {
      this->DoWriteSingleBand(this->GetInputData<ShortImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<IntImageType> (InputDataSetID))
      {
      this->DoWriteSingleBand(this->GetInputData<IntImageType> (InputDataSetID), outFormat);
      }
    // Multi band images
    else if (this->GetInputData<UCharVectorImageType> (InputDataSetID))
      {
      this->DoWriteMultiBand(this->GetInputData<UCharVectorImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<UShortVectorImageType> (InputDataSetID))
      {
      this->DoWriteMultiBand(this->GetInputData<UShortVectorImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<UIntVectorImageType> (InputDataSetID))
      {
      this->DoWriteMultiBand(this->GetInputData<UIntVectorImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<FloatVectorImageType> (InputDataSetID))
      {
      this->DoWriteMultiBand(this->GetInputData<FloatVectorImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<DoubleVectorImageType> (InputDataSetID))
      {
      this->DoWriteMultiBand(this->GetInputData<DoubleVectorImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<ShortVectorImageType> (InputDataSetID))
      {
      this->DoWriteMultiBand(this->GetInputData<ShortVectorImageType> (InputDataSetID), outFormat);
      }
    else if (this->GetInputData<IntVectorImageType> (InputDataSetID))
      {
      this->DoWriteMultiBand(this->GetInputData<IntVectorImageType> (InputDataSetID), outFormat);
      }
    // Vector Data
    else if (this->GetInputData<VectorType> (InputDataSetID))
      {
      VectorWriterType::Pointer writer = VectorWriterType::New();
      writer->SetInput(this->GetInputData<VectorType> (InputDataSetID));
      writer->SetFileName(m_Filename);
      m_ProcessObject = writer;
      writer->Update();
      }
    else if (this->GetInputData<LabeledVectorType> (InputDataSetID))
      {
      LabeledVectorWriterType::Pointer writer = LabeledVectorWriterType::New();
      writer->SetInput(this->GetInputData<LabeledVectorType> (InputDataSetID));
      writer->SetFileName(m_Filename);
      m_ProcessObject = writer;
      writer->Update();
      }
    }
  catch (itk::ExceptionObject& err)
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

  if (writer.IsNotNull())
    {
    writer->HideWindow();
    }
}

void WriterModule::SendErrorCallback(void * data)
{
  std::string * error = static_cast<std::string *>(data);
  //TODO test if error is null
  if (error == NULL)
    {
    MsgReporter::GetInstance()->SendError("Unknown error during update");
    }
  else
    {
    MsgReporter::GetInstance()->SendError(error->c_str());
    }
}
} // End namespace otb
