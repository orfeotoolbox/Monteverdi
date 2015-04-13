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
#ifndef __otbEdgeExtractionViewGUI_cxx
#define __otbEdgeExtractionViewGUI_cxx

#include "otbEdgeExtractionViewGUI.h"
#include "otbFeatureEdge.h"

namespace otb
{

EdgeExtractionViewGUI::EdgeExtractionViewGUI()
{
  // Create parameters for radiometric indices
  m_SpecificGUI = new EdgeExtractionViewGroup();
  m_SpecificGUI->CreateParameters();
  
  // Plug the specific menu items to the main GUI
  guiFeatureChoice->menu(m_SpecificGUI->fakeChoice->menu());
  guiFeatureChoice->user_data((void*)(this));
  
  // Plug the specific parameter widgets in the main window
  guiParameter->add(m_SpecificGUI->guiHarris); // panel 1
  guiParameter->add(m_SpecificGUI->guiRadius);
  guiParameter->add(m_SpecificGUI->guiGrad);
  guiParameter->add(m_SpecificGUI->guiEdgeSobel);
  guiParameter->add(m_SpecificGUI->guiEdgeCanny);
  guiParameter->add(m_SpecificGUI->guiTouzi);

  this->InitParameterGroupList();
  
  // Rename window title
  SetWindowTitle("Edge extraction");
  
  // Set up callbacks on menu items
  int curPos = 0;
#define otbMenuItemCallbackMacro(mMenuItem, featureIndex, panelIndex)         \
  {                                                                                 \
    m_FeatureTable[curPos] = featureIndex;                                  \
    m_FeatureTable[curPos+1] = panelIndex;                                                          \
    m_SpecificGUI->mMenuItem->callback((Fl_Callback*)GenericMenuItemCallback, (void*)(m_FeatureTable+curPos));\
    curPos += 2;                                                                  \
  }
  otbMenuItemCallbackMacro(mInit, otb::FeatureInfoEdge::UNKNOWN, 0);
  otbMenuItemCallbackMacro(mHarris, otb::FeatureInfoEdge::HARRIS, 1);
  otbMenuItemCallbackMacro(mVariance, otb::FeatureInfoEdge::VARIANCE, 2);
  otbMenuItemCallbackMacro(mGradient, otb::FeatureInfoEdge::GRADIENT, 3);
  otbMenuItemCallbackMacro(mSobel, otb::FeatureInfoEdge::EDGE_SOBEL, 4);
  otbMenuItemCallbackMacro(mTouzi, otb::FeatureInfoEdge::TOUZI, 5);
#undef otbMenuItemCallbackMacro
  
}

EdgeExtractionViewGUI::~EdgeExtractionViewGUI()
{
}

void
EdgeExtractionViewGUI
::UpdateFeatureInfo(FeatureType feat)
{
  FeatureInfoEdge        inf;
  std::ostringstream oss;
  oss << inf.GetMapInfo().find(feat)->second;
  guiFeatInfo->buffer()->remove(0, guiFeatInfo->buffer()->length());
  guiFeatInfo->insert(oss.str().c_str());
  guiFeatInfo->redraw();
}

void EdgeExtractionViewGUI::InitParameterGroupList()
{
  Superclass::InitParameterGroupList();
  m_ParameterGroupList.push_back(m_SpecificGUI->guiHarris); // panel 1
  m_ParameterGroupList.push_back(m_SpecificGUI->guiRadius);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiGrad);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiEdgeSobel);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiEdgeCanny);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiTouzi);
}

void
EdgeExtractionViewGUI::GenericMenuItemCallback_i(Fl_Menu_* o, void* v)
{
  int *index = (int*) v;
  UpdateParameterArea(*(index+1));
  SetFeatureType(*index);
}

void
EdgeExtractionViewGUI::GenericMenuItemCallback(Fl_Menu_* o, void* v)
{
  ((EdgeExtractionViewGUI *)(o->user_data()))->GenericMenuItemCallback_i(o, v);
}

}

#endif
