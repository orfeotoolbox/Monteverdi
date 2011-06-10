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
#ifndef __otbDEMToImageGeneratorModule_cxx
#define __otbDEMToImageGeneratorModule_cxx

#include "otbDEMToImageGeneratorModule.h"
#include "otbMsgReporter.h"

namespace otb
{
/** Constructor */
DEMToImageGeneratorModule::DEMToImageGeneratorModule()
{
  // Build mvc
  m_View       = DEMToImageGeneratorView::New();
  m_Controller = DEMToImageGeneratorController::New();
  m_Model      = DEMToImageGeneratorModel::New();
  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);
  m_Model->RegisterListener(this);
  m_Model->RegisterListener(m_View);

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage","reference image ", true, false);
}

/** Destructor */
DEMToImageGeneratorModule::~DEMToImageGeneratorModule()
{}

/** PrintSelf method */
void DEMToImageGeneratorModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void DEMToImageGeneratorModule::Run()
{
  InputImageType::Pointer inputImage = this->GetInputData<InputImageType>("InputImage");

  if (inputImage.IsNotNull())
    {
    inputImage->UpdateOutputInformation();
    if (inputImage->GetProjectionRef().empty())
      {
      ImageKeywordlist kwl;
      kwl = inputImage->GetImageKeywordlist();
      if(kwl.GetSize() == 0)
        {
        // index type image => return
        m_View->Hide();
        MsgReporter::GetInstance()->SendError("Wrong type of input image: index image without "
                                                "projection reference or keyword list.\n"
                                                "This message occurs with the DEMTotImageGenarator module.");
        return;
        }
      }
    m_Model->UpdateOutputParametersFromImage(inputImage);
    }
  m_View->Show();
}

/** The Notify */
void DEMToImageGeneratorModule::Notify()
{
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_Model->GetOutput(), "DEM Image", otbGetTextMacro("DEM image"));
    if(m_Model->GetHillShadingProcess())
      {
      this->AddOutputDescriptor(m_Model->GetHillShading()->GetOutput(),
                                "Hill Shading Image",
                                otbGetTextMacro("Hill Shading image"));
      }

    if(m_Model->GetReliefProcess())
      {
      this->AddOutputDescriptor(m_Model->GetReliefColored(),
                                "Relief colored Image",
                                otbGetTextMacro("Relief Colored Image"));
      }

    this->NotifyOutputsChange();
}

} // End namespace otb

#endif
