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

#include "otbGCPToSensorModelController.h"

#include "otbMsgReporter.h"
#include "otbCurlHelperInterface.h"

namespace otb
{

GCPToSensorModelController
::GCPToSensorModelController() : m_View(), m_Model(), m_WidgetController(), m_MapWidgetController(),
  m_MouseClickedController(), m_ResizingHandler(), m_ChangeRegionHandler(),
  m_ChangeScaledRegionHandler(), m_ChangeScaleHandler(),
  m_LeftMouseClickedHandler(), m_PixelActionHandler(), m_PixelModel(),
  m_PixelView(), m_MapResizingHandler(), m_MapScrollZoomHandler(),
  m_MapMouseMapActionHandler(), m_GuessActionHandler(), m_DEMPath("")
{
  // Build the widgets controller
  m_WidgetController          = WidgetControllerType::New();
  m_MapWidgetController       = WidgetControllerType::New();
  m_MouseClickedController    = MouseClickedController::New();

  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler        = ChangeScaleHandlerType::New();
  m_LeftMouseClickedHandler   = MouseClickedHandlertype::New();
  m_PixelActionHandler        = PixelDescriptionActionHandlerType::New();
  m_PixelModel                = PixelDescriptionModelType::New();
  m_PixelView                 = PixelDescriptionViewType::New();

  m_MapResizingHandler        = ResizingHandlerType::New();
  m_MapScrollZoomHandler      = ScrollZoomHandlerType::New();
  m_MapMouseMapActionHandler  = MouseMapActionHandlerType::New();
  m_GuessActionHandler        = GuessActionHandlerType::New();

  // Link pixel clicked model (controller in relity...)
  m_MouseClickedController->SetMouseButton(1);
  m_MouseClickedController->SetImageViewId(0);
  m_MouseClickedController->SetController(this);
  m_LeftMouseClickedHandler->SetModel(m_MouseClickedController);
  m_LeftMouseClickedHandler->SetActiveOnFullWidget(false);
  m_LeftMouseClickedHandler->SetActiveOnScrollWidget(false);

  // Add the action handlers to the widgets controller
  m_WidgetController->AddActionHandler(m_ResizingHandler);
  m_WidgetController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetController->AddActionHandler(m_ChangeScaleHandler);
  m_WidgetController->AddActionHandler(m_LeftMouseClickedHandler);
  m_WidgetController->AddActionHandler(m_GuessActionHandler);

  m_MapWidgetController->AddActionHandler(m_MapResizingHandler);
  m_MapWidgetController->AddActionHandler(m_MapScrollZoomHandler);
  m_MapWidgetController->AddActionHandler(m_MapMouseMapActionHandler);
}

GCPToSensorModelController
::~GCPToSensorModelController()
{
  MsgReporter::GetInstance()->Hide();
}

void
GCPToSensorModelController
::SetModel(ModelType* model)
{
  m_Model = model;
  // Register the model to the action handlers
  m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel());
  m_ChangeScaleHandler->SetModel(m_Model->GetVisualizationModel());

  m_MapResizingHandler->SetModel(m_Model->GetMapVisualizationModel());
  m_MapScrollZoomHandler->SetModel(m_Model);
  m_MapMouseMapActionHandler->SetModel(m_Model);
  m_GuessActionHandler->SetModel(m_Model);
}

GCPToSensorModelControllerInterface::ModelType*
GCPToSensorModelController::GetModel()
{
  return m_Model;
}

void
GCPToSensorModelController
::SetView(GCPToSensorModelView * view)
{
  m_View = view;
  m_ResizingHandler->SetView(m_View->GetImageView());
  m_ChangeRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaledRegionHandler->SetView(m_View->GetImageView());
  m_ChangeScaleHandler->SetView(m_View->GetImageView());
  m_LeftMouseClickedHandler->SetView(m_View->GetImageView());

  m_MapResizingHandler->SetView(m_View->GetMapView());
  m_MapScrollZoomHandler->SetView(m_View->GetMapView());
  m_MapMouseMapActionHandler->SetView(m_View->GetMapView());
  m_GuessActionHandler->SetView(m_View->GetImageView());
}

void
GCPToSensorModelController
::LinkPixelDescriptors()
{
  //  image pixel descriptor
  m_WidgetController->AddActionHandler(m_PixelActionHandler);
  m_PixelModel->SetLayers(m_Model->GetVisualizationModel()->GetLayers());
  m_PixelView->SetModel(m_PixelModel);
  m_PixelActionHandler->SetModel(m_PixelModel);
  m_PixelActionHandler->SetView(m_View->GetImageView());
  m_View->gPixelValue->add(m_PixelView->GetPixelDescriptionWidget());
  m_PixelView->GetPixelDescriptionWidget()->show();
  m_PixelView->GetPixelDescriptionWidget()->resize(m_View->gPixelValue->x(),
                                                   m_View->gPixelValue->y(),
                                                   m_View->gPixelValue->w(), m_View->gPixelValue->h());
}

