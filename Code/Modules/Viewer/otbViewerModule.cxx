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
#ifndef __otbViewerModule_cxx
#define __otbViewerModule_cxx

#include <string>
#include <sstream>
#include "otbViewerModule.h"
#include <FLU/Flu_File_Chooser.h>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Text_Buffer.H>
#include "otbFltkFilterWatcher.h"
#include "otbMsgReporter.h"
#include "otbViewerConst.h"
#include <boost/algorithm/string.hpp>

namespace otb
{

/**
 * Constructor
 */
ViewerModule::ViewerModule() :
  m_CurrentOpaqueImage(0), m_CurrentTransparentImage(0), m_RenderingModel(), m_PixelDescriptionModel(), m_View(),
      m_PixelDescriptionView(), m_CurveWidget(), m_Controller(), m_WindowsLayout(PACKED_WINDOWS_LAYOUT),
      m_DisplayMode(SLIDESHOW_DISPLAY_MODE), m_DisplayedLabel("+ "), m_UndisplayedLabel("- "),
      m_GenerateRandomColor(true)
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOn();

  // Color Definition
  m_Red.Fill(0.0);
  m_Green.Fill(0.0);
  m_Blue.Fill(0.0);
  m_Grey.Fill(0.5);
  m_Red[0] = 1.0;
  m_Red[3] = 0.5;
  m_Green[1] = 1.0;
  m_Green[3] = 0.5;
  m_Blue[2] = 1.0;
  m_Blue[3] = 0.5;

  // Build a new rendering model
  m_RenderingModel = RenderingModelType::New();
  m_PixelDescriptionModel = PixelDescriptionModelType::New();
  m_PixelDescriptionModel->SetLayers(m_RenderingModel->GetLayers());

  // Build a view
  m_View = ViewType::New();
  m_PixelDescriptionView = PixelDescriptionViewType::New();

  // Build a controller
  m_Controller = WidgetControllerType::New();

  // Standard rendering function : Needed for the histogram handler
  m_RenderingFunctionList = RenderingFunctionListType::New();

  // Vertical asymptotes for each channel
  m_BlueVaCurveR = VerticalAsymptoteCurveType::New();
  m_BlueVaCurveL = VerticalAsymptoteCurveType::New();

  m_GreenVaCurveR = VerticalAsymptoteCurveType::New();
  m_GreenVaCurveL = VerticalAsymptoteCurveType::New();

  m_RedVaCurveR = VerticalAsymptoteCurveType::New();
  m_RedVaCurveL = VerticalAsymptoteCurveType::New();

  // Build the curve widget
  m_CurveWidget = CurvesWidgetType::New();
  m_CurveWidget->SetXAxisLabel("Pixels");
  m_CurveWidget->SetYAxisLabel("Frequency");

  // Curvet Widget Instanciation
  m_BlueCurveWidgetGroup = CurvesWidgetType::New();
  m_GreenCurveWidgetGroup = CurvesWidgetType::New();
  m_RedCurveWidgetGroup = CurvesWidgetType::New();

  m_BlueCurveWidgetGroup->SetIdentifier("BlueCurve");
  m_GreenCurveWidgetGroup->SetIdentifier("GreenCurve");
  m_RedCurveWidgetGroup->SetIdentifier("RedCurve");

  m_BlueCurveWidgetGroup->SetController(m_Controller);
  m_GreenCurveWidgetGroup->SetController(m_Controller);
  m_RedCurveWidgetGroup->SetController(m_Controller);

  // Wire the MVC
  m_View->SetModel(m_RenderingModel);
  m_View->SetController(m_Controller);
  m_PixelDescriptionView->SetModel(m_PixelDescriptionModel);

  // Add the resizing handler
  ResizingHandlerType::Pointer resizingHandler = ResizingHandlerType::New();
  resizingHandler->SetModel(m_RenderingModel);
  resizingHandler->SetView(m_View);
  m_Controller->AddActionHandler(resizingHandler);

  // Add the change extract region handler
  ChangeRegionHandlerType::Pointer changeHandler = ChangeRegionHandlerType::New();
  changeHandler->SetModel(m_RenderingModel);
  changeHandler->SetView(m_View);
  m_Controller->AddActionHandler(changeHandler);

  // Add the change scaled region handler
  ChangeScaledRegionHandlerType::Pointer changeScaledHandler = ChangeScaledRegionHandlerType::New();
  changeScaledHandler->SetModel(m_RenderingModel);
  changeScaledHandler->SetView(m_View);
  m_Controller->AddActionHandler(changeScaledHandler);

  // Add the change scaled handler
  ChangeScaleHandlerType::Pointer changeScaleHandler = ChangeScaleHandlerType::New();
  changeScaleHandler->SetModel(m_RenderingModel);
  changeScaleHandler->SetView(m_View);
  m_Controller->AddActionHandler(changeScaleHandler);

  // Add the pixel description action handler
  PixelDescriptionActionHandlerType::Pointer pixelActionHandler = PixelDescriptionActionHandlerType::New();
  pixelActionHandler->SetView(m_View);
  pixelActionHandler->SetModel(m_PixelDescriptionModel);
  m_Controller->AddActionHandler(pixelActionHandler);

  // Add the action handler for the arrow key
  ArrowKeyMoveActionHandlerType::Pointer arrowKeyMoveHandler = ArrowKeyMoveActionHandlerType::New();
  arrowKeyMoveHandler->SetModel(m_RenderingModel);
  arrowKeyMoveHandler->SetView(m_View);
  m_Controller->AddActionHandler(arrowKeyMoveHandler);

  // Add the full widget handler
  DragFullActionHandlerType::Pointer dragHandler = DragFullActionHandlerType::New();
  dragHandler->SetModel(m_RenderingModel);
  dragHandler->SetView(m_View);
  m_Controller->AddActionHandler(dragHandler);

  // Add the blue histogram handler
  m_BlueHistogramHandler = HistogramActionHandlerType::New();
  m_BlueHistogramHandler->SetModel(m_RenderingModel);
  m_BlueHistogramHandler->SetView(m_View);
  m_BlueHistogramHandler->SetCurve(m_BlueCurveWidgetGroup);
  m_BlueHistogramHandler->SetLeftAsymptote(m_BlueVaCurveL);
  m_BlueHistogramHandler->SetRightAsymptote(m_BlueVaCurveR);
  m_Controller->AddActionHandler(m_BlueHistogramHandler);

  // Add the green histogram handler
  m_GreenHistogramHandler = HistogramActionHandlerType::New();
  m_GreenHistogramHandler->SetModel(m_RenderingModel);
  m_GreenHistogramHandler->SetView(m_View);
  m_GreenHistogramHandler->SetCurve(m_GreenCurveWidgetGroup);
  m_GreenHistogramHandler->SetLeftAsymptote(m_GreenVaCurveL);
  m_GreenHistogramHandler->SetRightAsymptote(m_GreenVaCurveR);
  m_Controller->AddActionHandler(m_GreenHistogramHandler);

  // Add the red histogram handler
  m_RedHistogramHandler = HistogramActionHandlerType::New();
  m_RedHistogramHandler->SetModel(m_RenderingModel);
  m_RedHistogramHandler->SetView(m_View);
  m_RedHistogramHandler->SetCurve(m_RedCurveWidgetGroup);
  m_RedHistogramHandler->SetLeftAsymptote(m_RedVaCurveL);
  m_RedHistogramHandler->SetRightAsymptote(m_RedVaCurveR);
  m_Controller->AddActionHandler(m_RedHistogramHandler);

  // Managed windows layout
  m_SplittedWindows = SplittedWidgetManagerType::New();
  m_PackedWindows = PackedWidgetManagerType::New();

  if (m_WindowsLayout == SPLITTED_WINDOWS_LAYOUT)
    {
    m_DisplayWindow = m_SplittedWindows;
    }
  else
    {
    m_DisplayWindow = m_PackedWindows;
    }
  m_DisplayWindow->RegisterFullWidget(m_View->GetFullWidget());
  m_DisplayWindow->RegisterScrollWidget(m_View->GetScrollWidget());
  m_DisplayWindow->RegisterZoomWidget(m_View->GetZoomWidget());
  m_DisplayWindow->RegisterPixelDescriptionWidget(m_PixelDescriptionView->GetPixelDescriptionWidget());
  m_DisplayWindow->RegisterHistogramWidget(m_CurveWidget);

  // Instanciation of the Image To VectorImage filter list
  m_CastFilterList = CastFilterListType::New();

  // Instanciation of the LabeledImage To VectorImage filter list
  m_LabeledCastFilterList = LabeledCastFilterListType::New();

  // Data List Instance
  m_VectorDataList = VectorDataListType::New();
  m_InputImageList = ImageListType::New();
  m_InputImageLayerList = ImageLayerListType::New();

