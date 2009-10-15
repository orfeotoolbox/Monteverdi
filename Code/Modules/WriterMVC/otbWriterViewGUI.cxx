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
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Buffer.H>
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"
#include "otbMacro.h"
#include "itkExceptionObject.h"
// #include "otbMsgReporter.h"
#include "otbFeature.h"


namespace otb
{

WriterViewGUI
::WriterViewGUI()
{
  // Model pointer and listener registration
//   m_WriterModel = WriterModel::GetInstance();
//   m_WriterModel->RegisterListener(this);

  // Initialisation
  m_FileNameList =  StringVectorType(4, "");
  m_ViewerNameList =  StringVectorType(4, "");
  m_TemplateViewerName = "";
  m_DisplayedLabelList =  StringVectorType(4, "+ ");
  m_UndisplayedLabelList =  StringVectorType(4, "+ ");
  m_DisplayStatusList = std::vector<bool>(4, true);
  m_PixelType = FeatureInfo::UNKNOWN;

  this->CreateGUI();

  m_SelectedPixel.SetSize(0);
  m_SelectedPixel.Fill(0);
  m_FeatureExtractPreviewParentBrowser = -1;

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


// Scroll
  gScroll->add(m_VisuView->GetScrollWidget());
  gScroll->resizable(m_VisuView->GetScrollWidget());
  gScroll->box(FL_NO_BOX);
  m_VisuView->GetScrollWidget()->show();
  m_VisuView->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),gScroll->w(),gScroll->h() );

// Full
  gFull->add(m_VisuView->GetFullWidget());
  gFull->resizable(m_VisuView->GetFullWidget());
  gFull->box(FL_NO_BOX);
  m_VisuView->GetFullWidget()->show();
  m_VisuView->GetFullWidget()->resize(gFull->x(),gFull->y(),gFull->w(),gFull->h() );

// Feature
  gFeature->add(m_ResultVisuView->GetFullWidget());
  gFeature->resizable(m_ResultVisuView->GetFullWidget());
  gFeature->box(FL_NO_BOX);
  m_ResultVisuView->GetFullWidget()->show();
  m_ResultVisuView->GetFullWidget()->resize(gFeature->x(),gFeature->y(),gFeature->w(),gFeature->h() );

  gScroll->show();
  gFull->show();
  gFeature->show();

  //Initi Feature Model
  
}

void
WriterViewGUI
::UpdateParameterArea( unsigned int groupId )
{
  for(unsigned int i=0; i<m_ParameterGroupList.size(); i++)
    m_ParameterGroupList[i]->hide();
  m_ParameterGroupList[groupId]->show();
}

void
WriterViewGUI
::UpdateFeaturePreview()
{
  if (guiOutputFeatureList->value()>0)
  {
  m_WriterModel->GetSingleOutput(guiOutputFeatureList->value()-1);
  }
}

void
WriterViewGUI
::UpdateFeaturePreviewFromOutputList()
{
  
  if (guiOutputFeatureList->value()>0)
  {
    if( static_cast<unsigned int>(guiOutputFeatureList->value()-1) <m_WriterModel->GetOutputListOrder().size() )
    {
     m_WriterModel->GetSingleOutput( m_WriterModel->GetOutputListOrder()[guiOutputFeatureList->value()-1]);
      
    }
  }
}


void
WriterViewGUI
::SetPixelType(PixelType i)
{
  m_PixelType = i;
}

void
WriterViewGUI
::Notify(const std::string & event)
{
  if (m_WriterModel->GetHasInput())
  {
    InitWidgets();
//     this->UpdateChannelSelection();
    this->UpdateInformation();
  }
}

void
WriterViewGUI
::OpenImage()
{
  const char * cfname = fl_file_chooser("Pick an image file", "*.*",m_LastPath.c_str());
  Fl::check();
  guiMainWindow->redraw();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  ossimFilename fname(cfname);
  m_LastPath = fname.path();

  this->ClearImage();
  m_WriterController->OpenInputImage(cfname);
  m_FeatureExtractPreviewParentBrowser = -1;
}

/*
void
WriterViewGUI
::UpdateChannelSelection()
{
  guiChannelSelection->clear();
  itk::OStringStream oss;InitWidgets
  int count = 1;
  for (unsigned int i = 0; i<m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel(); i++)
  {
    oss.str("");
    oss<<"Channel "<<i+1;
    guiChannelSelection->add(oss.str().c_str(), count);
    count++;
  }
  if(m_WriterModel->GetNumberOfChannels() > 1 )
  {
    guiChannelSelection->add("Intensity", count);
  }
  // Set all check box checked
  guiChannelSelection->check_all();
  guiChannelSelection->redraw();
  this->UpdateChannels();
}
*/


void
WriterViewGUI
::UpdateInformation()
{
  std::string imName = m_WriterModel->GetInputFileName();
  itk::OStringStream oss;
  oss.str("");
  oss<<"Writer Application";
  oss<<" : "<<imName.substr(imName.find_last_of("/")+1, imName.size());
  oss<<" ("<<m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel();
  if(m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel() != 1)
    oss<<" bands , ";
  else
    oss<<" band , ";

  oss<<m_WriterModel->GetInputImage()->GetLargestPossibleRegion().GetSize()<<")";
  guiMainWindow->label(oss.str().c_str());
}

