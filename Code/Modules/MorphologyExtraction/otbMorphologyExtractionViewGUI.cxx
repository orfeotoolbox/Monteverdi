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
#ifndef __otbMorphologyExtractionViewGUI_cxx
#define __otbMorphologyExtractionViewGUI_cxx

#include "otbMorphologyExtractionViewGUI.h"
#include "otbFeatureMorpho.h"

namespace otb
{

MorphologyExtractionViewGUI::MorphologyExtractionViewGUI()
{
  // Create parameters for radiometric indices
  m_SpecificGUI = new MorphologyExtractionViewGroup();
  m_SpecificGUI->CreateParameters();
  
  // Plug the specific menu items to the main GUI
  guiFeatureChoice->menu(m_SpecificGUI->fakeChoice->menu());
  guiFeatureChoice->user_data((void*)(this));
  
  // Plug the specific parameter widgets in the main window
  guiParameter->add(m_SpecificGUI->guiMorpho); // panel 1

  this->InitParameterGroupList();
  
  // Rename window title
  SetWindowTitle(otbGetTextMacro("Morphology extraction"));
  
  // Set up callbacks on menu items
  int curPos = 0;
#define otbMenuItemCallbackMacro(mMenuItem, featureIndex, panelIndex)         \
  {                                                                                 \
    m_FeatureTable[curPos] = featureIndex;                                  \
    m_FeatureTable[curPos+1] = panelIndex;                                                          \
    m_SpecificGUI->mMenuItem->callback((Fl_Callback*)GenericMenuItemCallback, (void*)(m_FeatureTable+curPos));\
    curPos += 2;                                                                  \
  }
  otbMenuItemCallbackMacro(mInit, otb::FeatureInfoMorpho::UNKNOWN, 0);
  otbMenuItemCallbackMacro(mOpening, otb::FeatureInfoMorpho::MORPHOLOGICALOPENING, 1);
  otbMenuItemCallbackMacro(mClosing, otb::FeatureInfoMorpho::MORPHOLOGICALCLOSING, 1);
#undef otbMenuItemCallbackMacro
  
}

MorphologyExtractionViewGUI::~MorphologyExtractionViewGUI()
{
}

void
MorphologyExtractionViewGUI
::UpdateFeatureInfo(FeatureType feat)
{
  FeatureInfoMorpho        inf;
  itk::OStringStream oss;
  oss << inf.GetMapInfo().find(feat)->second;
  guiFeatInfo->buffer()->remove(0, guiFeatInfo->buffer()->length());
  guiFeatInfo->insert(oss.str().c_str());
  guiFeatInfo->redraw();
}

void MorphologyExtractionViewGUI::InitParameterGroupList()
{
  Superclass::InitParameterGroupList();
  m_ParameterGroupList.push_back(m_SpecificGUI->guiMorpho); // panel 1
}

void
MorphologyExtractionViewGUI::GenericMenuItemCallback_i(Fl_Menu_* o, void* v)
{
  int *index = (int*) v;
  UpdateParameterArea(*(index+1));
  SetFeatureType(*index);
}

void
MorphologyExtractionViewGUI::GenericMenuItemCallback(Fl_Menu_* o, void* v)
{
  ((MorphologyExtractionViewGUI *)(o->user_data()))->GenericMenuItemCallback_i(o, v);
}

}

#endif
