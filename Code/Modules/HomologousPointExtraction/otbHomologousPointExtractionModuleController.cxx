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

#include "otbHomologousPointExtractionModuleController.h"

#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>



namespace otb
{

HomologousPointExtractionModuleController
::HomologousPointExtractionModuleController(): m_View(), m_Model(), m_FirstWidgetsController(), m_SecondWidgetsController(),
                                          m_FirstMouseClickedController(), m_SecondMouseClickedController(),
                                          m_FirstResizingHandler(), m_FirstChangeRegionHandler(), m_FirstChangeScaledRegionHandler(),
                                          m_FirstChangeScaleHandler(), m_FirstLeftMouseClickedHandler(), m_FirstPixelActionHandler(),
                                          m_FirstPixelModel(), m_FirstPixelView(), m_SecondResizingHandler(), m_SecondChangeRegionHandler(),
                                          m_SecondChangeScaledRegionHandler(), m_SecondChangeScaleHandler(), m_SecondLeftMouseClickedHandler(),
                                          m_SecondPixelActionHandler(), m_SecondPixelModel(), m_SecondPixelView()
{
  // Build the widgets controller
  m_FirstWidgetsController        = WidgetsControllerType::New();
  m_SecondWidgetsController       = WidgetsControllerType::New();
  m_FirstMouseClickedController   = MouseClickedController::New();
  m_SecondMouseClickedController  = MouseClickedController::New();

  // Build the action handlers
  m_FirstResizingHandler           = ResizingHandlerType::New();
  m_FirstChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_FirstChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_FirstChangeScaleHandler        = ChangeScaleHandlerType::New();
  m_FirstLeftMouseClickedHandler   = MouseClickedHandlertype::New();
  m_FirstPixelActionHandler        = PixelDescriptionActionHandlerType::New();
  m_FirstPixelModel                = PixelDescriptionModelType::New();
  m_FirstPixelView                 = PixelDescriptionViewType::New();

  m_SecondResizingHandler           = ResizingHandlerType::New();
  m_SecondChangeRegionHandler       = ChangeRegionHandlerType::New();
  m_SecondChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_SecondChangeScaleHandler        = ChangeScaleHandlerType::New();
  m_SecondLeftMouseClickedHandler   = MouseClickedHandlertype::New();
  m_SecondPixelActionHandler        = PixelDescriptionActionHandlerType::New();
  m_SecondPixelModel                = PixelDescriptionModelType::New();
  m_SecondPixelView                 = PixelDescriptionViewType::New();

  // Link pixel clicked model (controller in relity...)
  m_FirstMouseClickedController->SetMouseButton(1);
  m_FirstMouseClickedController->SetImageViewId(0);
  m_FirstMouseClickedController->SetController(this);
  m_SecondMouseClickedController->SetMouseButton(1);
  m_SecondMouseClickedController->SetImageViewId(1);
  m_SecondMouseClickedController->SetController(this);
  m_FirstLeftMouseClickedHandler->SetModel(m_FirstMouseClickedController);
  m_FirstLeftMouseClickedHandler->SetActiveOnFullWidget(false);
  m_FirstLeftMouseClickedHandler->SetActiveOnScrollWidget(false);
  m_SecondLeftMouseClickedHandler->SetModel(m_SecondMouseClickedController);
  m_SecondLeftMouseClickedHandler->SetActiveOnFullWidget(false);
  m_SecondLeftMouseClickedHandler->SetActiveOnScrollWidget(false);

  // Add the action handlers to the widgets controller
  m_FirstWidgetsController->AddActionHandler(m_FirstResizingHandler);
  m_FirstWidgetsController->AddActionHandler(m_FirstChangeRegionHandler);
  m_FirstWidgetsController->AddActionHandler(m_FirstChangeScaledRegionHandler);
  m_FirstWidgetsController->AddActionHandler(m_FirstChangeScaleHandler);
  m_FirstWidgetsController->AddActionHandler(m_FirstLeftMouseClickedHandler);
  
  m_SecondWidgetsController->AddActionHandler(m_SecondResizingHandler);
  m_SecondWidgetsController->AddActionHandler(m_SecondChangeRegionHandler);
  m_SecondWidgetsController->AddActionHandler(m_SecondChangeScaledRegionHandler);
  m_SecondWidgetsController->AddActionHandler(m_SecondChangeScaleHandler);
  m_SecondWidgetsController->AddActionHandler(m_SecondLeftMouseClickedHandler);

  m_TransformType = otb::UNKNOWN;
}

HomologousPointExtractionModuleController
::~HomologousPointExtractionModuleController()
{
  MsgReporter::GetInstance()->Hide();
}

void
HomologousPointExtractionModuleController
::SetModel(ModelType* model)
{
  m_Model = model;
  // Register the model to the action handlers
  m_FirstResizingHandler->SetModel(m_Model->GetVisualizationModel(0));
  m_FirstChangeRegionHandler->SetModel(m_Model->GetVisualizationModel(0));
  m_FirstChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel(0));
  m_FirstChangeScaleHandler->SetModel(m_Model->GetVisualizationModel(0));

