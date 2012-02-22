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

#include "otbFeatureExtractionBaseViewGUI.h"
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Buffer.H>
#include "otbMacro.h"
#include "itkExceptionObject.h"
#include "otbFeatureBase.h"

namespace otb
{

FeatureExtractionBaseViewGUI
::FeatureExtractionBaseViewGUI()
{
  // Initialisation
  m_FileNameList =  StringVectorType(4, "");
  m_ViewerNameList =  StringVectorType(4, "");
  m_TemplateViewerName = "";
  m_DisplayedLabelList =  StringVectorType(4, "+ ");
  m_UndisplayedLabelList =  StringVectorType(4, "+ ");
  m_DisplayStatusList = std::vector<bool>(4, true);
  m_FeatureType =  0;//FeatureInfo::UNKNOWN;

  this->CreateGUI();

  Fl_Text_Buffer * buffer = new Fl_Text_Buffer();
  guiFeatInfo->buffer(buffer);

  m_SelectedPixel.SetSize(0);
  m_SelectedPixel.Fill(0);
  m_FeatureExtractPreviewParentBrowser = -1;
  //this->InitParameterGroupList();
  //this->InitTextureLists();

  // Instanstiate the views
  m_VisuView = VisuViewType::New();
  m_ResultVisuView = VisuViewType::New();
}

void
FeatureExtractionBaseViewGUI
::InitVisu()
{
  // Set the model
  m_VisuView->SetModel(GetModel()->GetVisuModel());

  // Set the model
  m_ResultVisuView->SetModel(GetModel()->GetResultVisuModel());
}

FeatureExtractionBaseViewGUI
::~FeatureExtractionBaseViewGUI()
{

}

void
FeatureExtractionBaseViewGUI
::InitWidgets()
{
  assert(m_FeatureExtractionBaseController.IsNotNull() && "The controller is not created");

//  this->UpdateFeatureInfo(FeatureInfo::UNKNOWN);
  this->UpdateFeatureInfo(0);
//   guiHarBin->value(0);
//   guiAdvBin->value(0);
//   guiPanTexBin->value(0);

  pBar->minimum(0);
  pBar->maximum(1);
  pBar->hide();

  this->Show();
}

void
FeatureExtractionBaseViewGUI
::InitParameterGroupList()
{
  m_ParameterGroupList.clear();
  m_ParameterGroupList.push_back(guiNothing);
//   m_ParameterGroupList.push_back(guiTouzi);
//   m_ParameterGroupList.push_back(guiHarris);
//   m_ParameterGroupList.push_back(guiSpectAngle);
//   m_ParameterGroupList.push_back(guiRadius);
//   m_ParameterGroupList.push_back(guiGrad);
//   m_ParameterGroupList.push_back(guiMorpho);
//   m_ParameterGroupList.push_back(guiRAndNIR); // panel 7
//   m_ParameterGroupList.push_back(guiPVI);
//   m_ParameterGroupList.push_back(guiSAVI);
//   m_ParameterGroupList.push_back(guiMSAVI);
//   m_ParameterGroupList.push_back(guiTSAVI);
//   m_ParameterGroupList.push_back(guiWDVI);
//   m_ParameterGroupList.push_back(guiARVI);
//   m_ParameterGroupList.push_back(guiEVI); // 14
//   m_ParameterGroupList.push_back(guiTSARVI);
//   m_ParameterGroupList.push_back(guiAVI);
//   m_ParameterGroupList.push_back(guiSoil);
//   m_ParameterGroupList.push_back(guiIB2);
//   m_ParameterGroupList.push_back(guiNDBI);
//   m_ParameterGroupList.push_back(guiISU); // 20
//   m_ParameterGroupList.push_back(guiSRWI);
//   m_ParameterGroupList.push_back(guiNDWI);
//   m_ParameterGroupList.push_back(guiNDWI2);
//   m_ParameterGroupList.push_back(guiMNDWI);
//   m_ParameterGroupList.push_back(guiNDPI);
//   m_ParameterGroupList.push_back(guiNDTI); // 26
//   m_ParameterGroupList.push_back(guiSFS);
//   m_ParameterGroupList.push_back(guiEdgeSobel);
//   m_ParameterGroupList.push_back(guiMS);
//   m_ParameterGroupList.push_back(guiHaralick);
//   m_ParameterGroupList.push_back(guiAdvanced);
//   m_ParameterGroupList.push_back(guiPanTex);
  // for original data, uses guiNothing, ie.m_ParameterGroupList[0]

}

void
FeatureExtractionBaseViewGUI
::InitTextureLists()
{
//   guiHarList->add("Energy", 1);
//   guiHarList->add("Entropy", 2);
//   guiHarList->add("Correlation", 3);
//   guiHarList->add("Inverse dif. moment", 4);
//   guiHarList->add("Inertia", 5);
//   guiHarList->add("Cluster Prominence", 6);
//   guiHarList->add("Cluster Shade", 7);
//   guiHarList->add("Haralick Correlation", 8);
//   guiHarList->redraw();
// 
//   guiAdvList->add("Variance", 1);
//   guiAdvList->add("Mean", 2);
//   guiAdvList->add("Sum average", 3);
//   guiAdvList->add("Sum variance", 4);
//   guiAdvList->add("Sum entropy", 5);
//   guiAdvList->add("Difference entropy", 6);
//   guiAdvList->add("Difference variance", 7);
//   guiAdvList->add("Information. Correlation 1", 8);
//   guiAdvList->add("Information Correlation 2", 9);
//   guiAdvList->redraw();

}

void
FeatureExtractionBaseViewGUI
::UpdateParameterArea(unsigned int groupId)
{
  for (unsigned int i = 0; i < m_ParameterGroupList.size(); i++)
    m_ParameterGroupList[i]->hide();

  m_ParameterGroupList[groupId]->show();
//   if (m_ParameterGroupList[groupId] == guiSpectAngle)
//     {
//     if (GetModel()->GetInputImage()->GetNumberOfComponentsPerPixel() < 2)
//       {
//       m_ParameterGroupList[groupId]->deactivate();
//       }
//     else
//       {
//       m_ParameterGroupList[groupId]->activate();
//       }
//     }
}

void
FeatureExtractionBaseViewGUI
::UpdateFeaturePreview()
{
  if (guiFeatureListAction->value() > 0)
    {
    //GetModel()->GetSingleOutput(guiFeatureListAction->value()-1);
    m_FeatureExtractionBaseController->UpdateFeaturePreview(guiFeatureListAction->value() - 1);
    }
}

void
FeatureExtractionBaseViewGUI
::UpdateFeaturePreviewFromOutputList()
{
  unsigned int OutputListNb = guiOutputFeatureList->value();
  if (OutputListNb > 0)
    {
    if (static_cast<unsigned int>(OutputListNb - 1) < GetModel()->GetOutputListOrder().size())
      {
      //GetModel()->GetSingleOutput( GetModel()->GetOutputListOrder()[OutputListNb-1]);
      m_FeatureExtractionBaseController->UpdateFeaturePreview(GetModel()->GetOutputListOrder()[OutputListNb - 1]);
      }
    }
}

void
FeatureExtractionBaseViewGUI
::SetFeatureType(FeatureType i)
{
  m_FeatureType = i;
  this->UpdateFeatureInfo(i);
}

void
FeatureExtractionBaseViewGUI
::Notify(const std::string& event)
{
  if (GetModel()->GetHasInput())
    {
    if (event != "Cancel")
      {
      this->InitWidgets();
      this->UpdateChannelSelection();
      if (event != "SetInputImage")
        {
        this->UpdateChannels();
//         guiHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
//         guiHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
//         guiAdvHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
//         guiAdvHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
        }
      this->UpdateInformation();
      }
    }
}

void
FeatureExtractionBaseViewGUI
::UpdateChannelSelection()
{
  guiChannelSelection->clear();
  itk::OStringStream oss;
  int                count = 1;
  for (unsigned int i = 0; i < GetModel()->GetInputImage()->GetNumberOfComponentsPerPixel(); i++)
    {
    oss.str("");
    oss << "Channel " << i + 1;
    guiChannelSelection->add(oss.str().c_str(), count);
    count++;
    }
  if (GetModel()->GetNumberOfChannels() > 1)
    {
    guiChannelSelection->add("Intensity", count);
    }
  // Set all check box checked
  guiChannelSelection->check_all();
  guiChannelSelection->redraw();
}

void
FeatureExtractionBaseViewGUI
::UpdateInformation()
{
  std::string        imName = GetModel()->GetInputFileName();
  itk::OStringStream oss;
  oss.str("");
  oss << m_WindowTitle;
  oss << ": " << imName.substr(imName.find_last_of("/") + 1, imName.size());
  oss << " (" << GetModel()->GetInputImage()->GetNumberOfComponentsPerPixel();
  if (GetModel()->GetInputImage()->GetNumberOfComponentsPerPixel() != 1) oss << " bands , ";
  else oss << " band , ";

  oss << GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize() << ")";
  guiMainWindow->label(oss.str().c_str());
}

void
FeatureExtractionBaseViewGUI
::AddOutputChannel()
{
  if (guiFeatureList->size() != 0 && guiFeatureList->value() != 0)
    {
    m_InputOutputFeatureLink.push_back(guiFeatureList->value() - 1);
    m_FeatureExtractionBaseController->ChangeFilterStatus(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size() - 1]);
    guiOutputFeatureList->add(guiFeatureList->text(guiFeatureList->value()));
    m_FeatureExtractionBaseController->AddToOutputListOrder(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size() - 1]);
    guiOutputFeatureList->redraw();
    }
}

