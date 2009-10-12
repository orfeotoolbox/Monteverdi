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
#ifndef __otbInputViewGUI_cxx
#define __otbInputViewGUI_cxx

#include "otbInputViewGUI.h"

#include <FL/fl_ask.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Browser.H>
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"
#include "otbMacro.h"
#include "itkExceptionObject.h"



namespace otb
{

InputViewGUI
::InputViewGUI() : m_Model(), m_Controller(), m_ModuleInstanceId(""), m_InputChoiceMap(), m_Alive(true)
{}

InputViewGUI
::~InputViewGUI()
{
  m_Alive = false;
  wInputWindow->hide();
}

void
InputViewGUI
::BuildInputInterface()
{
  // Check if model and controller are available
  if( m_Model.IsNotNull() && m_Controller != NULL)
   { 
   // Size range
   wInputWindow->size_range(wInputWindow->w(), wInputWindow->h(), wInputWindow->w(), 0, 0, 0);
   gScrollInput->type(Fl_Scroll::VERTICAL_ALWAYS);
     
    // deactivate Ok button
    bOk->deactivate();

    // if there are several Fl_Input_Choice to draw
    unsigned int vIndex = 0;

    // Retrieve the module inputs
    InputDataDescriptorMapType lInputDataMap = m_Model->GetModuleInputsByInstanceId(m_ModuleInstanceId);
    InputDataDescriptorMapType::const_iterator it_in;

    // loop on the requiered input data of the module : m_ModuleInstanceId
    for (it_in = lInputDataMap.begin();it_in != lInputDataMap.end();it_in++)
      {
      InputViewComponent::Pointer inputChoice = InputViewComponent::New();
      inputChoice->SetInputDataDescriptor(it_in->second);
      inputChoice->SetModel(m_Model);
      inputChoice->SetController(m_Controller);

      // Add it to the scroll group
      std::cout<<"vindex= "<<vIndex<<std::endl;
      inputChoice->resize(0,vIndex,gScrollInput->w(),inputChoice->h());
      
      // Build the input Choice
      inputChoice->Rebuild();

      gScrollInput->add(inputChoice);

      // Update vertical index
      vIndex = inputChoice->y()+inputChoice->h();


      // Add it to the map
      m_InputChoiceMap[it_in->first]=inputChoice;
      
      // we check if there are convenient outputs in the modules
      std::vector<std::string> moduleInstances = m_Model->GetAvailableModuleInstanceIds();
      for(std::vector<std::string>::const_iterator instIt = moduleInstances.begin();instIt != moduleInstances.end();instIt++)
      {
        // look after all the outputs of each instance of module
        OutputDataDescriptorMapType lOutputDataMap = m_Model->GetModuleOutputsByInstanceId(*instIt);
        OutputDataDescriptorMapType::const_iterator it_out;
        for(it_out=lOutputDataMap.begin();it_out!=lOutputDataMap.end();it_out++)
        {
          // if the type is ok, we can add the label in the Fl_Input_Choice
          if(it_in->second.IsTypeCompatible(it_out->second.GetDataType()))
          {
	  inputChoice->AddChoice(StringPairType(*instIt,it_out->first));
          }
        }
      }

      // Change the module label
      gLabel->value(m_ModuleInstanceId.c_str());

      // Finally, start the watching process
      this->StartProcess1();
      }
   }
}

void InputViewGUI
::RunProcess1(void *)
{
  while(m_Alive)
    {
    bool globalOk = true;

    for(InputViewComponentMapType::const_iterator mIt = m_InputChoiceMap.begin(); mIt!=m_InputChoiceMap.end();++mIt)
      {
      // Update caching progress
      mIt->second->UpdateCachingProgress();
      globalOk =  globalOk && mIt->second->IsReady();
      }

    // Check if Ok button can be activated
    if(globalOk)
      {
      Fl::lock();
      bOk->activate();
      Fl::unlock();
      }
    else
      {
      Fl::lock();
      bOk->deactivate();
      Fl::unlock();
      }

    // Sleep for a while
    Sleep(500);
    }
}


void
InputViewGUI
::Ok()
{
  if(m_ModuleInstanceId != gLabel->value())
    {
      int res = m_Controller->ChangeInstanceId(m_ModuleInstanceId, gLabel->value());
      if(res != 0)
       return;
      m_ModuleInstanceId = gLabel->value();
    }

  // Connect modules
  for(InputViewComponentMapType::const_iterator mIt = m_InputChoiceMap.begin(); mIt!=m_InputChoiceMap.end();++mIt)
  {
  if(mIt->second->HasSelected())
    {
    InputViewComponent::StringPairVectorType inputs = mIt->second->GetSelected();

    for (InputViewComponent::StringPairVectorType::const_iterator it = inputs.begin();
	 it!= inputs.end();++it)
      {
       m_Controller->AddModuleConnection(it->first,it->second,m_ModuleInstanceId,mIt->first);
      }
    }
  }
  // Start()
  m_Controller->StartModuleByInstanceId(m_ModuleInstanceId);
  wInputWindow->hide();
  m_Alive = false;
}

void
InputViewGUI
::Cancel()
{
  m_Alive = false;
  wInputWindow->hide();
}

void
InputViewGUI
::Show()
{
  wInputWindow->show();
}

} // end namespace otb

#endif
