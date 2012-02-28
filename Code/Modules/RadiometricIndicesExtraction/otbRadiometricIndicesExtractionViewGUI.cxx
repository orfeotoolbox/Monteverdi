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
#ifndef __otbRadiometricIndicesExtractionViewGUI_cxx
#define __otbRadiometricIndicesExtractionViewGUI_cxx

#include "otbRadiometricIndicesExtractionViewGUI.h"
#include "otbFeatureRI.h"

namespace otb
{

RadiometricIndicesExtractionViewGUI::RadiometricIndicesExtractionViewGUI()
{
  // Create parameters for radiometric indices
  m_SpecificGUI = new RadiometricIndicesExtractionViewGroup();
  m_SpecificGUI->CreateParameters();
  
  // Plug the specific menu items to the main GUI
  guiFeatureChoice->menu(m_SpecificGUI->fakeChoice->menu());
  guiFeatureChoice->user_data((void*)(this));
  
  // Plug the specific parameter widgets in the main window
  guiParameter->add(m_SpecificGUI->guiRAndNIR); // panel 1
  guiParameter->add(m_SpecificGUI->guiPVI);
  guiParameter->add(m_SpecificGUI->guiSAVI);
  guiParameter->add(m_SpecificGUI->guiMSAVI);
  guiParameter->add(m_SpecificGUI->guiTSAVI);
  guiParameter->add(m_SpecificGUI->guiWDVI);
  guiParameter->add(m_SpecificGUI->guiARVI);
  guiParameter->add(m_SpecificGUI->guiEVI); // 8
  guiParameter->add(m_SpecificGUI->guiTSARVI);
  guiParameter->add(m_SpecificGUI->guiAVI);
  guiParameter->add(m_SpecificGUI->guiSoil);
  guiParameter->add(m_SpecificGUI->guiIB2);
  guiParameter->add(m_SpecificGUI->guiNDBI);
  guiParameter->add(m_SpecificGUI->guiISU); // 14
  guiParameter->add(m_SpecificGUI->guiSRWI);
  guiParameter->add(m_SpecificGUI->guiNDWI);
  guiParameter->add(m_SpecificGUI->guiNDWI2);
  guiParameter->add(m_SpecificGUI->guiMNDWI);
  guiParameter->add(m_SpecificGUI->guiNDPI);
  guiParameter->add(m_SpecificGUI->guiNDTI); // 20
  guiParameter->add(m_SpecificGUI->guiSpectAngle);

  this->InitParameterGroupList();
  
  // Rename window title
  SetWindowTitle(otbGetTextMacro("Radiometric indices extraction"));
  
  // Set up callbacks on menu items
  int curPos = 0;
#define otbMenuItemCallbackMacro(mMenuItem, featureIndex, panelIndex)         \
  {                                                                                 \
    m_FeatureTable[curPos] = featureIndex;                                  \
    m_FeatureTable[curPos+1] = panelIndex;                                                          \
    m_SpecificGUI->mMenuItem->callback((Fl_Callback*)GenericMenuItemCallback, (void*)(m_FeatureTable+curPos));\
    curPos += 2;                                                                  \
  }
  otbMenuItemCallbackMacro(mInit, otb::FeatureInfoRI::UNKNOWN, 0);
  otbMenuItemCallbackMacro(mNDVI, otb::FeatureInfoRI::NDVI, 1);
  otbMenuItemCallbackMacro(mRVI, otb::FeatureInfoRI::RVI, 1);
  otbMenuItemCallbackMacro(mPVI, otb::FeatureInfoRI::PVI, 2);
  otbMenuItemCallbackMacro(mSAVI, otb::FeatureInfoRI::SAVI, 3);
  otbMenuItemCallbackMacro(mMSAVI, otb::FeatureInfoRI::MSAVI, 4);
  otbMenuItemCallbackMacro(mMSAVI2, otb::FeatureInfoRI::MSAVI2, 1);
  otbMenuItemCallbackMacro(mTSAVI, otb::FeatureInfoRI::TSAVI, 5);
  otbMenuItemCallbackMacro(mGEMI, otb::FeatureInfoRI::GEMI, 1);
  otbMenuItemCallbackMacro(mWDVI, otb::FeatureInfoRI::WDVI, 6);
  otbMenuItemCallbackMacro(mIPVI, otb::FeatureInfoRI::IPVI, 1);
  otbMenuItemCallbackMacro(mTNDVI, otb::FeatureInfoRI::TNDVI, 1);
  otbMenuItemCallbackMacro(mARVI, otb::FeatureInfoRI::ARVI, 7);
  otbMenuItemCallbackMacro(mEVI, otb::FeatureInfoRI::EVI, 8);
  otbMenuItemCallbackMacro(mTSARVI, otb::FeatureInfoRI::TSARVI, 9);
  otbMenuItemCallbackMacro(mAVI, otb::FeatureInfoRI::AVI, 10);
  otbMenuItemCallbackMacro(mRI, otb::FeatureInfoRI::IR, 11);
  otbMenuItemCallbackMacro(mCI, otb::FeatureInfoRI::IC, 11);
  otbMenuItemCallbackMacro(mBI, otb::FeatureInfoRI::IB, 11);
  otbMenuItemCallbackMacro(mBI2, otb::FeatureInfoRI::IB2, 12);
  otbMenuItemCallbackMacro(mNDBI, otb::FeatureInfoRI::NDBI, 13);
  otbMenuItemCallbackMacro(mISU, otb::FeatureInfoRI::ISU, 14);
  otbMenuItemCallbackMacro(mSRWI, otb::FeatureInfoRI::SRWI, 15);
  otbMenuItemCallbackMacro(mNDWI, otb::FeatureInfoRI::NDWI, 16);
  otbMenuItemCallbackMacro(mNDWI2, otb::FeatureInfoRI::NDWI2, 17);
  otbMenuItemCallbackMacro(mMNDWI, otb::FeatureInfoRI::MNDWI, 18);
  otbMenuItemCallbackMacro(mNDPI, otb::FeatureInfoRI::NDPI, 19);
  otbMenuItemCallbackMacro(mNDTI, otb::FeatureInfoRI::NDTI, 20);
  otbMenuItemCallbackMacro(mSpectAngle, otb::FeatureInfoRI::SPECTRALANGLE, 21);
#undef otbMenuItemCallbackMacro
  
}

RadiometricIndicesExtractionViewGUI::~RadiometricIndicesExtractionViewGUI()
{
}

void
RadiometricIndicesExtractionViewGUI
::UpdateFeatureInfo(FeatureType feat)
{
  FeatureInfoRI        inf;
  itk::OStringStream oss;
  oss << inf.GetMapInfo().find(feat)->second;
  guiFeatInfo->buffer()->remove(0, guiFeatInfo->buffer()->length());
  guiFeatInfo->insert(oss.str().c_str());
  guiFeatInfo->redraw();
}

void
RadiometricIndicesExtractionViewGUI
::UpdateParameterArea(unsigned int groupId)
{
  Superclass::UpdateParameterArea(groupId);
  if (m_ParameterGroupList[groupId] == m_SpecificGUI->guiSpectAngle)
    {
    if (GetModel()->GetInputImage()->GetNumberOfComponentsPerPixel() < 2)
      {
      m_ParameterGroupList[groupId]->deactivate();
      }
    else
      {
      m_ParameterGroupList[groupId]->activate();
      }
    }
}

void
RadiometricIndicesExtractionViewGUI
::UpdateSelectedPixelGUI(const IndexType& index)
{
  itk::OStringStream oss;
  oss << "(" << index[0] << " , " << index[1] << ")";
  m_SpecificGUI->guiSpectAnglePixelCoordinates->value(oss.str().c_str());
  m_SpecificGUI->guiSpectAnglePixelCoordinates->redraw();
  oss.str("");
  oss << "[";

  unsigned int i = 0;
  for (i = 0; i < GetSelectedPixel().Size() - 1; i++)
    {
    oss << (GetSelectedPixel())[i] << ", ";
    }

  oss << (GetSelectedPixel())[i] << "]";
  m_SpecificGUI->guiSpectAnglePixelValue->value(oss.str().c_str());
  m_SpecificGUI->guiSpectAnglePixelValue->redraw();
}

void RadiometricIndicesExtractionViewGUI::InitParameterGroupList()
{
  Superclass::InitParameterGroupList();
  m_ParameterGroupList.push_back(m_SpecificGUI->guiRAndNIR); // panel 1
  m_ParameterGroupList.push_back(m_SpecificGUI->guiPVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiSAVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiMSAVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiTSAVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiWDVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiARVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiEVI); // 8
  m_ParameterGroupList.push_back(m_SpecificGUI->guiTSARVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiAVI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiSoil);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiIB2);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiNDBI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiISU); // 14
  m_ParameterGroupList.push_back(m_SpecificGUI->guiSRWI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiNDWI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiNDWI2);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiMNDWI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiNDPI);
  m_ParameterGroupList.push_back(m_SpecificGUI->guiNDTI); // 20
  m_ParameterGroupList.push_back(m_SpecificGUI->guiSpectAngle);
}

void
RadiometricIndicesExtractionViewGUI::GenericMenuItemCallback_i(Fl_Menu_* o, void* v)
{
  int *index = (int*) v;
  this->UpdateParameterArea(*(index+1));
  SetFeatureType(*index);
}

void
RadiometricIndicesExtractionViewGUI::GenericMenuItemCallback(Fl_Menu_* o, void* v)
{
  ((RadiometricIndicesExtractionViewGUI *)(o->user_data()))->GenericMenuItemCallback_i(o, v);
}

}

#endif