void
FeatureExtractionBaseViewGUI
::RemoveOutputChannel()
{
  if (guiOutputFeatureList->size() != 0 && guiOutputFeatureList->value() != 0)
    {
    m_FeatureExtractionBaseController->ChangeFilterStatus(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size() - 1]);
    m_FeatureExtractionBaseController->RemoveFromOutputListOrder(guiOutputFeatureList->value());
    m_InputOutputFeatureLink.erase(m_InputOutputFeatureLink.begin() + guiOutputFeatureList->value() - 1);
    guiOutputFeatureList->remove(guiOutputFeatureList->value());
    guiOutputFeatureList->redraw();
    }
}

void
FeatureExtractionBaseViewGUI
::UpOutputChannel()
{
  if (guiOutputFeatureList->size() != 0 && guiOutputFeatureList->value() != 0)
    {
    int id = guiOutputFeatureList->value();
    // exchnage the output list manager
    m_FeatureExtractionBaseController->ExchangeOutputListOrder(1);
    // exchnage the list display order
    if (id != 1)
      {
      guiOutputFeatureList->swap(id, id - 1);
      guiOutputFeatureList->value(id - 1);
      }
    else
      {
      guiOutputFeatureList->swap(id, guiOutputFeatureList->size());
      guiOutputFeatureList->value(guiOutputFeatureList->size());
      }
    }
}

