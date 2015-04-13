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
#ifndef __otbEdgeExtractionController_cxx
#define __otbEdgeExtractionController_cxx

#include "otbEdgeExtractionController.h"
#include "otbFeatureEdge.h"
#include "otbMsgReporter.h"

namespace otb
{

EdgeExtractionController::EdgeExtractionController()
{
}

EdgeExtractionController::~EdgeExtractionController()
{
}

void
EdgeExtractionController
::CreateSpecificFeature(FeatureBaseType featureType)
{
  try
    {
    int                 beforeSize = m_ModelEdge->GetOutputFilterInformation().size();
    std::vector<double> params;

    switch (featureType)
      {
      case FeatureInfoEdge::HARRIS:
        {
        double sigmaD = m_ViewEdge->m_SpecificGUI->guiHarrisSigmaD->value();
        double sigmaI = m_ViewEdge->m_SpecificGUI->guiHarrisSigmaI->value();
        double alpha = m_ViewEdge->m_SpecificGUI->guiHarrisAlpha->value();
        m_ModelEdge->AddHarrisFilter(sigmaD, sigmaI, alpha);
        break;
        }
      case FeatureInfoEdge::VARIANCE:
        {
        int radiusX = static_cast<int>(m_ViewEdge->m_SpecificGUI->guiRadiusX->value());
        int radiusY = static_cast<int>(m_ViewEdge->m_SpecificGUI->guiRadiusY->value());
        m_ModelEdge->AddVarianceFilter(radiusX, radiusY);
        break;
        }
      case FeatureInfoEdge::GRADIENT:
        {
        double sigma = m_ViewEdge->m_SpecificGUI->guiGradSigma->value();
        m_ModelEdge->AddGradientFilter(sigma);
        break;
        }
      case FeatureInfoEdge::EDGE_CANNY:
        {
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEdgeCannyRadius->value()));
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEdgeCannyMaxThresh->value()));
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEdgeCannyMinThresh->value()));
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEdgeCannyVariance->value()));
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEdgeCannyMaxError->value()));
        m_ModelEdge->AddEdgeDensityFilter(featureType, params);
        break;
        }
      case FeatureInfoEdge::EDGE_SOBEL:
        {
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEdgeSobelRadiusX->value()));
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEdgeSobelRadiusY->value()));
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEgdeSobelLowThresh->value()));
        params.push_back(static_cast<double>(m_ViewEdge->m_SpecificGUI->guiEgdeSobelUpThresh->value()));
        m_ModelEdge->AddEdgeDensityFilter(featureType, params);
        break;
        }
      case FeatureInfoEdge::TOUZI:
        {
        unsigned int radius = static_cast<unsigned int>(m_ViewEdge->m_SpecificGUI->guiTouziRadius->value());
        m_ModelEdge->AddTouziFilter(radius);
        break;
        }
      default:
        {
        return;
        }
      }
    int afterSize = m_ModelEdge->GetOutputFilterInformation().size();
    for (int i = beforeSize; i < afterSize; i++)
      {
      m_ViewEdge->guiFeatureList->add(m_ModelEdge->GetOutputFilterInformationId(i).c_str());
      m_ViewEdge->guiFeatureListAction->add(m_ModelEdge->GetOutputFilterInformationId(i).c_str());
      m_ViewEdge->guiOutputFeatureList->add(m_ModelEdge->GetOutputFilterInformationId(i).c_str());
      m_ViewEdge->AddToInputOutputFeatureLink(i);
      this->ChangeFilterStatus(i);
      }

    m_ViewEdge->guiFeatureList->redraw();
    m_ViewEdge->guiFeatureListAction->redraw();
    m_ViewEdge->guiOutputFeatureList->redraw();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
EdgeExtractionController
::SetView(EdgeExtractionViewGUI::Pointer specificView)
  {
  m_ViewEdge = specificView;
  FeatureExtractionBaseViewGUI::Pointer viewTemp(specificView);
  Superclass::SetView(viewTemp);
  }

void
EdgeExtractionController
::SetModel(EdgeExtractionModel::Pointer specificModel)
  {
  m_ModelEdge = specificModel;
  FeatureExtractionBaseModel::Pointer modelTemp(specificModel);
  Superclass::SetModel(modelTemp);
  }

}

#endif
