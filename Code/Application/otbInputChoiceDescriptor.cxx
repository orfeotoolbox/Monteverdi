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
#include "otbInputChoiceDescriptor.h"





namespace otb
{

InputChoiceDescriptor
::InputChoiceDescriptor() : Fl_Group(0,0,0,0)
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
  m_LeftMargin            = 85;
  m_SimpleWidgetsHeight   = 30;
  m_MultipleWidgetsHeight = 100;
  m_CentralWidgetsLength  = 400;
  m_WidgetsMargin         = 10;
}

InputChoiceDescriptor
::~InputChoiceDescriptor()
{
  this->clear();
}

/** Set the input data descriptor */
void InputChoiceDescriptor
::SetInputDataDescriptor(const InputDataDescriptor & desc)
{
  m_InputDataDescriptor = desc;
}


InputChoiceDescriptor::StringPairType
InputChoiceDescriptor
::GetSelected() const
{
  StringPairMapType::const_iterator mcIt = m_ChoiceMap.find(m_FlChoice->value()); 
  return mcIt->second; 
}

bool
InputChoiceDescriptor
::HasSelected() const
{
  return m_FlChoice->value()>=0;
}

/** Add Choice */
void InputChoiceDescriptor::AddChoice(const StringPairType & choice)
{
  if(m_FlChoice == NULL)
    {
    itkExceptionMacro("Input choice gui is not built !");
    }

  itk::OStringStream oss;
  oss<<choice.first<<"/"<<choice.second;
  
  // Add the choice in the gui
  int index = m_FlChoice->add(oss.str().c_str());

  // Keep the choice in the choice map
  m_ChoiceMap[index]=choice;
}

void InputChoiceDescriptor::Rebuild()
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
    this->size(this->w(),2*m_UpperMargin+m_SimpleWidgetsHeight+m_WidgetsMargin+m_MultipleWidgetsHeight);
    }

  // First, build the choice menu
  m_FlChoice = new Fl_Choice( m_LeftMargin,this->y()+m_UpperMargin, m_CentralWidgetsLength, m_SimpleWidgetsHeight);
  m_FlChoice->copy_label(m_InputDataDescriptor.GetDataDescription().c_str());
  m_FlChoice->box(FL_PLASTIC_DOWN_BOX);
  m_FlChoice->align(FL_ALIGN_TOP);
  this->add(m_FlChoice);

  // TODO: callbacks
  //m_FlChoice->callback((Fl_Callback *)InputViewGUI::InputChoiceChanged,(void *)m_FlChoiceDesc);
    
 // Build the statuBox
  m_StatusBox = new Fl_Box(m_LeftMargin+m_SimpleWidgetsHeight+m_WidgetsMargin,this->y()+m_UpperMargin, 40,m_SimpleWidgetsHeight);
  m_StatusBox->box(FL_PLASTIC_DOWN_BOX);
  m_StatusBox->align(FL_ALIGN_INSIDE);
  this->add(m_StatusBox);

  // Hide it for now
  m_StatusBox->hide();

  // Eventually build the optional environment
  if(m_InputDataDescriptor.IsOptional())
    {
    m_CheckButton = new Fl_Check_Button( m_LeftMargin-m_WidgetsMargin-25,this->y()+m_UpperMargin, 25, 25);
    this->add(m_CheckButton);
    m_FlChoice->deactivate();
    // TODO: callbacks
    // m_CheckButton->callback((Fl_Callback *)InputViewGUI::ActivateInputChoice,(void *)inputChoiceDesc);
    }

  // Eventually build the multiple environment
  if(m_InputDataDescriptor.IsMultiple())
    { 
    // Build the browser
    m_FlBrowser = new Fl_Browser(m_LeftMargin,this->y()+m_UpperMargin+m_SimpleWidgetsHeight+m_WidgetsMargin, m_CentralWidgetsLength,m_MultipleWidgetsHeight);
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
    
    //m_AddButton->callback((Fl_Callback *)InputViewGUI::AddInputToList,(void *)inputChoiceDesc);
    
    // Build the remove button
    m_RemoveButton = new Fl_Button(m_LeftMargin + m_CentralWidgetsLength + m_WidgetsMargin + 10 ,this->y() + m_SimpleWidgetsHeight + 2 * m_MultipleWidgetsHeight / 3 , 20,20, "-");
    m_RemoveButton->box(FL_PLASTIC_ROUND_DOWN_BOX);
    m_RemoveButton->color((Fl_Color)55);
    m_RemoveButton->labelfont(1);
    m_RemoveButton->labelsize(17);
    m_RemoveButton->labelcolor((Fl_Color)186);
    this->add(m_RemoveButton);
    
    //m_RemoveButton->callback((Fl_Callback *)InputViewGUI::RemoveInputFromList,(void *)inputChoiceDesc);
    
    // Build the clear button
    m_ClearButton = new Fl_Button(m_LeftMargin + m_CentralWidgetsLength + m_WidgetsMargin ,this->y() + m_SimpleWidgetsHeight + 3 * m_MultipleWidgetsHeight / 3 ,40,m_SimpleWidgetsHeight, "Clear");
    this->add(m_ClearButton);
    m_ClearButton->box(FL_PLASTIC_DOWN_BOX);
    m_ClearButton->color((Fl_Color)55);
    m_ClearButton->labelfont(1);
    m_ClearButton->labelsize(12);
    m_ClearButton->labelcolor((Fl_Color)186); 
    //m_ClearButton->callback((Fl_Callback *)InputViewGUI::ClearList,(void *)inputChoiceDesc);
    }
}




} // end namespace otb

