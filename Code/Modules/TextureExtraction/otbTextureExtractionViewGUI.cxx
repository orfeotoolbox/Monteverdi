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
#ifndef __otbTextureExtractionViewGUI_cxx
#define __otbTextureExtractionViewGUI_cxx

#include "otbTextureExtractionViewGUI.h"
#include "otbFeatureTex.h"

namespace otb
{

TextureExtractionViewGUI::TextureExtractionViewGUI()
{
  // Create parameters for textures
  m_SpecificGUI = new TextureExtractionViewGroup();
  m_SpecificGUI->CreateParameters();

  // Plug the specific menu items to the main GUI
  guiFeatureChoice->menu(m_SpecificGUI->fakeChoice->menu());
  guiFeatureChoice->user_data((void*)(this));

  // Plug the specific parameter widgets in the main window
  guiParameter->add(m_SpecificGUI->guiSFS); // panel 1
  guiParameter->add(m_SpecificGUI->guiHaralick);
  guiParameter->add(m_SpecificGUI->guiAdvanced);
  guiParameter->add(m_SpecificGUI->guiPanTex);

  // setup for callbacks
  m_SpecificGUI->guiHaralick->user_data((void*)(this));
  m_SpecificGUI->guiAdvanced->user_data((void*)(this));

  m_SpecificGUI->guiHarCk->callback((Fl_Callback*)HarCkCallback);
  m_SpecificGUI->guiHarMin->callback((Fl_Callback*)HarMinCallback);
  m_SpecificGUI->guiHarMax->callback((Fl_Callback*)HarMaxCallback);

  m_SpecificGUI->guiAdvHarCk->callback((Fl_Callback*)AdvHarCkCallback);
  m_SpecificGUI->guiAdvHarMin->callback((Fl_Callback*)AdvHarMinCallback);
  m_SpecificGUI->guiAdvHarMax->callback((Fl_Callback*)AdvHarMaxCallback);

  this->InitParameterGroupList();
  this->InitTextureLists();

  // Rename window title
  SetWindowTitle("Texture extraction");

  // Set up callbacks on menu items
  int curPos = 0;
#define otbMenuItemCallbackMacro(mMenuItem, featureIndex, panelIndex)         \
  {                                                                                 \
    m_FeatureTable[curPos] = featureIndex;                                  \
    m_FeatureTable[curPos+1] = panelIndex;                                                          \
    m_SpecificGUI->mMenuItem->callback((Fl_Callback*)GenericMenuItemCallback, (void*)(m_FeatureTable+curPos));\
    curPos += 2;                                                                  \
  }
  otbMenuItemCallbackMacro(mInit, otb::FeatureInfoTex::UNKNOWN, 0);
  otbMenuItemCallbackMacro(mPanTex, otb::FeatureInfoTex::TEXT_PANTEX, 4);
  otbMenuItemCallbackMacro(mSFSLen, otb::FeatureInfoTex::SFS_LEN, 1);
  otbMenuItemCallbackMacro(mSFSWid, otb::FeatureInfoTex::SFS_WID, 1);
  otbMenuItemCallbackMacro(mSFSPSI, otb::FeatureInfoTex::SFS_PSI, 1);
  otbMenuItemCallbackMacro(mSFSWme, otb::FeatureInfoTex::SFS_WME, 1);
  otbMenuItemCallbackMacro(mSFSRat, otb::FeatureInfoTex::SFS_RAT, 1);
  otbMenuItemCallbackMacro(mSFSSD, otb::FeatureInfoTex::SFS_SD, 1);
  otbMenuItemCallbackMacro(mHaralick1, otb::FeatureInfoTex::TEXT_HAR, 2);
  otbMenuItemCallbackMacro(mHaralick2, otb::FeatureInfoTex::TEXT_ADV, 3);
#undef otbMenuItemCallbackMacro

}

TextureExtractionViewGUI::~TextureExtractionViewGUI()
{
}

void
TextureExtractionViewGUI
::UpdateFeatureInfo(FeatureType feat)
{
  FeatureInfoTex        inf;
  std::ostringstream oss;
  oss << inf.GetMapInfo().find(feat)->second;
  guiFeatInfo->buffer()->remove(0, guiFeatInfo->buffer()->length());
  guiFeatInfo->insert(oss.str().c_str());
  guiFeatInfo->redraw();
}

void
TextureExtractionViewGUI
::UpdateChannels()
{
  Superclass::UpdateChannels();

  m_SpecificGUI->guiHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
  m_SpecificGUI->guiHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
  m_SpecificGUI->guiAdvHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
  m_SpecificGUI->guiAdvHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
}

void
TextureExtractionViewGUI
::InitWidgets()
{
  /* Init some specific widgets */
  m_SpecificGUI->guiHarBin->value(0);
  m_SpecificGUI->guiAdvBin->value(0);
  m_SpecificGUI->guiPanTexBin->value(0);

  Superclass::InitWidgets();
}

void
TextureExtractionViewGUI
::UpdateHarMinMax()
{
  if( m_SpecificGUI->guiHarCk->value() != 0 )
    {
      m_SpecificGUI->guiHarIm->activate();

      m_SpecificGUI->guiHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
      m_SpecificGUI->guiHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
    }
  else
    {
      m_SpecificGUI->guiHarIm->deactivate();
    }
}

void
TextureExtractionViewGUI
::UpdateAdvHarMinMax()
{
  if( m_SpecificGUI->guiAdvHarCk->value() != 0 )
    {
      m_SpecificGUI->guiAdvHarIm->activate();

      m_SpecificGUI->guiAdvHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
      m_SpecificGUI->guiAdvHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
    }
  else
    {
      m_SpecificGUI->guiAdvHarIm->deactivate();
    }
}

void TextureExtractionViewGUI::InitParameterGroupList()
{
  Superclass::InitParameterGroupList();
  m_ParameterGroupList.push_back(m_SpecificGUI->guiSFS); // panel 1
  m_ParameterGroupList.push_back(m_SpecificGUI->guiHaralick);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiAdvanced);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiPanTex);
}

