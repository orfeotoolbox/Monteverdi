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

/** The custom run command */
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
  std::ostringstream VarNames, ImageNames;
  for(unsigned int i = 0; i < numberOfInputImages; i++)
    {
    ImageType::Pointer image = this->GetInputData<ImageType>("InputImage", i);
    if (image.IsNull())
      {
      itkExceptionMacro(<< "The input image number " << i+1 << " is Null.");
      }
    m_ParserFilter->SetNthInput(i, image);
    VarNames << "(" << i+1 << ") - "; 
    VarNames << m_ParserFilter->GetNthInputName(i) << std::endl;
    VarNames << "--------------------" << std::endl;
    ImageNames << "(" << i+1 << ") - "; 
    ImageNames << this->GetInputDataDescription<ImageType>("InputImage", i) << std::endl;
    ImageNames << "--------------------" << std::endl;
    }

  ui_ImageNames->value(ImageNames.str().c_str());
  ui_VarNames->value(VarNames.str().c_str());

  // Show the GUI
  this->Show();

  
}

/**
 * OK CallBack
 */
void ParserModule::OK()
{
  // Apply the filter
  m_ParserFilter->SetExpression(ui_expression->value());
  m_ParserFilter->Update();

  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_ParserFilter->GetOutput(), "OutputImage", otbGetTextMacro("Result image"));
  this->NotifyOutputsChange();

  // close the GUI
  this->Hide();
}

} // End namespace otb

#endif