  m_SecondResizingHandler->SetModel(m_Model->GetVisualizationModel(1));
  m_SecondChangeRegionHandler->SetModel(m_Model->GetVisualizationModel(1));
  m_SecondChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel(1));
  m_SecondChangeScaleHandler->SetModel(m_Model->GetVisualizationModel(1));
}

void
HomologousPointExtractionModuleController
::SetView(HomologousPointExtractionModuleView * view)
{
  m_View = view;
  m_FirstResizingHandler->SetView(m_View->GetFirstImageView());
  m_FirstChangeRegionHandler->SetView(m_View->GetFirstImageView());
  m_FirstChangeScaledRegionHandler->SetView(m_View->GetFirstImageView());
  m_FirstChangeScaleHandler->SetView(m_View->GetFirstImageView());
  m_FirstLeftMouseClickedHandler->SetView(m_View->GetFirstImageView());
 
  m_SecondResizingHandler->SetView(m_View->GetSecondImageView());
  m_SecondChangeRegionHandler->SetView(m_View->GetSecondImageView());
  m_SecondChangeScaledRegionHandler->SetView(m_View->GetSecondImageView());
  m_SecondChangeScaleHandler->SetView(m_View->GetSecondImageView());
  m_SecondLeftMouseClickedHandler->SetView(m_View->GetSecondImageView());
}


void
HomologousPointExtractionModuleController
::LinkPixelDescriptors()
{
  // First image pixel descriptor
  m_FirstWidgetsController->AddActionHandler(m_FirstPixelActionHandler);
  m_FirstPixelModel->SetLayers(m_Model->GetVisualizationModel(0)->GetLayers());
  m_FirstPixelView->SetModel(m_FirstPixelModel);
  m_FirstPixelActionHandler->SetModel(m_FirstPixelModel);
  m_FirstPixelActionHandler->SetView(m_View->GetFirstImageView());
  m_View->gPixelValue1->add(m_FirstPixelView->GetPixelDescriptionWidget());
  m_FirstPixelView->GetPixelDescriptionWidget()->show();
  m_FirstPixelView->GetPixelDescriptionWidget()->resize(m_View->gPixelValue1->x(),m_View->gPixelValue1->y(), m_View->gPixelValue1->w(), m_View->gPixelValue1->h());

  // Second image pixel descriptor
  m_SecondWidgetsController->AddActionHandler(m_SecondPixelActionHandler);
  m_SecondPixelModel->SetLayers(m_Model->GetVisualizationModel(1)->GetLayers());
  m_SecondPixelView->SetModel(m_SecondPixelModel);
  m_SecondPixelActionHandler->SetModel(m_SecondPixelModel);
  m_SecondPixelActionHandler->SetView(m_View->GetSecondImageView());
  m_View->gPixelValue2->add(m_SecondPixelView->GetPixelDescriptionWidget());
  m_SecondPixelView->GetPixelDescriptionWidget()->show();
  m_SecondPixelView->GetPixelDescriptionWidget()->resize(m_View->gPixelValue2->x(),m_View->gPixelValue2->y(), m_View->gPixelValue2->w(), m_View->gPixelValue2->h());
}


