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
#ifndef __otbBandMathModule_cxx
#define __otbBandMathModule_cxx

#include "otbBandMathModule.h"
#include "otbMsgReporter.h"

#include "otbVectorImageToImageListFilter.h"

namespace otb
{
/**
 * Constructor
 */
BandMathModule::BandMathModule()
{
  // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Image to process"), false, true);
  this->AddTypeToInputDescriptor<VectorImageType>("InputImage");
  
  // Build the GUI
  this->CreateGUI();
}

/**
 * Destructor
 */
BandMathModule::~BandMathModule()
{}

/**
 * PrintSelf method
 */
void BandMathModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/**
 * Hide the Module GUI
 */
void BandMathModule::Hide()
{
  guiMainWindow->hide();
  ui_HelpWindow->hide();
  ui_Help->value(0);
}

/**
 * The custom run command
 */
void BandMathModule::Run()
{
  // First step is to retrieve the inputs
  // Get the number of input image
  unsigned int numberOfInputImages = this->GetNumberOfInputDataByKey("InputImage");
  unsigned int bandId = 0;
  unsigned int imageId = 0;
  
  m_ChannelExtractorList = ExtractROIFilterListType::New();

  if (numberOfInputImages == 0)
    {
    itkExceptionMacro(<< "The number of input images is Null.");
    }
  
  // Setup the filter
  m_BandMathFilter = BandMathFilterType::New();
  for(unsigned int i = 0; i < numberOfInputImages; i++)
    {
    // Test if the input is an Image or a VectorImage
    ImageType::Pointer image = this->GetInputData<ImageType>("InputImage", i);
    VectorImageType::Pointer vectorImage = this->GetInputData<VectorImageType>("InputImage", i);
    
    // If the input is neither an Image nor a vectorImage
    if (image.IsNull() && vectorImage.IsNull())
      {
      itkExceptionMacro(<< "The input image number " << bandId + 1 << " is Null.");
      }
    
    // From here on, the input is either an Image or a vectorImage
    // The input is an image
    if (image.IsNotNull())
      {
      std::ostringstream tmpParserVarName;
      tmpParserVarName << "im" << imageId+1 << "b1";

      m_BandMathFilter->SetNthInput(bandId, image, tmpParserVarName.str());
      
      ui_ImageNameList->add(this->GetInputDataDescription<ImageType>("InputImage", i).c_str());
      ui_VarNames->add(m_BandMathFilter->GetNthInputName(bandId).c_str());
      ui_ImageNames->add(this->GetInputDataDescription<ImageType>("InputImage", i).c_str());
      bandId ++;
      }
    // The input is an vectorImage
    else
      {
      // Extract bands from the vectorImage
      for(unsigned int j = 0; j < vectorImage->GetNumberOfComponentsPerPixel(); j++)
        {
        std::ostringstream tmpVarName, tmpParserVarName;
        tmpParserVarName << "im" << imageId+1 << "b" << j+1;
        
        ExtractROIFilterType::Pointer extractROIFilter = ExtractROIFilterType::New();
        extractROIFilter->SetInput(vectorImage);
        extractROIFilter->SetChannel(j+1);
        extractROIFilter->GetOutput()->UpdateOutputInformation();
        m_ChannelExtractorList->PushBack(extractROIFilter);
        m_BandMathFilter->SetNthInput(bandId, m_ChannelExtractorList->Back()->GetOutput(), tmpParserVarName.str());
        
        tmpVarName << this->GetInputDataDescription<ImageType>("InputImage", i) << "(band" << j+1 << ")";
        ui_ImageNameList->add(tmpVarName.str().c_str());
        ui_ImageNames->add(tmpVarName.str().c_str());
        ui_VarNames->add(m_BandMathFilter->GetNthInputName(bandId).c_str());
        bandId ++;
        }
      }
    imageId ++;
    }
  m_NumberOfInputBands = bandId;
  ui_ImageNameList->value(0);
  
  // Initialize the help window
  this->InitHelp();
  
  // Show the GUI
  this->Show();
}

/**
 * Help Initialization
 */
void BandMathModule::InitHelp()
{
  std::ostringstream helpContent;
  
  helpContent << "- Functions: "<< std::endl;
  helpContent << "-- Trigonometric Functions" << std::endl << "sin() - cos() - tan()" << std::endl;
  helpContent << "-- Arcus Functions" << std::endl << "asin() - acos() - atan()" << std::endl;
  helpContent << "-- Hyperbholic Functions" << std::endl << "sinh() - cosh() - tanh()" << std::endl;
  helpContent << "-- Arcus Hyperbolic Functions" << std::endl << "asinh() - acosh() - atanh()" << std::endl;
  helpContent << "-- Logarithm Functions" << std::endl << "log2() - log10() - log() - ln()" << std::endl;
  helpContent << "-- Conditional Functions" << std::endl << "if(condition, then, else)" << std::endl;
  helpContent << "-- Misc" << std::endl << "exp() - sqrt() - sign() - rint() - abs() - if()" << std::endl;
  helpContent << "-- Functions With Variable Number of Arguments" << std::endl;
  helpContent << "sum() - avg() - min() - max()" << std::endl;
  helpContent << "-- OTB Functions" << std::endl << "ndvi()" << std::endl << std::endl;
  helpContent << "- Constants: " << std::endl << "e - log2e - log10e - ln2 - ln10 - ";
  helpContent << "pi - euler" << std::endl;
  
  helpContent << "This filter is based on the mathematical parser library muParser. "
      "The built in functions and operators list is available at: "
      "http://muparser.sourceforge.net/mup_features.html" << std::endl;

  ui_HelpText->value(helpContent.str().c_str());
}

/**
 * Change a Variable name
 */
void BandMathModule::ChangeVarName()
{
  unsigned int idx = ui_ImageNameList->value();
  std::string newName(ui_NewVarName->value());
  size_t found1, found2;
  
  found1 = newName.find_first_of(" ");
  found2 = newName.find_first_of(".");
  
  if((found1 == std::string::npos) && (found2 == std::string::npos) && (newName.compare("")))
    {
    m_BandMathFilter->SetNthInputName(idx, ui_NewVarName->value());
    ui_VarNames->remove(idx+1);
    ui_VarNames->insert(idx+1, m_BandMathFilter->GetNthInputName(idx).c_str());
    }
  
  LiveCheck();
}

/**
 * Quick add a variable name into the expression
*/
void BandMathModule::QuickAdd(unsigned int idx)
{
  std::ostringstream tmpExpression;
  
  if((idx-1) < m_NumberOfInputBands)
    {
    ui_VarNames->select(idx);
    ui_ImageNames->select(idx);
    
    tmpExpression << ui_Expression->value() << " " << m_BandMathFilter->GetNthInputName(idx-1) << " ";
    ui_Expression->value(tmpExpression.str().c_str());
    ui_Expression->take_focus();
    }
  LiveCheck();
}

/**
 * Live Checking
 */
void BandMathModule::LiveCheck()
{
  ParserType::Pointer dummyParser = ParserType::New();
  std::vector<double> dummyVars;
  double value;
  
  ui_Expression->color(FL_GREEN);
  ui_Expression->tooltip("The Expression is Valid");
  ui_Ok->activate();
  
  // Setup the dummy parser
  for(unsigned int i = 0; i < m_NumberOfInputBands; i++)
    {
    dummyVars.push_back(1);
    dummyParser->DefineVar(m_BandMathFilter->GetNthInputName(i), &(dummyVars.at(i)));
    }
  dummyParser->SetExpr(ui_Expression->value());
  // Check the expression
  try
    {
    value = dummyParser->Eval();
    }
  catch(itk::ExceptionObject& err)
    {
    ui_Expression->color(FL_RED);
    ui_Expression->tooltip(err.GetDescription());
    ui_Ok->deactivate();
    }
  ui_Expression->redraw();
}

/**
 * Add Indexes
 */
void BandMathModule::AddIndexes()
{
  std::vector<std::string> tmpVarNames;
  
  tmpVarNames.resize(4);
  tmpVarNames.at(0) =  "index X";
  tmpVarNames.at(1) =  "index Y";
  tmpVarNames.at(2) =  "physical index X";
  tmpVarNames.at(3) =  "physical index Y";

  for(unsigned int j=0; j < 4; j++)
    {
    ui_ImageNameList->add(tmpVarNames.at(j).c_str());
    ui_ImageNames->add(tmpVarNames.at(j).c_str());
    ui_VarNames->add(m_BandMathFilter->GetNthInputName(m_NumberOfInputBands+j).c_str());
    }
  
  m_NumberOfInputBands += 4;
  ui_ImageNameList->value(0);
}

/**
 * OK CallBack
 */
void BandMathModule::OK()
{
  // Apply the filter
  m_BandMathFilter->SetExpression(ui_Expression->value());
  m_Output = m_BandMathFilter->GetOutput();
  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_BandMathFilter->GetOutput(), "OutputImage", otbGetTextMacro("Result image"));
  this->NotifyOutputsChange();
  
  // close the GUI
  this->Hide();
}

} // End namespace otb

#endif
