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

#include "otbFeatureExtractionViewGUI.h"
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

FeatureExtractionViewGUI
::FeatureExtractionViewGUI()
{
  // Model pointer and listener registration
  m_FeatureExtractionModel = FeatureExtractionModel::GetInstance();
  m_FeatureExtractionModel->RegisterListener(this);

  // Initialisation
  m_FileNameList =  StringVectorType(4, "");
  m_ViewerNameList =  StringVectorType(4, "");
  m_TemplateViewerName = "";
  m_DisplayedLabelList =  StringVectorType(4, "+ ");
  m_UndisplayedLabelList =  StringVectorType(4, "+ ");
  m_DisplayStatusList = std::vector<bool>(4, true);
  m_FeatureType = UNKNOWN;

  this->CreateGUI();

  Fl_Text_Buffer * buffer = new Fl_Text_Buffer();
  guiFeatInfo->buffer(buffer);

  m_SelectedPixel.SetSize(0);
  m_SelectedPixel.Fill(0);
  m_FeatureExtractPreviewParentBrowser = -1;
  this->InitParameterGroupList();

  /** NewVisu */
  // Instanstiate the view
  m_VisuView = VisuViewType::New();
  // Set the model
  m_VisuView->SetModel(m_FeatureExtractionModel->GetVisuModel());
  // Instanstiate the view
  m_ResultVisuView = VisuViewType::New();
  // Set the model
  m_ResultVisuView->SetModel(m_FeatureExtractionModel->GetResultVisuModel());

}


FeatureExtractionViewGUI
::~FeatureExtractionViewGUI()
{
}

void
FeatureExtractionViewGUI
::InitWidgets()
{
  assert(m_FeatureExtractionController.IsNotNull() && "The controller is not created");


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

  this->UpdateFeatureInfo(otb::UNKNOWN);

  gScroll->show();
  gFull->show();
  gFeature->show();

}

void
FeatureExtractionViewGUI
::InitParameterGroupList()
{
  m_ParameterGroupList.clear();
  m_ParameterGroupList.push_back(guiNothing);
  m_ParameterGroupList.push_back(guiTouzi);
  m_ParameterGroupList.push_back(guiHarris);
  m_ParameterGroupList.push_back(guiSpectAngle);
  m_ParameterGroupList.push_back(guiRadius);
  m_ParameterGroupList.push_back(guiGrad);
  m_ParameterGroupList.push_back(guiTextures);
  m_ParameterGroupList.push_back(guiMorpho);
  m_ParameterGroupList.push_back(guiRAndNIR); // panel 8
  m_ParameterGroupList.push_back(guiPVI);
  m_ParameterGroupList.push_back(guiSAVI);
  m_ParameterGroupList.push_back(guiMSAVI);
  m_ParameterGroupList.push_back(guiTSAVI);
  m_ParameterGroupList.push_back(guiWDVI);
  m_ParameterGroupList.push_back(guiARVI);
  m_ParameterGroupList.push_back(guiEVI); // 15
  m_ParameterGroupList.push_back(guiTSARVI);
  m_ParameterGroupList.push_back(guiAVI);
  m_ParameterGroupList.push_back(guiSoil);
  m_ParameterGroupList.push_back(guiIB2);
  m_ParameterGroupList.push_back(guiNDBI);
  m_ParameterGroupList.push_back(guiISU); // 21
  m_ParameterGroupList.push_back(guiSRWI);
  m_ParameterGroupList.push_back(guiNDWI);
  m_ParameterGroupList.push_back(guiNDWI2);
  m_ParameterGroupList.push_back(guiMNDWI);
  m_ParameterGroupList.push_back(guiNDPI);
  m_ParameterGroupList.push_back(guiNDTI); // 27
  m_ParameterGroupList.push_back(guiSFS);
  m_ParameterGroupList.push_back(guiEdgeSobel);
  m_ParameterGroupList.push_back(guiMS);
  // for original data, uses guiNothing, ie.m_ParameterGroupList[0]
  //m_ParameterGroupList.push_back(guiEdgeCanny);

}

void
FeatureExtractionViewGUI
::UpdateParameterArea( unsigned int groupId )
{
  for(unsigned int i=0; i<m_ParameterGroupList.size(); i++)
    m_ParameterGroupList[i]->hide();
  m_ParameterGroupList[groupId]->show();
}

void
FeatureExtractionViewGUI
::UpdateFeaturePreview()
{
  if (guiFeatureListAction->value()>0)
  {
    m_FeatureExtractionModel->GetSingleOutput(guiFeatureListAction->value()-1);
  }
}

