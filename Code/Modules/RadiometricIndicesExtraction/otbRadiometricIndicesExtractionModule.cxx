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
#ifndef __otbRadiometricIndicesExtractionModule_cxx
#define __otbRadiometricIndicesExtractionModule_cxx

#include "otbRadiometricIndicesExtractionModule.h"

namespace otb
{
/** Constructor */
RadiometricIndicesExtractionModule::RadiometricIndicesExtractionModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Build mvc
  m_ModelRI      = RadiometricIndicesExtractionModel::New();
  m_ViewRI       = RadiometricIndicesExtractionViewGUI::New();
  m_ControllerRI = RadiometricIndicesExtractionController::New();
  
  m_Model = m_ModelRI;
  m_View = m_ViewRI;
  m_Controller = m_ControllerRI;
  
  m_ControllerRI->SetModel(m_ModelRI);
  m_ControllerRI->SetView(m_ViewRI);

  m_View->SetFeatureExtractionBaseController(m_Controller);
  m_View->InitVisu();

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage", otbGetTextMacro("Image to apply radiometric indices extraction"));

  // the model registers its module
  m_ModelRI->RegisterListener(this);
}

RadiometricIndicesExtractionModule::~RadiometricIndicesExtractionModule()
{
}

/** PrintSelf method */
void RadiometricIndicesExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

}

#endif