  // Describe inputs
  this->AddInputDescriptor<ImageType> ("InputImage", "Image to display", false, true);
  this->AddTypeToInputDescriptor<SingleImageType> ("InputImage");
  this->AddTypeToInputDescriptor<LabeledImageType> ("InputImage");
  this->AddTypeToInputDescriptor<FloatImageWithQuicklook> ("InputImage");
  this->AddInputDescriptor<VectorDataType> ("VectorData", "Vector data to display", true, true);

  // Build GUI
  this->Build();

  // build the DEM GUI
  this->BuildDEM();

  // build the Screen shot GUI
  this->BuildScreenShot();
}

/**
 * Destructor
 */
ViewerModule::~ViewerModule()
{
}

/**
 * PrintSelf method
 */
void ViewerModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/**
 * The custom run command
 */
void ViewerModule::Run()
{
  // While the viewer is shown, it is busy
  this->BusyOn();

  ImageType::SizeType firstImageSize;

  // Load and process all selected images
  for (unsigned int i = 0; i < GetNumberOfInputDataByKey("InputImage"); i++)
    {
    std::string desc;

    // Get the input image
    ImageType::Pointer image = this->GetInputData<ImageType> ("InputImage", i);
    FloatImageWithQuicklook::Pointer imageQL = this->GetInputData<FloatImageWithQuicklook> ("InputImage", i);

    // If the image pointer is null, the data is probably a single band
    // (single image)
    if (image.IsNull())
      {
      // Try to get a single image
      // If the input image is an otb::Image instead of VectorImage then
      // cast it in Vector Image and continue the processing
      SingleImageType::Pointer singleImage = this->GetInputData<SingleImageType> ("InputImage", i);
      if (singleImage.IsNotNull())
        {
        CastSingleImageFilter::Pointer castFilter = CastSingleImageFilter::New();
        m_CastFilterList->PushBack(castFilter);

        castFilter->SetInput(singleImage);
        image = castFilter->GetOutput();

        // Get the description
        desc = this->GetInputDataDescription<SingleImageType> ("InputImage", i);
        }

      LabeledImageType::Pointer labeledImage = this->GetInputData<LabeledImageType> ("InputImage", i);
      if (labeledImage.IsNotNull())
        {
        CastLabeledImageFilter::Pointer labeledCastFilter = CastLabeledImageFilter::New();
        m_LabeledCastFilterList->PushBack(labeledCastFilter);

        labeledCastFilter->SetInput(labeledImage);
        image = labeledCastFilter->GetOutput();

        // Get the description
        desc = this->GetInputDataDescription<LabeledImageType> ("InputImage", i);
        }

      if (imageQL.IsNotNull())
        {
        image = imageQL->GetImage();

        // Get the description
        desc = this->GetInputDataDescription<LabeledImageType> ("InputImage", i);
        }

      // First check if there is actually an input image
      if (image.IsNull())
        {
        MsgReporter::GetInstance()->SendError("The image pointer is null, there is nothing to display. "
          "You probably forgot to set the image.");
        this->Quit();
        return;
        }
      }
    else
      {
      // Get the description
      desc = this->GetInputDataDescription<ImageType> ("InputImage", i);
      }

    // Update image info for further use
    image->UpdateOutputInformation();

    // Drop the images which have a different size than the first one.
    if (i == 0)
      {
      firstImageSize = image->GetLargestPossibleRegion().GetSize();
      }
    else
      {
      ImageType::SizeType currentImageSize = image->GetLargestPossibleRegion().GetSize();
      if (currentImageSize != firstImageSize)
        {
        std::ostringstream oss;
        oss << "Image '" << this->GetInputDataDescription<ImageType> ("InputImage", i)
            << "' dropped because its size is different that of the first image (" << currentImageSize << " vs "
            << firstImageSize << ")";
        MsgReporter::GetInstance()->SendError(oss.str());
        continue;
        }
      }

    // Generate the layer
    ImageLayerGeneratorPointerType generator = ImageLayerGeneratorType::New();
    generator->SetImage(image);

    if (imageQL.IsNotNull())
      {
      generator->GenerateQuicklookOff();
      generator->SetQuicklook(imageQL->GetQuicklook());
      generator->SetSubsamplingRate(imageQL->GetShrinkFactor());
      }

    try
      {
      FltkFilterWatcher qlwatcher(generator->GetProgressSource(), 0, 0, 200, 20,
                                  "Generating QuickLook ...");
      generator->GenerateLayer();
      }
    catch (itk::ExceptionObject & err)
      {
      std::ostringstream oss;
      oss << "Problem occurred while generation of QuickLook. The following error was returned:\n";
      oss << err.GetDescription();
      MsgReporter::GetInstance()->SendError(oss.str());
      this->Quit();
      }

    ImageLayerType::Pointer imageLayer = generator->GetLayer();

    // Work with standard rendering function
    ChannelListType channels = imageLayer->GetRenderingFunction()->GetChannelList();
    StandardRenderingFunctionType::Pointer renderer = StandardRenderingFunctionType::New();
    renderer->SetChannelList(channels);
    imageLayer->SetRenderingFunction(renderer);

    BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
    blender->SetAlpha(ALPHA_BLENDING_DEFAULT);
    imageLayer->SetBlendingFunction(blender);
    imageLayer->SetVisible(true);

    // Add this image to the stack of layers
    m_InputImageList->PushBack(image);
    m_InputImageLayerList->PushBack(imageLayer);
    m_RenderingFunctionList->PushBack(renderer);
    m_ContrastStretchList.push_back(LINEAR_CONTRAST_STRETCH);
    //add a viewerSetupStruct associated to each image and initialize it.
    ViewerSetupStructType viewerSetupStruct;
    viewerSetupStruct.pStretchResolution = QUICKLOOK_STRETCH;
    viewerSetupStruct.pLowerQuantile = static_cast<double> (bLowerQuantile->value() / 100);
    viewerSetupStruct.pUpperQuantile = static_cast<double> (bUpperQuantile->value() / 100);
    viewerSetupStruct.pStandardDeviation = static_cast<double> (guiSetStandardDeviation->value());
    viewerSetupStruct.pRGBMode = true;


    m_ViewerSetupStructList.push_back(viewerSetupStruct);

    // Add the module where the image comes from.
    // This was done because if two image has the same description, the Fl_Choice will only contain 1 possible item.

    const DataObjectWrapper& dow = this->GetInputDataDescriptorByKey(std::string("InputImage")).GetNthData(i);
    std::ostringstream text;
    text << "[" << dow.GetSourceInstanceId() << "] " << desc;
    guiOpaqueImageSelection->add(text.str().c_str());
    guiTransparentImageSelection->add(text.str().c_str());
    m_AlphaBlendingList.push_back(ALPHA_BLENDING_DEFAULT);
    }

  // Select first image as current opaque image
  m_CurrentOpaqueImage = 0;
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);
  m_RenderingModel->AddLayer(imageLayer);
  guiOpaqueImageSelection->value(m_CurrentOpaqueImage);
  BlendingFunctionType::Pointer blender = dynamic_cast<BlendingFunctionType *> (imageLayer->GetBlendingFunction());
  blender->SetAlpha(ALPHA_BLENDING_OPAQUE);
  m_AlphaBlendingList[m_CurrentOpaqueImage] = ALPHA_BLENDING_OPAQUE;

  // Select next image (or the current opaque image if there is only one) as
  // current transparent image
  if (m_InputImageLayerList->Size() > 1)
    {
    m_CurrentTransparentImage = 1;
    }
  else
    {
    m_CurrentTransparentImage = 0;
    }
  guiTransparentImageSelection->value(m_CurrentTransparentImage);

  // Load and process vector data sources
  for (unsigned int i = 0; i < GetNumberOfInputDataByKey("VectorData"); i++)
    {
    VectorDataType::Pointer vdata = GetInputData<VectorDataType> ("VectorData", i);
    m_VectorDataList->PushBack(vdata);
    m_DisplayedVectorData.push_back(true);
    UpdateVectorData(i);
    std::string desc = GetInputDataDescription<VectorDataType> ("VectorData", i);
    AddNameToVectorDataBrowser(desc);
    }

  if (m_WindowsLayout == SPLITTED_WINDOWS_LAYOUT)
    {
    guiViewerSetupSplittedLayout->set();
    guiViewerSetupPackedLayout->clear();
    }
  else
    {
    guiViewerSetupSplittedLayout->clear();
    guiViewerSetupPackedLayout->set();
    m_WindowsLayout = PACKED_WINDOWS_LAYOUT;
    }

  const DataObjectWrapper& dow = this->GetInputDataDescriptorByKey(std::string("InputImage")).GetNthData(0);
  std::ostringstream title;
  title << "[" << dow.GetSourceInstanceId() << "] " << dow.GetSourceOutputKey();
  bSetupWindow->copy_label(title.str().c_str());
  m_SplittedWindows->SetLabel(title.str().c_str());
  m_PackedWindows->SetLabel(title.str().c_str());

  //Set the default stretch mode (applied on quicklook)
  guiStretchQL->set();

  // No constrast stretch (NO_CONTRAST_STRETCH)
  guiContrastStretchSelection->add("Linear 0-255");

  // Linear contrast stretch (LINEAR_CONTRAST_STRETCH)
  guiContrastStretchSelection->add("Linear X %");

  // Gaussian contrast stretch (GAUSSIAN_CONTRAST_STRETCH)
  guiContrastStretchSelection->add("Gaussian");
  guiSetStandardDeviation->value(otb::Function::GAUSSIAN_STANDARD_DEVIATION);

  // Square root contrast stretch (SQUARE_ROOT_CONTRAST_STRETCH)
  guiContrastStretchSelection->add("Square root");

  // Default value : linear stretch applied
  guiContrastStretchSelection->value(LINEAR_CONTRAST_STRETCH);
  // Quantiles are only displayed when the linear stretch is selected
  guiGroupQuantiles->show();

  guiSetStandardDeviation->hide();

  // Show everything
  m_DisplayWindow->Show();

  try
    {
    // Update the rendering model
    m_RenderingModel->Update();

    // Handle Histogram
    this->UpdateHistogramCurve();

    // Update The Histogram Tab
    this->UpdateTabHistogram();

    // Show the interface setup
    gVectorData->value(guiTabData);
    bSetupWindow->show();

    // Update channel display. If mono channel, the call following to UpdateViewerSetupWindow
    // will update the channel display.
    this->RGBSet();

    // Update the color composition window
    this->UpdateViewerSetupWindow();

    // Update NoData value
    guiSetNoData->value(
                        static_cast<double> (m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage)->GetNoDataValue()));

    //Update the viewer setup struct
    RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);
    ChannelListType channels = renderer->GetChannelList();
    if (channels.size() == 1)
    {
       m_ViewerSetupStructList[m_CurrentOpaqueImage].pRGBMode = false;
       m_ViewerSetupStructList[m_CurrentOpaqueImage].pRedChannel = channels[0] + 1;
       m_ViewerSetupStructList[m_CurrentOpaqueImage].pGreenChannel = channels[0] + 1;
       m_ViewerSetupStructList[m_CurrentOpaqueImage].pBlueChannel = channels[0] + 1;
       m_ViewerSetupStructList[m_CurrentOpaqueImage].pGrayChannel = channels[0] + 1;
    }
    else
    {
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pRGBMode = true;
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pRedChannel = channels[0] + 1;
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pGreenChannel = channels[1] + 1;
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pBlueChannel = channels[2] + 1;
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pGrayChannel = channels[0] + 1;
    }
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pNoData = static_cast<double> (renderer->GetNoDataValue());

    }
  catch (itk::ExceptionObject & err)
    {
    std::ostringstream oss;
    oss << "Problem occured while loading input image. The following error was returned:\n";
    oss << err.GetDescription();
    MsgReporter::GetInstance()->SendError(oss.str());
    this->Quit();
    }
}