void
FeatureExtractionBaseViewGUI
::DownOutputChannel()
{
  if (guiOutputFeatureList->size() != 0 && guiOutputFeatureList->value() != 0)
    {
    int id = guiOutputFeatureList->value();
    // exchnage the output list manager
    m_FeatureExtractionBaseController->ExchangeOutputListOrder(-1);
    // exchnage the list display order
    if (id != guiOutputFeatureList->size())
      {
      guiOutputFeatureList->swap(id, id + 1);
      guiOutputFeatureList->value(id + 1);
      }
    else
      {
      guiOutputFeatureList->swap(id, 1);
      guiOutputFeatureList->value(1);
      }
    guiOutputFeatureList->redraw();
    }
}

void
FeatureExtractionBaseViewGUI
::UpdateSelectedPixel(const IndexType& id)
{
//   itk::OStringStream oss;
//   oss << "(" << id[0] << " , " << id[1] << ")";
//   guiSpectAnglePixelCoordinates->value(oss.str().c_str());
//   guiSpectAnglePixelCoordinates->redraw();
  m_SelectedPixel = GetModel()->GetInputImage()->GetPixel(id);
//   oss.str("");
//   oss << "[";
// 
//   unsigned int i = 0;
//   for (i = 0; i < m_SelectedPixel.Size() - 1; i++)
//     {
//     oss << m_SelectedPixel[i] << ", ";
//     }
// 
//   oss << m_SelectedPixel[i] << "]";
//   guiSpectAnglePixelValue->value(oss.str().c_str());
//   guiSpectAnglePixelValue->redraw();
}

void
FeatureExtractionBaseViewGUI
::Show()
{
  guiMainWindow->show();

  // Scroll
  gScroll->add(m_VisuView->GetScrollWidget());
  gScroll->resizable(m_VisuView->GetScrollWidget());
  gScroll->box(FL_NO_BOX);
  m_VisuView->GetScrollWidget()->show();
  m_VisuView->GetScrollWidget()->resize(gScroll->x(), gScroll->y(), gScroll->w(), gScroll->h());

// Full
  gFull->add(m_VisuView->GetFullWidget());
  gFull->resizable(m_VisuView->GetFullWidget());
  gFull->box(FL_NO_BOX);
  m_VisuView->GetFullWidget()->show();
  m_VisuView->GetFullWidget()->resize(gFull->x(), gFull->y(), gFull->w(), gFull->h());

// Feature
  gFeature->add(m_ResultVisuView->GetFullWidget());
  gFeature->resizable(m_ResultVisuView->GetFullWidget());
  gFeature->box(FL_NO_BOX);
  m_ResultVisuView->GetFullWidget()->show();
  m_ResultVisuView->GetFullWidget()->resize(gFeature->x(), gFeature->y(), gFeature->w(), gFeature->h());

  m_VisuView->GetScrollWidget()->redraw();
  m_VisuView->GetFullWidget()->redraw();
  m_ResultVisuView->GetFullWidget()->redraw();

  gScroll->show();
  gFull->show();
  gFeature->show();
}

