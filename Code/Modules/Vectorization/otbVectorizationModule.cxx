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
#include "otbVectorizationModule.h"
#include <FLU/Flu_File_Chooser.h>
#include "itksys/SystemTools.hxx"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
VectorizationModule::VectorizationModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // First, do constructor stuffs
  m_Controller = ControllerType::New();
  m_View = ViewType::New();
  m_Model = ModelType::New();

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_Controller->ButtonAutomaticCallbackOff();
  m_View->SetController(m_Controller);
  m_View->SetModel(m_Model);
  m_View->SetWidgetController(m_Controller->GetWidgetController());

  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module
  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage", otbGetTextMacro("Input image"));
  this->AddInputDescriptor<VectorDataType>("VectorData", otbGetTextMacro("Vector data"), true, true);
}

/** Destructor */
VectorizationModule::~VectorizationModule()
{}

/** PrintSelf method */
void VectorizationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void VectorizationModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();
  
  // Here is the body of the module.
  // When the Run() method is called, necessary inputs have been
  // passed to the module.
  
  if(this->GetNumberOfInputDataByKey("VectorData") > 0)
    {
    const char* defaultPath = "";

    if ( otb::ConfigurationFile::GetInstance()->IsValid() )
      {
      defaultPath = otb::ConfigurationFile::GetInstance()->GetDEMDirectory().c_str();
      }

    const char *cfname = flu_dir_chooser("Choose DEM directory if you want to...", defaultPath);
    Fl::check();
    if(cfname != NULL)
      {
      if( itksys::SystemTools::FileIsDirectory(cfname) )
        {
        m_Model->SetUseDEM(true);
        m_Model->SetDEMPath(cfname);
        }
      else
        {
        std::ostringstream oss;
        oss<<"Invalid DEm directory "<<cfname<<".";
        MsgReporter::GetInstance()->SendError(oss.str());
        }
      }
    }
  
  // Second step is to retrieve the input image
  // To handle an input with multiple supported type :
  FloatingVectorImageType::Pointer fpvImage = this->GetInputData<FloatingVectorImageType>("InputImage");
  
  // One of this pointer will be NULL:
  if (fpvImage.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
    m_View->BuildInterface();
    m_Model->SetImage(fpvImage);
    //m_View->InitColor();
    }
  else
    {
    itkExceptionMacro(<< "Input image is NULL.");
    }

  for( unsigned int i=0; i<this->GetNumberOfInputDataByKey("VectorData"); i++ )
    {
    VectorDataType::Pointer vdata = this->GetInputData<VectorDataType>("VectorData", i);
    if(vdata.IsNotNull())
      {
      // Load the vector data (still empty otherwise !!!)
      vdata->Update();
      m_Controller->AddVectorData(vdata);
      }
    else
      {
      itkExceptionMacro(<< "Input vector data is NULL.");
      }
    }
  
  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...
}

/** The Notify */
void VectorizationModule::Notify()
{
  if (m_Model->GetOutputChanged())
    {
      this->ClearOutputDescriptors();
      // Add outputs
      VectorDataType::Pointer vData = m_Model->GetOutput();
      this->AddOutputDescriptor(vData,"VectorData", otbGetTextMacro("New vector data"));

      this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
      this->BusyOff();
    }
  else if  (m_View->GetIsHide())
    {
      this->BusyOff();
    }

  // Once module is closed, it is no longer busy
 
}
} // End namespace otb
