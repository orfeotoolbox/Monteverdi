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
#ifndef __otbMorphologyExtractionModule_cxx
#define __otbMorphologyExtractionModule_cxx

#include "otbMorphologyExtractionModule.h"

namespace otb
{
/** Constructor */
MorphologyExtractionModule::MorphologyExtractionModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Build mvc
  m_ModelMorpho      = MorphologyExtractionModel::New();
  m_ViewMorpho       = MorphologyExtractionViewGUI::New();
  m_ControllerMorpho = MorphologyExtractionController::New();
  
  m_Model = m_ModelMorpho;
  m_View = m_ViewMorpho;
  m_Controller = m_ControllerMorpho;
  
  m_ControllerMorpho->SetModel(m_ModelMorpho);
  m_ControllerMorpho->SetView(m_ViewMorpho);

  m_View->SetFeatureExtractionBaseController(m_Controller);
  m_View->InitVisu();

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage", "Image to apply morphology extraction");

  // the model registers its module
  m_ModelMorpho->RegisterListener(this);
}

MorphologyExtractionModule::~MorphologyExtractionModule()
{
}

/** PrintSelf method */
void MorphologyExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

}

#endif
