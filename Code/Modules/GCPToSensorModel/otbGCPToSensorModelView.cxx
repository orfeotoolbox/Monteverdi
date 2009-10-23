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

#include "otbGCPToSensorModelView.h"

#include "otbMsgReporter.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>


namespace otb
{

GCPToSensorModelView
::GCPToSensorModelView(): m_Controller(), m_Model(), 
     m_ImageView(), m_CrossGlComponent(), m_CircleGlComponent()
{
  m_Model = GCPToSensorModelModel::New();
  m_ImageView = ImageViewType::New();
  m_CrossGlComponent = CrossGlComponent::New();
  m_CircleGlComponent = CircleGlComponent::New();
  m_Green.Fill(0);
  m_Green[1] = 1;
  m_Green[3] = 0.5;

  m_ColorList.clear();
}


GCPToSensorModelView
::~GCPToSensorModelView()
{
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}

void
GCPToSensorModelView
::SetModel(GCPToSensorModelModel* model)
{
  m_Model = model;
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_Model->RegisterListener(this);
}

void
GCPToSensorModelView
::BuildInterface()
{
  if(!m_Controller)
    {
    itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  if(!m_WidgetController)
    {
    itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }
  // Build the fltk code
  this->CreateGUI();


  // Register controllers
  m_ImageView->SetController(m_WidgetController);

  // Add registered visualization components from the interface
  gFull->add(m_ImageView->GetFullWidget());
  gScroll->add(m_ImageView->GetScrollWidget());
  gZoom->add(m_ImageView->GetZoomWidget());
  gFull->resizable(m_ImageView->GetFullWidget());
  gScroll->resizable(m_ImageView->GetScrollWidget());
  gZoom->resizable(m_ImageView->GetZoomWidget());

  m_ImageView->GetFullWidget()->resize(gFull->x(),gFull->y(),gFull->w(),gFull->h());
  m_ImageView->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),gScroll->w(),gScroll->h());
  m_ImageView->GetZoomWidget()->resize(gZoom->x(),gZoom->y(),gZoom->w(),gZoom->h());

  // Show and refresh the interface
  this->wMainWindow->show();

  m_ImageView->GetFullWidget()->AddGlComponent( m_CrossGlComponent );
  m_ImageView->GetScrollWidget()->AddGlComponent( m_CrossGlComponent );
  m_ImageView->GetZoomWidget()->AddGlComponent( m_CrossGlComponent );
  m_ImageView->GetFullWidget()->AddGlComponent( m_CircleGlComponent );
  m_ImageView->GetScrollWidget()->AddGlComponent( m_CircleGlComponent );
  m_ImageView->GetZoomWidget()->AddGlComponent( m_CircleGlComponent );

  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_ImageView->GetZoomWidget()->show();
 
  // Link pixel descriptors (not do before because widgets have to be instanciated)
  m_Controller->LinkPixelDescriptors();
}

void
GCPToSensorModelView
::RedrawWidgets()
{
  m_ImageView->GetFullWidget()->redraw();
  m_ImageView->GetScrollWidget()->redraw();
  m_ImageView->GetZoomWidget()->redraw();
}


void
GCPToSensorModelView
::AddPoints()
{
  float x = static_cast<float>(vX->value());
  float y = static_cast<float>(vY->value());
  float lat = static_cast<float>(vLat->value());
  float lon = static_cast<float>(vLong->value());
  m_Controller->AddPoints( x, y, lat, lon);
}

void
GCPToSensorModelView
::AddPointsToList( ContinuousIndexType id1, ContinuousIndexType id2, double height )
{
 itk::OStringStream oss;
 IndexType id1Int;
 id1Int[0] = static_cast<long>(id1[0]);
 id1Int[1] = static_cast<long>(id1[1]);
 oss<<id1Int<<" -> ";
 if( m_Model->GetProjectionType() == GCPToSensorModelModel::RPC)
   {
     oss<<"["<<id2[0]<<" , "<<id2[1]<<" , "<<height<<"]";
   }
 else
   oss<<id2;
 this->lPointList->add(oss.str().c_str());

 srand((id1[1]+id1[0])*123456);
 ColorType color;
 color[0]=rand()/(RAND_MAX+1.0);
 color[1]=rand()/(RAND_MAX+1.0);
 color[2]=rand()/(RAND_MAX+1.0);
 color[3]=1.0;

 fl_color(static_cast<unsigned char>(255*color[0]),
         static_cast<unsigned char>(255*color[1]),
         static_cast<unsigned char>(255*color[2]));
 m_ColorList.push_back(color);

 lPointList->value(lPointList->size());
 this->UpdateListSelectionColor(true);

 IndexType index;
 index[0] = static_cast<long>(id1[0]);
 index[1] = static_cast<long>(id1[1]);

 m_CrossGlComponent->AddIndex( index );
 m_CrossGlComponent->ChangeColor( color, m_CrossGlComponent->GetColorList().size()-1 );

 this->RedrawWidgets();
}

/**
 *
 */
