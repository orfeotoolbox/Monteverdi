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
#ifndef __otbRadiometricIndicesExtractionController_cxx
#define __otbRadiometricIndicesExtractionController_cxx

#include "otbRadiometricIndicesExtractionController.h"
#include "otbFeatureRI.h"
#include "otbMsgReporter.h"

namespace otb
{

RadiometricIndicesExtractionController::RadiometricIndicesExtractionController()
{
}

RadiometricIndicesExtractionController::~RadiometricIndicesExtractionController()
{
}

void
RadiometricIndicesExtractionController
::CreateSpecificFeature(FeatureBaseType featureType)
{
  try
    {
    int                 beforeSize = m_ModelRI->GetOutputFilterInformation().size();
    std::vector<double> floatParams;

    switch (featureType)
      {
      case FeatureInfoRI::NDVI:
      case FeatureInfoRI::RVI:
      case FeatureInfoRI::GEMI:
      case FeatureInfoRI::IPVI:
      case FeatureInfoRI::TNDVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiRAndNIRR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiRAndNIRNIR->value());
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::PVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiPVIR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiPVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiPVIa->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiPVIb->value()));
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::SAVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiSAVIR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiSAVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiSAVIL->value()));
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::MSAVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiMSAVIR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiMSAVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiMSAVIs->value()));
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::MSAVI2:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiRAndNIRR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiRAndNIRNIR->value());
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::TSAVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiTSAVIR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiTSAVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiTSAVIs->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiTSAVIa->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiTSAVIx->value()));
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::WDVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiWDVIR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiWDVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiWDVIs->value()));
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::ARVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiARVIR->value());
        int BId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiARVIB->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiARVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiARVIgamma->value()));
        m_ModelRI->AddRAndBAndNIRFilter(RId, BId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::EVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiEVIR->value());
        int BId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiEVIB->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiEVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiEVIG->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiEVIL->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiEVIC1->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiEVIC2->value()));
        m_ModelRI->AddRAndBAndNIRFilter(RId, BId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::TSARVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiTSARVIR->value());
        int BId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiTSARVIB->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiTSARVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiTSARVIa_rb->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiTSARVIb_rb->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiTSARVIx->value()));
        m_ModelRI->AddRAndBAndNIRFilter(RId, BId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::AVI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiAVIR->value());
        int GId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiAVIG->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiAVINIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiAVIlambda1->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiAVIlambda2->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiAVIlambda3->value()));
        m_ModelRI->AddRAndGAndNIRFilter(RId, GId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::IR:
      case FeatureInfoRI::IC:
      case FeatureInfoRI::IB:
        {
        int GId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiSoilG->value());
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiSoilR->value());
        m_ModelRI->Add2ChannelsFilter(GId, RId, featureType);
        break;
        }
      case FeatureInfoRI::IB2:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiIB2R->value());
        int GId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiIB2G->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiIB2NIR->value());
        m_ModelRI->AddRAndGAndNIRFilter(RId, GId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::NDBI:
        {
        int TM4Id = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDBITM4->value());
        int TM5Id = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDBITM5->value());
        m_ModelRI->Add2ChannelsFilter(TM4Id, TM5Id, featureType);
        break;
        }
      case FeatureInfoRI::ISU:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiISUR->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiISUNIR->value());
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiISUa->value()));
        floatParams.push_back(static_cast<double>(m_ViewRI->m_SpecificGUI->guiISUb->value()));
        m_ModelRI->AddRAndNIRFilter(RId, NIRId, featureType, floatParams);
        break;
        }
      case FeatureInfoRI::SRWI:
        {
        int R860Id = static_cast<int>(m_ViewRI->m_SpecificGUI->guiSRWIR860->value());
        int R1240Id = static_cast<int>(m_ViewRI->m_SpecificGUI->guiSRWIR1240->value());
        m_ModelRI->Add2ChannelsFilter(R860Id, R1240Id, featureType);
        break;
        }
      case FeatureInfoRI::NDWI:
        {
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDWINIR->value());
        int MIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDWIMIR->value());
        m_ModelRI->Add2ChannelsFilter(NIRId, MIRId, featureType);
        break;
        }
      case FeatureInfoRI::NDWI2:
        {
        int GId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDWI2G->value());
        int NIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDWI2NIR->value());
        m_ModelRI->Add2ChannelsFilter(GId, NIRId, featureType);
        break;
        }
      case FeatureInfoRI::MNDWI:
        {
        int GId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiMNDWIG->value());
        int MIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiMNDWIMIR->value());
        m_ModelRI->Add2ChannelsFilter(GId, MIRId, featureType);
        break;
        }
      case FeatureInfoRI::NDPI:
        {
        int MIRId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDPIMIR->value());
        int GId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDPIG->value());
        m_ModelRI->Add2ChannelsFilter(MIRId, GId, featureType);
        break;
        }
      case FeatureInfoRI::NDTI:
        {
        int RId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDTIR->value());
        int GId = static_cast<int>(m_ViewRI->m_SpecificGUI->guiNDTIG->value());
        m_ModelRI->Add2ChannelsFilter(RId, GId, featureType);
        break;
        }
      case FeatureInfoRI::SPECTRALANGLE:
        {
        if (m_ViewRI->GetSelectedPixel().GetSize() != m_ModelRI->GetNumberOfChannels())
          {
          MsgReporter::GetInstance()->SendError("Reference pixel is not set!");
          return;
          }
        if (m_ModelRI->GetInputImage()->GetNumberOfComponentsPerPixel() > 1)
          {
          m_ModelRI->AddSpectralAngleFilter(m_ViewRI->GetSelectedPixel());
          }
        else
          {
          MsgReporter::GetInstance()->SendError("Spectral Angle invalid with mono channel images");
          return;
          }
        break;
        }
      default:
        {
        return;
        }
      }
    int afterSize = m_ModelRI->GetOutputFilterInformation().size();
    for (int i = beforeSize; i < afterSize; i++)
      {
      m_ViewRI->guiFeatureList->add(m_ModelRI->GetOutputFilterInformationId(i).c_str());
      m_ViewRI->guiFeatureListAction->add(m_ModelRI->GetOutputFilterInformationId(i).c_str());
      m_ViewRI->guiOutputFeatureList->add(m_ModelRI->GetOutputFilterInformationId(i).c_str());
      m_ViewRI->AddToInputOutputFeatureLink(i);
      this->ChangeFilterStatus(i);
      }

    m_ViewRI->guiFeatureList->redraw();
    m_ViewRI->guiFeatureListAction->redraw();
    m_ViewRI->guiOutputFeatureList->redraw();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
RadiometricIndicesExtractionController
::SetView(RadiometricIndicesExtractionViewGUI::Pointer specificView)
  {
  m_ViewRI = specificView;
  FeatureExtractionBaseViewGUI::Pointer viewTemp(specificView);
  Superclass::SetView(viewTemp);
  }

void
RadiometricIndicesExtractionController
::SetModel(RadiometricIndicesExtractionModel::Pointer specificModel)
  {
  m_ModelRI = specificModel;
  FeatureExtractionBaseModel::Pointer modelTemp(specificModel);
  Superclass::SetModel(modelTemp);
  }

}

#endif