/**
 *
 */
void ViewerModule::UpdateHistogramCurve()
{
  // Clear the widget
  m_CurveWidget->ClearAllCurves();

  // Select the current image layer
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);

  unsigned int pixelRepresentationSize = imageLayer->GetRenderingFunction()->GetPixelRepresentationSize();

  if (pixelRepresentationSize >= 3)
    {
    HistogramCurveType::Pointer bhistogram = HistogramCurveType::New();
    bhistogram->SetHistogramColor(m_Blue);
    bhistogram->SetLabelColor(m_Blue);
    bhistogram->SetHistogram(imageLayer->GetHistogramList()->GetNthElement(2));
    m_CurveWidget->AddCurve(bhistogram);
    }

  if (pixelRepresentationSize >= 2)
    {
    HistogramCurveType::Pointer ghistogram = HistogramCurveType::New();
    ghistogram->SetHistogramColor(m_Green);
    ghistogram->SetLabelColor(m_Green);
    ghistogram->SetHistogram(imageLayer->GetHistogramList()->GetNthElement(1));
    m_CurveWidget->AddCurve(ghistogram);
    }

  HistogramCurveType::Pointer rhistogram = HistogramCurveType::New();
  if (pixelRepresentationSize == 1)
    {
    rhistogram->SetHistogramColor(m_Grey);
    rhistogram->SetLabelColor(m_Grey);
    }
  else
    {
    rhistogram->SetHistogramColor(m_Red);
    rhistogram->SetLabelColor(m_Red);
    }

  rhistogram->SetHistogram(imageLayer->GetHistogramList()->GetNthElement(0));
  m_CurveWidget->AddCurve(rhistogram);
}

/**
 * Project the vector Data in the image projection.
 * Extract the Remotesensing region corresponding to the image.
 */
void ViewerModule::UpdateVectorData(unsigned int index)
{
  VectorDataReprojectionType::Pointer reproj = VectorDataReprojectionType::New();
  reproj->SetInputImage(m_InputImageList->GetNthElement(m_CurrentOpaqueImage));
  reproj->SetInputVectorData(m_VectorDataList->GetNthElement(index).GetPointer());
  reproj->SetUseOutputSpacingAndOriginFromImage(true);
  // if (!m_DEMDirectory.empty())
  //   {
  //   reproj->SetDEMDirectory(m_DEMDirectory);
  //   }
  reproj->Update();

  // Create a VectorData gl component
  VectorDataGlComponentType::Pointer vgl = VectorDataGlComponentType::New();
  vgl->SetVectorData(reproj->GetOutput());

  // Flag to Generate Random Color or to use saved colors (used only when DEM use is requested)
  if (m_GenerateRandomColor)
    {
    vgl->SetColor(this->SetRandomColor());
    // Add the color to the list
    m_ColorList.push_back(vgl->GetColor());
    }
  else
    {
    vgl->SetColor(m_ColorList[index]);
    }

  // Add it to the image view
  m_View->GetScrollWidget()->AddGlComponent(vgl);
  m_View->GetFullWidget()->AddGlComponent(vgl);
  m_View->GetZoomWidget()->AddGlComponent(vgl);
}

/**
 *
 */
void ViewerModule::AddNameToVectorDataBrowser(const std::string & name)
{
  std::ostringstream oss;
  oss.str("");
  oss << m_DisplayedLabel << name;

  dVDList->add(oss.str().c_str());
  dVDList->redraw();
}

/**
 *
 */
void ViewerModule::SynchronizeAll()
{
  // Handle the histogram
  UpdateHistogramCurve();

  // Update the histogram tab
  UpdateTabHistogram();

  // Reload quantiles
  UpdateQuantiles();

  // Refresh the displayed informations
  RedrawWidget();
}

/**
 *
 */
void ViewerModule::SetAlphaBlending()
{
  if (m_AlphaBlendingList[m_CurrentTransparentImage] == guiAlphaBlending->value())
    {
    return;
    }
  m_AlphaBlendingList[m_CurrentTransparentImage] = guiAlphaBlending->value();

  ShowSelectedImages();
}

/**
 *
 */
void ViewerModule::ActivateTransparencyMode()
{
  if (m_DisplayMode == TRANSPARENCY_DISPLAY_MODE)
    {
    return;
    }

  m_DisplayMode = TRANSPARENCY_DISPLAY_MODE;

  // Activate overlay related widgets
  guiTransparentImageSelection->activate();
  guiAlphaBlending->activate();

  // Deactivate slide show related widgets
  bPreviousImage->deactivate();
  bNextImage->deactivate();

  guiAlphaBlending->value(m_AlphaBlendingList[m_CurrentTransparentImage]);

  ShowSelectedImages();
}

/**
 *
 */
void ViewerModule::ActivateSlideShowMode()
{
  if (m_DisplayMode == SLIDESHOW_DISPLAY_MODE)
    {
    return;
    }

  m_DisplayMode = SLIDESHOW_DISPLAY_MODE;

  // Activate slide show related widgets
  bPreviousImage->activate();
  bNextImage->activate();

  // Deactivate overlay related widgets
  guiTransparentImageSelection->deactivate();
  guiAlphaBlending->deactivate();

  ShowSelectedImages();
}

/**
 *
 */
void ViewerModule::SetOpaqueImage()
{
  if (m_CurrentOpaqueImage == guiOpaqueImageSelection->value())
    {
    return;
    }

  m_CurrentOpaqueImage = guiOpaqueImageSelection->value();

  ShowSelectedImages();
}

/**
 *
 */
void ViewerModule::SetTransparentImage()
{
  if (m_CurrentTransparentImage == guiTransparentImageSelection->value())
    {
    return;
    }

  m_CurrentTransparentImage = guiTransparentImageSelection->value();
  guiAlphaBlending->value(m_AlphaBlendingList[m_CurrentTransparentImage]);
  ShowSelectedImages();
}

/**
 *
 */
void ViewerModule::ShowPreviousImage()
{
  int previousOpaqueImage = m_CurrentOpaqueImage;
  m_CurrentOpaqueImage = (m_CurrentOpaqueImage - 1 + m_InputImageLayerList->Size()) % m_InputImageLayerList->Size();
  guiOpaqueImageSelection->value(m_CurrentOpaqueImage);

  this->UpdateViewerSetupWindow();
  ShowSelectedImages();

}

