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
#include "otbInputViewGUI.h"

#include <FL/fl_ask.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Text_Display.H>
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"
#include "otbMacro.h"
#include "itkExceptionObject.h"



namespace otb
{


void
InputViewGUI
::BuildInputInterface()
{

   if( m_Model.IsNotNull() && m_Controller.IsNotNull() )
   {
     wInputWindow->size_range(wInputWindow->w(), wInputWindow->h(), wInputWindow->w(), 0, 0, 0);
     gScrollInput->type(Fl_Scroll::VERTICAL_ALWAYS);

    // deactivate Ok button
    //bOk->deactivate();

    // to count the number of Fl_Input_Choice to display
    unsigned int cpt =0;
    // if there are several Fl_Input_Choice to draw
    unsigned int height = 60;
    unsigned int base = height/2;
    unsigned int i =0;
    InputDataDescriptorMapType lInputDataMap = m_Model->GetModuleInputsByInstanceId(m_ModuleInstanceId);
    InputDataDescriptorMapType::const_iterator it_in;

    // loop on the requiered input data of the module : m_ModuleInstanceId
    for (it_in = lInputDataMap.begin();it_in != lInputDataMap.end();it_in++)
    {
      /** Build the Fl_Choice **/
      Fl_Choice *inputChoice;
      // create Input Widgets considering the needed inputs   
      inputChoice = new Fl_Choice( 85,base + cpt* height, 400, 25, it_in->second.GetDataDescription().c_str() );
      inputChoice->box(FL_PLASTIC_DOWN_BOX);      
      inputChoice->align(FL_ALIGN_TOP);
      InputChoiceDescriptor inputChoiceDesc;
      inputChoiceDesc.m_FlChoice = inputChoice;

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
            inputChoice->add(it_out->second.GetDataDescription().c_str());
            inputChoiceDesc.m_ChoiceVector.push_back(StringPairType(moduleInstances[i],it_out->first));
          }
        }
      }
      m_InputChoiceMap[it_in->first] = inputChoiceDesc;
      gScrollInput->add(inputChoice);
   
      /** Build the Fl_Check_Button **/
      if(it_in->second.IsOptional())
      {
        this->BuildCheckBox(cpt,height,inputChoice);
      }
      /** Build the List **/
      if(it_in->second.IsMultiple())
      {
        this->BuildList(cpt,height);
        cpt+= 2;
      }
      cpt++;
    }
  }

}

void
InputViewGUI
::BuildCheckBox(int cpt,int height,Fl_Choice *inputChoice)
{
  std::cout << "---------------------------- case OPTIONAL "<<std::endl<<std::endl;
  Fl_Check_Button *checkButton = new Fl_Check_Button( 60,height/2+cpt* height, 25, 25);
  gScrollInput->add(checkButton);
  inputChoice->deactivate();
  checkButton->callback((Fl_Callback *)InputViewGUI::ActivateInputChoice,(void *)inputChoice);
}

void
InputViewGUI
::ActivateInputChoice(Fl_Widget * w, void * v)
{
  Fl_Choice* inputChoice = (Fl_Choice *)v;
  if(inputChoice->active())
    {
    inputChoice->deactivate();
    }
    else
    {
    inputChoice->activate();
    }
}

void 
InputViewGUI
::BuildList(int cpt,int height)
{
  std::cout << "---------------------------- case MULTIPLE "<<std::endl<<std::endl;

  Fl_Browser *browser = new Fl_Browser( 85,height+cpt* height, 400, 110);
  browser->box(FL_PLASTIC_DOWN_BOX);
  gScrollInput->add(browser);
  // tempppppp
  int * inputChoiceDesc;

  Fl_Button *plusButton = new Fl_Button( 490+15, height/2+cpt* height+2, 20, 20, "+");
  plusButton->box(FL_PLASTIC_ROUND_DOWN_BOX);
  plusButton->color((Fl_Color)55);
  plusButton->labelfont(1);
  plusButton->labelsize(17);
  plusButton->labelcolor((Fl_Color)186);
  gScrollInput->add(plusButton);
  plusButton->callback((Fl_Callback *)InputViewGUI::AddInputToList,(void *)inputChoiceDesc);

  Fl_Button *minusButton = new Fl_Button( 490+15, height+cpt* height + 37, 20, 20, "-");
  minusButton->box(FL_PLASTIC_ROUND_DOWN_BOX);
  minusButton->color((Fl_Color)55);
  minusButton->labelfont(1);
  minusButton->labelsize(17);
  minusButton->labelcolor((Fl_Color)186);
  gScrollInput->add(minusButton);
  minusButton->callback((Fl_Callback *)InputViewGUI::RemoveInputFromList,(void *)inputChoiceDesc);

  Fl_Button *clearButton = new Fl_Button( 490, height+cpt* height+85, 50, 25, "Clear");
  gScrollInput->add(clearButton);
  clearButton->box(FL_PLASTIC_DOWN_BOX);
  clearButton->color((Fl_Color)55);
  clearButton->labelfont(1);
  clearButton->labelsize(12);
  clearButton->labelcolor((Fl_Color)186);
  clearButton->callback((Fl_Callback *)InputViewGUI::ClearList,(void *)inputChoiceDesc);
}


// **        Callbacks         ** //

void 
InputViewGUI
::Ok()
{
  std::cout<< "Ok" <<std::endl;
  // Connect 
  for(InputChoiceDescriptorMapType::const_iterator mIt = m_InputChoiceMap.begin();
  mIt!=m_InputChoiceMap.end();++mIt)
  {
  if(mIt->second.HasSelected())
    {
    StringPairType spair = mIt->second.GetSelected();
    m_Controller->AddModuleConnection(spair.first,spair.second,m_ModuleInstanceId,mIt->first);
    }
  }
  // Start()
  std::cout<< "Start" <<std::endl;
  m_Controller->StartModuleByInstanceId(m_ModuleInstanceId);
  wInputWindow->hide();


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


void 
InputViewGUI
::AddInputToList(Fl_Widget * w, void * v)
{
  std::cout<<"ADDDDD"<<std::endl;
  InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;

  int choiceVal = inputChoiceDesc->GetFlChoice()->value();
  if(choiceVal != 0)
    {
      inputChoiceDesc->GetFlBrowser()->add(choiceVal);
      inputChoiceDesc->GetFlChoice()->redraw();
      inputChoiceDesc->GetFlBrowser()->redraw();
    }
}

void 
InputViewGUI
::RemoveInputFromList(Fl_Widget * w, void * v)
{
  std::cout<<"REMOVEEEEEE"<<std::endl;
  InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;
  
  int choiceVal = inputChoiceDesc->GetFlChoice()->value();
  if(inputChoiceDesc->GetFlChoice()->size() >= choiceVal)
    {
      inputChoiceDesc->GetFlBrowser()->remove(choiceVal);
      inputChoiceDesc->GetFlBrowser()->redraw();
    }
}

void 
InputViewGUI
::ClearList(Fl_Widget * w, void * v)
{
  std::cout<<"CLEAR"<<std::endl;
  InputChoiceDescriptor* inputChoiceDesc = (InputChoiceDescriptor *)v;
  inputChoiceDesc->GetFlBrowser()->clear();
  inputChoiceDesc->GetFlBrowser()->redraw();
}

} // end namespace otb
