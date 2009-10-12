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
::InputViewGUI() : m_Model(), m_Controller(), m_ModuleInstanceId(""), m_InputChoiceMap()
{
  std::cout<<"This: "<<this<<std::endl;
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

    // to count the number of Fl_Input_Choice to display
    unsigned int cpt =0;
    // if there are several Fl_Input_Choice to draw
    unsigned int vIndex = 0;
    unsigned int i = 0;

    // Retrieve the module inputs
    InputDataDescriptorMapType lInputDataMap = m_Model->GetModuleInputsByInstanceId(m_ModuleInstanceId);
    InputDataDescriptorMapType::const_iterator it_in;

    // loop on the requiered input data of the module : m_ModuleInstanceId
    for (it_in = lInputDataMap.begin();it_in != lInputDataMap.end();it_in++)
      {
      InputChoiceDescriptor::Pointer inputChoice = InputChoiceDescriptor::New();
      inputChoice->SetInputDataDescriptor(it_in->second);

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
      for(i=0;i<moduleInstances.size();i++)
      {
        // look after all the outputs of each instance of module
        OutputDataDescriptorMapType lOutputDataMap = m_Model->GetModuleOutputsByInstanceId(moduleInstances[i]);
        OutputDataDescriptorMapType::const_iterator it_out;
        for(it_out=lOutputDataMap.begin();it_out!=lOutputDataMap.end();it_out++)
        {
          // if the type is ok, we can add the label in the Fl_Input_Choice
          if(it_in->second.IsTypeCompatible(it_out->second.GetDataType()))
          {
	  inputChoice->AddChoice(StringPairType(moduleInstances[i],it_out->first));
          }
        }
      }

      // Change the module label
      gLabel->value(m_ModuleInstanceId.c_str());
      }
   }
}

// void
// InputViewGUI
// ::ActivateInputChoice(Fl_Widget * w, void * v)
// {
//   InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;
//   if(inputChoiceDesc->m_FlChoice->active())
//     {
//     inputChoiceDesc->m_FlChoice->deactivate();
//     if(inputChoiceDesc->IsMultiple())
//       inputChoiceDesc->m_FlBrowser->deactivate();
//     }
//     else
//     {
//     inputChoiceDesc->m_FlChoice->activate();
//     if(inputChoiceDesc->IsMultiple())
//       inputChoiceDesc->m_FlBrowser->activate();
//     }
// }


// **        Callbacks         ** //

void
InputViewGUI
::Ok()
{
//   unsigned int i;

//   if(m_ModuleInstanceId != gLabel->value())
//     {
//       int res = m_Controller->ChangeInstanceId(m_ModuleInstanceId, gLabel->value());
//       if(res != 0)
//        return;
//       m_ModuleInstanceId = gLabel->value();
//     }

//   // Connect modules
//   for(InputChoiceDescriptorMapType::const_iterator mIt = m_InputChoiceMap.begin(); mIt!=m_InputChoiceMap.end();++mIt)
//   {
//     // Multiple data
//     if(mIt->second->IsMultiple())
//     {
//       if( !mIt->second->IsOptional() ||
//           (mIt->second->IsOptional() && mIt->second->m_FlChoice->active() ) )
//       {
//         for(i=0;i<mIt->second->m_Indexes.size();i++)
//         {
//           int ind = mIt->second->m_Indexes[i];
//           if(ind >= 0)
//           {
//             StringPairType spair = mIt->second->m_ChoiceMap[ind];
//             m_Controller->AddModuleConnection(spair.first,spair.second,m_ModuleInstanceId,mIt->first);
//           }
//         }
//       }
//     }
//     else // Single data
//     {
//       // mandatory OR optional & active
//       if( !mIt->second->IsOptional() ||
//           (mIt->second->IsOptional() && mIt->second->m_FlChoice->active() ) )
//       {
//         if(mIt->second->HasSelected())
//         {
//           StringPairType spair = mIt->second->GetSelected();
//           m_Controller->AddModuleConnection(spair.first,spair.second,m_ModuleInstanceId,mIt->first);
//         }
//       }
//     }
//   }

//   // Start()
//   m_Controller->StartModuleByInstanceId(m_ModuleInstanceId);
//   wInputWindow->hide();

}

