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
#ifndef __otbEdgeExtractionModule_cxx
#define __otbEdgeExtractionModule_cxx

#include "otbEdgeExtractionModule.h"

namespace otb
{
/** Constructor */
EdgeExtractionModule::EdgeExtractionModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Build mvc
  m_ModelEdge      = EdgeExtractionModel::New();
  m_ViewEdge       = EdgeExtractionViewGUI::New();
  m_ControllerEdge = EdgeExtractionController::New();
  
  m_Model = m_ModelEdge;
  m_View = m_ViewEdge;
  m_Controller = m_ControllerEdge;
  
  m_ControllerEdge->SetModel(m_ModelEdge);
  m_ControllerEdge->SetView(m_ViewEdge);

  m_View->SetFeatureExtractionBaseController(m_Controller);
  m_View->InitVisu();

  // Describe inputs
  this->AddInputDescriptor<InputImageType>("InputImage", otbGetTextMacro("Image to apply edge detection"));

  // the model registers its module
  m_ModelEdge->RegisterListener(this);
}

EdgeExtractionModule::~EdgeExtractionModule()
{
}

/** PrintSelf method */
void EdgeExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

}

#endif