void
HomologousPointExtractionModuleController
::AddPoints( int x1, int y1, int x2, int y2 )
{
  IndexType id1, id2;
  id1[0] = x1; id1[1] = y1;
  id2[0] = x2; id2[1] = y2;

  if( !m_Model->GetFirstInputImage()->GetLargestPossibleRegion().IsInside(id1) )
    {
      itk::OStringStream oss1;
      oss1<<"Index "<<id1<<" is outside the image size "<<m_Model->GetFirstInputImage()->GetLargestPossibleRegion().GetSize();
      MsgReporter::GetInstance()->SendError(oss1.str().c_str());
      return;
    }
  if( !m_Model->GetSecondInputImage()->GetLargestPossibleRegion().IsInside(id2) )
    {
      itk::OStringStream oss2;
      oss2<<"Index "<<id2<<" is outside the image size "<<m_Model->GetSecondInputImage()->GetLargestPossibleRegion().GetSize();
      MsgReporter::GetInstance()->SendError(oss2.str().c_str());
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
HomologousPointExtractionModuleController
::ClearPointList()
{
  m_Model->ClearIndexesList();
  this->SetTransformationAvailable( false );
}

void
HomologousPointExtractionModuleController
::DeletePointFromList( unsigned int id )
{
  m_Model->RemovePointFromList( id );
  this->SetTransformationAvailable( false );
}


void
HomologousPointExtractionModuleController
::LeftMouseButtonClicked( IndexType index, unsigned int viewId )
{
  m_View->ChangePointValue(index, viewId);
}


void
HomologousPointExtractionModuleController
::FocusOn(IndexType id1, IndexType id2)
{
  m_FirstChangeRegionHandler->GetModel()->SetExtractRegionCenter(id1);
  m_FirstChangeRegionHandler->GetModel()->Update();
  m_SecondChangeRegionHandler->GetModel()->SetExtractRegionCenter(id2);
  m_SecondChangeRegionHandler->GetModel()->Update();
}


void
HomologousPointExtractionModuleController
::ComputeTransform()
{
  int transformType = m_View->gTransform->value();
  m_TransformType = otb::UNKNOWN;
  switch (transformType)
    {
    case 0:
      {
       m_TransformType = otb::TRANSLATION;
       break;
      }
    case 1:
      {
       m_TransformType = otb::AFFINE;
       break;
      }
    case 2:
      {
       m_TransformType = otb::SIMILARITY2D;
       break;
      }
    default:
      {
       break;
      }
    }
  
  if(m_TransformType == otb::UNKNOWN)
    {
      MsgReporter::GetInstance()->SendError("Please select a transform");
      return;
    }

  m_View->tTransform->value("");
  m_View->tError->clear();
  m_View->tMeanError->value("");

  try
    {
      m_Model->ComputeTransform(m_TransformType);
      this->SetTransformationAvailable( true );
      this->UpdateStats();
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
    }
 
}


void
HomologousPointExtractionModuleController
::UpdateStats()
{
  if(m_TransformType == otb::UNKNOWN)
    {
      MsgReporter::GetInstance()->SendError("Invalid Transform type.");
      return;
    }
  if( !this->GetTransformationAvailable() )
    {
      MsgReporter::GetInstance()->SendError("No transformation computed.");
      return;
    }
  itk::OStringStream oss;
  oss.str("");
  oss<<m_Model->GetTransformParameters();
  m_View->tTransform->value(oss.str().c_str());
   
  std::vector<double> values;
  
  ModelType::ContinuousIndexListType    outTranformedPoint = m_Model->TransformPoints(m_TransformType);
  IndexesListType  indexesList        = m_Model->GetIndexesList();
  IndexType idFix, idOut;

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
  m_View->gGuess->activate();
}


void
HomologousPointExtractionModuleController
::Evaluate( IndexType id )
{
  if(m_TransformType == otb::UNKNOWN)
    {
      MsgReporter::GetInstance()->SendError("Invalid Transform type.");
      return;
    }
  if( !this->GetTransformationAvailable() )
    {
      MsgReporter::GetInstance()->SendError("No transformation computed.");
      return;
    }
  
  ContinuousIndexType contOutPoint;

  try
    {
      contOutPoint = m_Model->TransformPoint(m_TransformType, id);//correctedId);
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
    }

  // Update view
  IndexType outPoint;
  outPoint[0] = static_cast<long>(vcl_floor(contOutPoint[0]+0.5));
  outPoint[1] = static_cast<long>(vcl_floor(contOutPoint[1]+0.5));

  m_View->ChangePointValue(outPoint,1);
  m_SecondChangeRegionHandler->GetModel()->SetExtractRegionCenter(outPoint);
  m_SecondChangeRegionHandler->GetModel()->Update();
}

void
HomologousPointExtractionModuleController
::OK()
{
  if( !this->GetTransformationAvailable() )
    {
      MsgReporter::GetInstance()->SendError("No transformation available.");
      return;
    }
  try
    {
      m_Model->OK();
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
      return;
    }
}

} // end namespace otb
