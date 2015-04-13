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
#ifndef __otbSmoothingController_cxx
#define __otbSmoothingController_cxx

#include "otbSmoothingController.h"
#include "otbFeatureSmooth.h"
#include "otbMsgReporter.h"

namespace otb
{

SmoothingController::SmoothingController()
{
}

SmoothingController::~SmoothingController()
{
}

void
SmoothingController
::ClearFeatures()
{
  Superclass::ClearFeatures();
  
  try
    {
    m_ModelSmooth->InitMeanShiftLists();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

void
SmoothingController
::CreateSpecificFeature(FeatureBaseType featureType)
{
  try
    {
    int                 beforeSize = m_ModelSmooth->GetOutputFilterInformation().size();
    std::vector<double> floatParams;

    switch (featureType)
      {
      case FeatureInfoSmooth::MEAN:
        {
        int radiusX = static_cast<int>(m_ViewSmooth->m_SpecificGUI->guiRadiusX->value());
        int radiusY = static_cast<int>(m_ViewSmooth->m_SpecificGUI->guiRadiusY->value());
        m_ModelSmooth->AddMeanFilter(radiusX, radiusY);
        break;
        }
      case FeatureInfoSmooth::MS_SMOOTH:
      case FeatureInfoSmooth::MS_CLUSTERED:
        {
        unsigned int spatial = static_cast<unsigned int>(m_ViewSmooth->m_SpecificGUI->guiMSSpatial->value());
        double       range = static_cast<double>(m_ViewSmooth->m_SpecificGUI->guiMSRange->value());
        unsigned int minSize = static_cast<unsigned int>(m_ViewSmooth->m_SpecificGUI->guiMSSize->value());
        m_ModelSmooth->AddMeanShiftFilter(featureType, spatial, range, minSize);
        break;
        }
      case FeatureInfoSmooth::ORIGINAL:
        {
        m_ModelSmooth->AddOriginalData();
        break;
        }
      default:
        {
        return;
        }
      }
    int afterSize = m_ModelSmooth->GetOutputFilterInformation().size();
    for (int i = beforeSize; i < afterSize; i++)
      {
      m_ViewSmooth->guiFeatureList->add(m_ModelSmooth->GetOutputFilterInformationId(i).c_str());
      m_ViewSmooth->guiFeatureListAction->add(m_ModelSmooth->GetOutputFilterInformationId(i).c_str());
      m_ViewSmooth->guiOutputFeatureList->add(m_ModelSmooth->GetOutputFilterInformationId(i).c_str());
      m_ViewSmooth->AddToInputOutputFeatureLink(i);
      this->ChangeFilterStatus(i);
      }

    m_ViewSmooth->guiFeatureList->redraw();
    m_ViewSmooth->guiFeatureListAction->redraw();
    m_ViewSmooth->guiOutputFeatureList->redraw();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
SmoothingController
::SetView(SmoothingViewGUI::Pointer specificView)
  {
  m_ViewSmooth = specificView;
  FeatureExtractionBaseViewGUI::Pointer viewTemp(specificView);
  Superclass::SetView(viewTemp);
  }

void
SmoothingController
::SetModel(SmoothingModel::Pointer specificModel)
  {
  m_ModelSmooth = specificModel;
  FeatureExtractionBaseModel::Pointer modelTemp(specificModel);
  Superclass::SetModel(modelTemp);
  }

}

#endif