void
GCPToSensorModelController
::AddPoints(float x, float y, float lon, float lat, float elev)
{
  // Try to add GCP
  try
    {
    m_Model->AddGCP(x, y, lon, lat, elev);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

void
GCPToSensorModelController
::ClearPointList()
{
  try
    {
    m_Model->ClearGCPsContainer();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

void
GCPToSensorModelController
::DeletePointFromList(unsigned int id)
{
  try
    {
    m_Model->RemovePointFromGCPsContainer(id);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

void
GCPToSensorModelController
::LeftMouseButtonClicked(ContinuousIndexType index)
{
  IndexType id;
  id[0] = static_cast<long>(index[0]);
  id[1] = static_cast<long>(index[1]);

  m_View->ChangePointValue(id);
}

void
GCPToSensorModelController
::FocusOn(ContinuousIndexType index)
{
  IndexType id;
  id[0] = static_cast<long>(index[0]);
  id[1] = static_cast<long>(index[1]);

  m_ChangeRegionHandler->GetModel()->SetExtractRegionCenter(id);
  m_ChangeRegionHandler->GetModel()->Update();

  if (otb::CurlHelperInterface::IsCurlAvailable())
    {
    try
      {
      // Focus on map
      m_Model->CenterMapOnSelectedPoint(id[0], id[1], 16);
      }
    catch (itk::ExceptionObject& err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
      }
    }
}

void
GCPToSensorModelController
::SetDEMPath(const std::string& DEMPath)
{
  // Check if DEM path is valid
  DEMHandler::Pointer dem = DEMHandler::Instance();
  if (dem->IsValidDEMDirectory(DEMPath.c_str()))
    {
    m_DEMPath = DEMPath;
    }
  else
    {
    MsgReporter::GetInstance()->SendError("Invalid directory \"" + DEMPath + "\", no DEM files found!");
    }
}

void
GCPToSensorModelController
::ChangeDEM()
{
  m_View->vElev->show();
  m_View->vElevMode->hide();

  // If Mean
  if (static_cast<bool>(m_View->cMean->value()))
    {
    try
      {
      m_Model->SetMeanElevation(static_cast<double>(m_View->vMeanElev->value()));
      m_Model->SetElevMgt(ModelType::MEAN);
      }
    catch (itk::ExceptionObject& err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
      }
    }
  // If DEM
  else if (static_cast<bool>(m_View->cDEM->value()))
    {
    try
      {
      m_View->vElevMode->value("DEM");
      m_View->vElevMode->show();
      m_View->vElev->hide();
      m_Model->SetDEMPath(m_DEMPath);
      m_Model->SetElevMgt(ModelType::DEM);
      }
    catch (itk::ExceptionObject& err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
      }
    }
  // If GCP
  else if (static_cast<bool>(m_View->cElev->value()))
    {
    try
      {
      m_Model->SetElevMgt(ModelType::GCP);
      }
    catch (itk::ExceptionObject& err)
      {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
      }
    }
}

void
GCPToSensorModelController
::ReloadGCPsList()
{
  try
    {
    m_Model->LoadGCP();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

bool
GCPToSensorModelController
::OK()
{
  if( m_View->GetNumberOfGCPPoints() >= m_Model->GetMinNbOfGCP() )
    {
      try
        {
          m_Model->OK();
        }
      catch (itk::ExceptionObject& err)
        {
          MsgReporter::GetInstance()->SendError(err.GetDescription());
          return false;
        }
      return true;
    }
  else
    {
      itk::OStringStream oss;
      oss<<"The module waits for "<<m_Model->GetMinNbOfGCP()<<" GCP points to compute the sensor model, only "<<m_View->GetNumberOfGCPPoints()<<" provided."<<std::endl;
      MsgReporter::GetInstance()->SendError( oss.str().c_str());
      return false;
    }
}

void
GCPToSensorModelController
::Quit()
{
  m_Model->Quit();
}

void
GCPToSensorModelController
::SearchPlaceName(double longitude, double latitude)
{
  try
    {
    m_Model->SearchPlaceName(longitude, latitude);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

void
GCPToSensorModelController
::SearchLonLat(std::string placename)
{
  try
    {
    m_Model->SearchLonLat(placename);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

void
GCPToSensorModelController
::DisplayMap(std::string placename,
             double longitude,
             double latitude,
             unsigned int depth,
             long int sizeX,
             long int sizeY)
{
  try
    {
    m_Model->DisplayMap(placename, longitude, latitude, depth, sizeX, sizeY);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

void
GCPToSensorModelController
::ExportGcpsToXmlFile(const char * fname)
{
  try
    {
    m_Model->ExportGCPsToXmlFile(fname);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

void
GCPToSensorModelController
::ImportGcpsFromXmlFile(const char * fname)
{
  try
    {
    m_Model->ImportGCPsFromXmlFile(fname);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
    }
}

} // end namespace otb