void
FeatureExtractionViewGUI
::UpdateFeaturePreviewFromOutputList()
{
  if (guiOutputFeatureList->value()>0)
  {
    if( static_cast<unsigned int>(guiOutputFeatureList->value()-1) <m_FeatureExtractionModel->GetOutputListOrder().size() )
      m_FeatureExtractionModel->GetSingleOutput( m_FeatureExtractionModel->GetOutputListOrder()[guiOutputFeatureList->value()-1]);
  }
}


void
FeatureExtractionViewGUI
::SetFeatureType(FeatureType i)
{
  m_FeatureType = i;
  this->UpdateFeatureInfo(i);
}

void
FeatureExtractionViewGUI
::Notify()
{
  if (m_FeatureExtractionModel->GetHasInput())
  {
    InitWidgets();
    this->UpdateChannelSelection();
    this->UpdateInformation();
  }
}

void
FeatureExtractionViewGUI
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
  m_FeatureExtractionController->OpenInputImage(cfname);
  m_FeatureExtractPreviewParentBrowser = -1;
}


void
FeatureExtractionViewGUI
::UpdateChannelSelection()
{
  guiChannelSelection->clear();
  itk::OStringStream oss;
  int count = 1;
  for (unsigned int i = 0; i<m_FeatureExtractionModel->GetInputImage()->GetNumberOfComponentsPerPixel(); i++)
  {
    oss.str("");
    oss<<"Channel "<<i+1;
    guiChannelSelection->add(oss.str().c_str(), count);
    count++;
  }
  if(m_FeatureExtractionModel->GetNumberOfChannels() > 1 )
  {
    guiChannelSelection->add("Intensity", count);
  }
  // Set all check box checked
  guiChannelSelection->check_all();
  guiChannelSelection->redraw();
  this->UpdateChannels();
}



void
FeatureExtractionViewGUI
::UpdateInformation()
{
  std::string imName = m_FeatureExtractionModel->GetInputFileName();
  itk::OStringStream oss;
  oss.str("");
  oss<<"Feature Extraction Application";
  oss<<" : "<<imName.substr(imName.find_last_of("/")+1, imName.size());
  oss<<" ("<<m_FeatureExtractionModel->GetInputImage()->GetNumberOfComponentsPerPixel();
  if(m_FeatureExtractionModel->GetInputImage()->GetNumberOfComponentsPerPixel() != 1)
    oss<<" bands , ";
  else
    oss<<" band , ";

  oss<<m_FeatureExtractionModel->GetInputImage()->GetLargestPossibleRegion().GetSize()<<")";
  guiMainWindow->label(oss.str().c_str());
}

void
FeatureExtractionViewGUI
::AddOutputChannel()
{
  if (guiFeatureList->size()!=0 && guiFeatureList->value()!=0)
  {
    m_InputOutputFeatureLink.push_back(guiFeatureList->value()-1);
    m_FeatureExtractionController->ChangeFilterStatus(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size()-1]);
    guiOutputFeatureList->add(guiFeatureList->text(guiFeatureList->value()));
    m_FeatureExtractionController->AddToOutputListOrder(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size()-1]);
    guiOutputFeatureList->redraw();
  }
}

void
FeatureExtractionViewGUI
::RemoveOutputChannel()
{
  if (guiOutputFeatureList->size()!=0 && guiOutputFeatureList->value()!=0)
  {
    m_FeatureExtractionController->ChangeFilterStatus(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size()-1]);
    m_FeatureExtractionController->RemoveFromOutputListOrder(guiOutputFeatureList->value());
    m_InputOutputFeatureLink.erase(m_InputOutputFeatureLink.begin()+guiOutputFeatureList->value()-1);
    guiOutputFeatureList->remove(guiOutputFeatureList->value());
    guiOutputFeatureList->redraw();
  }
}

