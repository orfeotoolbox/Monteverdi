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
 * Hide the Module GUI
 */
void ParserModule::Hide()
{
  guiMainWindow->hide();
  ui_HelpWindow->hide();
  ui_Help->value(0);
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
  
  // Initialize the help window
  this->InitHelp();

  // Show the GUI
  this->Show();
}

/** 
 * Help Initialization
 */
void ParserModule::InitHelp()
{
  std::ostringstream helpContent;
  
  helpContent << "- Functions : "<< std::endl;
  helpContent << "-- Trigonometric Functions" << std::endl << "sin() - cos() - tan()" << std::endl;
  helpContent << "-- Arcus Functions" << std::endl << "asin() - acos() - atan()" << std::endl;
  helpContent << "-- Hyperbholic Functions" << std::endl << "sinh() - cosh() - tanh()" << std::endl;
  helpContent << "-- Arcus Hyperbolic Functions" << std::endl << "asinh() - acosh() - atanh()" << std::endl;
  helpContent << "-- Logarithm Functions" << std::endl << "log2() - log10() - log() - ln()" << std::endl;
  helpContent << "-- Misc" << std::endl << "exp() - sqrt() - sign() - rint() - abs() - if()" << std::endl;
  helpContent << "-- Functions With Variable Number of Arguments" << std::endl;
  helpContent << "sum() - avg() - min() - max()" << std::endl;
  helpContent << "-- OTB Functions" << std::endl << "ndvi()" << std::endl << std::endl;
  helpContent << "- Constants : " << std::endl << "e - log2e - log10e - ln2 - ln10 - ";
  helpContent << "pi - euler" << std::endl;

  ui_HelpText->value(helpContent.str().c_str());
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
  
  if((idx-1) < numberOfInputImages)
    {
    ui_VarNames->select(idx);
    ui_ImageNames->select(idx);

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
  unsigned int numberOfInputImages = this->GetNumberOfInputDataByKey("InputImage");
  unsigned int exceptFlag = 0;
  ParserType::Pointer dummyParser = ParserType::New();
  std::vector<double> dummyVars;
  double value;

  // Setup the dummy parser
  for(unsigned int i = 0; i < numberOfInputImages; i++)
    {
    dummyVars.push_back(1);
    dummyParser->DefineVar(m_ParserFilter->GetNthInputName(i), &(dummyVars.at(i)));
    }
  dummyParser->SetExpr(ui_Expression->value());
    
  try
    {
    value = dummyParser->Eval();
    }
  catch(itk::ExceptionObject& err)
    {
    exceptFlag = 1;
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
  
  if(!exceptFlag)
    {
    ui_Expression->color((Fl_Color)7);
    ui_Expression->redraw();
    // Apply the filter
    m_ParserFilter->SetExpression(ui_Expression->value());
    m_Output = m_ParserFilter->GetOutput();
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_ParserFilter->GetOutput(), "OutputImage", otbGetTextMacro("Result image"));
    this->NotifyOutputsChange();

    // close the GUI
    this->Hide();
    ui_Expression->label("Enter Your Formula :");
    }
  else
    {
    exceptFlag=0;
    ui_Expression->label("[WRONG FORMULA] - Please Check Your Formula :");
    ui_Expression->color(1);
    ui_Expression->redraw();
    }
}

} // End namespace otb

#endif
