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
#include "otbOpticCalibrationModule.h"

namespace otb
{
/** Constructor */
OpticCalibrationModule::OpticCalibrationModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // First, do constructor stuffs
  m_Model = ModelType::New();
  m_Controller = ControllerType::New();
  m_View = ViewType::New();

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_View->SetOpticCalibrationController(m_Controller);
  m_View->SetOpticCalibrationModel(m_Model);

  m_Model->RegisterListener(this);

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage", otbGetTextMacro("Image to apply OpticCalibration on"));
}

/** Destructor */
OpticCalibrationModule::~OpticCalibrationModule()
{}

/** PrintSelf method */
void OpticCalibrationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void OpticCalibrationModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  // Here is the body of the module.
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs

  // To handle an input with multiple supported type :
  FloatingVectorImageType::Pointer fpvImage = this->GetInputData<FloatingVectorImageType>("InputImage");

  // One of this pointer will be NULL:
  if(fpvImage.IsNotNull())
    {
      m_View->BuildInterface();
      // Process the input as an FloatingVectorImageType
      m_Controller->SetInputImage( fpvImage );
    }
  else
    {
      itkExceptionMacro(<<"Input image is NULL.");
    }
  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...

}

/** The Notify */
void OpticCalibrationModule::Notify(const std::string & event)
{
  if (event == "SetInputImage")
    {
      m_View->Init();
    }
  else if (event == "OutputsUpdated")
   {
    this->ClearOutputDescriptors();

    // Add outputs
    FloatingVectorImageType::Pointer lumOutput = m_Model->GetLuminanceImage();
    this->AddOutputDescriptor(lumOutput,"Luminance image", otbGetTextMacro("Luminance image"));
    
    FloatingVectorImageType::Pointer reflOutput = m_Model->GetReflectanceImage();
    //this->AddOutputDescriptor(reflOutput,"TOA image", otbGetTextMacro("TOA Image"));
	
    FloatingVectorImageType::Pointer surfReflOutput = m_Model->GetSurfaceReflectanceImage();
    //this->AddOutputDescriptor(surfReflOutput,"TOC image", otbGetTextMacro("TOC Image"));
    
    FloatingVectorImageType::Pointer diffOutput = m_Model->GetDifferenceImage();
    //this->AddOutputDescriptor(diffOutput,"Difference TOA-TOC image", otbGetTextMacro("Difference TOA-TOC image"));
    
    if(m_Model->GetChangeReflectanceScale())
      {
	this->AddOutputDescriptor(reflOutput,"TOA image (*1000)", otbGetTextMacro("TOA image (*1000)"));
	this->AddOutputDescriptor(surfReflOutput,"TOC image (*1000)", otbGetTextMacro("TOC image (*1000)"));
	this->AddOutputDescriptor(diffOutput,"Difference TOA-TOC image (*1000)", otbGetTextMacro("Difference TOA-TOC image (*1000)"));
      }
    else
      {
	this->AddOutputDescriptor(reflOutput,"TOA image", otbGetTextMacro("TOA Image"));
	this->AddOutputDescriptor(surfReflOutput,"TOC image", otbGetTextMacro("TOC Image"));
	this->AddOutputDescriptor(diffOutput,"Difference TOA-TOC image", otbGetTextMacro("Difference TOA-TOC image"));
      }

    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));

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
} // End namespace otb

