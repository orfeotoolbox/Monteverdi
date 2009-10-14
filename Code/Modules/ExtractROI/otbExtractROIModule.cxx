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

#include "otbExtractROIModule.h"
#include "otbMsgReporter.h"
#include <stdio.h>
#include <stdlib.h>

namespace otb
{
/** Constructor */
ExtractROIModule::ExtractROIModule()
{
  // Describe inputs
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage","Image to read.");
  m_ExtractROIFilter = ExtractROIFilterType::New();
  this->BuildGUI();
}

/** Destructor */
ExtractROIModule::~ExtractROIModule()
{}

/** PrintSelf method */
void ExtractROIModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void ExtractROIModule::Run()
{
  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");
  vectorImage->UpdateOutputInformation();
  vInputImageSizeX->value(vectorImage->GetRequestedRegion().GetSize()[0]);
  vInputImageSizeY->value(vectorImage->GetRequestedRegion().GetSize()[1]);

  vStartX->minimum(static_cast<double>(vectorImage->GetRequestedRegion().GetIndex()[0]));
  vStartX->maximum(static_cast<double>(vectorImage->GetRequestedRegion().GetSize()[0]));
  vStartX->value(vStartX->minimum());
  vStartY->minimum(static_cast<double>(vectorImage->GetRequestedRegion().GetIndex()[1]));
  vStartY->maximum(static_cast<double>(vectorImage->GetRequestedRegion().GetSize()[1]));
  vStartY->value(vStartY->minimum());

  vSizeX->minimum(vStartX->minimum());
  vSizeX->maximum(vStartX->maximum());
  vSizeX->value(vSizeX->maximum());
  vSizeY->minimum(vStartY->minimum());
  vSizeY->maximum(vStartY->maximum());
  vSizeY->value(vSizeY->maximum());
  wExtractROIWindow->show();
}


/** Cancel */
void ExtractROIModule::Cancel()
{
    wExtractROIWindow->hide();
}
/** Ok */
void ExtractROIModule::Ok()
{
  try
    {
    // Get Input Image
    FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");

    m_ExtractROIFilter->SetStartX(static_cast<unsigned long>(vStartX->value()));
    m_ExtractROIFilter->SetStartY(static_cast<unsigned long>(vStartY->value()));
    m_ExtractROIFilter->SetSizeX(static_cast<unsigned long>(vSizeX->value()));
    m_ExtractROIFilter->SetSizeY(static_cast<unsigned long>(vSizeY->value()));
    m_ExtractROIFilter->SetInput(vectorImage);
    // Notify all listener
    // TODO: this should not be done by the user
    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_ExtractROIFilter->GetOutput(),"OutputImage","Image extracted." );
    this->NotifyOutputsChange();
    wExtractROIWindow->hide();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


} // End namespace otb


