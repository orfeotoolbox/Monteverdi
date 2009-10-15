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
#ifndef __otbWriterMVCModule_cxx
#define __otbWriterMVCModule_cxx

#include "otbWriterMVCModule.h"


namespace otb
{
/** Constructor */
WriterMVCModule::WriterMVCModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  //Build MVC
  m_Model      = WriterModel::New();
  m_View       = WriterViewGUI::New();
  m_Controller = WriterController::New();
  
  m_View->SetWriterModel(m_Model);
  m_View->InitVisu();
  m_View->SetWriterController(m_Controller);
  
  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
   
  // Describe inputs
  //this->AddInputDescriptor<FloatingVectorImageType>("InputDataSet","Dataset to write.");
  this->AddInputDescriptor<FloatingVectorImageType>("InputDataSet","Dataset to write.");
//   this->AddTypeToInputDescriptor<VectorType>("InputDataSet");
  
  m_Model->RegisterListener(this);
}

/** Destructor */
WriterMVCModule::~WriterMVCModule()
{}

/** PrintSelf method */
void WriterMVCModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void WriterMVCModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputDataSet");
//   VectorType::Pointer vectorData = this->GetInputData<VectorType>("InputDataSet");
  
  if(vectorImage.IsNotNull())
  {
    m_Model->SetInputImage(vectorImage);
    m_View->Show();
    m_Model->GenerateLayers();
  }
//   else if (vectorData.IsNotNull())
//   {
//     //TODO write vectordata
//     m_Model->GetVectorWriter()->SetInput(vectorData);
//   }
  else
  {
    itkExceptionMacro(<<"Input image is NULL.");
  }
  
//   pBar->minimum(0);
//   pBar->maximum(1);
}

void WriterMVCModule::ThreadedWatch()
{
//   // Deactivate window buttons
//   Fl::lock();
//   bBrowse->deactivate();
//   bCancel->deactivate();
//   bOk->deactivate();
//   vFilePath->deactivate();
//   Fl::unlock();
// 
//   double last = 0;
//   double updateThres = 0.01;
//   double current = -1;
// 
//   while( (m_ProcessObject.IsNull() && this->IsBusy()) || m_ProcessObject->GetProgress() != 1)
//   {
//     if(m_ProcessObject.IsNotNull())
//     {
//       current = m_ProcessObject->GetProgress();
//       if(current - last > updateThres)
//       {
//         // Make the main fltk loop update progress fields
//         Fl::awake(&UpdateProgressCallback,this);
//         last = current;
//       }
//     }
//        // Sleep for a while
//     Sleep(500);
//   }
//   
//   Fl::lock();
//   // Reactivate window buttons
//   bBrowse->activate();
//   bCancel->activate();
//   bOk->activate();
//   vFilePath->activate();
//   Fl::awake(&HideWindowCallback,this);
//   Fl::unlock();
}

void WriterMVCModule::ThreadedRun()
{
//   this->BusyOn();
// 
//   std::string filepath = vFilePath->value();
//   
//   FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputDataSet");
//   FloatingImageType::Pointer singleImage = this->GetInputData<FloatingImageType>("InputDataSet");
//   VectorType::Pointer vectorData = this->GetInputData<VectorType>("InputDataSet");
//   CharVectorImageType::Pointer charVectorImage = this->GetInputData<CharVectorImageType>("InputDataSet");
//   LabeledVectorType::Pointer labeledVectorData = this->GetInputData<LabeledVectorType>("InputDataSet");
// 
//   if ( charVectorImage.IsNotNull() )
//   {
//     CharVWriterType::Pointer charVWriter = CharVWriterType::New();
//     charVWriter->SetInput(charVectorImage);
//     charVWriter->SetFileName(filepath);
//     m_ProcessObject = charVWriter;
//     charVWriter->Update();
//   }
//   else if ( vectorImage.IsNotNull() )
//   {
//     FPVWriterType::Pointer fPVWriter = FPVWriterType::New();
//     fPVWriter->SetInput(vectorImage);
//     fPVWriter->SetFileName(filepath);
//     m_ProcessObject = fPVWriter;
//     fPVWriter->Update();
//   }
//   else if( singleImage.IsNotNull() )
//   {
//     FPWriterType::Pointer fPWriter = FPWriterType::New();
//     fPWriter->SetInput(singleImage);
//     fPWriter->SetFileName(filepath);
//     m_ProcessObject = fPWriter;
//     fPWriter->Update();
//   }
//   else if( vectorData.IsNotNull() )
//   {
//     VectorWriterType::Pointer vectorWriter = VectorWriterType::New();
//     vectorWriter->SetInput(vectorData);
//     vectorWriter->SetFileName(filepath);
//     m_ProcessObject = vectorWriter;
//     vectorWriter->Update();
//   }
//   else if( labeledVectorData.IsNotNull() )
//   {
//     LabeledVectorWriterType::Pointer labeledVectorWriter = LabeledVectorWriterType::New();
//     labeledVectorWriter->SetInput(labeledVectorData);
//     labeledVectorWriter->SetFileName(filepath);
//     m_ProcessObject = labeledVectorWriter;
//     labeledVectorWriter->Update();
//   }
//   else
//   {
//     this->BusyOff();
//     itkExceptionMacro(<<"Input data are NULL.");
//   }
//   this->BusyOff();
}



/** The Notify */
void WriterMVCModule::Notify(const std::string & event)
{
  if (event == "OutputsUpdated")
  {
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("Dataset written",m_InstanceId));

    // Once module is closed, it is no longer busy
    this->BusyOff();
  }
  else if (event == "BusyOff")
  {
    this->BusyOff();
  }
  else
  {
    
  }
}

void WriterMVCModule::HideWindowCallback(void * data)
{
//   Self::Pointer writer = static_cast<Self *>(data);
// 
//   if(writer.IsNotNull())
//   {
//     writer->HideWindow();
//   }
}

void WriterMVCModule::UpdateProgressCallback(void * data)
{
//   Self::Pointer writer = static_cast<Self *>(data);
// 
//   if(writer.IsNotNull())
//   {
//     writer->UpdateProgress();
//   }
}

} // End namespace otb

#endif
