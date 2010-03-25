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

#include <FLU/Flu_File_Chooser.h>
#include <FL/Fl.H>
#include <FL/fl_draw.H>

namespace otb
{

GCPToSensorModelView
::GCPToSensorModelView(): m_Controller(), m_Model(),
     m_ImageView(), m_MapView(), m_CrossGlComponent(), m_CircleGlComponent()
{
  m_Model = GCPToSensorModelModel::New();
  m_ImageView = ImageViewType::New();
  m_MapView = ImageViewType::New();
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
  // Remove registered visualization components from the interface
  m_ImageView->GetFullWidget()->ClearGlComponents();
  m_ImageView->GetScrollWidget()->ClearGlComponents();
  m_ImageView->GetZoomWidget()->ClearGlComponents();

  gFull->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
  gZoom->remove(m_ImageView->GetZoomWidget());
  
  m_MapView->GetFullWidget()->ClearGlComponents();
  m_MapView->GetScrollWidget()->ClearGlComponents();
  m_MapView->GetZoomWidget()->ClearGlComponents();
  
  gMFull->remove(m_MapView->GetFullWidget());
}

void
GCPToSensorModelView
::SetModel(GCPToSensorModelModel* model)
{
  m_Model = model;
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_MapView->SetModel(m_Model->GetMapVisualizationModel());
  
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
    
  if(!m_MapWidgetController)
    {
      itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }
    
  // Build the fltk code
  this->CreateGUI();

  // Register controllers
  m_ImageView->SetController(m_WidgetController);
  m_MapView->SetController(m_MapWidgetController);
  
  // Initialize Map Viewer
  vMPlaceName->value("");
  vMLongitude->value(0.0);
  vMLatitude->value(0.0);
  
  for (int i=1; i<=18; i++)
  {
    std::ostringstream label;
    label << i;
    vMDepth->add(label.str().c_str(),"",NULL);
  }
  vMDepth->value(0);
  
  // Show
  this->Show();
  
  // Link pixel descriptors (not do before because widgets have to be instanciated)
  m_Controller->LinkPixelDescriptors();

  // Disable Bilinear choice possibility
  cProjType->value(1);
  cProjType->do_callback();
  cProjType->hide();
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
::Show()
{
//   wMainWindow->position(0, 44);
//   wMapWindow->position(850, 44);
  
  wMainWindow->show();
  //wMapWindow->show();
  
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

  m_ImageView->GetFullWidget()->AddGlComponent( m_CrossGlComponent );
  m_ImageView->GetScrollWidget()->AddGlComponent( m_CrossGlComponent );
  m_ImageView->GetZoomWidget()->AddGlComponent( m_CrossGlComponent );
  m_ImageView->GetFullWidget()->AddGlComponent( m_CircleGlComponent );
  m_ImageView->GetScrollWidget()->AddGlComponent( m_CircleGlComponent );
  m_ImageView->GetZoomWidget()->AddGlComponent( m_CircleGlComponent );

  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_ImageView->GetZoomWidget()->show();

  m_MapView->GetFullWidget()->resize(gMFull->x(),gMFull->y(),gMFull->w(),gMFull->h());
  
  gMFull->add(m_MapView->GetFullWidget());
  gMFull->resizable(m_MapView->GetFullWidget());
}


void
GCPToSensorModelView
::AddPoints()
{
  float x = static_cast<float>(vX->value());
  float y = static_cast<float>(vY->value());
  float lat = static_cast<float>(vLat->value());
  float lon = static_cast<float>(vLong->value());
  float elev = static_cast<float>(vElev->value());
  m_Controller->AddPoints( x, y, lon, lat, elev);
}

void
GCPToSensorModelView
::AddPointsToList(GCPType gcp)
{
  Point2DType sensorPoint;
  Point3DType groundPoint;
  
  sensorPoint = gcp.first;
  groundPoint = gcp.second;

  itk::OStringStream oss;
  oss << sensorPoint << " -> " << groundPoint;
  
  this->lPointList->add(oss.str().c_str());
  
  srand(static_cast<unsigned int>(sensorPoint[1]+sensorPoint[0])*123456);
  ColorType color;
  color[0]=rand()/(RAND_MAX+1.0);
  color[1]=rand()/(RAND_MAX+1.0);
  color[2]=rand()/(RAND_MAX+1.0);
  color[3]=1.0;
  
  m_ColorList.push_back(color);
  
  lPointList->value(lPointList->size());
  this->UpdateListSelectionColor(true);
  
  IndexType index;
  index[0] = static_cast<long>(sensorPoint[0]);
  index[1] = static_cast<long>(sensorPoint[1]);
  
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
      Fl_Color flColor = fl_color_cube(static_cast<int>((FL_NUM_RED-1)*curColor[0]),
				       static_cast<int>((FL_NUM_GREEN-1)*curColor[1]),
				       static_cast<int>((FL_NUM_BLUE-1)*curColor[2]));
      
      //Update the List Point Color
      if(whichOne)
       {
         lPointList->selection_color(flColor);
         lPointList->redraw();
         if(tError->size() > static_cast<int>(selectedIndex))
           {
             tError->value(selectedIndex+1);
             tError->selection_color(flColor);
             tError->redraw();
           }
       }
      else
       {
         tError->selection_color(flColor);
         tError->redraw();
         if(lPointList->size() > static_cast<int>(selectedIndex))
           {
             lPointList->value(selectedIndex+1);
             lPointList->selection_color(flColor);
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
::Focus(unsigned int i)
{
  IndexType id;
 
  // Element selected in list
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
  const char * cfname = NULL;

  // Choose directory
  cfname = flu_dir_chooser(otbGetTextMacro("Choose the dataset dir..."), "");

  if (cfname == NULL)
  {
    otbMsgDebugMacro(<<"Empty directory!");
    return ;
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
  this->UpdatePointList();
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
::UpdateGCPView()
{
  // Clear lPointList
  lPointList->clear();
  
  // Clear transformation info
  this->ClearTransformationInfo();
  
  // Get the GCPsContainer
  GCPsContainerType GCPsContainer;
  GCPsContainer = m_Model->GetGCPsContainer();
  
  // Add point to list
  for(int i=0; i<GCPsContainer.size(); i++)
  {
    this->AddPointsToList(GCPsContainer[i]);
  }
  
  // TODO Update error
  std::cout << "GCPToSensorModelView::UpdateGCPView() TODO : update error" << std::endl;
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
::Notify()
{
  if(m_Model->GetGCPsContainerHasChanged())
  {
    this->UpdateGCPView();
  }
  if(m_Model->GetHasNewImage())
  {
    this->ReloadGCPs();
  }
  if(m_Model->GetPlaceNameChanged())
  {
    std::string placename = m_Model->GetPlaceName();
    vMPlaceName->value(placename.c_str());
  }
  if(m_Model->GetLatLongChanged())
  {
    vMLatitude->value(m_Model->GetLatitude());
    vMLongitude->value(m_Model->GetLongitude());
  }
  if(m_Model->GetHasNewMap())
  {
    this->DrawMap();
  }
  if(m_Model->GetDepthChanged())
  {
    vMDepth->value(m_Model->GetDepth()-1);
  }
  if(m_Model->GetSelectedPointChanged())
  {
    vLat->value(m_Model->GetSelectedLatitude());
    vLong->value(m_Model->GetSelectedLongitude());
  }
}


void
GCPToSensorModelView
::SetProjectionType()
{
/*  if(cProjType->value() == 0)
    {
      wDEMWindow->hide();
      guiDEM->deactivate();
      lPointList->clear();
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

  this->UpdatePointList();*/
}

void
GCPToSensorModelView
::UpdatePointList()
{
  lPointList->clear();
  this->ClearTransformationInfo();
  m_Controller->UpdatePointList();
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
  wMainWindow->hide();
  wMapWindow->hide();
  wDEMWindow->hide();
}
  
// void 
// GCPToSensorModelView
// ::ShowMap()
// {
//   if (wMapWindow->shown())
//     wMapWindow->hide();
//   else
//   {
//     wMapWindow->show();
//     this->DrawMap();
//   }
// }
  
void
GCPToSensorModelView
::UpdatePlaceName()
{
  double lLatitude = static_cast<double>(vMLatitude->value());
  double lLongitude = static_cast<double>(vMLongitude->value());
  
  m_Controller->SearchPlaceName(lLatitude, lLongitude);
}
  
void
GCPToSensorModelView
::UpdateLatLong()
{
  std::string lPlacename = vMPlaceName->value();
  
  if(lPlacename != "")
  {
    m_Controller->SearchLatLong(lPlacename);
  }
}
  
void 
GCPToSensorModelView
::DisplayMap()
{
  std::string lPlacename = vMPlaceName->value();
  double lLatitude = static_cast<double>(vMLatitude->value());
  double lLongitude = static_cast<double>(vMLongitude->value());
  unsigned int lDepth = static_cast<unsigned int>(vMDepth->value()) + 1;
  
  m_Controller->DisplayMap(lPlacename, lLatitude, lLongitude, lDepth, gMFull->w(), gMFull->h());
}

void
GCPToSensorModelView
::DrawMap()
{
  m_MapView->GetFullWidget()->redraw();
  m_MapView->GetFullWidget()->show();
}

void
GCPToSensorModelView
::ImportGcpsFromXmlFile()
{
  const char * fname = NULL;

  // Choose file
  fname = flu_file_chooser(otbGetTextMacro("Choose an XML file containing GCPs ..."), "*.xml","");

  if (fname == NULL)
    {
    return ;
    }

  m_Controller->ImportGcpsFromXmlFile(fname);
}

void
GCPToSensorModelView
::ExportGcpsToXmlFile()
{
  const char * fname = NULL;

    // Choose file
    fname = flu_file_chooser(otbGetTextMacro("Choose an XML file to export GCPs ..."), "*.xml","");

    if (fname == NULL)
      {
      return ;
      }

    m_Controller->ExportGcpsToXmlFile(fname);
}

}// end namespace
