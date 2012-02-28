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
#ifndef __otbSmoothingViewGUI_cxx
#define __otbSmoothingViewGUI_cxx

#include "otbSmoothingViewGUI.h"
#include "otbFeatureSmooth.h"

namespace otb
{

SmoothingViewGUI::SmoothingViewGUI()
{
  // Create parameters for radiometric indices
  m_SpecificGUI = new SmoothingViewGroup();
  m_SpecificGUI->CreateParameters();
  
  // Plug the specific menu items to the main GUI
  guiFeatureChoice->menu(m_SpecificGUI->fakeChoice->menu());
  guiFeatureChoice->user_data((void*)(this));
  
  // Plug the specific parameter widgets in the main window
  
  guiParameter->add(m_SpecificGUI->guiRadius); // panel 1
  guiParameter->add(m_SpecificGUI->guiMS);

  this->InitParameterGroupList();
  
  // Rename window title
  SetWindowTitle(otbGetTextMacro("Smoothing"));
  
  // Set up callbacks on menu items
  int curPos = 0;
#define otbMenuItemCallbackMacro(mMenuItem, featureIndex, panelIndex)         \
  {                                                                                 \
    m_FeatureTable[curPos] = featureIndex;                                  \
    m_FeatureTable[curPos+1] = panelIndex;                                                          \
    m_SpecificGUI->mMenuItem->callback((Fl_Callback*)GenericMenuItemCallback, (void*)(m_FeatureTable+curPos));\
    curPos += 2;                                                                  \
  }
  otbMenuItemCallbackMacro(mInit, otb::FeatureInfoSmooth::UNKNOWN, 0);
  otbMenuItemCallbackMacro(mOriginal, otb::FeatureInfoSmooth::ORIGINAL, 0);
  otbMenuItemCallbackMacro(mMean, otb::FeatureInfoSmooth::MEAN, 1);
  otbMenuItemCallbackMacro(mMSSmooth, otb::FeatureInfoSmooth::MS_SMOOTH, 2);
  otbMenuItemCallbackMacro(mMSCluster, otb::FeatureInfoSmooth::MS_CLUSTERED, 2);
#undef otbMenuItemCallbackMacro
  
}

SmoothingViewGUI::~SmoothingViewGUI()
{
}

void
SmoothingViewGUI
::UpdateFeatureInfo(FeatureType feat)
{
  FeatureInfoSmooth        inf;
  itk::OStringStream oss;
  oss << inf.GetMapInfo().find(feat)->second;
  guiFeatInfo->buffer()->remove(0, guiFeatInfo->buffer()->length());
  guiFeatInfo->insert(oss.str().c_str());
  guiFeatInfo->redraw();
}

void SmoothingViewGUI::InitParameterGroupList()
{
  Superclass::InitParameterGroupList();
  m_ParameterGroupList.push_back(m_SpecificGUI->guiRadius); // panel 1
  m_ParameterGroupList.push_back(m_SpecificGUI->guiMS);
}

void
SmoothingViewGUI::GenericMenuItemCallback_i(Fl_Menu_* o, void* v)
{
  int *index = (int*) v;
  UpdateParameterArea(*(index+1));
  SetFeatureType(*index);
}

void
SmoothingViewGUI::GenericMenuItemCallback(Fl_Menu_* o, void* v)
{
  ((SmoothingViewGUI *)(o->user_data()))->GenericMenuItemCallback_i(o, v);
}

}

#endif
