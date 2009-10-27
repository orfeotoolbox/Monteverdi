/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#include "otbInputViewComponent.h"

#include <FL/fl_draw.H>

namespace otb
{

InputViewComponent
::InputViewComponent() : Fl_Group(0,0,0,0)
{
  // Widgets
  m_FlChoice     = NULL;
  m_StatusBox    = NULL;
  m_FlBrowser    = NULL;
  m_CheckButton  = NULL;
  m_AddButton    = NULL;
  m_RemoveButton = NULL;
  m_ClearButton  = NULL;

  // Sizes
  m_UpperMargin           = 20;
  m_LeftMargin            = 60;
  m_SimpleWidgetsHeight   = 25;
  m_MultipleWidgetsHeight = 100;
  m_CentralWidgetsLength  = 400;
  m_WidgetsMargin         = 10;

  // Is caching ?
  m_CachingInProgress = false;
}

InputViewComponent
::~InputViewComponent()
{
  this->clear();
}

/** Set the input data descriptor */
void InputViewComponent
::SetInputDataDescriptor(const InputDataDescriptor & desc)
{
  m_InputDataDescriptor = desc;
}


InputViewComponent::StringPairVectorType
InputViewComponent
::GetSelected() const
{
  StringPairVectorType resp;

  if(!m_InputDataDescriptor.IsMultiple())
    {
    StringPairMapType::const_iterator mcIt = m_ChoiceMap.find(m_FlChoice->value());

    if(mcIt != m_ChoiceMap.end())
      {
      resp.push_back(mcIt->second);
      }
    }
  else
    {
    for(std::vector<int>::const_iterator lIt = m_Indices.begin();
       lIt != m_Indices.end();++lIt)
      {
      StringPairMapType::const_iterator mcIt = m_ChoiceMap.find(*lIt);
      
      if(mcIt != m_ChoiceMap.end())
       {
       resp.push_back(mcIt->second);
       }
      }
    }

  return resp;
}

bool
InputViewComponent
::HasSelected() const
{
  return (m_InputDataDescriptor.IsMultiple() && !m_Indices.empty()) || (m_FlChoice->value()>=0);
}

/** Add Choice */
void InputViewComponent::AddChoice(const StringPairType & choice, bool locked, const std::string & locker)
{
  if(m_FlChoice == NULL)
    {
    itkExceptionMacro("Input choice gui is not built !");
    }

  itk::OStringStream oss;
  oss<<choice.first<<"/"<<choice.second;

  // Handle the locked data case
  if(locked)
    {
    oss<<" (locked by module "<<locker<<")";
    }
  
  // Add the choice in the gui
  int index = m_FlChoice->add(oss.str().c_str());

  if(locked)
    {
    // This const_cast is necessary to be able to deactivate one menu_item
    Fl_Menu_Item * items = const_cast<Fl_Menu_Item *>(m_FlChoice->menu());
    items[index].deactivate();
    }

  // Keep the choice in the choice map
  m_ChoiceMap[index]=choice;
}

void InputViewComponent::Rebuild()
{
  // Clear all previous widgets
  this->clear();
  m_FlChoice = NULL;
  m_StatusBox = NULL;
  m_FlBrowser = NULL;
  
  if(!m_InputDataDescriptor.IsMultiple())
    {
    this->size(this->w(),m_SimpleWidgetsHeight+2*m_UpperMargin);
    }
  else
    {
    this->size(this->w(),2*m_UpperMargin+m_SimpleWidgetsHeight+2*m_WidgetsMargin+m_MultipleWidgetsHeight);
    }

  // First, build the choice menu
  m_FlChoice = new Fl_Choice( m_LeftMargin,this->y()+m_UpperMargin, m_CentralWidgetsLength, m_SimpleWidgetsHeight);
  m_FlChoice->copy_label(m_InputDataDescriptor.GetDataDescription().c_str());
  m_FlChoice->box(FL_PLASTIC_DOWN_BOX);
  m_FlChoice->align(FL_ALIGN_TOP);
  this->add(m_FlChoice);
  m_FlChoice->callback((Fl_Callback *)InputViewComponent::InputChanged,(void *)this);
    
 // Build the statuBox
  m_StatusBox = new Fl_Button(m_LeftMargin+m_CentralWidgetsLength + m_WidgetsMargin,this->y()+m_UpperMargin, 60,m_SimpleWidgetsHeight);
  m_StatusBox->box(FL_PLASTIC_DOWN_BOX);
  m_StatusBox->align(FL_ALIGN_INSIDE);
  m_StatusBox->labelsize(8);
  m_StatusBox->callback((Fl_Callback *)InputViewComponent::StartCaching, (void *)this);
  this->add(m_StatusBox);

  // Hide it for now
  m_StatusBox->hide();

  // Build the progress bar
  m_ProgressBar = new Fl_Progress(m_LeftMargin,this->y()+m_UpperMargin+m_SimpleWidgetsHeight, m_CentralWidgetsLength,15);
  m_ProgressBar->copy_label("0 %");
  m_ProgressBar->minimum(0);
  m_ProgressBar->maximum(1);
  m_ProgressBar->box(FL_PLASTIC_DOWN_BOX);
  m_ProgressBar->align(FL_ALIGN_INSIDE);
  m_ProgressBar->value(0);
  m_ProgressBar->labelsize(8);
  this->add(m_ProgressBar);

  // Hide it for now
  m_ProgressBar->hide();

  // Eventually build the optional environment
  if(m_InputDataDescriptor.IsOptional())
    {
    m_CheckButton = new Fl_Check_Button( m_LeftMargin-m_WidgetsMargin-25,this->y()+m_UpperMargin, 25, 25);
    this->add(m_CheckButton);
    m_FlChoice->deactivate();
    m_CheckButton->callback((Fl_Callback *)InputViewComponent::Switch,(void *)this);
    }

  // Eventually build the multiple environment
  if(m_InputDataDescriptor.IsMultiple())
    {
    // Build the browser
    m_FlBrowser = new Fl_Browser(m_LeftMargin,this->y()+m_UpperMargin+m_SimpleWidgetsHeight+2*m_WidgetsMargin, m_CentralWidgetsLength,m_MultipleWidgetsHeight);
    m_FlBrowser->box(FL_PLASTIC_DOWN_BOX);
    m_FlBrowser->type(2);
    m_FlBrowser->selection_color(m_FlChoice->selection_color());
    this->add(m_FlBrowser);

    // Build the add button
    m_AddButton = new Fl_Button(m_LeftMargin + m_CentralWidgetsLength + m_WidgetsMargin + 10, this->y() + m_SimpleWidgetsHeight + 1* m_MultipleWidgetsHeight / 3, 20,20, "+");
    m_AddButton->box(FL_PLASTIC_ROUND_DOWN_BOX);
    m_AddButton->color((Fl_Color)55);
    m_AddButton->labelfont(1);
    m_AddButton->labelsize(17);
    m_AddButton->labelcolor((Fl_Color)186);
    this->add(m_AddButton);
    m_AddButton->callback((Fl_Callback *)InputViewComponent::AddInput,(void *)this);
    
    // Build the remove button
    m_RemoveButton = new Fl_Button(m_LeftMargin + m_CentralWidgetsLength + m_WidgetsMargin + 10 ,this->y() + m_SimpleWidgetsHeight + 2 * m_MultipleWidgetsHeight / 3 , 20,20, "-");
    m_RemoveButton->box(FL_PLASTIC_ROUND_DOWN_BOX);
    m_RemoveButton->color((Fl_Color)55);
    m_RemoveButton->labelfont(1);
    m_RemoveButton->labelsize(17);
    m_RemoveButton->labelcolor((Fl_Color)186);
    this->add(m_RemoveButton);
    m_RemoveButton->callback((Fl_Callback *)InputViewComponent::RemoveInput,(void *)this);
    
    // Build the clear button
    m_ClearButton = new Fl_Button(m_LeftMargin + m_CentralWidgetsLength + m_WidgetsMargin ,this->y() + m_SimpleWidgetsHeight + 3 * m_MultipleWidgetsHeight / 3 ,40,m_SimpleWidgetsHeight, "Clear");
    this->add(m_ClearButton);
    m_ClearButton->box(FL_PLASTIC_DOWN_BOX);
    m_ClearButton->color((Fl_Color)55);
    m_ClearButton->labelfont(1);
    m_ClearButton->labelsize(12);
    m_ClearButton->labelcolor((Fl_Color)186);
    m_ClearButton->callback((Fl_Callback *)InputViewComponent::ClearInputs,(void *)this);

    if(m_InputDataDescriptor.IsOptional())
      {
      m_FlBrowser->deactivate();
      m_AddButton->deactivate();
      m_RemoveButton->deactivate();
      m_ClearButton->deactivate();
      }
    }
}

void InputViewComponent::Switch(Fl_Widget *w, void * v)
{
  // Retrieve the pointer
  Self * pthis = static_cast<Self *>(v);

   if(pthis->m_CheckButton->value())
     {
     pthis->Activate();
     }
   else
     {
     pthis->Deactivate();
     }
}

void InputViewComponent::AddInput(Fl_Widget * w, void * v)
{
  Self * pthis = static_cast<Self *>(v);
  
  int choiceVal = pthis->m_FlChoice->value();
  if(choiceVal >= 0)
    {
    pthis->m_FlBrowser->add(pthis->m_FlChoice->text(choiceVal));
    pthis->m_FlChoice->redraw();
    pthis->m_FlBrowser->redraw();
    pthis->m_Indices.push_back(choiceVal);
    }
}

void InputViewComponent::RemoveInput(Fl_Widget * w, void * v)
{
   Self * pthis = static_cast<Self *>(v);

   int choiceVal = pthis->m_FlBrowser->value();

   if( choiceVal <= pthis->m_FlBrowser->size() )
     {
     pthis->m_FlBrowser->value(choiceVal);
     pthis->m_FlBrowser->remove(choiceVal);
     pthis->m_Indices.erase(pthis->m_Indices.begin()+choiceVal-1);
     }
   else
     {
       pthis->m_FlBrowser->value(choiceVal-1);
     }

   pthis->m_FlBrowser->redraw();
}

void InputViewComponent::ClearInputs(Fl_Widget * w, void * v)
{
   Self * pthis = static_cast<Self *>(v);
   pthis->m_FlBrowser->clear();
   pthis->m_FlBrowser->redraw();

   // Cheat the indexe is set to -1
   pthis->m_Indices.clear();
}

void InputViewComponent::InputChanged(Fl_Widget * w, void * v)
{
   // Retrieve input choice descriptor
   Self * pthis  = static_cast<Self *>(v);

   if(pthis->m_FlChoice->value() >= 0)
     {
     std::string id = pthis->m_ChoiceMap[pthis->m_FlChoice->value()].first;
     std::string key = pthis->m_ChoiceMap[pthis->m_FlChoice->value()].second;

     if(pthis->m_Model->SupportsCaching(id,key))
       {

       if(pthis->m_Model->IsCached(id,key))
        {
        pthis->m_StatusBox->copy_label("cached");
        pthis->m_StatusBox->color(FL_GREEN);
       pthis->m_StatusBox->deactivate();
        }
       else
        {
        pthis->m_StatusBox->copy_label("streamed");
        pthis->m_StatusBox->color(FL_RED);
        pthis->m_StatusBox->activate();
        }
       pthis->m_StatusBox->show();
       }
     else
       {
       pthis->m_StatusBox->hide();
       }
     }
}

void InputViewComponent::StartCaching(Fl_Widget * w, void * v)
{
  // Retrieve input choice descriptor
  Self * pthis  = static_cast<Self *>(v);

  if(pthis->m_FlChoice->value() >= 0)
    {
    std::string id = pthis->m_ChoiceMap[pthis->m_FlChoice->value()].first;
    std::string key = pthis->m_ChoiceMap[pthis->m_FlChoice->value()].second;

    pthis->m_StatusBox->copy_label("caching ...");
    pthis->m_StatusBox->color(fl_rgb_color(255,128,0));
    pthis->m_StatusBox->deactivate();
    pthis->m_ProgressBar->selection_color(fl_rgb_color(255,128,0));
    pthis->m_ProgressBar->show();
    
    pthis->m_Controller->StartCaching(id,key);
    
    pthis->Deactivate();

    if(pthis->m_CheckButton != NULL)
      {
      pthis->m_CheckButton->deactivate();
      }

    pthis->m_CachingInProgress = true;
    }
}

void InputViewComponent::UpdateCachingProgress()
{
  if(m_CachingInProgress && m_FlChoice->value() >= 0)
    {
    std::string id =  m_ChoiceMap[m_FlChoice->value()].first;
    std::string key = m_ChoiceMap[m_FlChoice->value()].second;
    
    double progress = m_Model->GetCachingProgress(id,key);
    
    // Check if we need to update progress
    if(progress >= (m_ProgressBar->value()+0.01) )
      {
      itk::OStringStream oss;
      oss.str("");
      oss<<std::floor(100*progress);
      oss<<" %";
      
      Fl::lock();
      m_ProgressBar->value( progress );
      m_ProgressBar->copy_label( oss.str().c_str());
      Fl::awake();
      Fl::unlock();

      if(progress == 1.)
       {
       Fl::lock();
       m_CachingInProgress = false;
       m_ProgressBar->hide();
       m_StatusBox->copy_label("cached");
        m_StatusBox->color(FL_GREEN);
       m_StatusBox->deactivate();
       if(m_CheckButton != NULL)
         {
         m_CheckButton->deactivate();
         }
       this->Activate();
       Fl::unlock();
       }
      }
    }
}

void InputViewComponent::Activate()
{
  if(m_FlChoice != NULL)
    {
    m_FlChoice->activate();
    }
  if(m_FlBrowser != NULL)
    {
    m_FlBrowser->activate();
    }
  if(m_AddButton != NULL)
    {
    m_AddButton->activate();
    }
  if(m_RemoveButton != NULL)
    {
    m_RemoveButton->activate();
    }
  if(m_ClearButton != NULL)
    {
    m_ClearButton->activate();
    }
}

void InputViewComponent::Deactivate()
{
  if(m_FlChoice != NULL)
    {
    m_FlChoice->deactivate();
    }
  if(m_FlBrowser != NULL)
    {
    m_FlBrowser->deactivate();
    }
  if(m_AddButton != NULL)
    {
    m_AddButton->deactivate();
    }
  if(m_RemoveButton != NULL)
    {
    m_RemoveButton->deactivate();
    }
  if(m_ClearButton != NULL)
    {
    m_ClearButton->deactivate();
    }
}

bool InputViewComponent::IsReady()
{
  bool resp = true;

  if(m_CachingInProgress)
    {
    resp = false;
    }
  else
    {
    if( (m_InputDataDescriptor.IsOptional() && m_CheckButton->value() == 1))
      {
      if(m_CheckButton->value() ==1)
       {
       if(m_InputDataDescriptor.IsMultiple())
         {
         resp = (m_FlBrowser->size() > 0);
         }
       else
         {
         resp = (m_FlChoice->value()>=0);
         }
       }
      }
    else if(!m_InputDataDescriptor.IsOptional())
      {
      if(m_InputDataDescriptor.IsMultiple())
       {
       resp =  (m_FlBrowser->size() > 0);
       }
      else
       {
       resp = (m_FlChoice->value()>=0);
       }
      }
    }
  return resp;
}
} // end namespace otb

