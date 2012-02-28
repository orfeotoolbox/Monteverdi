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
#ifndef __otbTextureExtractionModule_cxx
#define __otbTextureExtractionModule_cxx

#include "otbTextureExtractionModule.h"

namespace otb
{
/** Constructor */
TextureExtractionModule::TextureExtractionModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Build mvc
  m_ModelTex      = TextureExtractionModel::New();
  m_ViewTex       = TextureExtractionViewGUI::New();
  m_ControllerTex = TextureExtractionController::New();
  
  m_Model = m_ModelTex;
  m_View = m_ViewTex;
  m_Controller = m_ControllerTex;
  
  m_ControllerTex->SetModel(m_ModelTex);
  m_ControllerTex->SetView(m_ViewTex);

  m_View->SetFeatureExtractionBaseController(m_Controller);
  m_View->InitVisu();

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage", otbGetTextMacro("Image to apply texture extraction"));

  // the model registers its module
  m_ModelTex->RegisterListener(this);
}

TextureExtractionModule::~TextureExtractionModule()
{
}

/** PrintSelf method */
void TextureExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

}

#endif
