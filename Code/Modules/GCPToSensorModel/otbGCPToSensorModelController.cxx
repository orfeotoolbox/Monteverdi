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
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>



namespace otb
{

GCPToSensorModelController
::GCPToSensorModelController(): m_View(), m_Model(), m_WidgetController(), m_MouseClickedController(),
                                          m_ResizingHandler(), m_ChangeRegionHandler(), m_ChangeScaledRegionHandler(),
                                          m_ChangeScaleHandler(), m_LeftMouseClickedHandler(), m_PixelActionHandler(),
                                          m_PixelModel(), m_PixelView()
{
  // Build the widgets controller
  m_WidgetController        = WidgetControllerType::New();
  m_MouseClickedController   = MouseClickedController::New();

  // Build the action handlers
  m_ResizingHandler           = ResizingHandlerType::New();
  m_ChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler        = ChangeScaleHandlerType::New();
  m_LeftMouseClickedHandler   = MouseClickedHandlertype::New();
  m_PixelActionHandler        = PixelDescriptionActionHandlerType::New();
  m_PixelModel                = PixelDescriptionModelType::New();
  m_PixelView                 = PixelDescriptionViewType::New();

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
  m_PixelView->GetPixelDescriptionWidget()->resize(m_View->gPixelValue->x(),m_View->gPixelValue->y(), m_View->gPixelValue->w(), m_View->gPixelValue->h());
}


void
GCPToSensorModelController
::AddPoints( float x, float y, float lat, float lon )
{
  ContinuousIndexType id1, id2;
  id1[0] = x; id1[1] = y;
  id2[0] = lat; id2[1] = lon;

  if( !m_Model->GetInputImage()->GetLargestPossibleRegion().IsInside(id1) )
    {
      itk::OStringStream oss;
      oss<<"Index "<<id1<<" is outside the image size "<<m_Model->GetInputImage()->GetLargestPossibleRegion().GetSize();
      MsgReporter::GetInstance()->SendError(oss.str().c_str());
      return;
    }
  try
    {
      m_Model->AddIndexesToList( id1, id2 );
      m_View->AddPointsToList( id1, id2 );
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
    }
}

void
GCPToSensorModelController
::ClearPointList()
{
  m_Model->ClearIndexesList();
  this->SetTransformationAvailable( false );
}

void
GCPToSensorModelController
::DeletePointFromList( unsigned int id )
{
  m_Model->RemovePointFromList( id );
  this->SetTransformationAvailable( false );
}


void
GCPToSensorModelController
::LeftMouseButtonClicked( ContinuousIndexType index )
{
  m_View->ChangePointValue(index);
}


void
GCPToSensorModelController
::FocusOn(ContinuousIndexType id)
{
  IndexType index;
 index[0] = static_cast<long>(id[0]);
 index[1] = static_cast<long>(id[1]);
  m_ChangeRegionHandler->GetModel()->SetExtractRegionCenter(index);
  m_ChangeRegionHandler->GetModel()->Update();
}




void
GCPToSensorModelController
::UpdateStats()
{
  if( !this->GetTransformationAvailable() )
    {
      MsgReporter::GetInstance()->SendError("No transformation computed.");
      return;
    }

  itk::OStringStream oss;
  std::vector<double> values; 
  ModelType::ContinuousIndexListType outTranformedPoint = m_Model->TransformPoints();
  IndexesListType                    indexesList        = m_Model->GetIndexesList();
  ContinuousIndexType idFix, idOut;

  for(unsigned int i=0; i<indexesList.size(); i++)
    {
      idFix = indexesList[i].first;
      idOut = indexesList[i].second;
 
      oss.str("");
      oss<<idFix<<" -> ";
      values.push_back(  vcl_pow( static_cast<double>(idOut[0])-outTranformedPoint[i][0], 2 ) 
			 + vcl_pow( static_cast<double>(idOut[1])-outTranformedPoint[i][1], 2 ) );
  
     oss<<outTranformedPoint[i]<<", error: "<<vcl_sqrt(values[values.size()-1]);
      m_View->tError->add( oss.str().c_str() );
    }

  double sum = 0;
  for(unsigned int i=0; i<values.size(); i++)
    {
      sum += values[i];
    }
  sum = vcl_sqrt(sum);
  sum = sum / static_cast<double>(values.size());

  oss.str("");
  oss<<sum;
  m_View->tMeanError->value(oss.str().c_str());
}

void
GCPToSensorModelController
::SetDEMPath( const std::string & filePath )
{
 try
    {
      m_Model->SetDEMPath( filePath );
    }
  catch (itk::ExceptionObject & err)
    {
      std::cout<<"err : "<<err.GetDescription()<<std::endl;
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
    }
}

void
GCPToSensorModelController
::ChangeDEM()
{
  if( static_cast<bool>(m_View->cMean->value()) )
    {
      m_Model->SetMeanElevation( static_cast<double>(m_View->vMeanElev->value()) );
      m_Model->SetUseDEM(false);
    }
  else if( static_cast<bool>(m_View->cDEM->value()) )
    {
      if( m_Model->GetDEMPath() == "")
	{
	  MsgReporter::GetInstance()->SendError("No DEM directory path selected.");
	  return;
	}
      m_Model->SetUseDEM(true);
    }
}

void
GCPToSensorModelController
::OK()
{
  if( !this->GetTransformationAvailable() )
    {
      std::cout<<"No transformation available."<<std::endl;
      MsgReporter::GetInstance()->SendError("No transformation available.");
      return;
    }
  try
    {
      m_Model->OK();
    }
  catch (itk::ExceptionObject & err)
    {
      std::cout<<"err : "<<err.GetDescription()<<std::endl;
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
    }
}

} // end namespace otb