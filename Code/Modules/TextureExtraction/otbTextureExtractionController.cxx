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
#ifndef __otbTextureExtractionController_cxx
#define __otbTextureExtractionController_cxx

#include "otbTextureExtractionController.h"
#include "otbFeatureTex.h"
#include "otbMsgReporter.h"

namespace otb
{

TextureExtractionController::TextureExtractionController()
{
}

TextureExtractionController::~TextureExtractionController()
{
}

void
TextureExtractionController
::CreateSpecificFeature(FeatureBaseType featureType)
{
  try
    {
    int                 beforeSize = m_ModelTex->GetOutputFilterInformation().size();
    std::vector<double> floatParams;

    switch (featureType)
      {
      case FeatureInfoTex::TEXT_PANTEX:
        {
        SizeType rad;
        rad[0] = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiPanTexRadiusX->value());
        rad[1] = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiPanTexRadiusY->value());
        // set offset to be able to call the generic texture add filter
        OffsetType off;
        off.Fill(0);

        unsigned int bin = static_cast<unsigned int>(atoi(m_ViewTex->m_SpecificGUI->guiPanTexBin->value()));

        m_ModelTex->AddTextureFilter(featureType, rad, off, bin);
        break;
        }
      case FeatureInfoTex::TEXT_HAR:
        {
        SizeType rad;
        rad[0] = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiHarRadiusX->value());
        rad[1] = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiHarRadiusY->value());
        // set offset to be able to call the generic texture add filter
        OffsetType off;
        off[0] = static_cast<int>(m_ViewTex->m_SpecificGUI->guiHarOffsetX->value());
        off[1] = static_cast<int>(m_ViewTex->m_SpecificGUI->guiHarOffsetY->value());
        HaralickTextureVectorType harList;
        harList = this->GetHaralickTextList();

        unsigned int bin = static_cast<unsigned int>(atoi(m_ViewTex->m_SpecificGUI->guiHarBin->value()));

        if( m_ViewTex->m_SpecificGUI->guiHarCk->value() )
          {
            double minHar = static_cast<double>(m_ViewTex->m_SpecificGUI->guiHarMin->value());
            double maxHar =static_cast<double>(m_ViewTex->m_SpecificGUI->guiHarMax->value());
            m_ModelTex->AddHaralickTextureFilter(harList, rad, off, minHar, maxHar, bin);
          }
        else
          {
            m_ModelTex->AddHaralickTextureFilter(harList, rad, off, bin);
          }
 