void
TextureExtractionViewGUI
::InitTextureLists()
{
  m_SpecificGUI->guiHarList->add("Energy", 1);
  m_SpecificGUI->guiHarList->add("Entropy", 2);
  m_SpecificGUI->guiHarList->add("Correlation", 3);
  m_SpecificGUI->guiHarList->add("Inverse dif. moment", 4);
  m_SpecificGUI->guiHarList->add("Inertia", 5);
  m_SpecificGUI->guiHarList->add("Cluster Prominence", 6);
  m_SpecificGUI->guiHarList->add("Cluster Shade", 7);
  m_SpecificGUI->guiHarList->add("Haralick Correlation", 8);
  m_SpecificGUI->guiHarList->redraw();

  m_SpecificGUI->guiAdvList->add("Variance", 1);
  m_SpecificGUI->guiAdvList->add("Mean", 2);
  m_SpecificGUI->guiAdvList->add("Dissimilarity", 3);
  m_SpecificGUI->guiAdvList->add("Sum average", 4);
  m_SpecificGUI->guiAdvList->add("Sum variance", 5);
  m_SpecificGUI->guiAdvList->add("Sum entropy", 6);
  m_SpecificGUI->guiAdvList->add("Difference entropy", 7);
  m_SpecificGUI->guiAdvList->add("Difference variance", 8);
  m_SpecificGUI->guiAdvList->add("Information. Correlation 1", 9);
  m_SpecificGUI->guiAdvList->add("Information Correlation 2", 10);
  m_SpecificGUI->guiAdvList->redraw();

}

void
TextureExtractionViewGUI::GenericMenuItemCallback_i(Fl_Menu_* o, void* v)
{
  int *index = (int*) v;
  UpdateParameterArea(*(index+1));
  SetFeatureType(*index);
  if ((*index) == otb::FeatureInfoTex::SFS_WME)
    {
    m_SpecificGUI->guiSFSAlpha->activate();
    }
  else
    {
    m_SpecificGUI->guiSFSAlpha->deactivate();
    }
  if ((*index) == otb::FeatureInfoTex::SFS_RAT)
    {
    m_SpecificGUI->guiSFSRatMaxConsNb->activate();
    }
  else
    {
    m_SpecificGUI->guiSFSRatMaxConsNb->deactivate();
    }
}

