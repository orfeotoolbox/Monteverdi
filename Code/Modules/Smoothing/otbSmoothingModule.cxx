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
#ifndef __otbSmoothingModule_cxx
#define __otbSmoothingModule_cxx

#include "otbSmoothingModule.h"

namespace otb
{
/** Constructor */
SmoothingModule::SmoothingModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Build mvc
  m_ModelSmooth      = SmoothingModel::New();
  m_ViewSmooth       = SmoothingViewGUI::New();
  m_ControllerSmooth = SmoothingController::New();
  
  m_Model = m_ModelSmooth;
  m_View = m_ViewSmooth;
  m_Controller = m_ControllerSmooth;
  
  m_ControllerSmooth->SetModel(m_ModelSmooth);
  m_ControllerSmooth->SetView(m_ViewSmooth);

  m_View->SetFeatureExtractionBaseController(m_Controller);
  m_View->InitVisu();

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage", otbGetTextMacro("Image to apply smoothing"));

  // the model registers its module
  m_ModelSmooth->RegisterListener(this);
}

SmoothingModule::~SmoothingModule()
{
}

/** PrintSelf method */
void SmoothingModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

}

#endif
