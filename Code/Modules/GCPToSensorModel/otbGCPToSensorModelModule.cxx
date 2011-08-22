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
#include "otbGCPToSensorModelModule.h"

namespace otb
{
/** Constructor */
GCPToSensorModelModule::GCPToSensorModelModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // First, do constructor stuffs

  m_Controller = ControllerType::New();
  m_View = ViewType::New();
  m_Model = ModelType::New();

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);

  m_View->SetController(m_Controller);
  //m_View->SetModel(m_Model);
  m_View->SetWidgetController(m_Controller->GetWidgetController());
  m_View->SetMapWidgetController(m_Controller->GetMapWidgetController());

  m_Model->RegisterListener(this);

  // Instanciation of the Image To VectorImage Filter
  m_CastFilter = CastSingleImageFilterType::New();

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage", otbGetTextMacro("Input image"));
  this->AddTypeToInputDescriptor<FloatingSingleImageType>("InputImage");
}

/** Destructor */
GCPToSensorModelModule::~GCPToSensorModelModule()
{}

/** PrintSelf method */
void GCPToSensorModelModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void GCPToSensorModelModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  // Here is the body of the module.
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs

  // To handle an input with multiple supported type :
  FloatingVectorImageType::Pointer fpvImage = this->GetInputData<FloatingVectorImageType>("InputImage");

  // Try to get a single Image
  // If the input image is an otb::Image instead of VectorImage then cast it
  // in Vector Image and continue the processing
  FloatingSingleImageType::Pointer fSingleImage = this->GetInputData<FloatingSingleImageType>("InputImage");

  //Test single floating image
  if (fSingleImage.IsNotNull() && fpvImage.IsNull())
    {
    //cast Image in VectorImage
    m_CastFilter->SetInput(fSingleImage);
    fpvImage = m_CastFilter->GetOutput();
    }

  // One of this pointer will be NULL:
  if (fpvImage.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
    m_View->BuildInterface();
    m_Model->SetImage(fpvImage);
    
    if (CurlHelperInterface::IsCurlAvailable())
      {
      m_Model->SetMap(m_View->gMFull->w(), m_View->gMFull->h());
      }
    else
      {
      m_View->wMapWindow->hide();
      }
    }
  else
    {
    itkExceptionMacro(<< "Input image is NULL.");
    }

  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...
}

/** The Notify */
void GCPToSensorModelModule::Notify()
{
  if (m_Model->GetOutputChanged())
    {
      this->ClearOutputDescriptors();
      // Add outputs
      FloatingVectorImageType::Pointer filteredOutput = m_Model->GetOutput();
      this->AddOutputDescriptor(filteredOutput, "OutputImage", otbGetTextMacro("Input image with new keyword list"));
      this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
      // Once module is closed, it is no longer busy
      this->BusyOff();
    }
  // Quit button pushed => nothing to be done
  else if( m_Model->GetQuitWithoutOutput() )
    {
      this->ClearOutputDescriptors();
      // Once module is closed, it is no longer busy
      this->BusyOff();
    }
}
} // End namespace otb
