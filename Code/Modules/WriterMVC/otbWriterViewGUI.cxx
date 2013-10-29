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

#include "otbWriterViewGUI.h"
#include <FLU/Flu_File_Chooser.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Buffer.H>
#include "otbMacro.h"
#include "itkExceptionObject.h"

namespace otb
{

WriterViewGUI
::WriterViewGUI()
{
  // Initialisation
  m_FileNameList =  StringVectorType(4, "");
  m_ViewerNameList =  StringVectorType(4, "");
  m_TemplateViewerName = "";
  m_DisplayedLabelList =  StringVectorType(4, "+ ");
  m_UndisplayedLabelList =  StringVectorType(4, "+ ");
  m_DisplayStatusList = std::vector<bool>(4, true);
  this->CreateGUI();

  m_SelectedPixel.SetSize(0);
  m_SelectedPixel.Fill(0);
  m_FeatureExtractPreviewParentBrowser = -1;

 // Check file existance before overwrite and warn user
  m_CheckFileExistance = true;
}

void
WriterViewGUI
::InitVisu()
{
  /** NewVisu */
  // Instanstiate the view
  m_VisuView = VisuViewType::New();
  // Set the model
  m_VisuView->SetModel(m_WriterModel->GetVisuModel());
  // Instanstiate the view
  m_ResultVisuView = VisuViewType::New();
  // Set the model
  m_ResultVisuView->SetModel(m_WriterModel->GetResultVisuModel());
}

WriterViewGUI
::~WriterViewGUI()
{
}

void
WriterViewGUI
::InitWidgets()
{
  assert(m_WriterController.IsNotNull() && "The controller is not created");

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

  gScroll->show();
  gFull->show();
  gFeature->show();

  //Initi Feature Model

}

void
WriterViewGUI
::UpdateParameterArea(unsigned int groupId)
{
  for (unsigned int i = 0; i < m_ParameterGroupList.size(); i++)
    m_ParameterGroupList[i]->hide();
  m_ParameterGroupList[groupId]->show();
}

void
WriterViewGUI
::UpdateFeaturePreview()
{
  if (guiOutputFeatureList->value() > 0)
    {
    m_WriterModel->GetSingleOutput(guiOutputFeatureList->value() - 1);
    }
}

void
WriterViewGUI
::UpdateFeaturePreviewFromOutputList()
{

  if (guiOutputFeatureList->value() > 0)
    {
    if (static_cast<unsigned int>(guiOutputFeatureList->value() - 1) < m_WriterModel->GetOutputListOrder().size())
      {
      m_WriterModel->GetSingleOutput(m_WriterModel->GetOutputListOrder()[guiOutputFeatureList->value() - 1]);

      }
    }
}

void
WriterViewGUI
::Notify(const std::string& event)
{
  if (m_WriterModel->GetHasInput())
    {
    InitWidgets();
//     this->UpdateChannelSelection();
    this->UpdateInformation();
    }
}
/*
void
WriterViewGUI
::OpenImage()
{
//   const char * cfname = fl_file_chooser("Pick an image file", "*.*", m_LastPath.c_str());
  Fl::check();
  guiMainWindow->redraw();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return;
  }
  m_LastPath = itksys::SystemTools::GetFilenamePath(cfname);

  this->ClearImage();
  m_WriterController->OpenInputImage(cfname);
  m_FeatureExtractPreviewParentBrowser = -1;
}
*/

void
WriterViewGUI
::UpdateInformation()
{
  std::string        imName = m_WriterModel->GetInputFileName();
  std::ostringstream oss;
  oss.str("");
  oss << "Writer Application";
  oss << ": " << imName.substr(imName.find_last_of("/") + 1, imName.size());
  oss << " (" << m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel();
  if (m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel() != 1) oss << " bands , ";
  else oss << " band , ";

  oss << m_WriterModel->GetInputImage()->GetLargestPossibleRegion().GetSize() << ")";
  guiMainWindow->label(oss.str().c_str());
}

void
WriterViewGUI
::AddOutputChannel()
{
  unsigned int listValue = guiFeatureList->value();
  if ((guiFeatureList->size() != 0) && (listValue != 0))
    {
    m_InputOutputFeatureLink.push_back(listValue - 1);
    guiOutputFeatureList->add(guiFeatureList->text(listValue));
    m_WriterController->AddToOutputListOrder(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size() - 1]);
    guiOutputFeatureList->redraw();
    }
}

void
WriterViewGUI
::RemoveOutputChannel()
{
  unsigned int listValue = guiOutputFeatureList->value();
  if ((guiOutputFeatureList->size() != 0) && (listValue != 0))
    {
    m_WriterController->RemoveFromOutputListOrder(listValue);
    m_InputOutputFeatureLink.erase(m_InputOutputFeatureLink.begin() + listValue - 1);
    guiOutputFeatureList->remove(listValue);
    guiOutputFeatureList->redraw();
    }
}