void
FeatureExtractionBaseViewGUI
::OK()
{
  m_FeatureExtractionBaseController->SaveOutput();

  this->Quit();
}

void
FeatureExtractionBaseViewGUI
::UpdateChannels()
{
  // Gets the used channels
  std::vector<unsigned int> ckeckedList(guiChannelSelection->nchecked(), 0);
  int                       j = 1;
  int                       count = 0;
  while (j <= guiChannelSelection->nitems() && count < guiChannelSelection->nchecked())
    {
    if (guiChannelSelection->checked(j) != 0)
      {
      ckeckedList[count] = j;
      count++;
      }
    j++;
    }

  m_FeatureExtractionBaseController->AddInputChannels(ckeckedList);
}

void
FeatureExtractionBaseViewGUI
::AddFeature()
{
  m_FeatureExtractionBaseController->CreateFeature(m_FeatureType);
}

// void
// FeatureExtractionBaseViewGUI
// ::UpdateHarMinMax()
// {
//   if( guiHarCk->value() != 0 )
//     {
//       guiHarIm->activate();
//       
//       guiHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
//       guiHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
//     }
//   else
//     {
//       guiHarIm->deactivate();
//     }
// }

// void
// FeatureExtractionBaseViewGUI
// ::UpdateAdvHarMinMax()
// {
//   if( guiAdvHarCk->value() != 0 )
//     {
//       guiAdvHarIm->activate();
//       
//       guiAdvHarMin->value( this->GetModel()->GetMinValues()[ this->GetModel()->GetMinValues().size()-1 ] );
//       guiAdvHarMax->value( this->GetModel()->GetMaxValues()[ this->GetModel()->GetMaxValues().size()-1 ] );
//     }
//   else
//     {
//       guiAdvHarIm->deactivate();
//     }
// }

void
FeatureExtractionBaseViewGUI
::ClearFeature()
{
  guiFeatureList->clear();
  guiFeatureList->redraw();
  guiFeatureListAction->clear();
  guiFeatureListAction->redraw();
  guiOutputFeatureList->clear();
  guiOutputFeatureList->redraw();
  m_InputOutputFeatureLink.clear();
  m_FeatureExtractionBaseController->ClearFeatures();

  if (m_ResultVisuView.IsNotNull())
    {
    m_ResultVisuView->GetFullWidget()->ClearBuffer();
    m_ResultVisuView->GetFullWidget()->redraw();
    GetModel()->GetResultVisuModel()->ClearLayers();
    }
}

void
FeatureExtractionBaseViewGUI
::ClearImage()
{
  guiChannelSelection->clear();
  guiChannelSelection->redraw();
  this->ClearFeature();

  guiFeatureChoice->value(0);
  this->UpdateParameterArea(0);
  this->SetFeatureType(0);

  // NewVisu
  if (m_VisuView.IsNotNull())
    {
    m_VisuView->GetScrollWidget()->ClearBuffer();
    m_VisuView->GetScrollWidget()->redraw();
    m_VisuView->GetFullWidget()->ClearBuffer();
    m_VisuView->GetFullWidget()->redraw();
    GetModel()->GetVisuModel()->ClearLayers();

    }

  if (m_ResultVisuView.IsNotNull())
    {
    m_ResultVisuView->GetFullWidget()->ClearBuffer();
    m_ResultVisuView->GetFullWidget()->redraw();
    GetModel()->GetResultVisuModel()->ClearLayers();
    }

  m_FeatureExtractionBaseController->InitInput();
  // Reset the title
  guiMainWindow->label(m_WindowTitle.c_str());

}

void
FeatureExtractionBaseViewGUI
::Quit()
{
  /** Notify that we can unlock data */
  m_FeatureExtractionBaseController->Quit();
  guiMainWindow->hide();
}

void
FeatureExtractionBaseViewGUI
::Cancel()
{
  /** Notify that we can cancel module registration */
  guiMainWindow->hide();
  m_FeatureExtractionBaseController->Cancel();
}

FeatureExtractionBaseModel*
FeatureExtractionBaseViewGUI
::GetModel()
{
  return m_FeatureExtractionBaseController->GetModel();
}

}
