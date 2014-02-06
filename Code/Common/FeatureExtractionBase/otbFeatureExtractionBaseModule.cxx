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
#ifndef __otbFeatureExtractionBaseModule_cxx
#define __otbFeatureExtractionBaseModule_cxx

#include "otbFeatureExtractionBaseModule.h"

namespace otb
{
/** Constructor */
FeatureExtractionBaseModule::FeatureExtractionBaseModule()
{
}

/** Destructor */
FeatureExtractionBaseModule::~FeatureExtractionBaseModule()
{}

/** PrintSelf method */
void FeatureExtractionBaseModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void FeatureExtractionBaseModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  InputImageType::Pointer input = this->GetInputData<InputImageType>("InputImage");
  input->UpdateOutputInformation();
  if (input.IsNotNull())
    {
    m_Model->SetInputImage(input);
    m_View->SetFeatureExtractPreviewParentBrowser(-1);
    m_View->Show();
    m_Model->GenerateLayers();
    }
  else
    {
    itkExceptionMacro(<< "Input image is NULL.");
    }
}

/** The Notify */
void FeatureExtractionBaseModule::Notify(const std::string& event)
{
  if (event == "OutputsUpdated")
    {
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Model->GetOutputImage(), "OutputImage", "Feature image extraction");
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
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
    this->NotifyAll(MonteverdiEvent("Cancel", m_InstanceId));
    }
  else
    {

    }

}

} // End namespace otb

#endif
