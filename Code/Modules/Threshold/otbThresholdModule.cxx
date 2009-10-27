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
#include "otbThresholdModule.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"

#include "itkMinimumMaximumImageCalculator.h"

namespace otb
{
/** Constructor */
ThresholdModule::ThresholdModule()
{
  // Instanciate an instance of thresholder
  m_ThresholdFilter    = ThresholdFilterType::New();
  m_ThresholdQuicklook = ThresholdFilterType::New();

  // Layer Generators
  m_Generator          = ImageLayerGeneratorType::New();
  m_ThresholdGenerator = ImageLayerGeneratorType::New();

  // Instanciate extract roi filter 
  m_ExtractROI = ExtractROIFilterType::New();

  // Build a view
  m_View                 = ViewType::New();
  
  // Build a controller
  m_Controller           = WidgetControllerType::New();
  
  // Build a new rendering model
  m_RenderingModel       = RenderingModelType::New();

  // Wire the MVC
  m_View->SetModel(m_RenderingModel);
  m_View->SetController(m_Controller);
  
  
  // Add the resizing handler
  ResizingHandlerType::Pointer resizingHandler = ResizingHandlerType::New();
  resizingHandler->SetModel(m_RenderingModel);
  resizingHandler->SetView(m_View);
  m_Controller->AddActionHandler(resizingHandler);
   
  // Add the change extract region handler
  ChangeRegionHandlerType::Pointer changeHandler =ChangeRegionHandlerType::New();
  changeHandler->SetModel(m_RenderingModel);
  changeHandler->SetView(m_View);
  m_Controller->AddActionHandler(changeHandler);
   
  // Add the change scaled region handler
  ChangeScaledRegionHandlerType::Pointer changeScaledHandler =ChangeScaledRegionHandlerType::New();
  changeScaledHandler->SetModel(m_RenderingModel);
  changeScaledHandler->SetView(m_View);
  m_Controller->AddActionHandler(changeScaledHandler);
  
  // Add the change scaled handler
  ChangeScaleHandlerType::Pointer changeScaleHandler =ChangeScaleHandlerType::New();
  changeScaleHandler->SetModel(m_RenderingModel);
  changeScaleHandler->SetView(m_View);
  m_Controller->AddActionHandler(changeScaleHandler);

  // Add the action handler for the arrow key
  ArrowKeyMoveActionHandlerType::Pointer arrowKeyMoveHandler = ArrowKeyMoveActionHandlerType::New();
  arrowKeyMoveHandler->SetModel(m_RenderingModel);
  arrowKeyMoveHandler->SetView(m_View);
  m_Controller->AddActionHandler(arrowKeyMoveHandler);

  // Build GUI
  this->CreateGUI();

  // Add the widgets to the GUI
  gFull->add(m_View->GetFullWidget());
  gFull->resizable(m_View->GetFullWidget());
  m_View->GetFullWidget()->resize(gFull->x(),gFull->y(),
                                  gFull->w(),gFull->h());
  
  gScroll->add(m_View->GetScrollWidget());
  gScroll->resizable(m_View->GetScrollWidget());
  m_View->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),
                                    gScroll->w(),gScroll->h());
  
  //Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage","Image to threshold");
}

/** Destructor */
ThresholdModule::~ThresholdModule()
{}

/** PrintSelf method */
void ThresholdModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void ThresholdModule::Run()
{
  // Get input image
  m_InputImage = this->GetInputData<ImageType>("InputImage");

  if(m_InputImage.IsNull())
    {
      itkExceptionMacro(<<"Input image is NULL");
    }
  
  m_ThresholdFilter->SetInput(m_InputImage);

  // Generate the layer
  //ImageLayerGeneratorPointerType generator = ImageLayerGeneratorType::New();
  m_Generator->SetImage(m_InputImage);
  FltkFilterWatcher qlwatcher(m_Generator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  m_Generator->GenerateLayer();
  m_InputImageLayer = m_Generator->GetLayer();
  
  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(m_Generator->GetLayer());
  
  // Set tresholders inputs : Quicklook
  m_ThresholdQuicklook->SetInput(m_Generator->GetQuicklook());

  //ImageLayerGeneratorType::Pointer thresholdGenerator = ImageLayerGeneratorType::New();
  m_ThresholdGenerator->SetImage(m_ThresholdFilter->GetOutput());
  m_ThresholdGenerator->GenerateQuicklookOff();
  m_ThresholdGenerator->SetQuicklook(m_ThresholdQuicklook->GetOutput());
  m_ThresholdGenerator->GenerateLayer();

  m_RenderingModel->AddLayer(m_ThresholdGenerator->GetLayer());

  // Get the minimum & maximum values of the image
  this->UpdateSlidersExtremum();
  
  // Show the main window
  wMainWindow->show();
  m_View->GetScrollWidget()->show();
  m_View->GetFullWidget()->show();

  this->UpdateDetails();
}



void ThresholdModule::UpdateSlidersExtremum()
{
  // Get the extremum of the image
  typedef itk::MinimumMaximumImageCalculator<ImageType>      ExtremumCalculatorType;
  ExtremumCalculatorType::Pointer extremumCalculator = ExtremumCalculatorType::New();
  extremumCalculator->SetImage(m_InputImage);
  extremumCalculator->ComputeMinimum();
  extremumCalculator->ComputeMaximum();
  
  // Edit the sliders with the min and the max.
  double min = extremumCalculator->GetMinimum();
  double max = extremumCalculator->GetMaximum();

  guiMinDetails->range(min,max);
  guiMaxDetails->range(min,max);
  guiMinDetails->value(min);
  guiMaxDetails->value(max);
  
  // Redraw the sliders
  guiMinDetails->redraw();
  guiMaxDetails->redraw();
}

void ThresholdModule::OK()
{
  wMainWindow->hide();
}


/**
 * Update the thresholders 
 */
void ThresholdModule::UpdateDetails()
{
  m_ThresholdFilter->SetInsideValue(guiInsideValue->value());
  m_ThresholdFilter->SetOutsideValue(guiOutsideValue->value());
  m_ThresholdFilter->SetLowerThreshold(guiMinDetails->value());
  m_ThresholdFilter->SetUpperThreshold(guiMaxDetails->value());
  
  m_ThresholdQuicklook->SetInsideValue(guiInsideValue->value());
  m_ThresholdQuicklook->SetOutsideValue(guiOutsideValue->value());
  m_ThresholdQuicklook->SetLowerThreshold(guiMinDetails->value());
  m_ThresholdQuicklook->SetUpperThreshold(guiMaxDetails->value());
  m_ThresholdQuicklook->Update();

  m_RenderingModel->Update();
}


} // End namespace otb