/**
 *
 */
void ViewerModule::ShowNextImage()
{
  m_CurrentOpaqueImage = (m_CurrentOpaqueImage + 1) % m_InputImageLayerList->Size();
  guiOpaqueImageSelection->value(m_CurrentOpaqueImage);

  this->UpdateViewerSetupWindow();
  ShowSelectedImages();

}

/**
 *
 */
void ViewerModule::ShowSelectedImages()
{
  ImageLayerType::Pointer imageLayer;
  BlendingFunctionType::Pointer blender;

  m_RenderingModel->ClearLayers();

  imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);
  blender = dynamic_cast<BlendingFunctionType *> (imageLayer->GetBlendingFunction());
  blender->SetAlpha(ALPHA_BLENDING_OPAQUE);
  m_RenderingModel->AddLayer(imageLayer);

  const DataObjectWrapper& dow = this->GetInputDataDescriptorByKey(std::string("InputImage")).GetNthData(m_CurrentOpaqueImage);
  std::ostringstream title;
  title << "[" << dow.GetSourceInstanceId() << "] " << dow.GetSourceOutputKey();
  bSetupWindow->copy_label(title.str().c_str());
  m_SplittedWindows->SetLabel(title.str().c_str());
  m_PackedWindows->SetLabel(title.str().c_str());

  if ((m_DisplayMode == TRANSPARENCY_DISPLAY_MODE) && (m_CurrentOpaqueImage != m_CurrentTransparentImage))
    {
    imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentTransparentImage);
    blender = dynamic_cast<BlendingFunctionType *> (imageLayer->GetBlendingFunction());
    blender->SetAlpha(m_AlphaBlendingList[m_CurrentTransparentImage]);
    m_RenderingModel->AddLayer(imageLayer);
    }

  m_RenderingModel->Update();

  //Update the channels used to display
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);
  ChannelListType channels = renderer->GetChannelList();
  if (channels.size() == 1)
  {
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pRGBMode = false;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pRedChannel = channels[0] + 1;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pGreenChannel = channels[0] + 1;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pBlueChannel = channels[0] + 1;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pGrayChannel = channels[0] + 1;
  }
  else
  {
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pRGBMode = true;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pRedChannel = channels[0] + 1;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pGreenChannel = channels[1] + 1;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pBlueChannel = channels[2] + 1;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pGrayChannel = channels[0] + 1;
  }
  m_ViewerSetupStructList[m_CurrentOpaqueImage].pNoData = static_cast<double> (renderer->GetNoDataValue());
  this->UpdateViewerSetupWindowValues();

  SynchronizeAll();
}

/**
 *
 */
void ViewerModule::UpdateVectorDataListShowed(unsigned int selectedItem, std::string status)
{
  /* Update the ImageList using the status label "+" or "-" */
  std::string filename = this->GetInputDataDescription<VectorDataType> ("VectorData", selectedItem - 1);

  std::ostringstream oss;
  oss << status << filename;
  dVDList->text(selectedItem, oss.str().c_str());
  oss.str("");
}

/**
 *
 */
void ViewerModule::UpdateListSelectionColor()
{
  //selectedIndex
  unsigned int selectedIndex = dVDList->value();

  if (selectedIndex)
    {
    ColorType curColor = m_ColorList[selectedIndex - 1];

    // color To fl_color
    fl_color(static_cast<unsigned char> (255 * curColor[0]), static_cast<unsigned char> (255 * curColor[1]),
             static_cast<unsigned char> (255 * curColor[2]));

    //Update the text color
    dVDList->selection_color(fl_color());
    dVDList->redraw();

    //Update the ROIColorItem
    dROIColor->color(fl_color());
    dROIColor->redraw();
    }
}

/**
 *
 */
void ViewerModule::DeleteVectorData()
{
  unsigned int selectedIndex = dVDList->value();

  if (selectedIndex)
    {
    // Remove the VectorDataGl Compenent
    m_View->GetScrollWidget()->GetNthGlComponent(selectedIndex)->SetVisible(false);
    m_View->GetFullWidget()->GetNthGlComponent(selectedIndex)->SetVisible(false);
    // Below, "selectedIndex - 1" because in the Zoom Widget there is no red
    // square which is a GlComponent
    m_View->GetZoomWidget()->GetNthGlComponent(selectedIndex - 1)->SetVisible(false);

    // Update the status of the selectedItem vector data
    m_DisplayedVectorData[selectedIndex - 1] = false;
    this->UpdateVectorDataListShowed(selectedIndex, m_UndisplayedLabel);

    //Redraw all the widgets
    this->RedrawWidget();
    }
}

/**
 * Hide all the vector datas
 */
void ViewerModule::ClearAll()
{
  for (unsigned int i = 0; i < m_VectorDataList->Size(); i++)
    {
    // Hide the Gl Componenent int the view
    m_View->GetScrollWidget()->GetNthGlComponent(i + 1)->SetVisible(false);
    m_View->GetFullWidget()->GetNthGlComponent(i + 1)->SetVisible(false);
    m_View->GetZoomWidget()->GetNthGlComponent(i)->SetVisible(false);

    // Update the display status of each vector
    m_DisplayedVectorData[i] = false;
    this->UpdateVectorDataListShowed(i + 1, m_UndisplayedLabel);
    }
  //Redraw all the widgets
  this->RedrawWidget();
}

/**
 *  Display if Hidden VectorData
 */
void ViewerModule::DisplaySelectedVectorData()
{
  // Get the selected vectordata
  unsigned int selectedIndex = dVDList->value();
  if (selectedIndex > 0)
    {
    // Display the selected VD if not already displayed
    if (!m_DisplayedVectorData[selectedIndex - 1])
      {
      // Hide all displayed vector datas
      m_View->GetScrollWidget()->GetNthGlComponent(selectedIndex)->SetVisible(true);
      m_View->GetFullWidget()->GetNthGlComponent(selectedIndex)->SetVisible(true);
      m_View->GetZoomWidget()->GetNthGlComponent(selectedIndex - 1)->SetVisible(true);
      // Set visible status to selected VectorDataGl Compenents
      m_DisplayedVectorData[selectedIndex - 1] = true;
      this->UpdateVectorDataListShowed(selectedIndex, m_DisplayedLabel);
      }
    //Redraw all the widgets
    this->RedrawWidget();
    }
}

/**
 *  Display if Hidden VectorData
 */
void ViewerModule::DisplayVectorData()
{
  // loop over all the vector datas
  for (unsigned int i = 0; i < m_VectorDataList->Size(); i++)
    {
    // Display the selected VD if not already displayed
    if (!m_DisplayedVectorData[i])
      {
      m_View->GetScrollWidget()->GetNthGlComponent(i + 1)->SetVisible(true);
      m_View->GetFullWidget()->GetNthGlComponent(i + 1)->SetVisible(true);
      m_View->GetZoomWidget()->GetNthGlComponent(i)->SetVisible(true);
      m_DisplayedVectorData[i] = true;
      this->UpdateVectorDataListShowed(i + 1, m_DisplayedLabel);
      }
    }
  //Redraw all the widgets
  this->RedrawWidget();
}

/**
 *
 */
ViewerModule::ColorType ViewerModule::SetRandomColor()
{
  ColorType color;
  // Update Vector Data color
  color[0] = rand() / (RAND_MAX + 1.0);
  color[1] = rand() / (RAND_MAX + 1.0);
  color[2] = rand() / (RAND_MAX + 1.0);
  color[3] = 1.0;

  return color;
}

/**
 *
 */
void ViewerModule::ChangeROIColor()
{
  unsigned int selectedIndex = dVDList->value();

  if (selectedIndex && m_DisplayedVectorData[selectedIndex - 1])
    {
    VectorDataGlComponentType
        * selecedVectorDataGlComponent = dynamic_cast<VectorDataGlComponentType*> (m_View->GetZoomWidget()->GetNthGlComponent(
                                                                                                                                  selectedIndex
                                                                                                                                  - 1));
    ColorType curColor = selecedVectorDataGlComponent->GetColor();

    double r = (double) curColor[0];
    double g = (double) curColor[1];
    double b = (double) curColor[2];

    int ok = fl_color_chooser("Changed class color", r, g, b);

    if (ok)
      {
      curColor[0] = (float) r;
      curColor[1] = (float) g;
      curColor[2] = (float) b;

      fl_color(static_cast<unsigned char> (255 * curColor[0]), static_cast<unsigned char> (255 * curColor[1]),
               static_cast<unsigned char> (255 * curColor[2]));

      // Change the color of the text
      dVDList->selection_color(fl_color());
      dVDList->redraw();
      // Change the color of the RoiButton
      dROIColor->color(fl_color());
      // Change the color of the VectorData
      selecedVectorDataGlComponent->SetColor(curColor);
      m_ColorList[selectedIndex - 1] = curColor;
      this->RedrawWidget();
      }
    }
}

