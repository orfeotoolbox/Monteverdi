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
#ifndef __otbFeatureExtractionModule_cxx
#define __otbFeatureExtractionModule_cxx

#include "otbFeatureExtractionModule.h"

namespace otb
{
/** Constructor */
FeatureExtractionModule::FeatureExtractionModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Build mvc
  m_Model      = FeatureExtractionModel::New();
  m_View       = FeatureExtractionViewGUI::New();
  m_Controller = FeatureExtractionController::New();

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);

  //m_View->SetFeatureExtractionModel(m_Model);

  m_View->SetFeatureExtractionController(m_Controller);
  m_View->InitVisu();


  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage", otbGetTextMacro("Image to apply feature extraction"));

  // the FeatureExtractionModel registers its module
  m_Model->RegisterListener(this);
}

/** Destructor */
FeatureExtractionModule::~FeatureExtractionModule()
{}

/** PrintSelf method */
void FeatureExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void FeatureExtractionModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  InputImageType::Pointer input = this->GetInputData<InputImageType>("InputImage");
  input->UpdateOutputInformation();
  if(input.IsNotNull())
    {
std::cout<<"hophophop "<<std::endl;
    m_Model->SetInputImage(input); 
std::cout<<"SetInputImage done "<<std::endl;
    m_View->SetFeatureExtractPreviewParentBrowser(-1);
std::cout<<"browser machin "<<std::endl;
    m_View->Show();
std::cout<<"shwo "<<std::endl;
    m_Model->GenerateLayers();
std::cout<<"sGenerateLayers "<<std::endl;
    }
  else
    {
    itkExceptionMacro(<<"Input image is NULL.");
    }
}

/** The Notify */
void FeatureExtractionModule::Notify(const std::string & event)
{
  std::cout<<"Module notification "<<event<<std::endl;
  if (event == "OutputsUpdated")
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Model->GetOutputImage(),"OutputImage", otbGetTextMacro("Feature image extraction"));
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
    // Once module is closed, it is no longer busy
    //this->BusyOff();
  }
  else if (event == "BusyOff")
  {
    this->BusyOff();
  }
  else if (event == "Cancel")
  {
    this->BusyOff();
    this->NotifyAll(MonteverdiEvent("Cancel",m_InstanceId));
  }
  else
  {

  }

}

} // End namespace otb

#endif