void
TextureExtractionViewGUI::GenericMenuItemCallback(Fl_Menu_* o, void* v)
{
  ((TextureExtractionViewGUI *)(o->user_data()))->GenericMenuItemCallback_i(o, v);
}

void
TextureExtractionViewGUI::HarCkCallback_i(Fl_Check_Button* o, void* v)
{
  this->UpdateHarMinMax();
}

void
TextureExtractionViewGUI::HarCkCallback(Fl_Check_Button* o, void* v)
{
  ((TextureExtractionViewGUI *)(o->parent()->user_data()))->HarCkCallback_i(o, v);
}

void
TextureExtractionViewGUI::HarMinCallback_i(Fl_Value_Input* o, void* v)
{
  if( static_cast<double>(m_SpecificGUI->guiHarMin->value()) > static_cast<double>(m_SpecificGUI->guiHarMax->value()) )
    {
    m_SpecificGUI->guiHarMin->value(m_SpecificGUI->guiHarMax->value());
    }
}

void
TextureExtractionViewGUI::HarMinCallback(Fl_Value_Input* o, void* v)
{
  ((TextureExtractionViewGUI *)(o->parent()->parent()->user_data()))->HarMinCallback_i(o, v);
}

void
TextureExtractionViewGUI::HarMaxCallback_i(Fl_Value_Input* o, void* v)
{
  if( static_cast<double>(m_SpecificGUI->guiHarMax->value()) < static_cast<double>(m_SpecificGUI->guiHarMin->value()) )
    {
    m_SpecificGUI->guiHarMax->value(m_SpecificGUI->guiHarMin->value());
    }
}

void
TextureExtractionViewGUI::HarMaxCallback(Fl_Value_Input* o, void* v)
{
  ((TextureExtractionViewGUI *)(o->parent()->parent()->user_data()))->HarMaxCallback_i(o, v);
}

void
TextureExtractionViewGUI::AdvHarCkCallback_i(Fl_Check_Button* o, void* v)
{
  this->UpdateAdvHarMinMax();
}

void
TextureExtractionViewGUI::AdvHarCkCallback(Fl_Check_Button* o, void* v)
{
  ((TextureExtractionViewGUI *)(o->parent()->user_data()))->AdvHarCkCallback_i(o, v);
}

void
TextureExtractionViewGUI::AdvHarMinCallback_i(Fl_Value_Input* o, void* v)
{
  if( static_cast<double>(m_SpecificGUI->guiAdvHarMin->value()) > static_cast<double>(m_SpecificGUI->guiAdvHarMax->value()) )
    {
    m_SpecificGUI->guiAdvHarMin->value(m_SpecificGUI->guiAdvHarMax->value());
    }
}

void
TextureExtractionViewGUI::AdvHarMinCallback(Fl_Value_Input* o, void* v)
{
  ((TextureExtractionViewGUI *)(o->parent()->parent()->user_data()))->AdvHarMinCallback_i(o, v);
}

void
TextureExtractionViewGUI::AdvHarMaxCallback_i(Fl_Value_Input* o, void* v)
{
  if( static_cast<double>(m_SpecificGUI->guiAdvHarMax->value()) < static_cast<double>(m_SpecificGUI->guiAdvHarMin->value()) )
    {
    m_SpecificGUI->guiAdvHarMax->value(m_SpecificGUI->guiAdvHarMin->value());
    }
}

void
TextureExtractionViewGUI::AdvHarMaxCallback(Fl_Value_Input* o, void* v)
{
  ((TextureExtractionViewGUI *)(o->parent()->parent()->user_data()))->AdvHarMaxCallback_i(o, v);
}

}

#endif