/**
 *
 */
void ViewerModule::RedrawWidget()
{
  m_DisplayWindow->Refresh();
}

/**
 *
 */
void ViewerModule::UseDEM()
{
  // show the DEM GUI
  if (bDEM->value())
    {
    wDEM->show();
    }
}

/**
 *
 */
void ViewerModule::UpdateDEMSettings()
{
  if (gDEMPath->value())
    {
    // Copy the DEM pathname
    m_DEMDirectory = gDEMPath->value();

    typedef otb::DEMHandler DEMHandlerType;
    DEMHandlerType::Pointer DEMTest = DEMHandlerType::Instance();

    if (!DEMTest->IsValidDEMDirectory(m_DEMDirectory.c_str()))
      {
      m_DEMDirectory = "";
      MsgReporter::GetInstance()->SendError("invalid DEM directory, no DEM directory has been set.");
      }

    DEMTest->OpenDEMDirectory(m_DEMDirectory);
    // Delete the vector data
    for (unsigned int i = 0; i < m_VectorDataList->Size(); i++)
      {
      m_View->GetScrollWidget()->RemoveGlComponent(1);
      m_View->GetFullWidget()->RemoveGlComponent(1);
      m_View->GetZoomWidget()->RemoveGlComponent(0);
      }

    // Deactivate the Random Generation of the color when render vector
    m_GenerateRandomColor = false;

    // Reproject using the DEM this time
    for (unsigned int i = 0; i < m_VectorDataList->Size(); i++)
      {
      this->UpdateVectorData(i);
      }

    // Refresh widgets
    this->RedrawWidget();
    }
}


/**
 * Update values in the gui (for color composition and contrast) with the lattest values used for the current image.
 */
void ViewerModule::UpdateViewerSetupWindowValues()
{
  //Update values in the gui (for color composition and contrast) with the lattest values used for the current image.
  guiRedChannelChoice->value(m_ViewerSetupStructList[m_CurrentOpaqueImage].pRedChannel);
  guiGreenChannelChoice->value(m_ViewerSetupStructList[m_CurrentOpaqueImage].pGreenChannel);
  guiBlueChannelChoice->value(m_ViewerSetupStructList[m_CurrentOpaqueImage].pBlueChannel);
  guiGrayscaleChannelChoice->value(m_ViewerSetupStructList[m_CurrentOpaqueImage].pGrayChannel);
  if (m_ViewerSetupStructList[m_CurrentOpaqueImage].pRGBMode)
    {
    guiViewerSetupColorMode->set();
    guiViewerSetupGrayscaleMode->clear();
    guiGrayscaleChannelChoice->deactivate();
    guiRedChannelChoice->activate();
    guiGreenChannelChoice->activate();
    guiBlueChannelChoice->activate();
    }
  else
    {
    guiViewerSetupGrayscaleMode->set();
    guiViewerSetupColorMode->clear();
    guiRedChannelChoice->deactivate();
    guiGreenChannelChoice->deactivate();
    guiBlueChannelChoice->deactivate();
    guiGrayscaleChannelChoice->activate();
    }

  guiContrastStretchSelection->value(m_ContrastStretchList[m_CurrentOpaqueImage]);
  if(m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution == FULL_STRETCH)
    {
      guiStretchQL->clear();
      guiStretchFull->set();
      guiStretchZoom->clear();
    }
  else if (m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution == ZOOM_STRETCH)
    {
      guiStretchQL->clear();
      guiStretchFull->clear();
      guiStretchZoom->set();
    }
  else
    {
      guiStretchQL->set();
      guiStretchFull->clear();
      guiStretchZoom->clear();
    }
  if (guiContrastStretchSelection->value() == LINEAR_CONTRAST_STRETCH || guiContrastStretchSelection->value() == SQUARE_ROOT_CONTRAST_STRETCH)
    {
    guiSetStandardDeviation->hide();
    guiGroupQuantiles->show();
    bLowerQuantile->value(m_ViewerSetupStructList[m_CurrentOpaqueImage].pLowerQuantile * 100);
    bUpperQuantile->value(m_ViewerSetupStructList[m_CurrentOpaqueImage].pUpperQuantile * 100);
    }
  else
    if (guiContrastStretchSelection->value() == GAUSSIAN_CONTRAST_STRETCH)
      {
      guiGroupQuantiles->hide();
      guiSetStandardDeviation->show();
      guiSetStandardDeviation->value(m_ViewerSetupStructList[m_CurrentOpaqueImage].pStandardDeviation);
      }
    else /* guiContrastStretchSelection->value() == NO_CONTRAST_STRETCH */
      {
      guiGroupQuantiles->hide();
      guiSetStandardDeviation->hide();
      }
}

/**
 *
 */
void ViewerModule::UpdateViewerSetupWindow()
{
  // Select the current image
  ImageType::Pointer image = m_InputImageList->GetNthElement(m_CurrentOpaqueImage);

  //Get the number of components per pixel
  unsigned int nbComponent = image->GetNumberOfComponentsPerPixel();

  std::ostringstream oss;
  oss.str("");

  //Clear all the choices
  guiGrayscaleChannelChoice->range(1, nbComponent);
  guiRedChannelChoice->range(1, nbComponent);
  guiGreenChannelChoice->range(1, nbComponent);
  guiBlueChannelChoice->range(1, nbComponent);

  guiTabSetup->redraw();

  if (nbComponent == 1 || nbComponent == 2)
    {
    this->GrayScaleSet();
    }


}

/**
 * RGBSet();
 */
void ViewerModule::RGBSet()
{
  // Select the current image layer
  ImageType::Pointer image = m_InputImageList->GetNthElement(m_CurrentOpaqueImage);

  unsigned int nbComponent = image->GetNumberOfComponentsPerPixel();

  guiViewerSetupColorMode->set();
  guiViewerSetupGrayscaleMode->clear();
  guiGrayscaleChannelChoice->deactivate();

  guiRedChannelChoice->activate();
  guiGreenChannelChoice->activate();
  guiBlueChannelChoice->activate();

  // Select the current rendering function
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);

  ChannelListType channels = renderer->GetChannelList();
  unsigned int i = 0;
  while (channels.size() < 3)
    {
    channels.push_back(i);
    ++i;
    }

  guiRedChannelChoice->value(std::min(channels[0] + 1, nbComponent));
  guiGreenChannelChoice->value(std::min(channels[1] + 1, nbComponent));
  guiBlueChannelChoice->value(std::min(channels[2] + 1, nbComponent));
}

/**
 * Change the render function to gray scale one
 */
void ViewerModule::GrayScaleSet()
{
  // Select the current image layer
  ImageType::Pointer image = m_InputImageList->GetNthElement(m_CurrentOpaqueImage);

  unsigned int nbComponent = image->GetNumberOfComponentsPerPixel();

  guiViewerSetupGrayscaleMode->set();
  guiViewerSetupColorMode->clear();

  guiRedChannelChoice->deactivate();
  guiGreenChannelChoice->deactivate();
  guiBlueChannelChoice->deactivate();

  guiGrayscaleChannelChoice->activate();

  // Select the current rendering function
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);

  ChannelListType channels = renderer->GetChannelList();
  if (channels.size() < 1)
    {
    channels.push_back(0);
    }
  guiGrayscaleChannelChoice->value(std::min(channels[0] + 1, nbComponent));
}

/**
 * Display the viewed elements in splitted windows
 */
void ViewerModule::SplittedLayout()
{
  if (m_WindowsLayout == SPLITTED_WINDOWS_LAYOUT)
    {
    return;
    }

  guiViewerSetupSplittedLayout->set();
  guiViewerSetupPackedLayout->clear();
  UpdateWindowsLayout(SPLITTED_WINDOWS_LAYOUT);
}

/**
 * Display the viewed elements in packed windows
 */
void ViewerModule::PackedLayout()
{
  if (m_WindowsLayout == PACKED_WINDOWS_LAYOUT)
    {
    return;
    }

  guiViewerSetupSplittedLayout->clear();
  guiViewerSetupPackedLayout->set();
  UpdateWindowsLayout(PACKED_WINDOWS_LAYOUT);
}

/**
 * Set the display mode to "windowsLayout" value
 */
void ViewerModule::UpdateWindowsLayout(const WindowsLayoutEnumType windowsLayout)
{
  WidgetManagerPointerType tmpHandler = m_DisplayWindow;

  m_DisplayWindow->Hide();
  m_DisplayWindow->UnRegisterAll();

  if (windowsLayout == SPLITTED_WINDOWS_LAYOUT)
    {
    m_DisplayWindow = m_SplittedWindows;
    }
  else
    {
    m_DisplayWindow = m_PackedWindows;
    }

  m_DisplayWindow->RegisterFullWidget(m_View->GetFullWidget());
  m_DisplayWindow->RegisterScrollWidget(m_View->GetScrollWidget());
  m_DisplayWindow->RegisterZoomWidget(m_View->GetZoomWidget());
  m_DisplayWindow->RegisterPixelDescriptionWidget(m_PixelDescriptionView->GetPixelDescriptionWidget());
  m_DisplayWindow->RegisterHistogramWidget(m_CurveWidget);

  tmpHandler->Hide();
  tmpHandler->UnRegisterAll();

  m_DisplayWindow->Refresh();
  m_DisplayWindow->Show();

  m_WindowsLayout = windowsLayout;
}