void
FeatureExtractionViewGUI
::UpOutputChannel()
{
  if ( guiOutputFeatureList->size()!=0 && guiOutputFeatureList->value()!=0 )
    {
      int id = guiOutputFeatureList->value();
      // exchnage the output list manager
      m_FeatureExtractionController->ExchangeOutputListOrder(1);
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
FeatureExtractionViewGUI
::DownOutputChannel()
{
  if ( guiOutputFeatureList->size()!=0 && guiOutputFeatureList->value()!=0)
    {
      int id = guiOutputFeatureList->value();
      // exchnage the output list manager
      m_FeatureExtractionController->ExchangeOutputListOrder(-1);
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
FeatureExtractionViewGUI
::UpdateSelectedPixel(const IndexType & id)
{
  itk::OStringStream oss;
  oss<<"("<<id[0]<<" , "<<id[1]<<")";
  guiSpectAnglePixelCoordinates->value(oss.str().c_str());
  guiSpectAnglePixelCoordinates->redraw();

  m_SelectedPixel = m_FeatureExtractionModel->GetInputImage()->GetPixel(id);
  oss.str("");
  oss<<"[";
  unsigned int i = 0;
  for (i=0; i<m_SelectedPixel.Size()-1; i++)
  {
    oss<<m_SelectedPixel[i]<<", ";
  }
  oss<<m_SelectedPixel[i]<<"]";
  guiSpectAnglePixelValue->value(oss.str().c_str());
  guiSpectAnglePixelValue->redraw();
}



void
FeatureExtractionViewGUI
::UpdateFeatureInfo(FeatureType feat)
{
  FeatureInfo inf;
  itk::OStringStream oss;
  oss<<inf.GetMapInfo().find(feat)->second;
  guiFeatInfo->buffer()->remove(0,guiFeatInfo->buffer()->length());
  guiFeatInfo->insert(oss.str().c_str());
  guiFeatInfo->redraw();
}


void
FeatureExtractionViewGUI
::Show()
{
  guiMainWindow->show();
}


void
FeatureExtractionViewGUI
::OK()
{
  /*
  const char * cfname = fl_file_chooser("Save as...", "*.*",m_LastPath.c_str());
  Fl::check();
  guiMainWindow->redraw();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return;
  }
  m_FeatureExtractionController->SetOutputFileName(cfname);
  m_FeatureExtractionController->SaveOutput();
  */
  //Here we need to go back to the app Monteverdi //TODO
  this->Quit();
}


void
FeatureExtractionViewGUI
::UpdateChannels()
{
  // Gets the used channels
  std::vector<unsigned int> ckeckedList(guiChannelSelection->nchecked(), 0);
  int j = 1;
  int count = 0;
  while ( j<guiChannelSelection->nitems() || count<guiChannelSelection->nchecked() )
  {
    if (guiChannelSelection->checked(j) == 1)
    {
      ckeckedList[count] = j;
      count++;
    }

    j++;
  }


  m_FeatureExtractionController->AddInputChannels(ckeckedList);
}


void
FeatureExtractionViewGUI
::AddFeature()
{
  m_FeatureExtractionController->CreateFeature(m_FeatureType);
}



void
FeatureExtractionViewGUI
::ClearFeature()
{
  guiFeatureList->clear();
  guiFeatureList->redraw();
  guiFeatureListAction->clear();
  guiFeatureListAction->redraw();
  guiOutputFeatureList->clear();
  guiOutputFeatureList->redraw();
  m_InputOutputFeatureLink.clear();
  m_FeatureExtractionController->ClearFeatures();

  if (m_ResultVisuView.IsNotNull())
    {
      m_ResultVisuView->GetFullWidget()->ClearBuffer();
      m_ResultVisuView->GetFullWidget()->redraw();
      m_FeatureExtractionModel->GetResultVisuModel()->ClearLayers();
    }
}


void
FeatureExtractionViewGUI
::ClearImage()
{
  guiChannelSelection->clear();
  guiChannelSelection->redraw();
  this->ClearFeature();

  guiFeatureChoice->value(0);
  this->UpdateParameterArea(0);
  this->SetFeatureType(otb::UNKNOWN);

  // NewVisu 
  if (m_VisuView.IsNotNull())
  {
    m_VisuView->GetScrollWidget()->ClearBuffer();
    m_VisuView->GetScrollWidget()->redraw();
    m_VisuView->GetFullWidget()->ClearBuffer();
    m_VisuView->GetFullWidget()->redraw();
    m_FeatureExtractionModel->GetVisuModel()->ClearLayers();

  }

  if (m_ResultVisuView.IsNotNull())
  {
    m_ResultVisuView->GetFullWidget()->ClearBuffer();
    m_ResultVisuView->GetFullWidget()->redraw();
    m_FeatureExtractionModel->GetResultVisuModel()->ClearLayers();
  }

  m_FeatureExtractionController->InitInput();
 // Reset the title
 guiMainWindow->label("Feature Extraction Application");

}

void
FeatureExtractionViewGUI
::Quit()
{
  guiMainWindow->hide();
//   MsgReporter::GetInstance()->Hide();
}


}


