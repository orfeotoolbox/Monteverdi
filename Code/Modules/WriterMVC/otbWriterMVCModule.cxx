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
  this->AddInputDescriptor<FloatingVectorImageType>("InputDataSet","Dataset to write.");

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

  if(vectorImage.IsNotNull())
  {
    m_Model->SetInputImage(vectorImage);
    m_View->Show();
    m_Model->GenerateLayers();
  }
  else
  {
    itkExceptionMacro(<<"Input image is NULL.");
  }
}

void WriterMVCModule::ThreadedWatch()
{
  // Deactivate window buttons
  m_View->ManageActivationWindowButtons(false);

  double last = 0;
  double updateThres = 0.01;
  double current = -1;

  while( (m_ProcessObject.IsNull() || this->IsBusy()) )
  {
    if(m_ProcessObject.IsNotNull())
    {
      current = m_ProcessObject->GetProgress();
      if(current - last > updateThres)
      {
        // Make the main fltk loop update progress fields
        m_View->AwakeProgressFields(m_ProcessObject->GetProgress());
        last = current;
      }
    }
    // Sleep for a while
    Sleep(500);
    m_ProcessObject = m_Model->GetProcessObjectModel();
  }
  
  // Reactivate window buttons
//   std::cout << "activation" <<std::endl;
  m_View->ManageActivationWindowButtons(true);
}

void WriterMVCModule::ThreadedRun()
{
  this->BusyOn();
  
  m_ProcessObject = m_Model->GetProcessObjectModel();
  m_Model->GenerateOutputImage();

  this->BusyOff();
}


/** The Notify */
void WriterMVCModule::Notify(const std::string & event)
{
  if (event == "OutputsUpdated")
  {
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
    // Once module is closed, it is no longer busy
    this->BusyOff();
  }
  else if ( ( event == "BusyOff" ) ||  ( event == "Quit" ) )
  {
//     std::cout << "quit module" << std::endl;
    this->BusyOff();
  }
  else if (event == "SaveDataSet")
  {
    this->StartProcess2();
    this->StartProcess1();
  }
  else
  {
  }
}

} // End namespace otb

#endif