void
WriterViewGUI
::AddOutputChannel()
{
  if (guiFeatureList->size()!=0 && guiFeatureList->value()!=0)
  {
    m_InputOutputFeatureLink.push_back(guiFeatureList->value()-1);
//     m_WriterController->ChangeFilterStatus(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size()-1]);
    guiOutputFeatureList->add(guiFeatureList->text(guiFeatureList->value()));
    m_WriterController->AddToOutputListOrder(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size()-1]);
    guiOutputFeatureList->redraw();
  }
}

void
WriterViewGUI
::RemoveOutputChannel()
{
  if (guiOutputFeatureList->size()!=0 && guiOutputFeatureList->value()!=0)
  {
    m_WriterController->RemoveFromOutputListOrder(guiOutputFeatureList->value());
    m_InputOutputFeatureLink.erase(m_InputOutputFeatureLink.begin()+guiOutputFeatureList->value()-1);
    guiOutputFeatureList->remove(guiOutputFeatureList->value());
    guiOutputFeatureList->redraw();
  }
}

void
WriterViewGUI
::UpOutputChannel()
{
  if ( guiOutputFeatureList->size()!=0 && guiOutputFeatureList->value()!=0 )
    {
      int id = guiOutputFeatureList->value();
      // exchnage the output list manager
      m_WriterController->ExchangeOutputListOrder(1);
      // exchnage the list display order
      if(id!=1)
       {
         guiOutputFeatureList->swap( id, id-1 );
         guiOutputFeatureList->value(id-1);
       }
      else
       {
         guiOutputFeatureList->swap(id, guiOutputFeatureList->size() );
         guiOutputFeatureList->value(guiOutputFeatureList->size());
       }
    }
}

void
WriterViewGUI
::DownOutputChannel()
{
  if ( guiOutputFeatureList->size()!=0 && guiOutputFeatureList->value()!=0)
    {
      int id = guiOutputFeatureList->value();
      // exchnage the output list manager
      m_WriterController->ExchangeOutputListOrder(-1);
      // exchnage the list display order
      if(id!=guiOutputFeatureList->size())
       {
         guiOutputFeatureList->swap( id, id+1 );
         guiOutputFeatureList->value(id+1);
       }
      else
       {
         guiOutputFeatureList->swap(id, 1 );
         guiOutputFeatureList->value(1);
       }
      guiOutputFeatureList->redraw();
    }
}


void
WriterViewGUI
::UpdateSelectedPixel(const IndexType & id)
{
//   itk::OStringStream oss;
//   oss<<"("<<id[0]<<" , "<<id[1]<<")";
//   guiSpectAnglePixelCoordinates->value(oss.str().c_str());
//   guiSpectAnglePixelCoordinates->redraw();
//
//   m_SelectedPixel = m_WriterModel->GetInputImage()->GetPixel(id);
//   oss.str("");
//   oss<<"[";
//   unsigned int i = 0;
//   for (i=0; i<m_SelectedPixel.Size()-1; i++)
//   {
//     oss<<m_SelectedPixel[i]<<", ";
//   }
//   oss<<m_SelectedPixel[i]<<"]";
//   guiSpectAnglePixelValue->valGetViewue(oss.str().c_str());
//   guiSpectAnglePixelValue->redraw();
}






void
WriterViewGUI
::Show()
{
  //this->InitFeatureOutputList();
  m_WriterController->CreateFeature();
//   this->UpdateChannels();
  guiMainWindow->show();
  
  pBar->minimum(0);
  pBar->maximum(1);
}

void
WriterViewGUI
::InitFeatureOutputList()
{
  //this->UpdateChannels();
  
  std::ostringstream oss;
  std::string strBase = "channel number: ";
  for (unsigned int i=0;i<m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel();++i)
  {
    std::ostringstream oss;
    oss << i+1;
    this->guiFeatureList->add( m_WriterModel->GetOutputChannelsInformation()[i].c_str() );
    this->guiOutputFeatureList->add( m_WriterModel->GetOutputChannelsInformation()[i].c_str() );
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
  const bool useScale = static_cast <bool> ( guiScale->value() );
  m_WriterController->SaveOutput(filepath, static_cast<unsigned int>(m_PixelType), useScale);
  
  //Here we need to go back to the app Monteverdi //TODO
  this->Quit();
}


void
WriterViewGUI
::UpdateChannels()
{
  // Gets the used channels
  unsigned int nbBand = m_WriterModel->GetInputImage()->GetNumberOfComponentsPerPixel();
  std::vector<unsigned int> ckeckedList(nbBand, 0);
  for (unsigned int i=0;i<nbBand;++i)
  {
    ckeckedList[i] = i+1;
  }
  m_WriterController->AddInputChannels(ckeckedList);
}

/*
void
WriterViewGUI
::AddFeature()
{
  m_WriterController->CreateFeature(m_PixelType);
}
*/


void
WriterViewGUI
::ClearFeature()
{
  guiFeatureList->clear();
  guiFeatureList->redraw();
//   guiFeatureListAction->clear();
//   guiFeatureListAction->redraw();
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
//   guiChannelSelection->clear();
//   guiChannelSelection->redraw();
  this->ClearFeature();

//   guiFeatureChoice->value(0);
  this->UpdateParameterArea(0);
  this->SetPixelType(FeatureInfo::UNKNOWN);

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
  guiMainWindow->hide();
//   MsgReporter::GetInstance()->Hide();
}

void WriterViewGUI::Browse()
{
  const char * filename = NULL;

  filename = fl_file_chooser("Choose the dataset file...", "*.*",".");
  Fl::check();
  if (filename == NULL)
  {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
  }
  vFilePath->value(filename);
  
  //m_WriterController->OpenInputImage(filename);
  
  
}

}


