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
#ifndef __otbParserModule_cxx
#define __otbParserModule_cxx

#include "otbParserModule.h"
#include "otbMsgReporter.h"

namespace otb
{
/**
 * Constructor
 */
ParserModule::ParserModule()
{
   // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Image to process"), false, true);
  
  // Build the GUI
  this->CreateGUI();
}

/**
 * Destructor
 */
ParserModule::~ParserModule()
{}

/**
 * PrintSelf method
 */
void ParserModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** 
 * The custom run command 
 */
void ParserModule::Run()
{
  // First step is to retrieve the inputs
  // Get the number of input image
  unsigned int numberOfInputImages = this->GetNumberOfInputDataByKey("InputImage");

  if (numberOfInputImages == 0)
    {
    itkExceptionMacro(<< "The number of input images is Null.");
    }

  // Setup the filter
  m_ParserFilter = ParserFilterType::New();
  for(unsigned int i = 0; i < numberOfInputImages; i++)
    {
    ImageType::Pointer image = this->GetInputData<ImageType>("InputImage", i);
    if (image.IsNull())
      {
      itkExceptionMacro(<< "The input image number " << i+1 << " is Null.");
      }
    m_ParserFilter->SetNthInput(i, image);
    
    ui_VarNameList->add(this->GetInputDataDescription<ImageType>("InputImage", i).c_str());
    ui_VarNames->add(m_ParserFilter->GetNthInputName(i).c_str());
    ui_ImageNames->add(this->GetInputDataDescription<ImageType>("InputImage", i).c_str());
    }

  ui_VarNameList->value(0);
  
  // Show the GUI
  this->Show();
}

/** 
 * Change a Variable name
 */
void ParserModule::ChangeVarName()
{
  unsigned int idx = ui_VarNameList->value();
  std::string newName(ui_NewVarName->value());
  size_t found;

  found = newName.find_first_of(" ");

  if((found == std::string::npos) && (newName.compare("")))
    {
     m_ParserFilter->SetNthInputName(idx, ui_NewVarName->value());
     ui_VarNames->remove(idx+1);
     ui_VarNames->insert(idx+1, m_ParserFilter->GetNthInputName(idx).c_str());
    }
}

/**
 * Quick add a variable name into the expression
*/
void ParserModule::QuickAdd(unsigned int idx)
{
  unsigned int numberOfInputImages = this->GetNumberOfInputDataByKey("InputImage");
  std::ostringstream tmpExpression;
  
  ui_VarNames->select(idx);
  ui_ImageNames->select(idx);

  if((idx-1) < numberOfInputImages)
    {
    tmpExpression << ui_Expression->value() << " " << m_ParserFilter->GetNthInputName(idx-1) << " ";
    ui_Expression->value(tmpExpression.str().c_str());
    ui_Expression->take_focus();
    }
}

/**
 * OK CallBack
 */
void ParserModule::OK()
{
  // Apply the filter
  m_ParserFilter->SetExpression(ui_Expression->value());
  
  try
    {
    m_Output = m_ParserFilter->GetOutput();
    }
  catch(itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
  
  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_ParserFilter->GetOutput(), "OutputImage", otbGetTextMacro("Result image"));
  this->NotifyOutputsChange();

  // close the GUI
  this->Hide();
}

} // End namespace otb

#endif
