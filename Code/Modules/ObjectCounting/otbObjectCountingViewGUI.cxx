/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/

#include "otbObjectCountingViewGUI.h"
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Buffer.H>
#include "otbMacro.h"
#include "itkExceptionObject.h"

namespace otb
{

ObjectCountingViewGUI
::ObjectCountingViewGUI()
{
  m_Model = ObjectCountingModel::GetInstance();
  m_Model->RegisterListener(this);
/*
  ControllerType::Pointer controller = ControllerType::New();
  //ViewType::Pointer view = ViewType::New();
  controller->SetView(this);
  this->SetController(controller);
  */
  this->CreateGUI();

  vThresholdValue->value( m_Model->GetThresholdValue() );

  this->StartRunning();
}


ObjectCountingViewGUI
::~ObjectCountingViewGUI()
{
}



void
ObjectCountingViewGUI
::RefreshViewer()
{
  if (m_Model->GetState() == NO_IMAGE)
  {
    return;
  }

  BoxType::Pointer box = BoxType::New();
  SizeType boxSize;
  IndexType boxIndex;

  // Defining the full image widget
  if (m_FullImageWidget.IsNull())
  {
    m_FullImageWidget  = FullWidgetType::New();
    m_FullImageWidget->SetController(m_Controller);
    m_FullImageWidget->SetInput(m_Model->GetExtractedImage());
    if (m_Model->GetExtractedImage()->GetNumberOfComponentsPerPixel()>3)
    {
      m_FullImageWidget->SetRedChannelIndex(2);
      m_FullImageWidget->SetGreenChannelIndex(1);
      m_FullImageWidget->SetBlueChannelIndex(0);
    }
    m_FullImageWidget->Init(gImageViewer->x(),gImageViewer->y(),gImageViewer->w(),gImageViewer->h(),"Full resolution images");
    gImageViewer->box(FL_NO_BOX);
    gImageViewer->add(m_FullImageWidget);
    m_FullImageWidget->SetFormOverlayVisible(true);
    m_FullImageWidget->Show();
  }
  else
  {
    m_FullImageWidget->SetInput(m_Model->GetExtractedImage());
    m_FullImageWidget->Init(gImageViewer->x(),gImageViewer->y(),gImageViewer->w(),gImageViewer->h(),"Full resolution images");
    m_FullImageWidget->ClearBufferedRegion();
  }

  m_FullImageWidget->GetTransferFunctionList()->GetNthElement(0)->SetLowerBound(m_Model->GetMin(0));
  m_FullImageWidget->GetTransferFunctionList()->GetNthElement(0)->SetUpperBound(m_Model->GetMax(0));
  m_FullImageWidget->GetTransferFunctionList()->GetNthElement(1)->SetLowerBound(m_Model->GetMin(1));
  m_FullImageWidget->GetTransferFunctionList()->GetNthElement(1)->SetUpperBound(m_Model->GetMax(1));
  m_FullImageWidget->GetTransferFunctionList()->GetNthElement(2)->SetLowerBound(m_Model->GetMin(2));
  m_FullImageWidget->GetTransferFunctionList()->GetNthElement(2)->SetUpperBound(m_Model->GetMax(2));


  // Defining the box of the viewed full region
  boxSize[0]=(m_Model->GetCurrentRegion().GetSize()[0]);
  boxSize[1]=(m_Model->GetCurrentRegion().GetSize()[1]);
  boxIndex[0]=(m_Model->GetCurrentRegion().GetIndex()[0]-1);
  boxIndex[1]=(m_Model->GetCurrentRegion().GetIndex()[1]+1);
  box->SetSize(boxSize);
  box->SetIndex(boxIndex);

  // defining the scroll widget
  if (m_ScrollWidget.IsNull())
  {
    m_ScrollWidget  = ScrollWidgetType::New();
    m_ScrollWidget->SetController(m_Controller);
    m_ScrollWidget->SetInput(m_Model->GetQuicklook());
    if (m_Model->GetQuicklook()->GetNumberOfComponentsPerPixel()>3)
    {
      m_ScrollWidget->SetRedChannelIndex(2);
      m_ScrollWidget->SetGreenChannelIndex(1);
      m_ScrollWidget->SetBlueChannelIndex(0);
    }
    m_ScrollWidget->Init(gScroll->x(),gScroll->y(),gScroll->w(),gScroll->h(),"Scroll");
    m_ScrollWidget->SetFormOverlayVisible(true);
    gScroll->add(m_ScrollWidget);
    gScroll->box(FL_NO_BOX);
    m_ScrollWidget->GetFormList()->PushBack(box);

    m_ScrollWidget->Show();
  }
  else
  {
    m_ScrollWidget->GetFormList()->SetNthElement(0,box);
    m_ScrollWidget->SetInput(m_Model->GetQuicklook());
    m_ScrollWidget->ClearBufferedRegion();
  }
  m_ScrollWidget->GetTransferFunctionList()->SetNthElement(0,m_FullImageWidget->GetTransferFunctionList()->GetNthElement(0));
  m_ScrollWidget->GetTransferFunctionList()->SetNthElement(1,m_FullImageWidget->GetTransferFunctionList()->GetNthElement(1));
  m_ScrollWidget->GetTransferFunctionList()->SetNthElement(2,m_FullImageWidget->GetTransferFunctionList()->GetNthElement(2));
  m_ScrollWidget->SetSubSamplingRate(m_Model->GetShrinkFactor());

  m_ScrollWidget->redraw();
  m_FullImageWidget->redraw();
}



void ObjectCountingViewGUI
::UpdateInterface()
{
  switch (m_Model->GetState())
  {
  case NO_IMAGE:
  {
    this->RefreshViewer();
    gImageViewer->hide();
    gScroll->hide();
    bRunPreview->hide();
    bRunPreview->deactivate();
    tStat->value("");
    tStat->redraw();
//     mSavePoly->activate();
    cbUseSmoothing->value(0);
    break;
  }

  case HAS_IMAGE:
  {
    this->RefreshViewer();
    gImageViewer->show();
    gScroll->show();
    bRunPreview->activate();
    tStat->value("");
    tStat->redraw();
//     mSavePoly->activate();
    cbUseSmoothing->value(0);
    break;
  }

  case EXAMPLES_SELECTED:
  {
    this->UpdatePolygon();
    break;
  }

  case POLYGONS_DETECTED:
  {
    itk::OStringStream oss;
    oss<<"Number of Object(s) found : ";
    oss<<m_Model->GetNumberOfObjects();
    tStat->value( oss.str().c_str() );
    tStat->redraw();
    if (m_Model->GetWhichImage() == EXTRACT)
    {
      this->UpdateOutputPolyBoundaries();
//       mSavePoly->activate();
    }
  }
  break;

  case HAS_REFERENCEPIXEL:
  {
    itk::OStringStream oss;
    oss<<m_Model->GetReferencePixel();
    tRefPixel->value(oss.str().c_str());
    tRefPixel->redraw();
    break;
  }
  }
}

void ObjectCountingViewGUI::Notify(const std::string & event)
{
  if (m_Model->GetState()==HAS_IMAGE && !m_FullImageWidget.IsNull())
    m_FullImageWidget->GetFormList()->Clear();
  this->UpdateInterface();
}

void
ObjectCountingViewGUI
::Show()
{
  guiMainWindow->show();
}

void
ObjectCountingViewGUI
::OpenImage()
{
//   const char * cfname = fl_file_chooser("Select an Input Image", "*.*",".");
//   Fl::check();
//   guiMainWindow->redraw();
//   if (cfname == NULL || strlen(cfname)<1)
//   {
//     return ;
//   }
//   m_Controller->OpenImage( cfname );
}

void
ObjectCountingViewGUI
::Quit()
{
  this->StopRunning();
}

void
ObjectCountingViewGUI
::RunPreview()
{
  this->CleanResultDisplay();
  m_Controller->ComputeOverExtract();
}

void
ObjectCountingViewGUI
::SaveResult()
{
//   const char * cfname = fl_file_chooser("Save Output Image as", "*.*",".");
//   Fl::check();
//   guiMainWindow->redraw();
//   if (cfname == NULL || strlen(cfname)<1)
//   {
//     return ;
//   }
//   m_Controller->SaveResult(cfname);
//   mSavePoly->activate();
}

void
ObjectCountingViewGUI
::SavePolygon()
{
  const char * cfname = fl_file_chooser("Vector data file :", "*.shp\t*.kml",".");
  Fl::check();
  guiMainWindow->redraw();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  m_Controller->SavePolygon(cfname);
}


void
ObjectCountingViewGUI
::UpdatePolygon()
{
  // Erase done
  if ( m_Model->GetErasedPolygonIndex() != -1 )
    m_FullImageWidget->GetFormList()->Erase( m_Model->GetErasedPolygonIndex() );
  else
  {
    PolylineType::ColorType color;
    color[0] = 1;
    color[1] = 0;
    color[2] = 0;
    color[3] = 0.6;
    unsigned int pos = m_Model->GetInputPolyList()->Size()-1;
    PolygonPointerType poly = PolygonType::New();
    poly->SetPolygon(m_Model->GetInputPolyList()->GetNthElement(pos));
    poly->SetColor(color);
    // add a polygon
    if ( m_Model->GetInputPolygonListIndex() == pos )
      m_FullImageWidget->GetFormList()->PushBack(poly);
    else
      m_FullImageWidget->GetFormList()->SetNthElement(pos,poly);
  }

  m_FullImageWidget->redraw();
}


void
ObjectCountingViewGUI
::UpdateOutputPolyBoundaries()
{
  PolylineType::ColorType color;
  color[0] = 0;
  color[1] = 0;
  color[2] = 1;
  color[3] = 1;
  for (unsigned int j=0; j<m_Model->GetOutputPolyList()->Size(); j++)
  {
    PolygonType::VertexListConstIteratorType it, itStop;
    ObjectCountingModelType::PolygonPointerType outputPolygon;
    outputPolygon = m_Model->GetOutputPolyList()->GetNthElement(j);
    it = outputPolygon->GetVertexList()->Begin();
    itStop = outputPolygon->GetVertexList()->End();

    PolylinePointerType poly = PolylineType::New();
    poly->SetColor( color );
    while ( it !=  itStop )
    {
      poly->GetPolyline()->AddVertex(it.Value());
      ++it;
    }
    it = outputPolygon->GetVertexList()->Begin();
    poly->GetPolyline()->AddVertex(it.Value());
    m_FullImageWidget->GetFormList()->PushBack(poly);

    m_FullImageWidget->redraw();
  }
}

void
ObjectCountingViewGUI
::CleanResultDisplay()
{
  tStat->value("");
  tStat->redraw();

  unsigned int objectDisplayed = m_FullImageWidget->GetFormList()->Size();
  // erase change the list size, have to erase always the same index
  unsigned int val = m_Model->GetInputPolyList()->Size();
  for (unsigned int t= m_Model->GetInputPolyList()->Size(); t<objectDisplayed ; t++)
    m_FullImageWidget->GetFormList()->Erase(val);

  m_FullImageWidget->redraw();
}


void
ObjectCountingViewGUI
::ActivateClassifArea(int i)
{
  if ( i == 0 )
  {
    cbUseSpectralAngle->value(0);
    cbUseSVM->value(1);
    gSpectralAngle->deactivate();
    tRefPixel->value("");
    tRefPixel->redraw();
    tRefPixel->deactivate();
    vNuParameter->activate();
    vThresholdValue->deactivate();
  }
  else
  {
    if ( i == 1 )
    {
      cbUseSpectralAngle->value(1);
      cbUseSVM->value(0);
      gSpectralAngle->activate();
      tRefPixel->activate();
      vThresholdValue->activate();
      vNuParameter->deactivate();
    }
  }
  m_Controller->UseSVM(cbUseSVM->value());
}


void
ObjectCountingViewGUI
::UpdateThresholdValue()
{
  m_Controller->UpdateThresholdValue( vThresholdValue->value() );
}


void
ObjectCountingViewGUI
::UpdateSpatialRadius()
{
  m_Controller->UpdateSpatialRadius( static_cast<int>(vSpatialRadius->value()) );
}
void
ObjectCountingViewGUI
::UpdateRangeRadius()
{
  m_Controller->UpdateRangeRadius( static_cast<int>(vRangeRadius->value()) );
}
void
ObjectCountingViewGUI
::UpdateScale()
{
  m_Controller->UpdateScale( static_cast<int>(vScale->value()) );
}

void
ObjectCountingViewGUI
::UpdateNuParameter()
{
  m_Controller->UpdateNuParameter( static_cast<double>(vNuParameter->value()) );
}

void
ObjectCountingViewGUI
::UpdateMinRegionSize()
{
  m_Controller->UpdateMinRegionSize( static_cast<int>(vMinRegionSize->value()) );
}

void
ObjectCountingViewGUI
::UseSmoothing()
{
  m_Controller->UseSmoothing( cbUseSmoothing->value() );
}

void ObjectCountingViewGUI::Build()
{
  if(!m_Controller)
    {
    itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  this->Show();
  std::cout << "Open image in the controller" << std::endl;
  this->GetController()->OpenImage();
  std::cout << "Need to run" << std::endl;
  //otbFlRunMacro(this->IsRunning());
  std::cout << "Module is now running" << std::endl;
}

void ObjectCountingViewGUI::Hide()
{
  guiMainWindow->hide();
}


void ObjectCountingViewGUI::SetModel(ObjectCountingModelType* model)
{
  m_Model = model;
  //m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_Model->RegisterListener(this);
}
}// end namespace
