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
#include "otbResampleModule.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"

#include "itkMinimumMaximumImageCalculator.h"
#include "otbUniformAlphaBlendingFunction.h"
#include "otbBlendingFunction.h"
#include "otbMath.h"

#include "itkNearestNeighborInterpolateImageFunction.h"


namespace otb
{
  
/** Constructor */
  ResampleModule::ResampleModule(): m_HasToGenerateLayer(true)
{
  
  m_ResampleFilter = ResampleFilterType::New();
  
  m_ScalableTransform = ScalableTransformType::New();
  m_ScalableTransform->SetIdentity();
  
  m_ResampleFilter->SetTransform( m_ScalableTransform );
  
  // Interpolator
  m_NearestNeighborInterpolator = NearestNeighborInterpolateType::New();
  
  m_BCOInterpolator = BCOInterpolatorType::New();
  
  m_LinearInterpolator = LinearInterpolateType::New();
  
  m_ResampleFilter->SetInterpolator( m_NearestNeighborInterpolator );
  
  // Layer Generators
  m_Generator          = ImageLayerGeneratorType::New();
  m_ResampleGenerator  = ImageLayerGeneratorType::New();
  
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
  m_ChangeScaleHandler = ChangeScaleHandlerType::New();
  m_ChangeScaleHandler->SetModel(m_RenderingModel);
  m_ChangeScaleHandler->SetView(m_View);
  m_Controller->AddActionHandler(m_ChangeScaleHandler);

  // Add the action handler for the arrow key
  ArrowKeyMoveActionHandlerType::Pointer arrowKeyMoveHandler = ArrowKeyMoveActionHandlerType::New();
  arrowKeyMoveHandler->SetModel(m_RenderingModel);
  arrowKeyMoveHandler->SetView(m_View);
  m_Controller->AddActionHandler(arrowKeyMoveHandler);

  // Build GUI
  this->CreateGUI();

  pBusyBar->minimum(0);
  pBusyBar->maximum(1);

  // Add the widgets to the GUI
  gFull->add(m_View->GetFullWidget());
  gFull->resizable(m_View->GetFullWidget());
  m_View->GetFullWidget()->resize(gFull->x(), gFull->y(),
                                  gFull->w(), gFull->h());

  gScroll->add(m_View->GetScrollWidget());
  gScroll->resizable(m_View->GetScrollWidget());
  m_View->GetScrollWidget()->resize(gScroll->x(), gScroll->y(),
                                    gScroll->w(), gScroll->h());

  // Populate combo box Interpalator
  guiInterpolator->clear();
  
  guiInterpolator->add("Nearest Neighbor");
  guiInterpolator->add("Bi-cubic");
  guiInterpolator->add("Linear");
  guiInterpolator->value( (int) NEARESTNEIGHBOR );
  
  //Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Image to resample"));
}

/** Destructor */
ResampleModule::~ResampleModule()
{}

/** PrintSelf method */
void ResampleModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void ResampleModule::Run()
{
  // Get input image
  m_InputImage = this->GetInputData<ImageType>("InputImage");

  if(m_InputImage.IsNull()) {
    itkExceptionMacro(<<"Input image is NULL");
  }
  
  ImageType::PointType origin = m_InputImage->GetOrigin();
  ImageType::SpacingType spacing = m_InputImage->GetSpacing();
    
  // Generate the layer
  m_Generator->SetImage(m_InputImage);
  FltkFilterWatcher qlwatcher(m_Generator->GetProgressSource(), 0, 0, 200, 20, otbGetTextMacro("Generating QuickLook ..."));
  m_Generator->GenerateLayer();
  m_InputImageLayer = m_Generator->GetLayer();
  m_InputImageLayer->SetName("ImageLayer");

  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(m_InputImageLayer);
  
  // Set size of image
  guiWidth->value( m_InputImage->GetLargestPossibleRegion().GetSize()[0] );
  guiHeight->value( m_InputImage->GetLargestPossibleRegion().GetSize()[1] );
  
  m_ResampleFilter->SetInput(m_InputImage);
  
  m_RenderingModel->Update();
  m_Generator->GetRenderingFunction()->SetAutoMinMax(false);

  this->Show();
}

void ResampleModule::Show()
{
  // Show the main window
  wMainWindow->show();
  m_View->GetScrollWidget()->show();
  m_View->GetFullWidget()->show();
}


void ResampleModule::UpdateResampleLayer()
{
  m_RenderingModel->ClearLayers();
  m_ResampleGenerator  = ImageLayerGeneratorType::New();

  // Keep the same min/max
  m_ResampleGenerator->SetRenderingFunction(m_Generator->GetRenderingFunction());

  unsigned int x, y;
  x=wMainWindow->x()+(wMainWindow->w()-200)/2;
  y=wMainWindow->y()+(wMainWindow->h()-20)/2;
 
   // Show busy bar
  this->pBusyBar->value(1);
  this->pBusyBar->show();
  Fl::check();

  m_ResampleGenerator->SetImage(m_ResampleFilter->GetOutput());
  m_ResampleGenerator->GenerateLayer();

  // Hide busy bar
  this->pBusyBar->value(0);
  this->pBusyBar->hide();
  Fl::check();


  // Add Layer
  m_ResampleGenerator->GetLayer()->SetName("ResampleLayer");

  m_RenderingModel->AddLayer(m_ResampleGenerator->GetLayer());
  m_ResampleImageLayer = m_ResampleGenerator->GetLayer();
  
  m_HasToGenerateLayer = false;
  
  
}

/**
 * Update size
 */
void ResampleModule::UpdateSize()
{
  m_ResampleFilter = ResampleFilterType::New();
  m_ScalableTransform = ScalableTransformType::New();
  
  m_ResampleFilter->SetInput(m_InputImage);
  
  
  ImageType::IndexType origin = m_InputImage->GetLargestPossibleRegion().GetIndex();
  ImageType::SpacingType spacing = m_InputImage->GetSpacing();

  
  ImageType::IndexType center;
  center[0] = origin[0] + m_InputImage->GetLargestPossibleRegion().GetSize()[0] / 2.0;
  center[1] = origin[1] + m_InputImage->GetLargestPossibleRegion().GetSize()[1] / 2.0;

  
  ImageType::PointType centerPoint;
  m_InputImage->TransformIndexToPhysicalPoint(center, centerPoint);
  
  //image boundary
  ImageType::IndexType ULindex, URindex, LRindex, LLindex;
  
  ULindex[0]=origin[0];
  ULindex[1]=origin[1];
  URindex[0]=origin[0]+ m_InputImage->GetLargestPossibleRegion().GetSize()[0];
  URindex[1]=origin[1];
  LRindex[0]=origin[0]+ m_InputImage->GetLargestPossibleRegion().GetSize()[0];
  LRindex[1]=origin[1]+ m_InputImage->GetLargestPossibleRegion().GetSize()[1];
  LLindex[0]=origin[0];
  LLindex[1]=origin[1]+ m_InputImage->GetLargestPossibleRegion().GetSize()[1];
  
  ImageType::PointType orig, ULpoint, URpoint, LRpoint, LLpoint;
  m_InputImage->TransformIndexToPhysicalPoint(ULindex, ULpoint);
  m_InputImage->TransformIndexToPhysicalPoint(URindex, URpoint);
  m_InputImage->TransformIndexToPhysicalPoint(LRindex, LRpoint);
  m_InputImage->TransformIndexToPhysicalPoint(LLindex, LLpoint);

  //angle of rotation
  ScalarType rot_angle = (ScalarType ) guiRotAngle->value();
  // Scale Transform
  OutputVectorType scale;
  scale[0] = (ScalarType )( 1.0 / guiXFactor->value() );
  scale[1] = (ScalarType )( 1.0 / guiYFactor->value() );

  m_ScalableTransform->SetIdentity();
  if(spacing[0] > 0 && spacing[1] > 0) m_ScalableTransform->Rotate2D( rot_angle * CONST_PI_180 );
  else m_ScalableTransform->Rotate2D( - rot_angle * CONST_PI_180 );
  m_ScalableTransform->SetCenter( centerPoint );
  m_ScalableTransform->Scale( scale );
  
  //inverse transform
  ScalableTransformType::Pointer inverseTransform = ScalableTransformType::New();
  m_ScalableTransform->GetInverse(inverseTransform);
  m_ResampleFilter->SetTransform( m_ScalableTransform );
  
  ImageType::PointType ULpointTrans, URpointTrans, LRpointTrans, LLpointTrans, CenterPointTrans;
  
  ULpointTrans=inverseTransform->TransformPoint(ULpoint);
  URpointTrans=inverseTransform->TransformPoint(URpoint);
  LRpointTrans=inverseTransform->TransformPoint(LRpoint);
  LLpointTrans=inverseTransform->TransformPoint(LLpoint);
  CenterPointTrans=inverseTransform->TransformPoint(centerPoint);
  
  //compute min and max
  std::vector<ImageType::PointType>   voutput;
  voutput.push_back(ULpointTrans);
  voutput.push_back(URpointTrans);
  voutput.push_back(LRpointTrans);
  voutput.push_back(LLpointTrans);
  
  double minX = voutput[0][0];
  double maxX = voutput[0][0];
  double minY = voutput[0][1];
  double maxY = voutput[0][1];

  for(unsigned int i = 0; i<voutput.size(); i++)
    {
      // Origins
      if ( minX > voutput[i][0] )
       {
         minX = voutput[i][0];
       }
      if ( minY > voutput[i][1] )
       {
         minY = voutput[i][1];
       }

      // Sizes
      if ( maxX < voutput[i][0] )
       {
         maxX = voutput[i][0];
       }
      if ( maxY < voutput[i][1] )
       {
         maxY = voutput[i][1];
       }
    }
  
  if( spacing[0] > 0 ) orig[0] = minX;
  else orig[0] = maxX;
  
  if( spacing[1] > 0 ) orig[1] = minY;
  else orig[1] = maxY;

  m_ResampleFilter->SetOutputOrigin( orig );
  
  //size of output image
  ResampleFilterType::SizeType size;
  size[0]=vcl_abs(maxX-minX);
  size[1]=vcl_abs(maxY-minY);
  
  // Evaluate spacing
  ImageType::SpacingType OutputSpacing;
  OutputSpacing=spacing;
  
  m_ResampleFilter->SetOutputSpacing( OutputSpacing );
  ImageType::PixelType defPix;
  defPix.SetSize( m_InputImage->GetNumberOfComponentsPerPixel() );
  defPix.Fill(0);
  m_ResampleFilter->SetEdgePaddingValue(defPix);

  //m_ResampleFilter->SetDeformationFieldSpacing( OutputSpacing );
  
  // Evaluate size
  ResampleFilterType::SizeType RecomputedSize;
  RecomputedSize[0] = static_cast<unsigned int>(vcl_floor(vcl_abs(size[0]/OutputSpacing[0])));
  RecomputedSize[1] = static_cast<unsigned int>(vcl_floor(vcl_abs(size[1]/OutputSpacing[1])));

  //m_ResampleFilter->SetSize( RecomputedSize );
  m_ResampleFilter->SetOutputSize( RecomputedSize );

  guiWidth->value( RecomputedSize[0] );
  guiHeight->value( RecomputedSize[1] );
  
}

/**
 * Update the resamplers
 */
void ResampleModule::UpdateDetails()
{
  UpdateSize();
  this->ChangeInterpolator();
  UpdateResampleLayer();
  m_RenderingModel->Update();
}

void ResampleModule::OK()
{
  this->ClearOutputDescriptors();
  
  this->AddOutputDescriptor(m_ResampleFilter->GetOutput(),
                         "OutputImage",
                         otbGetTextMacro("Resampled image"));
  
  this->NotifyOutputsChange();

  // Close the GUI
  this->Hide();
}

void ResampleModule::Hide()
{
  wMainWindow->hide();
}
  

/**
 * Change Angle of rotation
 */
void ResampleModule::ChangeAngleRotation()
{
   this->UpdateSize();
}

/**
 * Change Interpolator
 */
void ResampleModule::ChangeInterpolator()
{

  switch ( guiInterpolator->value() ) {

  case NEARESTNEIGHBOR :
  {
    m_NearestNeighborInterpolator = NearestNeighborInterpolateType::New();
    m_ResampleFilter->SetInterpolator( m_NearestNeighborInterpolator );
    break;
  }
    
  case BICUBIC :
  {
    m_BCOInterpolator = BCOInterpolatorType::New();
    m_ResampleFilter->SetInterpolator( m_BCOInterpolator );
    break;
  }
   
  case LINEAR:
  {
    m_LinearInterpolator = LinearInterpolateType::New();
    m_ResampleFilter->SetInterpolator( m_LinearInterpolator );
    break;
  }
  
  default:
    break;
  }
}

/**
 * Change X Factor
 */
void ResampleModule::ChangeXFactor()
{
  if( guiKeepProportion->value()==1 )
    guiYFactor->value( guiXFactor->value() );
  
  if( vcl_abs(guiXFactor->value()) < 1e-6 )
    {
    guiWidth->value( 0 );
    if( guiKeepProportion->value()==1)
      guiHeight->value( 0 );
    return;
    }

  this->UpdateSize();
  
}
  
/**
 * Change Y Factor
 */
void ResampleModule::ChangeYFactor()
{
  if( guiKeepProportion->value()==1)
    guiXFactor->value( guiYFactor->value() );

  if( vcl_abs(guiYFactor->value()) < 1e-6 )
    {
    guiHeight->value( 0 );
    if( guiKeepProportion->value()==1)
      guiWidth->value( 0 );
    return;
    }

  this->UpdateSize();
}

/**
 * Toggle proportion
 */
void ResampleModule::ToggleProportion()
{
  // Change image if toggled
  if( guiKeepProportion->value()==1)
    {
    // Toggled
    guiYFactor->value( guiXFactor->value() );

    if( vcl_abs(guiYFactor->value()) < 1e-6 )
      {
      guiHeight->value( 0 );
      guiWidth->value( 0 );
      }
    else
      this->UpdateSize();
    }
}

} // End namespace otb