void
InputViewGUI
::Cancel()
{
  wInputWindow->hide();
}

void
InputViewGUI
::Show()
{
  wInputWindow->show();
}

// void
// InputViewGUI
// ::AddInputToList(Fl_Widget * w, void * v)
// {
//   InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;
//   int choiceVal = inputChoiceDesc->m_FlChoice->value();
//   if(choiceVal >= 0)
//     {
//       inputChoiceDesc->m_FlBrowser->add(inputChoiceDesc->m_FlChoice->text(choiceVal));
//       inputChoiceDesc->m_FlChoice->redraw();
//       inputChoiceDesc->m_FlBrowser->redraw();
//     }

//     inputChoiceDesc->m_Indexes.push_back(choiceVal);
// }

// void
// InputViewGUI
// ::RemoveInputFromList(Fl_Widget * w, void * v)
// {
//   InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;

//   int choiceVal = inputChoiceDesc->m_FlBrowser->value();
//   inputChoiceDesc->m_FlBrowser->remove(choiceVal);

//   if( choiceVal <= inputChoiceDesc->m_FlBrowser->size() )
//     {
//       inputChoiceDesc->m_FlBrowser->value(choiceVal);
//     }
//   else
//     {
//       inputChoiceDesc->m_FlBrowser->value(choiceVal-1);
//     }

//   inputChoiceDesc->m_FlBrowser->redraw();

//   // Erase the target index
//   inputChoiceDesc->m_Indexes.erase(inputChoiceDesc->m_Indexes.begin()+choiceVal-1);

// }

// void
// InputViewGUI
// ::ClearList(Fl_Widget * w, void * v)
// {
//   InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;
//   inputChoiceDesc->m_FlBrowser->clear();
//   inputChoiceDesc->m_FlBrowser->redraw();

//   // Cheat the indexe is set to -1
//   inputChoiceDesc->m_Indexes.clear();
// }


// void 
// InputViewGUI
// ::InputChoiceChanged(Fl_Widget *w, void *v)
// {
//   // Retrieve input choice descriptor
//   InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;

//   // Retrieve the input view gui instance
//   InputViewGUI * pthis = dynamic_cast<InputViewGUI*>(static_cast<InputViewGroup *>(w->parent()->parent()->user_data()));

//   std::cout<<"Pthis: "<<pthis<<std::endl;

//   if(inputChoiceDesc->m_FlChoice->value() >= 0)
//     {
//     std::string id = inputChoiceDesc->m_ChoiceMap[inputChoiceDesc->m_FlChoice->value()].first;
//     std::string key = inputChoiceDesc->m_ChoiceMap[inputChoiceDesc->m_FlChoice->value()].second;

//     if(pthis->GetModel()->SupportsCaching(id,key))
//       {

//       if(pthis->GetModel()->IsCached(id,key))
// 	{
// 	inputChoiceDesc->m_StatusBox->copy_label("cached");
// 	inputChoiceDesc->m_StatusBox->color(FL_GREEN);
// 	}
//       else
// 	{
// 	inputChoiceDesc->m_StatusBox->copy_label("streamed");
// 	inputChoiceDesc->m_StatusBox->color(FL_RED);
// 	}

//       inputChoiceDesc->m_StatusBox->show();
//       }
//     else
//       {
//       inputChoiceDesc->m_StatusBox->hide();
//       }
//     }

//   // Activate Ok button if needed
//   InputChoiceDescriptorMapType lInputChoiceMap = pthis->GetInputChoiceMap();
//   InputChoiceDescriptorMapType::const_iterator mcIt;

//   bool allInputsSet = true;
//   // Check all the FlChoices
//   for( mcIt=lInputChoiceMap.begin();mcIt!=lInputChoiceMap.end();mcIt++)
//   {
//     // If the input choice is not set
//     if ((mcIt->second->m_FlChoice->value()<0) 
//         // and if the input choice is active
//         &&(mcIt->second->m_FlChoice->active()))
//     {
//       allInputsSet = false;
//     }

//     // TODO Multiple case (and multiple + optional)
//   }
//   if(allInputsSet)
//     pthis->bOk->activate();

// }

} // end namespace otb

#endif
