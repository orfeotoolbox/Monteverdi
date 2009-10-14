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
#include "otbMeanShiftModule.h"

namespace otb
{
/** Constructor */
MeanShiftModule::MeanShiftModule()
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
  m_View->SetModel(m_Model);
  m_View->SetWidgetsController(m_Controller->GetWidgetsController());

  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage","Image to apply MeanShift on");
  
}

/** Destructor */
MeanShiftModule::~MeanShiftModule()
{}

/** PrintSelf method */
void MeanShiftModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void MeanShiftModule::Run()
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
    // Process the input as an FloatingVectorImageType
      m_Model->SetInputImage( fpvImage );
      m_View->Build();
    }
  else
    {
      itkExceptionMacro(<<"Input image is NULL.");
    }
  
  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...

}

/** The Notify */
void MeanShiftModule::Notify()
{

  if (m_Model->GetOutputChanged())
    {
      this->ClearOutputDescriptors();
      
      // Add outputs


      if(m_Controller->GenerateFiltered())
       {
         FloatingVectorImageType::Pointer filteredOutput =
           m_Controller->GetFilteredOutput();
         this->AddOutputDescriptor(filteredOutput,"Filtered Image", "Result of the MeanShift filtering");
       }

      if(m_Controller->GenerateClustered())
       {
         FloatingVectorImageType::Pointer clusteredOutput =
           m_Controller->GetClusteredOutput();
         this->AddOutputDescriptor(clusteredOutput,"Clustered Image", "Result of the MeanShift clustering");
       }

      //   if(m_Controller->GenerateLabeled())
      //     {
      //       LabelImageType::Pointer labeledOutput = m_Controller->GetLabeledOutput();
      //       this->AddOutputDescriptor(labeledOutput,"Labeled Image", "Result of the MeanShift labeling");
      //     }
  

      // Send an event to Monteverdi application
      this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));

      // Once module is closed, it is no longer busy
      this->BusyOff();

    }
}
} // End namespace otb