/**
 *
 */
void ViewerModule::ViewerSetupOk()
{
  m_ViewerSetupStructList[m_CurrentOpaqueImage].pRedChannel = static_cast<int> (guiRedChannelChoice->value());
  m_ViewerSetupStructList[m_CurrentOpaqueImage].pGreenChannel = static_cast<int> (guiGreenChannelChoice->value());
  m_ViewerSetupStructList[m_CurrentOpaqueImage].pBlueChannel = static_cast<int> (guiBlueChannelChoice->value());
  m_ViewerSetupStructList[m_CurrentOpaqueImage].pGrayChannel = static_cast<int> (guiGrayscaleChannelChoice->value());

  if (guiViewerSetupColorMode->value())
    {
    int redChoice = static_cast<int> (guiRedChannelChoice->value() - 1);
    int greenChoice = static_cast<int> (guiGreenChannelChoice->value() - 1);
    int blueChoice = static_cast<int> (guiBlueChannelChoice->value() - 1);
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pRGBMode = true;
    this->UpdateRGBChannelOrder(redChoice, greenChoice, blueChoice);
    }
  else
    if (guiViewerSetupGrayscaleMode->value())
      {
      int grayChoice = static_cast<int> (guiGrayscaleChannelChoice->value() - 1);
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pRGBMode = false;
      this->UpdateGrayScaleChannelOrder(grayChoice);
      }

  SynchronizeAll();
}

/**
 * Gray Scale channel order
 */
void ViewerModule::UpdateGrayScaleChannelOrder(int choice)
{
  // Select the current rendering function
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);

  // Select the current image
  ImageType::Pointer image = m_InputImageList->GetNthElement(m_CurrentOpaqueImage);

  // Build the appropriate rendering function
  ChannelListType channels;

  channels.resize(3);
  channels[0] = choice;
  channels[1] = choice;
  channels[2] = choice;
  renderer->SetChannelList(channels);

  // Apply the new rendering function to the Image layer
  renderer->SetAutoMinMax(true);

  // Select the current image layer
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);

  imageLayer->SetRenderingFunction(renderer);
  renderer->Initialize(image->GetMetaDataDictionary());
  m_RenderingModel->Update();
}

/**
 * RGB Channel Order
 */
void ViewerModule::UpdateRGBChannelOrder(int red, int green, int blue)
{
  // Select the current rendering function
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);

  // Select the current image
  ImageType::Pointer image = m_InputImageList->GetNthElement(m_CurrentOpaqueImage);

  // Build the appropriate rendering function
  ChannelListType channels;

  channels.resize(3);
  channels[0] = red;
  channels[1] = green;
  channels[2] = blue;
  renderer->SetChannelList(channels);

  // Apply the new rendering function to the Image layer
  renderer->SetAutoMinMax(true);

  // Select the current image layer
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);

  imageLayer->SetRenderingFunction(renderer);
  renderer->Initialize(image->GetMetaDataDictionary());
  m_RenderingModel->Update();

}

/**
 *
 */
void ViewerModule::UpdateTabHistogram()
{
  // Get the number of subdivisons in the GUI
  int blank = 4;
  int width = (int) ((gHistogram->w() - blank) / 2.0);
  int height = (int) ((gHistogram->h() - blank) / 2.0);

  // Clear the widget
  m_BlueCurveWidgetGroup->ClearAllCurves();
  m_GreenCurveWidgetGroup->ClearAllCurves();
  m_RedCurveWidgetGroup->ClearAllCurves();

  // Select the current image layer
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);

  // Select the current rendering function
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);

  // Select the current image
  ImageType::Pointer image = m_InputImageList->GetNthElement(m_CurrentOpaqueImage);

  /// Update the rendering function
  ChannelListType channels = imageLayer->GetRenderingFunction()->GetChannelList();
  renderer->SetChannelList(channels);
  renderer->Initialize(image->GetMetaDataDictionary());
  imageLayer->SetRenderingFunction(renderer);

  // Set the rendering to the histogram handlers
  m_BlueHistogramHandler->SetRenderingFunction(renderer);
  m_GreenHistogramHandler->SetRenderingFunction(renderer);
  m_RedHistogramHandler->SetRenderingFunction(renderer);

  /// BLUE band : draw the BLUE band histogram and the asymptote
  if (imageLayer->GetRenderingFunction()->GetHistogramList()->Size() >= 3)
    {
    HistogramCurveType::Pointer bhistogram = HistogramCurveType::New();
    bhistogram->SetHistogramColor(m_Blue);
    bhistogram->SetLabelColor(m_Blue);
    bhistogram->SetHistogram(imageLayer->GetHistogramList()->GetNthElement(2));
    // Before rendering is called to get the extremum of the histogram
    bhistogram->BeforeRendering();
    m_BlueCurveWidgetGroup->AddCurve(bhistogram);

    // Add to the gHistogram group
    gHistogram->add(m_BlueCurveWidgetGroup);
    gHistogram->resizable(m_BlueCurveWidgetGroup);
    m_BlueCurveWidgetGroup->resize(gHistogram->x(), gHistogram->y() + height + blank, width, height);

    // Left Asymptote
    m_BlueVaCurveL->SetAbcisse(renderer->GetParameters().GetElement(4));
    m_BlueVaCurveL->SetVerticalAsymptoteColor(m_Blue);

    // Right Asymptote
    m_BlueVaCurveR->SetAbcisse(renderer->GetParameters().GetElement(5));
    m_BlueVaCurveR->SetVerticalAsymptoteColor(m_Blue);

    // Add the asymptote to the curve
    m_BlueCurveWidgetGroup->AddCurve(m_BlueVaCurveR);
    m_BlueCurveWidgetGroup->AddCurve(m_BlueVaCurveL);

    // Set the extremum of the curve
    m_BlueHistogramHandler->SetMinimumAbcisse(bhistogram->GetMinimum()[0]);
    m_BlueHistogramHandler->SetMaximumAbcisse(bhistogram->GetMaximum()[0]);

    // Edit the channel we're changing
    m_BlueHistogramHandler->SetChannel(2);
    }
  /// GREEN band : draw the GREEN band histogram and the asymptote
  if (imageLayer->GetRenderingFunction()->GetHistogramList()->Size() >= 2)
    {
    HistogramCurveType::Pointer ghistogram = HistogramCurveType::New();
    ghistogram->SetHistogramColor(m_Green);
    ghistogram->SetLabelColor(m_Green);
    ghistogram->SetHistogram(imageLayer->GetHistogramList()->GetNthElement(1));
    // Before rendering is called to get the extremum of the histogram
    ghistogram->BeforeRendering();
    m_GreenCurveWidgetGroup->AddCurve(ghistogram);

    //Add to the gHistogram group
    gHistogram->add(m_GreenCurveWidgetGroup);
    gHistogram->resizable(m_GreenCurveWidgetGroup);
    m_GreenCurveWidgetGroup->resize(gHistogram->x() + width + blank, gHistogram->y(), width, height);

    // Left Asymptote
    m_GreenVaCurveL->SetAbcisse(renderer->GetParameters().GetElement(2));
    m_GreenVaCurveL->SetVerticalAsymptoteColor(m_Green);

    // Right Asymptote
    m_GreenVaCurveR->SetAbcisse(renderer->GetParameters().GetElement(3));
    m_GreenVaCurveR->SetVerticalAsymptoteColor(m_Green);

    // Add the asymptote to the curve
    m_GreenCurveWidgetGroup->AddCurve(m_GreenVaCurveR);
    m_GreenCurveWidgetGroup->AddCurve(m_GreenVaCurveL);

    // Set the extremum of the curve
    m_GreenHistogramHandler->SetMinimumAbcisse(ghistogram->GetMinimum()[0]);
    m_GreenHistogramHandler->SetMaximumAbcisse(ghistogram->GetMaximum()[0]);

    // Edit the channel we're changing
    m_GreenHistogramHandler->SetChannel(1);
    }
  /// RED band : draw the RED band histogram and the asymptote
  HistogramCurveType::Pointer rhistogram = HistogramCurveType::New();
  if (imageLayer->GetRenderingFunction()->GetHistogramList()->Size() == 1)
    {
    rhistogram->SetHistogramColor(m_Grey);
    rhistogram->SetLabelColor(m_Grey);

    //Add to the gHistogram group
    gHistogram->add(m_RedCurveWidgetGroup);
    gHistogram->resizable(m_RedCurveWidgetGroup);
    m_RedCurveWidgetGroup->resize(gHistogram->x(), gHistogram->y(), width, height);
    }
  else
    {
    rhistogram->SetHistogramColor(m_Red);
    rhistogram->SetLabelColor(m_Red);

    //Add to the gHistogram group
    gHistogram->add(m_RedCurveWidgetGroup);
    gHistogram->resizable(m_RedCurveWidgetGroup);
    m_RedCurveWidgetGroup->resize(gHistogram->x(), gHistogram->y(), width, height);
    }

  rhistogram->SetHistogram(imageLayer->GetHistogramList()->GetNthElement(0));
  // Before rendering is called to get the extremum of the histogram
  rhistogram->BeforeRendering();
  m_RedCurveWidgetGroup->AddCurve(rhistogram);
  // Left Asymptote
  m_RedVaCurveL->SetAbcisse(renderer->GetParameters().GetElement(0));
  m_RedVaCurveL->SetVerticalAsymptoteColor(rhistogram->GetLabelColor());

  // Right Asymptote
  m_RedVaCurveR->SetAbcisse(renderer->GetParameters().GetElement(1));
  m_RedVaCurveR->SetVerticalAsymptoteColor(rhistogram->GetLabelColor());

  // Add the asymptote to the curve
  m_RedCurveWidgetGroup->AddCurve(m_RedVaCurveR);
  m_RedCurveWidgetGroup->AddCurve(m_RedVaCurveL);

  // Edit the channel we're changing
  m_RedHistogramHandler->SetChannel(0);
  m_RedHistogramHandler->SetMinimumAbcisse(rhistogram->GetMinimum()[0]);
  m_RedHistogramHandler->SetMaximumAbcisse(rhistogram->GetMaximum()[0]);

}

