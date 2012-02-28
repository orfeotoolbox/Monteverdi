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
#ifndef __otbMorphologyExtractionController_cxx
#define __otbMorphologyExtractionController_cxx

#include "otbMorphologyExtractionController.h"
#include "otbFeatureMorpho.h"
#include "otbMsgReporter.h"

namespace otb
{

MorphologyExtractionController::MorphologyExtractionController()
{
}

MorphologyExtractionController::~MorphologyExtractionController()
{
}

void
MorphologyExtractionController
::CreateSpecificFeature(FeatureBaseType featureType)
{
  try
    {
    int                 beforeSize = m_ModelMorpho->GetOutputFilterInformation().size();
    std::vector<double> floatParams;

    switch (featureType)
      {
      case FeatureInfoMorpho::MORPHOLOGICALOPENING:
        {
        int size = static_cast<int>(m_ViewMorpho->m_SpecificGUI->guiMorphoProfileSize->value());
        int value = static_cast<int>(m_ViewMorpho->m_SpecificGUI->guiMorphoInitVal->value());
        int step = static_cast<int>(m_ViewMorpho->m_SpecificGUI->guiMorphoStep->value());
        m_ModelMorpho->AddMorphoOpeningProfileFilter(size, value, step);
        break;
        }
      case FeatureInfoMorpho::MORPHOLOGICALCLOSING:
        {
        int size = static_cast<int>(m_ViewMorpho->m_SpecificGUI->guiMorphoProfileSize->value());
        int value = static_cast<int>(m_ViewMorpho->m_SpecificGUI->guiMorphoInitVal->value());
        int step = static_cast<int>(m_ViewMorpho->m_SpecificGUI->guiMorphoStep->value());
        m_ModelMorpho->AddMorphoClosingProfileFilter(size, value, step);
        break;
        }
      
      default:
        {
        return;
        }
      }
    int afterSize = m_ModelMorpho->GetOutputFilterInformation().size();
    for (int i = beforeSize; i < afterSize; i++)
      {
      m_ViewMorpho->guiFeatureList->add(m_ModelMorpho->GetOutputFilterInformationId(i).c_str());
      m_ViewMorpho->guiFeatureListAction->add(m_ModelMorpho->GetOutputFilterInformationId(i).c_str());
      m_ViewMorpho->guiOutputFeatureList->add(m_ModelMorpho->GetOutputFilterInformationId(i).c_str());
      m_ViewMorpho->AddToInputOutputFeatureLink(i);
      this->ChangeFilterStatus(i);
      }

    m_ViewMorpho->guiFeatureList->redraw();
    m_ViewMorpho->guiFeatureListAction->redraw();
    m_ViewMorpho->guiOutputFeatureList->redraw();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
MorphologyExtractionController
::SetView(MorphologyExtractionViewGUI::Pointer specificView)
  {
  m_ViewMorpho = specificView;
  FeatureExtractionBaseViewGUI::Pointer viewTemp(specificView);
  Superclass::SetView(viewTemp);
  }

void
MorphologyExtractionController
::SetModel(MorphologyExtractionModel::Pointer specificModel)
  {
  m_ModelMorpho = specificModel;
  FeatureExtractionBaseModel::Pointer modelTemp(specificModel);
  Superclass::SetModel(modelTemp);
  }

}

#endif