void
GCPToSensorModelView
::UpdateListSelectionColor(bool whichOne)
{
  //selectedIndex
  unsigned int selectedIndex;
  // tListPoint callback
  if(whichOne)
    selectedIndex = lPointList->value()-1;
  else // tError callback
    selectedIndex = tError->value()-1;

  if( selectedIndex < m_ColorList.size() )
    {
      ColorType curColor = m_ColorList[selectedIndex];

      // color To fl_color
      fl_color(static_cast<unsigned char>(255*curColor[0]),
              static_cast<unsigned char>(255*curColor[1]),
              static_cast<unsigned char>(255*curColor[2]));

      //Update the List Point Color
      if(whichOne)
       {
         lPointList->selection_color(fl_color());
         lPointList->redraw();
         if(tError->size() > static_cast<int>(selectedIndex))
           {
             tError->value(selectedIndex+1);
             tError->selection_color(fl_color());
             tError->redraw();
           }
       }
      else
       {
         tError->selection_color(fl_color());
         tError->redraw();
         if(lPointList->size() > static_cast<int>(selectedIndex))
           {
             lPointList->value(selectedIndex+1);
             lPointList->selection_color(fl_color());
             lPointList->redraw();
           }
       }
    }
}

void
GCPToSensorModelView
::ChangePointValue(ContinuousIndexType index)
{
  IndexType id;
  id[0] = static_cast<long>(index[0]);
  id[1] = static_cast<long>(index[1]);
  vX->value(id[0]);
  vY->value(id[1]);
  m_CircleGlComponent->Clear();
  m_CircleGlComponent->AddIndex(id);
  m_ImageView->GetFullWidget()->redraw();
  m_ImageView->GetScrollWidget()->redraw();
  m_ImageView->GetZoomWidget()->redraw();
}

void
GCPToSensorModelView
::ClearPointList()
{
  m_Controller->ClearPointList();
  lPointList->clear();
  lPointList->redraw();
  m_ColorList.clear();

  m_CrossGlComponent->Clear();
  
  this->ClearTransformationInfo();

  this->RedrawWidgets();
}


void
GCPToSensorModelView
::DeletePoint()
{
  unsigned int id = lPointList->value();
  if( id != 0 )
    {
      m_Controller->DeletePointFromList(id-1);
      lPointList->remove(id);
      tError->remove(id);
      m_ColorList.erase(m_ColorList.begin()+id-1);
      if(id<=static_cast<unsigned int>(lPointList->size()))
       lPointList->value(id);
      else
       lPointList->value(1);
      
      m_CrossGlComponent->ClearIndex(id-1);
      this->RedrawWidgets();

      this->ClearTransformationInfo();
      m_Controller->UpdateStats();
      this->UpdateListSelectionColor(true);
    }
}

void
GCPToSensorModelView
::ClearTransformationInfo()
{
  tError->clear();
  tMeanError->value("");
  tGroundError->value("");
}



void
GCPToSensorModelView
::Focus(unsigned int i)
{
  IndexType id;
  unsigned int value = lPointList->value();
  // Focus cross
  if((i == 0) && (value != 0))
    {
      if(m_CrossGlComponent->GetIndexList().size() >= value) 
        {
	  id = m_CrossGlComponent->GetIndexList()[value-1];
	  m_Controller->FocusOn(id);
	}
    }
  // Focus circles
  else if( i == 1 )
    {
      if( m_CircleGlComponent->GetIndexList().size() != 0 ) 
       {
         id = m_CircleGlComponent->GetIndexList()[0];
         m_Controller->FocusOn(id);
       }
    }
}



void
GCPToSensorModelView
::SelectDEM()
{
  // Choose directory
  const char * cfname = fl_dir_chooser("Pick a directory", "*.*");
  // Check path
  if (cfname == NULL || strlen(cfname)<1)
  {
    return;
  }
  
  std::string DEMfile = cfname;
  m_Controller->SetDEMPath( DEMfile );
  tDEMFile->value( cfname );
  this->ClearTransformationInfo();
  m_Controller->UpdateStats();
}

void
GCPToSensorModelView
::ChangeDEM()
{
  m_Controller->ChangeDEM();
}

void
GCPToSensorModelView
::ReloadGCPs()
{
  lPointList->clear();
  this->ClearTransformationInfo();
  m_Controller->ReloadGCPsList();
}

void
GCPToSensorModelView
::Notify()
{
  if(m_Model->GetHasNewImage())
    {
      this->ReloadGCPs();
      m_Controller->UpdateStats();
    }
  else if(m_Model->GetProjectionUpdated())
    {
      this->ClearTransformationInfo();
      m_Controller->UpdateStats();
    }
}


void
GCPToSensorModelView
::SetProjectionType()
{
  if(cProjType->value() == 0)
    {
      wDEMWindow->hide();
      guiDEM->deactivate();
      vElev->hide();
      m_Model->SetProjectionType(GCPToSensorModelModel::BILINEAR);
    }
  else if (cProjType->value() == 1)
    {
      guiDEM->activate();
      vElev->show();
      m_Model->SetProjectionType(GCPToSensorModelModel::RPC); 
    }
  else
    return;
  
  this->ClearTransformationInfo();
  m_Controller->UpdateStats();
}

void
GCPToSensorModelView
::Quit()
{
  m_Controller->OK();
  this->HideAll();
}

void
GCPToSensorModelView
::HideAll()
{
  // Remove registered visualization components from the interface
  m_ImageView->GetFullWidget()->ClearGlComponents();
  m_ImageView->GetScrollWidget()->ClearGlComponents();
  m_ImageView->GetZoomWidget()->ClearGlComponents();

  this->RedrawWidgets();

  gFull->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
  gZoom->remove(m_ImageView->GetZoomWidget());

  wMainWindow->hide();
  wDEMWindow->hide();
}


}// end namespace