        break;
        }
      case FeatureInfoTex::TEXT_ADV:
        {
        SizeType rad;
        rad[0] = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiAdvRadiusX->value());
        rad[1] = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiAdvRadiusY->value());
        // set offset to be able to call the generic texture add filter
        OffsetType off;
        off[0] = static_cast<int>(m_ViewTex->m_SpecificGUI->guiAdvOffsetX->value());
        off[1] = static_cast<int>(m_ViewTex->m_SpecificGUI->guiAdvOffsetY->value());
        AdvancedTextureVectorType advList;
        advList = this->GetAdvancedTextList();

        unsigned int bin = static_cast<unsigned int>(atoi(m_ViewTex->m_SpecificGUI->guiAdvBin->value()));

       if( m_ViewTex->m_SpecificGUI->guiAdvHarCk->value() )
          {
            double minHar = static_cast<double>(m_ViewTex->m_SpecificGUI->guiAdvHarMin->value());
            double maxHar =static_cast<double>(m_ViewTex->m_SpecificGUI->guiAdvHarMax->value());
            m_ModelTex->AddAdvancedTextureFilter(advList, rad, off, minHar, maxHar, bin);
          }
        else
          {
            m_ModelTex->AddAdvancedTextureFilter(advList, rad, off, bin);
          }

        break;
        }
      case FeatureInfoTex::SFS_LEN:
      case FeatureInfoTex::SFS_WID:
      case FeatureInfoTex::SFS_PSI:
      case FeatureInfoTex::SFS_WME:
      case FeatureInfoTex::SFS_RAT:
      case FeatureInfoTex::SFS_SD:
        {
        double       dSpectralThreshold = static_cast<double>(m_ViewTex->m_SpecificGUI->guiSFSSpectralThreshold->value());
        unsigned int uiSpectralThresh = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiSFSSpatialThreshold->value());
        unsigned int uiNbDirections = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiSFSNbDirections->value());
        double       dAlpha = static_cast<double>(m_ViewTex->m_SpecificGUI->guiSFSAlpha->value());
        unsigned int uiRatMaxConsNb = static_cast<unsigned int>(m_ViewTex->m_SpecificGUI->guiSFSRatMaxConsNb->value());
        m_ModelTex->AddSFSTexturesFilter(featureType,
                                      dSpectralThreshold,
                                      uiSpectralThresh,
                                      uiNbDirections,
                                      dAlpha,
                                      uiRatMaxConsNb);
        break;
        }
      default:
        {
        return;
        }
      }
    int afterSize = m_ModelTex->GetOutputFilterInformation().size();
    for (int i = beforeSize; i < afterSize; i++)
      {
      m_ViewTex->guiFeatureList->add(m_ModelTex->GetOutputFilterInformationId(i).c_str());
      m_ViewTex->guiFeatureListAction->add(m_ModelTex->GetOutputFilterInformationId(i).c_str());
      m_ViewTex->guiOutputFeatureList->add(m_ModelTex->GetOutputFilterInformationId(i).c_str());
      m_ViewTex->AddToInputOutputFeatureLink(i);
      this->ChangeFilterStatus(i);
      }

    m_ViewTex->guiFeatureList->redraw();
    m_ViewTex->guiFeatureListAction->redraw();
    m_ViewTex->guiOutputFeatureList->redraw();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

TextureExtractionController::HaralickTextureVectorType TextureExtractionController::GetHaralickTextList()
{
  HaralickTextureVectorType harList(m_ViewTex->m_SpecificGUI->guiHarList->nchecked(), HaralickTexture::UNKNOWN);
  int                       j = 1;
  int                       count = 0;

  while (j <= m_ViewTex->m_SpecificGUI->guiHarList->nitems() && count < m_ViewTex->m_SpecificGUI->guiHarList->nchecked())
    {
    if (m_ViewTex->m_SpecificGUI->guiHarList->checked(j) != 0)
      {
      harList[count] = HaralickTexture::FindTextureType(j - 1);
      count++;
      }
    j++;
    }

  return harList;
}

TextureExtractionController::AdvancedTextureVectorType TextureExtractionController::GetAdvancedTextList()
{
  AdvancedTextureVectorType advList(m_ViewTex->m_SpecificGUI->guiAdvList->nchecked(), AdvancedTexture::UNKNOWN);

  int j = 1;
  int count = 0;

  while (j <= m_ViewTex->m_SpecificGUI->guiAdvList->nitems() && count < m_ViewTex->m_SpecificGUI->guiAdvList->nchecked())
    {
    if (m_ViewTex->m_SpecificGUI->guiAdvList->checked(j) != 0)
      {
      advList[count] = AdvancedTexture::FindTextureType(j - 1);
      count++;
      }
    j++;
    }

  return advList;
}

void
TextureExtractionController
::SetView(TextureExtractionViewGUI::Pointer specificView)
  {
  m_ViewTex = specificView;
  FeatureExtractionBaseViewGUI::Pointer viewTemp(specificView);
  Superclass::SetView(viewTemp);
  }

void
TextureExtractionController
::SetModel(TextureExtractionModel::Pointer specificModel)
  {
  m_ModelTex = specificModel;
  FeatureExtractionBaseModel::Pointer modelTemp(specificModel);
  Superclass::SetModel(modelTemp);
  }

}

#endif
