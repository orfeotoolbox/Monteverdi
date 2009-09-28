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
  m_WriterModel = WriterModel::GetInstance();
  m_WriterModel->RegisterListener(this);

  // Initialisation
  m_FileNameList =  StringVectorType(4, "");
  m_ViewerNameList =  StringVectorType(4, "");
  m_TemplateViewerName = "";
  m_DisplayedLabelList =  StringVectorType(4, "+ ");
  m_UndisplayedLabelList =  StringVectorType(4, "+ ");
  m_DisplayStatusList = std::vector<bool>(4, true);
  m_FeatureType = UNKNOWN;

  this->CreateGUI();

//   Fl_Text_Buffer * buffer = new Fl_Text_Buffer();
//   guiFeatInfo->buffer(buffer);

  m_SelectedPixel.SetSize(0);
  m_SelectedPixel.Fill(0);
  m_FeatureExtractPreviewParentBrowser = -1;
//   this->InitParameterGroupList();

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

  this->UpdateFeatureInfo(otb::UNKNOWN);

  gScroll->show();
  gFull->show();
  gFeature->show();

  //Initi Feature Model
  
}
/*
void
WriterViewGUI
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
*/
void
WriterViewGUI
::UpdateParameterArea( unsigned int groupId )
{
  for(unsigned int i=0; i<m_ParameterGroupList.size(); i++)
    m_ParameterGroupList[i]->hide();
  m_ParameterGroupList[groupId]->show();
}
/*
void
WriterViewGUI
::UpdateFeaturePreview()
{
  if (guiFeatureListAction->value()>0)
  {
    m_WriterModel->GetSingleOutput(guiFeatureListAction->value()-1);
  }
}
*/
void
WriterViewGUI
::UpdateFeaturePreviewFromOutputList()
{
  if (guiOutputFeatureList->value()>0)
  {
    if( static_cast<unsigned int>(guiOutputFeatureList->value()-1) <m_WriterModel->GetOutputListOrder().size() )
      m_WriterModel->GetSingleOutput( m_WriterModel->GetOutputListOrder()[guiOutputFeatureList->value()-1]);
  }
}


void
WriterViewGUI
::SetFeatureType(FeatureType i)
{
  m_FeatureType = i;
  this->UpdateFeatureInfo(i);
}

void
WriterViewGUI
::Notify()
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
  itk::OStringStream oss;
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
  oss<<"Feature Extraction Application";
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
    m_WriterController->ChangeFilterStatus(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size()-1]);
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
    m_WriterController->ChangeFilterStatus(m_InputOutputFeatureLink[m_InputOutputFeatureLink.size()-1]);
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
//   guiSpectAnglePixelValue->value(oss.str().c_str());
//   guiSpectAnglePixelValue->redraw();
}



void
WriterViewGUI
::UpdateFeatureInfo(FeatureType feat)
{
//   FeatureInfo inf;
//   itk::OStringStream oss;
//   oss<<inf.GetMapInfo().find(feat)->second;
//   guiFeatInfo->buffer()->remove(0,guiFeatInfo->buffer()->length());
//   guiFeatInfo->insert(oss.str().c_str());
//   guiFeatInfo->redraw();
}


void
WriterViewGUI
::Show()
{
  this->InitFeatureOutputList();
  
  guiMainWindow->show();
}

void 
WriterViewGUI
::InitFeatureOutputList()
{
  std::ostringstream oss;
  std::string strBase = "channel number: ";
  for (unsigned int i=0;i<m_WriterModel->GetInputImageList()->Size();++i)
  {
    
    this->guiFeatureList->add( (strBase + oss.str()).c_str() );
  }
  
  
  //m_View->guiOutputFeatureList->add( m_Model->GetOutputFilterInformationId(i).c_str() );
  this->guiFeatureList->redraw();
//   this->guiOutputFeatureList->redraw();
}

void
WriterViewGUI
::OK()
{
  
  //const char * cfname = fl_file_chooser("Save as...", "*.*",m_LastPath.c_str());
//   Fl::check();
//   guiMainWindow->redraw();
//   if (cfname == NULL || strlen(cfname)<1)
//   {
//     return;
//   }
//   m_WriterController->SetOutputFileName(cfname);
  m_WriterController->SaveOutput();
  
  //Here we need to go back to the app Monteverdi //TODO
//   m_WriterController
  
  this->Quit();
}


void
WriterViewGUI
::UpdateChannels()
{
//   // Gets the used channels
//   std::vector<unsigned int> ckeckedList(guiChannelSelection->nchecked(), 0);
//   int j = 1;
//   int count = 0;
//   while ( j<guiChannelSelection->nitems() || count<guiChannelSelection->nchecked() )
//   {
//     if (guiChannelSelection->checked(j) == 1)
//     {
//       ckeckedList[count] = j;
//       count++;
//     }
// 
//     j++;
//   }
// 
// 
//   m_WriterController->AddInputChannels(ckeckedList);
}

/*
void
WriterViewGUI
::AddFeature()
{
  m_WriterController->CreateFeature(m_FeatureType);
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
  this->SetFeatureType(otb::UNKNOWN);

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

  filename = fl_file_chooser("Choose the dataset file ...", "*.*",".");
  Fl::check();
  if (filename == NULL)
  {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
  }
  vFilePath->value(filename);
  
//   m_WriterController->OpenInputImage(filename);
  
  
}

}


