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
#ifndef __otbAlgebraModule_cxx
#define __otbAlgebraModule_cxx

#include "otbAlgebraModule.h"

namespace otb
{
/**
 * Constructor
 */
AlgebraModule::AlgebraModule()
{
  m_HasSecondInput = false;

  // Describe inputs
  this->AddInputDescriptor<ImageType>("FirstImage", otbGetTextMacro("First image"));
  this->AddInputDescriptor<ImageType>("SecondImage", otbGetTextMacro("Second image"), true);

  // Build the GUI
  this->CreateGUI();
  guiRatioInput2->value(1);
}

/**
 * Destructor
 */
AlgebraModule::~AlgebraModule()
{}

/**
 * PrintSelf method
 */
void AlgebraModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/**
 * Addition Filter
 */
void AlgebraModule::AddImage()
{
  m_AddFilter   = AddFilterType::New();
  m_AddFilter->SetInput1(m_InputImage1);
  m_AddFilter->SetInput2(m_InputImage2);
  m_Output = m_AddFilter->GetOutput();
}

/**
* Subtract Filter
*/
void AlgebraModule::SubtractImage()
{
  m_SubtractFilter   = SubtractFilterType::New();
  m_SubtractFilter->SetInput1(m_InputImage1);
  m_SubtractFilter->SetInput2(m_InputImage2);
  m_Output = m_SubtractFilter->GetOutput();
}

/**
 * Multiply Filter
 */
void AlgebraModule::MultiplyImage()
{
  m_MultiplyFilter   = MultiplyFilterType::New();
  m_MultiplyFilter->SetInput1(m_InputImage1);
  m_MultiplyFilter->SetInput2(m_InputImage2);
  m_Output = m_MultiplyFilter->GetOutput();
}

/**
 * Divide Filter
 */
void AlgebraModule::DivideImage()
{
  m_DivideFilter = DivideFilterType::New();
  if (guiRatioInput1->value() == 0)
    {
    std::cout << "A/B" << std::endl;
    m_DivideFilter->SetInput1(m_InputImage1);
    m_DivideFilter->SetInput2(m_InputImage2);
    }
  else
    {
    std::cout << "B/A" << std::endl;
    m_DivideFilter->SetInput1(m_InputImage2);
    m_DivideFilter->SetInput2(m_InputImage1);
    }

  m_Output = m_DivideFilter->GetOutput();
}

/**
 * ShiftScale  Filter
 */
void AlgebraModule::ShiftScaleImage()
{
  m_ShiftScaleFilter   = ShiftScaleFilterType::New();

  // Get the scale and the shift from the GUI
  m_ShiftScaleFilter->SetScale(guiShiftScaleProduct->value());
  m_ShiftScaleFilter->SetShift(guiShiftScaleAddition->value());

  //If one Input deactivate the item to select second image
  if (!m_HasSecondInput)
    {
    m_ShiftScaleFilter->SetInput(m_InputImage1);
    }

  if (m_HasSecondInput)
    {
    switch (guiInputChoice->value())
      {
      case 0:
        m_ShiftScaleFilter->SetInput(m_InputImage1);
        break;
      case 1:
        m_ShiftScaleFilter->SetInput(m_InputImage2);
        break;
      default:
        break;
      }
    }

  m_Output = m_ShiftScaleFilter->GetOutput();
}

/** The custom run command */
void AlgebraModule::Run()
{
  // Check if there is two inputs
  if (this->GetNumberOfInputDataByKey("SecondImage") > 0) m_HasSecondInput = true;

  // Get the input image
  m_InputImage1 = this->GetInputData<ImageType>("FirstImage");

  // Check if not null
  if (m_InputImage1.IsNull())
    {
    itkExceptionMacro(<< "First image is NULL");
    }

  // Check if there is a second image
  if (m_HasSecondInput)
    {
    m_InputImage2 = this->GetInputData<ImageType>("SecondImage");
    if (m_InputImage2.IsNull())
      {
      itkExceptionMacro(<< "Second image is NULL");
      }
    }

  // Show the GUI
  this->Show();

  // Deactivate Filter that need 2 Inputs.
  if (!m_HasSecondInput)
    {
    iAdd->deactivate();
    iSubtract->deactivate();
    iMultiply->deactivate();
    iRatio->deactivate();
    iSecondImage->deactivate();
    // Set the operation to shift-scale
    guiOperation->value(4);
    guiOperation->deactivate();
    // Display the shift scale group
    guiShiftScaleGroup->show();
    }
  else
    {
    iAdd->activate();
    iSubtract->activate();
    iMultiply->activate();
    iRatio->activate();
    iSecondImage->activate();
    guiOperation->activate();
    guiShiftScaleGroup->hide();
    }
}

/**
 * OK CallBack
 */
void AlgebraModule::OK()
{
  // Apply the filter
  switch (guiOperation->value())
    {
    case 0:
      if (m_HasSecondInput) this->AddImage();
      break;
    case 1:
      if (m_HasSecondInput) this->SubtractImage();
      break;
    case 2:
      if (m_HasSecondInput) this->MultiplyImage();
      break;
    case 3:
      if (m_HasSecondInput) this->DivideImage();
      break;
    case 4:
      this->ShiftScaleImage();
      break;
    default:
      break;
    }

  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_Output, "OutputImage", otbGetTextMacro("Result image"));
  this->NotifyOutputsChange();

  // close the GUI
  this->Hide();
}

} // End namespace otb

#endif