/**
 *
 */
void ViewerModule::TabSetupPosition()
{
  this->UpdateViewerSetupWindowValues();
  // Select the current image layer
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);

  if (gVectorData->value() == guiTabHistogram)
    {
    unsigned int nbBands = imageLayer->GetRenderingFunction()->GetHistogramList()->Size();
    // Avoid Little Window Opened when nbBands less than 3 or 2
    if (nbBands >= 3)
      {
      m_BlueCurveWidgetGroup->show();
      }
    if (nbBands >= 2)
      {
      m_GreenCurveWidgetGroup->show();
      }
    if (nbBands >= 1)
      {
      m_RedCurveWidgetGroup->show();
      }

    guiContrastStretchSelection->value(m_ContrastStretchList[m_CurrentOpaqueImage]);
    }
  else
    if (gVectorData->value() == guiTabSetup)
      {
      m_BlueCurveWidgetGroup->hide();
      m_GreenCurveWidgetGroup->hide();
      m_RedCurveWidgetGroup->hide();
      // Update the color composition window
      this->UpdateViewerSetupWindow();

      }
    else
      {

      m_BlueCurveWidgetGroup->hide();
      m_GreenCurveWidgetGroup->hide();
      m_RedCurveWidgetGroup->hide();
      }
}


/**
 *
 */
void ViewerModule::SetContrastStretch()
{
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);

  // ContrastStretchEnumType contrastStretch;

  ChannelListType channels = imageLayer->GetRenderingFunction()->GetChannelList();

  //RenderingFunctionType::Pointer renderer;

  if (guiContrastStretchSelection->value() == LINEAR_CONTRAST_STRETCH)
    {
    //renderer = StandardRenderingFunctionType::New();
    // contrastStretch = LINEAR_CONTRAST_STRETCH;
    guiSetStandardDeviation->hide();
    guiGroupQuantiles->show();
    }
  else
    if (guiContrastStretchSelection->value() == GAUSSIAN_CONTRAST_STRETCH)
      {
      //renderer = GaussianRenderingFunctionType::New();
      // contrastStretch = GAUSSIAN_CONTRAST_STRETCH;
      guiGroupQuantiles->hide();
      guiSetStandardDeviation->show();
      }
    else
      if (guiContrastStretchSelection->value() == SQUARE_ROOT_CONTRAST_STRETCH)
        {
        //renderer = SquareRootRenderingFunctionType::New();
        // contrastStretch = SQUARE_ROOT_CONTRAST_STRETCH;
        guiGroupQuantiles->show();
        guiSetStandardDeviation->hide();
        }
      else /* guiContrastStretchSelection->value() == NO_CONTRAST_STRETCH */
        {
        //renderer = NoStretchRenderingFunctionType::New();
        // contrastStretch = NO_CONTRAST_STRETCH;
        guiGroupQuantiles->hide();
        guiSetStandardDeviation->hide();
        //guiGroupStretch->redraw();
        }

  if(static_cast<int>(guiStretchFull->value()) == 1)
    {
      imageLayer->SetComputeHistoOnFullResolution(true);
      imageLayer->SetComputeHistoOnZoomResolution(false);
    }
  else if (static_cast<int>(guiStretchZoom->value()) == 1)
    {
      imageLayer->SetComputeHistoOnFullResolution(false);
      imageLayer->SetComputeHistoOnZoomResolution(true);
    }
  else
    {
      imageLayer->SetComputeHistoOnFullResolution(false);
      imageLayer->SetComputeHistoOnZoomResolution(false);
    }
  imageLayer->SetUpdateHisto(true);

  //renderer->SetChannelList(channels);
  //renderer->SetNoDataValue(static_cast<ImageType::InternalPixelType> (guiSetNoData->value()));
  //renderer->SetAutoMinMax(true);
  //imageLayer->SetRenderingFunction(renderer);
  //m_RenderingFunctionList->SetNthElement(m_CurrentOpaqueImage, renderer);
  //m_ContrastStretchList[m_CurrentOpaqueImage] = contrastStretch;

}

/**
 * Method call when click Apply button in contrast stretch gui.
 */
void ViewerModule::ApplyContrastStretch()
{
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);
  imageLayer->SetUpdateHisto(true);
  ContrastStretchEnumType contrastStretch;

  ChannelListType channels = imageLayer->GetRenderingFunction()->GetChannelList();

  RenderingFunctionType::Pointer renderer;

  //create a new rendering function.
  if (guiContrastStretchSelection->value() == LINEAR_CONTRAST_STRETCH)
    {
    renderer = StandardRenderingFunctionType::New();
    contrastStretch = LINEAR_CONTRAST_STRETCH;
    }
  else if (guiContrastStretchSelection->value() == GAUSSIAN_CONTRAST_STRETCH)
    {
    renderer = GaussianRenderingFunctionType::New();
    contrastStretch = GAUSSIAN_CONTRAST_STRETCH;
    }
  else if (guiContrastStretchSelection->value() == SQUARE_ROOT_CONTRAST_STRETCH)
    {
    renderer = SquareRootRenderingFunctionType::New();
    contrastStretch = SQUARE_ROOT_CONTRAST_STRETCH;
    }
  else /* guiContrastStretchSelection->value() == NO_CONTRAST_STRETCH */
    {
    renderer = NoStretchRenderingFunctionType::New();
    contrastStretch = NO_CONTRAST_STRETCH;
    }

  renderer->SetChannelList(channels);
  renderer->SetNoDataValue(static_cast<ImageType::InternalPixelType> (guiSetNoData->value()));

  renderer->SetAutoMinMax(true);
  /*//Check if we need to compute again the histogram.
  if ((contrastStretch == m_ContrastStretchList[m_CurrentOpaqueImage]))
  {
    if ((static_cast<int>(guiStretchFull->value()) == 1) && (m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution == FULL_STRETCH))
    {
      renderer->SetAutoMinMax(false);
    }
    else if ((static_cast<int>(guiStretchQL->value()) == 1) && (m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution == QUICKLOOK_STRETCH))
    {
      renderer->SetAutoMinMax(false);
    }
    else if ((static_cast<int>(guiStretchZoom->value()) == 1) && (m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution == ZOOM_STRETCH))
    {
      renderer->SetAutoMinMax(false);
    }
    else
    {
      renderer->SetAutoMinMax(true);
    }
  }
  else
  {
    renderer->SetAutoMinMax(true);
  }*/

  imageLayer->SetRenderingFunction(renderer);
  m_RenderingFunctionList->SetNthElement(m_CurrentOpaqueImage, renderer);
  m_ContrastStretchList[m_CurrentOpaqueImage] = contrastStretch;

  //Update m_ViewerSetupStructList with selected parameters.
  m_ViewerSetupStructList[m_CurrentOpaqueImage].pNoData = static_cast<ImageType::InternalPixelType> (guiSetNoData->value());
  if(static_cast<int>(guiStretchFull->value()) == 1)
    {
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution = FULL_STRETCH;
    }
  else if (static_cast<int>(guiStretchZoom->value()) == 1)
    {
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution = ZOOM_STRETCH;
    }
  else
    {
      m_ViewerSetupStructList[m_CurrentOpaqueImage].pStretchResolution = QUICKLOOK_STRETCH;
    }


  // Select the current rendering function
  RenderingFunctionType::Pointer currentRenderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);
  m_RenderingModel->Update();

  if (guiContrastStretchSelection->value() == GAUSSIAN_CONTRAST_STRETCH)
    {
    // Get the rendering function parameters and modify the standard deviation
    ParametersType params;
    params = renderer->GetParameters();
    params[1] = guiSetStandardDeviation->value();
    currentRenderer->SetParameters(params);
    }

  //m_RenderingModel->Update();
  UpdateQuantiles();

}

