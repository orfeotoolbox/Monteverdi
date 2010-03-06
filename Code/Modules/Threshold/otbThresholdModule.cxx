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
#include "otbUniformAlphaBlendingFunction.h"
#include "otbBlendingFunction.h"


namespace otb
{
/** Constructor */
  ThresholdModule::ThresholdModule(): m_HasToGenerateLayer(true)
{
  // Instanciate an instance of thresholder
  m_ThresholdFilter    = ThresholdFilterType::New();
  m_ThresholdQuicklook = ThresholdFilterType::New();

  m_BinaryThresholdFilter    = BinaryThresholdFilterType::New();
  m_BinaryThresholdQuicklook = BinaryThresholdFilterType::New();

  // Layer Generators
  m_Generator          = ImageLayerGeneratorType::New();
  m_ThresholdGenerator = ImageLayerGeneratorType::New();

  // Instanciate extract roi filter
  m_Rescaler          = RescaleFilterType::New();
  m_RescalerQuicklook = RescaleFilterType::New();

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
  this->AddInputDescriptor<ImageType>("InputImage",otbGetTextMacro("Image to threshold"));
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

  // link rescalers
  m_Rescaler->SetInput( m_BinaryThresholdFilter->GetOutput() );
  m_RescalerQuicklook->SetInput( m_BinaryThresholdQuicklook->GetOutput() );

  if(m_InputImage.IsNull())
    {
      itkExceptionMacro(<<"Input image is NULL");
    }

  // Generate the layer
  m_Generator->SetImage(m_InputImage);
  FltkFilterWatcher qlwatcher(m_Generator->GetResampler(),0,0,200,20,otbGetTextMacro("Generating QuickLook ..."));
  m_Generator->GenerateLayer();
  m_InputImageLayer = m_Generator->GetLayer();
  m_InputImageLayer->SetName("ImageLayer");

  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(m_InputImageLayer);

  // Get the minimum & maximum values of the image
  this->UpdateSlidersExtremum();
  this->UpdateDetails();
  this->Show();
}

void ThresholdModule::Show()
{
  // Show the main window
  wMainWindow->show();
  m_View->GetScrollWidget()->show();
  m_View->GetFullWidget()->show();
}


void ThresholdModule::UpdateThresholdLayer()
{
  // First delete Threshold Layer if any
  m_RenderingModel->DeleteLayerByName("ThresholdLayer");
  m_ThresholdGenerator->GenerateQuicklookOff();

  if(guiGenericThreshold->value())
  {
    // Threshold the streamed region of the input
    m_ThresholdFilter->SetInput(m_InputImage);
    // Set Quicklook thresholder input
    m_ThresholdQuicklook->SetInput(m_InputImageLayer->GetQuicklook());

    // Generate the Thresholded layer
    m_ThresholdGenerator->SetImage(m_ThresholdFilter->GetOutput());
    m_ThresholdGenerator->SetQuicklook(m_ThresholdQuicklook->GetOutput());
  }

  if(guiBinaryThreshold->value())
  {
    // BinaryThreshold the streamed region of the input
    m_BinaryThresholdFilter->SetInput(m_InputImage);
    // Set Quicklook thresholder input
    m_BinaryThresholdQuicklook->SetInput(m_InputImageLayer->GetQuicklook());

    // Generate the BinaryThresholded layer
    m_ThresholdGenerator->SetImage(m_Rescaler->GetOutput());
    m_ThresholdGenerator->SetQuicklook(m_RescalerQuicklook->GetOutput());
  }

  m_ThresholdGenerator->GenerateLayer();
  m_ThresholdGenerator->GetLayer()->SetName("ThresholdLayer");
  m_ThresholdImageLayer = m_ThresholdGenerator->GetLayer();
  m_RenderingModel->AddLayer(m_ThresholdGenerator->GetLayer());

  // Update the layer genereation flag
  m_HasToGenerateLayer = false;
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

  guiMinDetails->step(vcl_floor( ((max - min)/100.)*100)/100.);  
  guiMaxDetails->step(vcl_floor( ((max - min)/100.)*100)/100.); 

  // Redraw the sliders
  guiMinDetails->redraw();
  guiMaxDetails->redraw();

  guiInsideValue->value(min);
  guiOutsideValue->value(max);

  m_Rescaler->SetOutputMinimum(static_cast<PrecisionType>(min));
  m_Rescaler->SetOutputMaximum(static_cast<PrecisionType>(max));
  m_RescalerQuicklook->SetOutputMinimum(static_cast<PrecisionType>(min));
  m_RescalerQuicklook->SetOutputMaximum(static_cast<PrecisionType>(max));
}

/**
 * Update the thresholders
 */
void ThresholdModule::UpdateDetails()
{
  if(m_HasToGenerateLayer)
  {
    // Set the appropriate Layer
    this->UpdateThresholdLayer();
  }

  // Set the outside values
  m_ThresholdFilter->SetOutsideValue(guiOutsideValue->value());
  m_ThresholdQuicklook->SetOutsideValue(guiOutsideValue->value());

  //CASE 1 : Generic Threshold
  if(guiGenericThreshold->value() && !guiBinaryThreshold->value())
  {
    // Set the mode
    if(guiOutside->value())
    {
      m_ThresholdFilter->ThresholdOutside(guiMinDetails->value(),guiMaxDetails->value());
      m_ThresholdQuicklook->ThresholdOutside(guiMinDetails->value(),guiMaxDetails->value());
    }

    if(guiAbove->value())
    {
      m_ThresholdFilter->ThresholdAbove(guiMaxDetails->value());
      m_ThresholdQuicklook->ThresholdAbove(guiMaxDetails->value());
    }

    if(guiBelow->value())
    {
      m_ThresholdFilter->ThresholdBelow(guiMinDetails->value());
      m_ThresholdQuicklook->ThresholdBelow(guiMinDetails->value());
    }
  }

  // Case 2 : Binary Threshold
  if(!guiGenericThreshold->value() && guiBinaryThreshold->value())
  {
    m_BinaryThresholdFilter->SetUpperThreshold(guiMaxDetails->value());
    m_BinaryThresholdFilter->SetLowerThreshold(guiMinDetails->value());
    m_BinaryThresholdQuicklook->SetUpperThreshold(guiMaxDetails->value());
    m_BinaryThresholdQuicklook->SetLowerThreshold(guiMinDetails->value());
    
    m_BinaryThresholdFilter->SetInsideValue(guiInsideValue->value());
    m_BinaryThresholdFilter->SetOutsideValue(guiOutsideValue->value());
    m_BinaryThresholdQuicklook->SetInsideValue(guiInsideValue->value());
    m_BinaryThresholdQuicklook->SetOutsideValue(guiOutsideValue->value());
  }

  m_RenderingModel->Update();
}

void ThresholdModule::AlphaBlending()
{
  typedef Function::BlendingFunction<RGBPixelType>              BlendingFunctionType;
  typedef BlendingFunctionType::Pointer                         BlendingFunctionPointerType;
  typedef Function::UniformAlphaBlendingFunction<RGBPixelType>  UniformAlphaBlendingFunctionType;
  typedef UniformAlphaBlendingFunctionType::Pointer             UniformAlphaBlendingFunctionPointerType;

  UniformAlphaBlendingFunctionPointerType threshBlendFunc = dynamic_cast<UniformAlphaBlendingFunctionType *>
    (m_RenderingModel->GetLayerByName("ThresholdLayer")->GetBlendingFunction());
  threshBlendFunc->SetAlpha(guiAlpha->value());

  m_RenderingModel->Update();
}

void ThresholdModule::UpdateLayerGenerationFlag()
{
  m_HasToGenerateLayer = true;
}

void ThresholdModule::OK()
{
  this->ClearOutputDescriptors();
  
  int genValue = static_cast<int>(guiGenericThreshold->value());
  int binValue = static_cast<int>(guiBinaryThreshold->value());

  if( genValue != 0 && binValue == 0 )
    {
      this->AddOutputDescriptor(m_ThresholdFilter->GetOutput(),"OutputImage",otbGetTextMacro("Thresholded image"));
    }
  else if( genValue == 0 && binValue != 0 )
    {
      this->AddOutputDescriptor(m_BinaryThresholdFilter->GetOutput(),"OutputImage",otbGetTextMacro("Thresholded image"));
    }
  else
    {
      return;
    }

  this->NotifyOutputsChange();

  // Close the GUI
  this->Hide();
}

void ThresholdModule::Hide()
{
  wMainWindow->hide();
}
} // End namespace otb