void
WriterViewGUI
::UpOutputChannel()
{
  int id = guiOutputFeatureList->value();
  if ((guiOutputFeatureList->size() != 0) && (id != 0))
    {

    // exchnage the output list manager
    m_WriterController->ExchangeOutputListOrder(1);
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
WriterViewGUI
::DownOutputChannel()
{
  int id = guiOutputFeatureList->value();
  if ((guiOutputFeatureList->size() != 0) && (id != 0))
    {
    // exchnage the output list manager
    m_WriterController->ExchangeOutputListOrder(-1);
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
WriterViewGUI
::Show()
{
  m_WriterController->CreateFeature();
  guiMainWindow->show();

  //Initialize progress bar
  pBar->minimum(0);
  pBar->maximum(1);
}

void
WriterViewGUI
::InitFeatureOutputList()
{
  std::ostringstream oss;
  std::string        strBase = "channel number: ";

  for (unsigned int i = 0; i < m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel(); ++i)
    {
    std::ostringstream oss;
    oss << i + 1;
    this->guiFeatureList->add(m_WriterModel->GetOutputChannelsInformation()[i].c_str());
    this->guiOutputFeatureList->add(m_WriterModel->GetOutputChannelsInformation()[i].c_str());
    this->AddToInputOutputFeatureLink(i);
    }
  this->guiFeatureList->redraw();
  this->guiOutputFeatureList->redraw();
}

void
WriterViewGUI
::OK()
{
  std::string filepath = vFilePath->value();
  std::ifstream isFileNameExist( filepath.c_str() );
  bool isProcessing = true;

  if(isFileNameExist && m_CheckFileExistance==true)
    {
      isFileNameExist.close();
      isProcessing = ::fl_choice("File already exist, do you want to overwrite this file?", "cancel", "OK", NULL );
    }

  if(isProcessing)
    {
    const bool  useScale = static_cast <bool> (guiScale->value());
    const bool  isWriteGeomFile  = static_cast <bool> (guiWriteMetadata->value());
    const int   pixelType = guiOutputPixelTypeChoice->value();

    m_WriterController->SaveOutput(filepath, pixelType, useScale, isWriteGeomFile);
    }
}

void
WriterViewGUI
::UpdateChannels()
{
  // Gets the used channels
  unsigned int nbBand = m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel();

  std::vector<unsigned int> ckeckedList(nbBand, 0);
  for (unsigned int i = 0; i < nbBand; ++i)
    {
    ckeckedList[i] = i + 1;
    }
  m_WriterController->AddInputChannels(ckeckedList);
}

void
WriterViewGUI
::ClearFeature()
{
  guiFeatureList->clear();
  guiFeatureList->redraw();
  guiOutputFeatureList->clear();
  guiOutputFeatureList->redraw();
  m_InputOutputFeatureLink.clear();
  m_WriterController->ClearFeatures();

  if (m_ResultVisuView.IsNotNull())
    {
    m_ResultVisuView->GetFullWidget()->ClearBuffer();
    m_ResultVisuView->GetFullWidget()->redraw();
    m_WriterModel->GetResultVisuModel()->ClearLayers();
    }
}

void
WriterViewGUI
::ClearImage()
{
  this->ClearFeature();
  this->UpdateParameterArea(0);

  // NewVisu
  if (m_VisuView.IsNotNull())
    {
    m_VisuView->GetScrollWidget()->ClearBuffer();
    m_VisuView->GetScrollWidget()->redraw();
    m_VisuView->GetFullWidget()->ClearBuffer();
    m_VisuView->GetFullWidget()->redraw();
    m_WriterModel->GetVisuModel()->ClearLayers();
    }
  if (m_ResultVisuView.IsNotNull())
    {
    m_ResultVisuView->GetFullWidget()->ClearBuffer();
    m_ResultVisuView->GetFullWidget()->redraw();
    m_WriterModel->GetResultVisuModel()->ClearLayers();
    }

  m_WriterController->InitInput();
  // Reset the title
  guiMainWindow->label("Feature Extraction Application");
}

void
WriterViewGUI
::Quit()
{
  /** Unlock data*/
  m_WriterController->Quit();
  guiMainWindow->hide();
}

void WriterViewGUI::Browse()
{
  const char * filename = NULL;

  filename = flu_file_chooser("Choose the dataset file...", "*.*", "");

  if (filename == NULL)
    {
    otbMsgDebugMacro(<< "Empty file name!");
    return;
    }
  vFilePath->value(filename);
}

void WriterViewGUI::UpdateProgress()
{
  double progress = this->GetProgress();

  std::ostringstream oss1, oss2;
  oss1.str("");
  oss1 << "Writing dataset  (" << std::floor(100 * progress) << "%)";
  oss2.str("");
  oss2 << std::floor(100 * progress);
  oss2 << "%";
  pBar->value(progress);
  vFilePath->copy_label(oss1.str().c_str());
  pBar->copy_label(oss2.str().c_str());
}

void WriterViewGUI::UpdateProgressCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if (writer.IsNotNull())
    {
    writer->UpdateProgress();
    }
}

void WriterViewGUI::QuitCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if (writer.IsNotNull())
    {
    writer->Quit();
    }
}

void WriterViewGUI::ManageActivationWindowButtons(bool withAwake)
{
  Fl::lock();
  if (withAwake)
    {
    // Reactivate window buttons
    guiQuit->activate();
    guiOK->activate();
    vFilePath->activate();
    bBrowse->activate();
    guiScale->activate();
    guiFeatureList->activate();
    guiOutputFeatureList->activate();
    guiAddChannel->activate();
    guiRemoveChannel->activate();
    guiUpChannel->activate();
    guiDownChannel->activate();
    Fl::awake(&QuitCallback, this);
    }
  else
    {
    // Deactivate window buttons
    guiQuit->deactivate();
    guiOK->deactivate();
    vFilePath->deactivate();
    bBrowse->deactivate();
    guiScale->deactivate();
    guiFeatureList->deactivate();
    guiOutputFeatureList->deactivate();
    guiAddChannel->deactivate();
    guiRemoveChannel->deactivate();
    guiUpChannel->deactivate();
    guiDownChannel->deactivate();
    }
  Fl::unlock();
}

void WriterViewGUI::AwakeProgressFields(double progress)
{
  this->SetProgress(progress);
  Fl::awake(&UpdateProgressCallback, this);
}
} //end namespace