/**
 *
 */
void ViewerModule::UpdateQuantiles()
{

  double lowerQuantile, upperQuantile;
  bool isVisible;

  if ((guiContrastStretchSelection->value() == LINEAR_CONTRAST_STRETCH) || (guiContrastStretchSelection->value()
      == SQUARE_ROOT_CONTRAST_STRETCH))
    {
    guiGroupQuantiles->show();
    isVisible = true;
    lowerQuantile = bLowerQuantile->value() / 100.0;
    upperQuantile = bUpperQuantile->value() / 100.0;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pLowerQuantile = lowerQuantile;
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pUpperQuantile = upperQuantile;
    }
  else
    {
    guiGroupQuantiles->hide();
    isVisible = false;
    lowerQuantile = 0.0;
    upperQuantile = 0.0;
    }

  m_BlueVaCurveR->SetVisible(isVisible);
  m_BlueVaCurveL->SetVisible(isVisible);
  m_GreenVaCurveR->SetVisible(isVisible);
  m_GreenVaCurveL->SetVisible(isVisible);
  m_RedVaCurveR->SetVisible(isVisible);
  m_RedVaCurveL->SetVisible(isVisible);

  UpdateQuantiles(lowerQuantile, upperQuantile);

}

/**
 *
 */
void ViewerModule::UpdateQuantiles(double lowerQuantile, double upperQuantile)
{
  // Select the current rendering function
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);

  // Cancel the automatic computation of the quantile
  renderer->SetAutoMinMax(false);

  // Get the rendering function extrema parameters
  ParametersType params;
  ParametersType paramsMinMax;
  params = renderer->GetParameters();
  if (guiContrastStretchSelection->value() == GAUSSIAN_CONTRAST_STRETCH)
    {
    paramsMinMax.SetSize(params.GetSize() - 2);
    for (unsigned int i = 0; i < paramsMinMax.GetSize(); i++)
      {
      paramsMinMax[i] = params[i + 2];
      }
    }
  else
    {
    paramsMinMax = params;
    }

  // Update the parameters
  for (unsigned int i = 0; i < paramsMinMax.Size(); i = i + 2)
    {
    unsigned int n = i / 2;

    double min = renderer->GetHistogramList()->GetNthElement(n)->Quantile(0, lowerQuantile);
    double max = renderer->GetHistogramList()->GetNthElement(n)->Quantile(0, 1 - upperQuantile);
    paramsMinMax.SetElement(i, min);
    paramsMinMax.SetElement(i + 1, max);
    }

  // Update the layer
  if (guiContrastStretchSelection->value() == GAUSSIAN_CONTRAST_STRETCH)
    {
    m_ViewerSetupStructList[m_CurrentOpaqueImage].pStandardDeviation = guiSetStandardDeviation->value();
    params[1] = guiSetStandardDeviation->value();
    for (unsigned int i = 0; i < paramsMinMax.GetSize(); i++)
      {
      params[i + 2] = paramsMinMax[i];
      }
    }
  else
    {
    params = paramsMinMax;
    }
  renderer->SetParameters(params);
  renderer->SetNoDataValue(static_cast<ImageType::InternalPixelType> (guiSetNoData->value()));
  m_RenderingModel->Update();

  // Redraw the histogram curves
  UpdateTabHistogram();

  // Added cause the asymptote doesn't update their position
  m_BlueCurveWidgetGroup->hide();
  m_GreenCurveWidgetGroup->hide();
  m_RedCurveWidgetGroup->hide();

  // Needed to show the curves
  TabSetupPosition();

}

/**
 *
 */
void ViewerModule::UpdateStandardDeviation()
{
  // Select the current rendering function
  RenderingFunctionType::Pointer renderer = m_RenderingFunctionList->GetNthElement(m_CurrentOpaqueImage);

  // Get the rendering function parameters and modify the standard deviation
  ParametersType params;

  params = renderer->GetParameters();
  params[1] = guiSetStandardDeviation->value();
  renderer->SetParameters(params);
  m_RenderingModel->Update();

}

/**
 *
 */
void ViewerModule::UpdateNoData()
{
  // Simply call the SetContrastStretch to recreate the rendering function to have the new sample lists
  this->SetContrastStretch();
  this->ApplyContrastStretch();
}

/**
 *
 */
void ViewerModule::UpdatePixelInformationWindow()
{
  // Set the index
  ImageType::IndexType index;
  index[0] = static_cast<ImageType::IndexType::IndexValueType> (bX->value());
  index[1] = static_cast<ImageType::IndexType::IndexValueType> (bY->value());

  // Center the view around the index selected
  m_RenderingModel->SetScaledExtractRegionCenter(index);
  m_RenderingModel->SetExtractRegionCenter(index);
  m_RenderingModel->Update();

  //Create buffer for the guiViewerInformation
  Fl_Text_Buffer * buffer = new Fl_Text_Buffer();
  bPixelInfo->buffer(buffer);

  // Select the current image layer
  ImageLayerType::Pointer imageLayer = m_InputImageLayerList->GetNthElement(m_CurrentOpaqueImage);

  // Get the description
  std::ostringstream oss;
  oss.str("");
  bPixelInfo->buffer()->remove(0, bPixelInfo->buffer()->length());
  oss << imageLayer->GetPixelDescription(index);
  bPixelInfo->insert(oss.str().c_str());
}

/**
 *
 */
void ViewerModule::ShowHide()
{
  m_DisplayWindow->Show();
}

/**
 *
 */
void ViewerModule::Quit()
{
  this->Hide();
  // Once module is closed, it is no longer busy
  this->BusyOff();
}

/**
 * Proceed to screen shot
 */
void ViewerModule::ScreenShot()
{
  if (rbScreenZoom->value() == 1)
    {
    this->SaveScreenShot("Save Zoom view screen shot as...", m_View->GetZoomWidget());
    }

  if (rbScreenFull->value() == 1)
    {
    this->SaveScreenShot("Save Full view screen shot as...", m_View->GetFullWidget());
    }

  if (rbScreenNav->value() == 1)
    {
    this->SaveScreenShot("Save Navigation view screen shot as...", m_View->GetScrollWidget());
    }

  if (rbScreenNav->value() != 1 && rbScreenFull->value() != 1 && rbScreenZoom->value() != 1)
    {
    MsgReporter::GetInstance()->SendError("No view selected for screen shot...");
    }

  wScreenShot->hide();
}

void ViewerModule::SaveScreenShot(const char * winLab, ImageWidgetType * widget)
{
  const char * filename = NULL;

  ScreenShotFilterType::Pointer screener = ScreenShotFilterType::New();
  screener->SetNumberOfChannels(3);
  screener->SetInverseXSpacing(true);

  filename = flu_file_chooser(winLab, "*.*", "");

  if (filename == NULL)
    {
    MsgReporter::GetInstance()->SendError("Empty file name!");
    return;
    }

  screener->SetFileName(filename);
  screener->SetBuffer(widget->GetOpenGlBuffer());
  screener->SetImageSize(widget->GetOpenGlBufferedRegion().GetSize());
  screener->Update();
}

/**
 *
 */
void ViewerModule::UpdateInformation()
{
  // Select the current image
  ImageType::Pointer image = m_InputImageList->GetNthElement(m_CurrentOpaqueImage);

  std::string imName = m_Label.c_str();
  std::ostringstream oss;
  oss.str("");
  oss << imName.substr(imName.find_last_of("/") + 1, imName.size()) << " (" << image->GetNumberOfComponentsPerPixel();
  if (image->GetNumberOfComponentsPerPixel() != 1)
    {
    oss << " bands, ";
    }
  else
    {
    oss << " band, ";
    }

  oss << image->GetLargestPossibleRegion().GetSize() << ")";

  m_DisplayWindow->SetLabel(oss.str().c_str());
  bSetupWindow->copy_label(oss.str().c_str());
}

/**
 *
 */
void ViewerModule::Hide()
{
  // Hide the main window
  m_DisplayWindow->Hide();
  // Hide the DEM Window
  wDEM->hide();
  // Hide the Screen shot Window
  wScreenShot->hide();
  // Hide the Setup Propreties Window
  bSetupWindow->hide();
}


/**
 * FIXME: code refactoring in progress...
 */
void ViewerModule::RefreshTabData()
{
}

/**
 * FIXME: code refactoring in progress...
 */
void ViewerModule::RefreshTabSetup()
{
}

/**
 * FIXME: code refactoring in progress...
 */
void ViewerModule::RefreshTabHistogram()
{
}

/**
 * FIXME: code refactoring in progress...
 */
void ViewerModule::RefreshTabPixelDescription()
{
}

} // End namespace otb

#endif
