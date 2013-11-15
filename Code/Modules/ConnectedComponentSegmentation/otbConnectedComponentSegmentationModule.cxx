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
#ifndef __otbConnectedComponentSegmentationModule_cxx
#define __otbConnectedComponentSegmentationModule_cxx

#include "otbConnectedComponentSegmentationModule.h"
#include "otbMsgReporter.h"
#include <FLU/Flu_File_Chooser.h>
#include "otbFltkFilterWatcher.h"
#include "itksys/SystemTools.hxx"

#include "otbVectorImageToImageListFilter.h"

namespace otb
{
/**
 * Constructor
 */
ConnectedComponentSegmentationModule::ConnectedComponentSegmentationModule()
{
  // Build a view
  m_View = ViewType::New();
  m_PixelView = PixelViewType::New();

  // Build a controller
  m_WidgetsController = WidgetControllerType::New();

  // Layer Generators
  m_ImageGenerator = VectorImageLayerGeneratorType::New();
  m_MaskGenerator = ImageLayerGeneratorType::New();
  m_MaskedImageGenerator = VectorImageLayerGeneratorType::New();

  m_CCSegmentationGenerator = RGBImageLayerGeneratorType::New();

  m_RelabelRGBGenerator = RGBImageLayerGeneratorType::New();
  m_OBIAOpeningGenerator = RGBImageLayerGeneratorType::New();

  m_CCSegmentationLabelGenerator = LabelLayerGeneratorType::New();
  m_RelabelGenerator = LabelLayerGeneratorType::New();
  m_OBIAOpeningLabelGenerator = LabelLayerGeneratorType::New();

  // we instantiate a render for the mask layer to disable outlier rejection
  m_MaskRenderer = StandardRenderingFunctionType::New();
  m_MaskRenderer->SetAutoMinMax(false);

  // Get the rendering function extrema parameters

  ParametersType paramsMinMax;
  paramsMinMax.SetSize(2);
  paramsMinMax[0] = 0.0;
  paramsMinMax[1] = 1.0;
  m_MaskRenderer->SetParameters(paramsMinMax);

  m_MaskGenerator->SetRenderingFunction(m_MaskRenderer);


  // Build a new rendering model
  m_RenderingModel = RenderingModelType::New();
  m_PixelDescriptionModel = PixelDescriptionModelType::New();

  // Build the handlers
  m_ResizingHandler = ResizingHandlerType::New();
  m_ChangeRegionHandler = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler = ChangeScaleHandlerType::New();

  // Add the pixel description action handler
  m_PixelActionHandler = PixelDescriptionActionHandlerType::New();
  m_PixelActionHandler->SetView(m_View);
  m_PixelActionHandler->SetModel(m_PixelDescriptionModel);

  m_ObjectDescriptionActionHandler = ObjectDescriptionActionHandlerType::New();
  m_ObjectDescriptionActionHandler->SetView(m_View);
  m_ObjectDescriptionActionHandler->SetModel(m_PixelDescriptionModel);

  // Add the action handlers to the widgets controller
  m_WidgetsController->AddActionHandler(m_ResizingHandler);
  m_WidgetsController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaleHandler);
  m_WidgetsController->AddActionHandler(m_PixelActionHandler);
  m_WidgetsController->AddActionHandler(m_ObjectDescriptionActionHandler);

  // Wire the MVC
  m_View->SetModel(m_RenderingModel);
  m_PixelView->SetModel(m_PixelDescriptionModel);
  m_View->SetController(m_WidgetsController);

  // Add the resizing handler
  m_ResizingHandler->SetModel(m_RenderingModel);
  m_ResizingHandler->SetView(m_View);

  m_ChangeRegionHandler->SetModel(m_RenderingModel);
  m_ChangeScaledRegionHandler->SetModel(m_RenderingModel);
  m_ChangeScaleHandler->SetModel(m_RenderingModel);

  m_ChangeRegionHandler->SetView(m_View);
  m_ChangeScaledRegionHandler->SetView(m_View);
  m_ChangeScaleHandler->SetView(m_View);

  m_NoMask = true;
  m_NoOBIAOpening = true;

  // reduced set of attributes
  m_ShapeReducedSetOfAttributes = false;
  m_StatsReducedSetOfAttributes = false;
  m_ComputeFlusser = false;
  m_ComputePolygon = false;
  m_ComputeFeretDiameter = false;
  m_ComputePerimeter = false;

  // output not available
  m_MaskOutputReady = false;
  m_CCSegmentationReady = false;
  m_RelabelOutputReady = false;
  m_OBIAOpeningOutputReady = false;

  //use a DEM
  m_UseDEM = false;
  m_DEMPath = "";

  // Build the GUI
  this->CreateGUI();
  this->pBusyBar->minimum(0);
  this->pBusyBar->maximum(1);

  this->pBusyBar->value(0);
  this->pBusyBar->hide();
  Fl::check();
  // Remove registered visualization components from the interface
  gImageViewer->add(m_View->GetFullWidget());
  gScroll->add(m_View->GetScrollWidget());
  gZoom->add(m_View->GetZoomWidget());
  gPixelDescription->add(m_PixelView->GetPixelDescriptionWidget());
  //
  gObjectDescription->add(m_ObjectDescriptionActionHandler->GetObjectDescriptionWidget());
  gObjectDescription->resizable(m_ObjectDescriptionActionHandler->GetObjectDescriptionWidget());
  m_ObjectDescriptionActionHandler->GetObjectDescriptionWidget()->resize(gObjectDescription->x(),
                                                                         gObjectDescription->y(),
                                                                         gObjectDescription->w(),
                                                                         gObjectDescription->h());
  m_ObjectDescriptionActionHandler->GetObjectDescriptionWidget()->show();

  gImageViewer->resizable(m_View->GetFullWidget());
  gScroll->resizable(m_View->GetScrollWidget());
  gScroll->resizable(m_View->GetZoomWidget());
  gPixelDescription->resizable(m_PixelView->GetPixelDescriptionWidget());

  m_View->GetFullWidget()->resize(gImageViewer->x(), gImageViewer->y(), gImageViewer->w(), gImageViewer->h());
  m_View->GetScrollWidget()->resize(gScroll->x(), gScroll->y(), gScroll->w(), gScroll->h());
  m_View->GetZoomWidget()->resize(gZoom->x(), gZoom->y(), gZoom->w(), gZoom->h());
  m_PixelView->GetPixelDescriptionWidget()->resize(gPixelDescription->x(), gPixelDescription->y(),
                                                   gPixelDescription->w(), gPixelDescription->h());

  // INPUT_IMAGE
  uiTmpOutputSelection->add("Input image");

  // Default value : INPUT_IMAGE
  uiTmpOutputSelection->value(INPUT_IMAGE);

  // Describe inputs

  this->AddInputDescriptor<VectorImageType> ("InputImage", otbGetTextMacro("Image to process"), false, true);
  this->AddTypeToInputDescriptor<ImageType> ("InputImage");

}

/**
 * Destructor
 */
ConnectedComponentSegmentationModule::~ConnectedComponentSegmentationModule()
{
}

/**
 * PrintSelf method
 */
void ConnectedComponentSegmentationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/**
 * Hide the Module GUI
 */
void ConnectedComponentSegmentationModule::Hide()
{
  guiMainWindow->hide();
  ui_HelpWindow->hide();
  guiFormula->hide();
}

void ConnectedComponentSegmentationModule::Show()
{
  // Show the main window
  guiMainWindow->show();
  guiFormula->show();
  m_View->GetScrollWidget()->show();
  m_View->GetFullWidget()->show();
  m_View->GetZoomWidget()->show();
  m_PixelView->GetPixelDescriptionWidget()->show();
  m_ObjectDescriptionActionHandler->GetObjectDescriptionWidget()->show();

}

void ConnectedComponentSegmentationModule::OBIA_functor_init()
{

  m_ShapeAttributes.push_back("Elongation");
  m_ShapeAttributes.push_back("PhysicalSize");

  if (!m_ShapeReducedSetOfAttributes)
    {
    m_ShapeAttributes.push_back("EquivalentEllipsoidRadius0");
    m_ShapeAttributes.push_back("EquivalentEllipsoidRadius1");
    m_ShapeAttributes.push_back("EquivalentPerimeter");
    m_ShapeAttributes.push_back("EquivalentRadius");
    m_ShapeAttributes.push_back("PhysicalCentroid0");
    m_ShapeAttributes.push_back("PhysicalCentroid1");
    m_ShapeAttributes.push_back("PhysicalSizeOnBorder");
    m_ShapeAttributes.push_back("PrincipalAxis00");
    m_ShapeAttributes.push_back("PrincipalAxis01");
    m_ShapeAttributes.push_back("PrincipalAxis10");
    m_ShapeAttributes.push_back("PrincipalAxis11");
    m_ShapeAttributes.push_back("PrincipalMoments0");
    m_ShapeAttributes.push_back("PrincipalMoments1");
    m_ShapeAttributes.push_back("RegionElongation");
    m_ShapeAttributes.push_back("RegionIndex0");
    m_ShapeAttributes.push_back("RegionIndex1");
    m_ShapeAttributes.push_back("RegionRatio");
    m_ShapeAttributes.push_back("RegionSize0");
    m_ShapeAttributes.push_back("RegionSize1");
    m_ShapeAttributes.push_back("Size");
    m_ShapeAttributes.push_back("SizeOnBorder");
    };

  if (m_ComputeFlusser)
    {

    m_ShapeAttributes.push_back("Flusser01");
    m_ShapeAttributes.push_back("Flusser02");
    m_ShapeAttributes.push_back("Flusser03");
    m_ShapeAttributes.push_back("Flusser04");
    m_ShapeAttributes.push_back("Flusser05");
    m_ShapeAttributes.push_back("Flusser06");
    m_ShapeAttributes.push_back("Flusser07");
    m_ShapeAttributes.push_back("Flusser08");
    m_ShapeAttributes.push_back("Flusser09");
    m_ShapeAttributes.push_back("Flusser10");
    m_ShapeAttributes.push_back("Flusser11");
    }
  if (m_ComputeFeretDiameter) m_ShapeAttributes.push_back("FeretDiameter");

  if (m_ComputePerimeter)
    {
    //TO DO JGU
    //check of m_PerimeterCalculator->HasLabel(label) is needed to
    // add Perimeter and Roundness
    }

  m_StatAttributes.push_back("Kurtosis");
  m_StatAttributes.push_back("Mean");
  m_StatAttributes.push_back("Skewness");
  m_StatAttributes.push_back("Variance");

  if (!m_StatsReducedSetOfAttributes)
    {
    m_StatAttributes.push_back("Elongation");
    m_StatAttributes.push_back("CenterOfGravity0");
    m_StatAttributes.push_back("CenterOfGravity1");
    m_StatAttributes.push_back("FirstMaximumIndex0");
    m_StatAttributes.push_back("FirstMaximumIndex1");
    m_StatAttributes.push_back("FirstMinimumIndex0");
    m_StatAttributes.push_back("FirstMinimumIndex1");
    m_StatAttributes.push_back("Maximum");
    m_StatAttributes.push_back("Minimum");
    m_StatAttributes.push_back("PrincipalAxis00");
    m_StatAttributes.push_back("PrincipalAxis01");
    m_StatAttributes.push_back("PrincipalAxis10");
    m_StatAttributes.push_back("PrincipalAxis11");
    m_StatAttributes.push_back("PrincipalMoments0");
    m_StatAttributes.push_back("PrincipalMoments1");
    m_StatAttributes.push_back("Sigma");
    m_StatAttributes.push_back("Sum");
    };

  m_OBIAOpeningFilter->SetAttributes(m_ShapeAttributes, m_StatAttributes, m_NumberOfInputBands);
}

/**
 * The custom run command
 */
void ConnectedComponentSegmentationModule::Run()
{
  // Until window closing, module will be busy
  this->BusyOn();

  // Get input image
  m_InputImage = this->GetInputData<VectorImageType> ("InputImage");

  if (m_InputImage.IsNull())
    {
    itkExceptionMacro(<< "Input image is NULL");
    }

  m_NumberOfInputBands = m_InputImage->GetNumberOfComponentsPerPixel();
  m_InputMask = ImageType::New();

  // Instantiate filters
  m_CCFilter = ConnectedComponentFilterType::New();
  m_MaskFilter = MaskMuParserFilterType::New();
  m_MaskImageFilter = MaskImageFilterType::New();
  m_CCRelabelFilter = RelabelComponentFilterType::New();
  m_OBIAOpeningFilter = LabelObjectOpeningFilterType::New();
  m_OBIAOpeningLabelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
  m_CCImageToCCLabelMapFilter = LabelImageToLabelMapFilterType::New();
  m_ShapeLabelMapFilter = ShapeLabelMapFilterType::New();
  m_RadiometricLabelMapFilter = RadiometricLabelMapFilterType::New();
  m_OBIAOpeningLabelMapToVectorDataFilter = LabelMapToVectorDataFilterType::New();

  // instantiate colormapper
  m_CCColorMapper = ColorMapFilterType::New();
  m_RelabelColorMapper = ColorMapFilterType::New();
  m_OBIAOpeningColorMapper = ColorMapFilterType::New();

  // Generate the layer
  m_ImageGenerator->SetImage(m_InputImage);
  FltkFilterWatcher qlwatcher(m_ImageGenerator->GetProgressSource(), 0, 0, 200, 20,
                              otbGetTextMacro("Generating QuickLook ..."));
  m_ImageGenerator->GenerateLayer();

  m_InputImageLayer = m_ImageGenerator->GetLayer();
  m_InputImageLayer->SetName("ImageLayer");

  m_MaskedImageGenerator->SetImage(m_InputImage);
  m_MaskedImageGenerator->GenerateLayer();

  // Clear previous layers
  m_RenderingModel->ClearLayers();
  m_PixelDescriptionModel->ClearLayers();
  // m_InputImageLayer->SetVisible(false);

  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(m_InputImageLayer);
  m_PixelDescriptionModel->AddLayer(m_InputImageLayer);
  m_PixelDescriptionModel->NotifyAll();
  m_RenderingModel->Update();

  itk::ImageRegion<2> imageRegion;
  imageRegion.SetSize(1, 1);
  imageRegion.SetSize(0, 1);
  imageRegion.SetIndex(m_InputImage->GetLargestPossibleRegion().GetIndex());

  m_InputImage->SetRequestedRegion(imageRegion);
  m_InputImage->Update();

  // if no data we don't use mask
  m_MaskFilter->SetInput(m_InputImage);
  m_MaskImageFilter->SetInput1(m_InputImage);

  PixelType backgroundPixel = m_InputImage->GetPixel(m_InputImage->GetBufferedRegion().GetIndex());
  backgroundPixel.Fill(0.0);
  m_MaskImageFilter->SetOutsideValue(backgroundPixel);

  this->UpdateMaskFormulaVariablesList();
  this->LiveCheckMask();
  //
  m_CCFilter->SetInput(m_InputImage);

  this->UpdateCCFormulaVariablesList();
  this->LiveCheckCC();

  this->OBIA_functor_init();
  this->UpdateOBIAFormulaVariablesList();
  this->LiveCheckOBIA();

  // other layers has to be generated
  m_HasToGenerateMaskLayer = true;
  m_HasToGenerateCCSegmentationLayer = true;
  m_HasToGenerateRelabelLayer = true;
  m_HasToGenerateOBIAOpeningLayer = true;

  //uiTmpOutputSelection->->deactivate();
  // Initialize the help window
  this->InitHelp();

  // Show the GUI
  this->Show();
}

void ConnectedComponentSegmentationModule::UpdateMaskFormulaVariablesList()
{

  std::vector<std::string> variables = m_MaskFilter->GetVar();

  // Get the number of variables
  std::vector<std::string>::const_iterator item = variables.begin();

  // Query the variables
  for (; item != variables.end(); ++item)
    {
    // spectralangle is not usable without choice of reference pixel
    // TODO JGU adapt GUI to allow user defined spectralangle reference pixel
    if ( *item != "spectralAngle" )
      ui_VarNamesMask->add(item->c_str());
    }
  Parser::FunctionMapType functions = m_MaskFilter->GetFunList();
  Parser::FunctionMapType::const_iterator funItem = functions.begin();

  // Query the functions, the function list is same for all expression
  // hence the list is displayed in help window
  for (; funItem != functions.end(); ++funItem)
    {
    if(funItem->second != -1)
      {
       m_FunList<<funItem->first.c_str()<<" "<<funItem->second<<std::endl;
      }
    else
      {
      m_FunList << funItem->first.c_str() << " X" << std::endl;
      }
    }

}

void ConnectedComponentSegmentationModule::UpdateCCFormulaVariablesList()
{

  CCFunctorType& tempFunctor = m_CCFilter->GetFunctor();
  tempFunctor.SetExpression(ui_CCExpression->value());

  PixelType pixel = m_InputImage->GetPixel(m_InputImage->GetBufferedRegion().GetIndex());
  // Define the iterators
  try
    {
    tempFunctor(pixel, pixel);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendWarning(err.GetDescription());
    }

  const std::map<std::string, double*> & variables = tempFunctor.GetVar();

  std::map<std::string, double*>::const_iterator item = variables.begin();

  // Query the variables
  for (; item != variables.end(); ++item)
    {
    ui_VarNamesCC->add(item->first.c_str());
    }

}

void ConnectedComponentSegmentationModule::UpdateOBIAFormulaVariablesList()
{

  const std::map<std::string, double*>& variables = m_OBIAOpeningFilter->GetVar();

  // Get the number of variables
  std::map<std::string, double*>::const_iterator item = variables.begin();

  // Query the variables
  for (; item != variables.end(); ++item)
    {
    ui_VarNamesOBIA->add(item->first.c_str());
    }
}

/**
 * Help Initialization
 */
void ConnectedComponentSegmentationModule::InitHelp()
{
  std::ostringstream helpContent;
  helpContent << "- Connected Component Segmentation module " << std::endl << std::endl;
              helpContent
              << " The aim of this module is to performs connected component based segmentation \n with user defined segmentation criteria, followed by an object analysis post processing."
          << std::endl << std::endl;
  helpContent << "Fill each formula, choose your visualization output and click on Update button" << std::endl;
  helpContent << "If formula area is colored in red the expression is not valid" << std::endl;
          helpContent
          << "Available variables for each expression can be found using the dedicated choice menu \n on the right of each formula."
      << std::endl;
  helpContent << "Available functions for each expression can be found below." << std::endl;
  helpContent << "If Mask formula is left blank, no mask used" << std::endl;
  helpContent << "If Object analysis formula is left blank, no post processing is applied " << std::endl;
  helpContent << "When a first pass have been done. Labeled objects properties will be displayed  " << std::endl;
  helpContent << "if you use right click on image (Only available in \"Filter output\" vizualisation mode)."
      << std::endl;
  helpContent << "Save and quit button process the entire image, and output a vector data of labeled objects."
      << std::endl;
  helpContent << "More informations and examples can be found on OTB wiki" << std::endl;
  helpContent << "http://wiki.orfeo-toolbox.org/index.php/Connected_component_segmentation_module" << std::endl;
  helpContent << std::endl;
  helpContent << "Available functions" << std::endl;
  helpContent << "(second column is the number of arguments, X denotes variable list of arguments)  :";
  helpContent << std::endl;
  helpContent << m_FunList.str();
  helpContent << std::endl;
  ui_HelpText->value(helpContent.str().c_str());
}

/**
 * Quick add a variable name into the expression
 */
void ConnectedComponentSegmentationModule::QuickAddMask()
{
  std::ostringstream tmpExpression;

  tmpExpression << ui_MaskExpression->value() << " " << ui_VarNamesMask->text();
  ui_MaskExpression->value(tmpExpression.str().c_str());
  ui_MaskExpression->take_focus();

  LiveCheckMask();
}

/**
 * Quick add a variable name into the expression
 */
void ConnectedComponentSegmentationModule::QuickAddCC()
{
  std::ostringstream tmpExpression;

  tmpExpression << ui_CCExpression->value() << " " << ui_VarNamesCC->text();
  ui_CCExpression->value(tmpExpression.str().c_str());
  ui_CCExpression->take_focus();

  LiveCheckCC();
}

/**
 * Quick add a variable name into the expression
 */
void ConnectedComponentSegmentationModule::QuickAddOBIA()
{
  std::ostringstream tmpExpression;

  tmpExpression << ui_OBIAExpression->value() << " " << ui_VarNamesOBIA->text();
  ui_OBIAExpression->value(tmpExpression.str().c_str());
  ui_OBIAExpression->take_focus();

  LiveCheckOBIA();
}

void ConnectedComponentSegmentationModule::CheckProcess()
{

  int maxVal = 0;
  uiTmpOutputSelection->remove(OUTPUT);
  uiTmpOutputSelection->remove(SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION);
  uiTmpOutputSelection->remove(CONNECTED_COMPONENT_SEGMENTATION_OUTPUT);
  uiTmpOutputSelection->remove(MASKED_IMAGE);
  uiTmpOutputSelection->remove(MASK);

  if (m_IsMaskExpressionOK)
    {
    maxVal++;
    uiTmpOutputSelection->add("Mask Output");
    maxVal++;
    uiTmpOutputSelection->add("Masked Image");

    if (m_IsCCExpressionOK)
      {
      maxVal++;
      uiTmpOutputSelection->add("Segmentation output");
      maxVal++;
      uiTmpOutputSelection->add("Relabeling with small objects rejection");

      if (m_IsOBIAExpressionOK)
        {
        uiTmpOutputSelection->add("Filter Output");
        maxVal++;
        }
      }
    }

  if (uiTmpOutputSelection->value() > maxVal)
    {
    ui_Update->deactivate();
    // uiTmpOutputSelection->value(maxVal);
    }
  else
    {
    ui_Update->activate();
    }

  uiTmpOutputSelection->redraw();
  //this->Process();

}

void ConnectedComponentSegmentationModule::LiveCheckMask()
{
  m_IsMaskExpressionOK = true;
  m_NoMask = false;

  ui_MaskExpression->color(FL_GREEN);
  ui_MaskExpression->tooltip("The Expression is Valid");
  ui_CCExpression->activate();
  this->LiveCheckCC();
  if (strcmp(ui_MaskExpression->value(), ""))
    {
    m_MaskFilter->SetExpression(ui_MaskExpression->value());
    if (!(m_IsMaskExpressionOK = m_MaskFilter->CheckExpression()))
      {
      ui_MaskExpression->color(FL_RED);
      ui_MaskExpression->tooltip("The Expression is not Valid");
      m_NoMask = true;

      // deactivate other formula
      ui_CCExpression->deactivate();
      uiMinSize->deactivate();
      ui_OBIAExpression->deactivate();
      }

    }
  else
    {
    m_MaskFilter->SetExpression(ui_MaskExpression->value());
    m_NoMask = true;
    ui_CCExpression->activate();
    this->LiveCheckCC();
    }

  ui_MaskExpression->redraw();

  this->CheckProcess();

}

/**
 * Live Checking
 */
void ConnectedComponentSegmentationModule::LiveCheckCC()
{
  m_IsCCExpressionOK = false;

  ui_CCExpression->color(FL_RED);
  ui_CCExpression->tooltip("The Expression is Not Valid");

  CCFunctorType& checkFunctor = m_CCFilter->GetFunctor();
  checkFunctor.SetExpression(ui_CCExpression->value());

  try
    {
    PixelType pixel = m_InputImage->GetPixel(m_InputImage->GetBufferedRegion().GetIndex());
    checkFunctor(pixel, pixel);
    m_IsCCExpressionOK = true;
    ui_CCExpression->color(FL_GREEN);
    ui_CCExpression->tooltip("The Expression is Valid");

    uiMinSize->activate();
    ui_OBIAExpression->activate();
    }
  catch (itk::ExceptionObject& err)
    {
    ui_CCExpression->tooltip("The Expression is Not Valid");
    err.GetDescription();

    // deactivate other formula
    uiMinSize->deactivate();
    ui_OBIAExpression->deactivate();

    }

  ui_CCExpression->redraw();
  this->CheckProcess();

}

void ConnectedComponentSegmentationModule::LiveCheckOBIA()
{
  m_NoOBIAOpening = false;
  m_IsOBIAExpressionOK = true;
  if (strcmp(ui_OBIAExpression->value(), ""))
    {
    ui_OBIAExpression->color(FL_RED);
    ui_OBIAExpression->tooltip("The Expression is not Valid");

    m_OBIAOpeningFilter->SetExpression(ui_OBIAExpression->value());

    if ((m_IsOBIAExpressionOK = m_OBIAOpeningFilter->CheckExpression()))
      {
      ui_OBIAExpression->color(FL_GREEN);
      ui_OBIAExpression->tooltip("The Expression is Valid");
      }
    }
  else
    {
    m_OBIAOpeningFilter->SetExpression(ui_OBIAExpression->value());
    ui_OBIAExpression->color(FL_GREEN);
    m_NoOBIAOpening = true;
    ui_OBIAExpression->tooltip("No OBIA Opening, all labeled objects are valid");
    }
  ui_OBIAExpression->redraw();
  this->CheckProcess();

}

void ConnectedComponentSegmentationModule::SetObjectMinArea()
{
  this->CheckProcess();
}

void ConnectedComponentSegmentationModule::UpdateTmpOutput()
{
  this->CheckProcess();
  this->Process();
}

void ConnectedComponentSegmentationModule::UpdateMaskLayer()
{
  //test layer generation

  m_HasToGenerateMaskLayer = false;
  string mask_expression;
  if (m_NoMask)
    {
    mask_expression = "1";
    }
  else
    {
    mask_expression = ui_MaskExpression->value();
    }

  if (m_CurrentExpressionMask.compare(m_MaskFilter->GetExpression()))
    {
    m_HasToGenerateMaskLayer = true;
    }

  if (m_HasToGenerateMaskLayer || !m_MaskOutputReady)
    {

    this->pBusyBar->value(1);
    this->pBusyBar->show();
    Fl::check();
    // mask has to be regenerated, all processing has to be regenerated
    m_CCSegmentationReady = false;
    m_RelabelOutputReady = false;
    m_OBIAOpeningOutputReady = false;

    ImageLayerType::LayerValueType minLayerValue, maxLayerValue;
    minLayerValue.SetSize(1);
    minLayerValue.Fill(0);
    maxLayerValue.SetSize(1);
    maxLayerValue.Fill(1);

    m_RenderingModel->DeleteLayerByName("Mask");
    m_MaskFilter->SetExpression(mask_expression);
    m_CurrentExpressionMask = mask_expression;
    m_MaskGenerator->SetImage(m_MaskFilter->GetOutput());
    m_MaskGenerator->GenerateQuicklookOff();
    m_MaskGenerator->GenerateLayer();
    m_MaskGenerator->GetLayer()->SetMinValues(minLayerValue);
    m_MaskGenerator->GetLayer()->SetMaxValues(maxLayerValue);

    m_MaskGenerator->GetLayer()->SetName("Mask");

    m_RenderingModel->AddLayer(m_MaskGenerator->GetLayer());

    //Masked Image
    m_MaskImageFilter->SetInput2(m_MaskFilter->GetOutput());
    m_RenderingModel->DeleteLayerByName("Masked Image");

    m_MaskedImageGenerator->SetImage(m_MaskImageFilter->GetOutput());
    m_MaskedImageGenerator->GenerateQuicklookOff();
    m_MaskedImageGenerator->GenerateLayer();
    m_MaskedImageGenerator->GetLayer()->SetName("Masked Image");
    m_RenderingModel->AddLayer(m_MaskedImageGenerator->GetLayer());

    //update generator output information
    m_MaskFilter->GetOutput()->UpdateOutputInformation();
    m_MaskImageFilter->GetOutput()->UpdateOutputInformation();
    }
}

void ConnectedComponentSegmentationModule::UpdateCCSegmentationLayer()
{

  if (m_CurrentExpressionCC.compare(m_CCFilter->GetFunctor().GetExpression()))
    {
    m_HasToGenerateCCSegmentationLayer = true;
    // CC segmentation has to be regenerated, further processing has to be regenerated
    m_RelabelOutputReady = false;
    m_OBIAOpeningOutputReady = false;

    }

  if (m_HasToGenerateCCSegmentationLayer || !m_CCSegmentationReady)
    {

    this->pBusyBar->value(1);
    this->pBusyBar->show();
    Fl::check();
    // Functor Update
    m_CCFilter->SetMaskImage(m_MaskFilter->GetOutput());
    m_CurrentExpressionCC = ui_CCExpression->value();
    m_CCFilter->GetFunctor().SetExpression(m_CurrentExpressionCC);
    m_CCFilter->Modified(); // Modified() method must be explicitly called to take into account Functor change

    m_CCColorMapper->SetInput(m_CCFilter->GetOutput());

    m_RenderingModel->DeleteLayerByName("CCSegmentation");

    m_CCSegmentationGenerator->SetImage(m_CCColorMapper->GetOutput());

    m_CCSegmentationLabelGenerator->SetImage(m_CCFilter->GetOutput());

    m_CCSegmentationGenerator->GenerateQuicklookOff();
    m_CCSegmentationGenerator->GenerateLayer();
    m_CCSegmentationGenerator->GetLayer()->SetName("CCSegmentation");

    m_CCSegmentationLabelGenerator->GenerateQuicklookOff();
    m_CCSegmentationLabelGenerator->GenerateLayer();
    m_CCSegmentationLabelGenerator->GetLayer()->SetName("CCSegmentationLabel");

    m_RenderingModel->AddLayer(m_CCSegmentationGenerator->GetLayer());
    m_RenderingModel->AddLayer(m_CCSegmentationLabelGenerator->GetLayer());
    m_CCFilter->GetOutput()->UpdateOutputInformation();
    // ColorMapper MetDataDictionnary need to be cleared to avoid bandlist problem
    // for example with WV2 img
    itk::MetaDataDictionary blankMetaDataDictionary;
    m_CCColorMapper->GetOutput()->SetMetaDataDictionary(blankMetaDataDictionary);
    m_CCColorMapper->GetOutput()->UpdateOutputInformation();

    m_HasToGenerateCCSegmentationLayer = false;

    }
}

void ConnectedComponentSegmentationModule::UpdateRelabelLayer()
{

  double minObjectSize = uiMinSize->value();
  if (!m_RelabelOutputReady || m_CCRelabelFilter->GetMinimumObjectSize() != minObjectSize)
    {
    m_HasToGenerateRelabelLayer = true;
    m_OBIAOpeningOutputReady = false;
    }

  if (m_HasToGenerateRelabelLayer)
    {

    this->pBusyBar->value(1);
    this->pBusyBar->show();
    Fl::check();

    m_CCRelabelFilter->SetInput(m_CCFilter->GetOutput());
    m_CCRelabelFilter->SetMinimumObjectSize(minObjectSize);
    m_RelabelColorMapper->SetInput(m_CCRelabelFilter->GetOutput());

    m_RenderingModel->DeleteLayerByName("RelabelRGB");

    m_RelabelRGBGenerator->SetImage(m_RelabelColorMapper->GetOutput());

    m_RelabelRGBGenerator->GenerateQuicklookOff();
    m_RelabelRGBGenerator->GenerateLayer();
    m_RelabelRGBGenerator->GetLayer()->SetName("RelabelRGB");

    m_RelabelGenerator->SetImage(m_CCRelabelFilter->GetOutput());

    m_RelabelGenerator->GenerateQuicklookOff();
    m_RelabelGenerator->GenerateLayer();
    m_RelabelGenerator->GetLayer()->SetName("Relabel");

    m_RenderingModel->AddLayer(m_RelabelRGBGenerator->GetLayer());
    m_RenderingModel->AddLayer(m_RelabelGenerator->GetLayer());

    m_CCRelabelFilter->GetOutput()->UpdateOutputInformation();
    // ColorMapper MetDataDictionnary need to be cleared to avoid bandlist problem
    // for example with WV2 img
    itk::MetaDataDictionary blankMetaDataDictionary;
    m_RelabelColorMapper->GetOutput()->SetMetaDataDictionary(blankMetaDataDictionary);
    m_RelabelColorMapper->GetOutput()->UpdateOutputInformation();

    m_HasToGenerateRelabelLayer = false;
    }
}

void ConnectedComponentSegmentationModule::UpdateOBIAOpeningLayer()
{

  if (m_CurrentExpressionOBIA.compare(m_OBIAOpeningFilter->GetExpression()))
    {
    m_HasToGenerateOBIAOpeningLayer = true;
    }

  if (m_HasToGenerateOBIAOpeningLayer || !m_OBIAOpeningOutputReady)
    {

    this->pBusyBar->value(1);
    this->pBusyBar->show();
    Fl::check();
    m_CCImageToCCLabelMapFilter->SetInput(m_CCRelabelFilter->GetOutput());
    m_CCImageToCCLabelMapFilter->SetBackgroundValue(0);
    // intermediate step : Fusion
    // TBD

    // shape attributes computation
    m_ShapeLabelMapFilter->SetInput(m_CCImageToCCLabelMapFilter->GetOutput());
    //m_ShapeLabelMapFilter->SetReducedAttributeSet(false);
    m_ShapeLabelMapFilter->SetReducedAttributeSet(m_ShapeReducedSetOfAttributes);
    m_ShapeLabelMapFilter->SetComputePolygon(m_ComputePolygon);
    m_ShapeLabelMapFilter->SetComputeFlusser(m_ComputeFlusser);
    m_ShapeLabelMapFilter->SetComputeFeretDiameter(m_ComputeFeretDiameter);
    m_ShapeLabelMapFilter->SetComputeFlusser(m_ComputeFlusser);

    m_ShapeLabelMapFilter->SetLabelImage(m_CCRelabelFilter->GetOutput());

    // band stat attributes computation

    m_RadiometricLabelMapFilter->SetInput(m_ShapeLabelMapFilter->GetOutput());

    m_RadiometricLabelMapFilter->SetFeatureImage(m_InputImage);

    m_RadiometricLabelMapFilter->SetReducedAttributeSet(m_StatsReducedSetOfAttributes);

    // step 4 OBIA Filtering using shape and radiometric object attributes.
    // m_RadiometricLabelMapFilter->GetOutput()->UpdateOutputInformation();
    if (m_NoOBIAOpening)
      {
      m_OutputLabelMap = m_RadiometricLabelMapFilter->GetOutput();
      // m_OutputLabelMap = m_ShapeLabelMapFilter->GetOutput();
      m_CurrentExpressionOBIA = "";
      m_OBIAOpeningFilter->SetExpression(m_CurrentExpressionOBIA);
      }
    else
      {
      m_CurrentExpressionOBIA = ui_OBIAExpression->value();

      m_OBIAOpeningFilter->SetExpression(m_CurrentExpressionOBIA);

      m_OBIAOpeningFilter->SetInput(m_RadiometricLabelMapFilter->GetOutput());

      //m_OBIAOpeningFilter->SetInput(m_ShapeLabelMapFilter->GetOutput());
      m_OutputLabelMap = m_OBIAOpeningFilter->GetOutput();

      }

    //m_OutputLabelMap = m_ShapeLabelMapFilter->GetOutput();
    m_OBIAOpeningLabelMapToLabelImageFilter->SetInput(m_OutputLabelMap);
    m_OBIAOpeningLabelMapToLabelImageFilter->UpdateOutputInformation();
    m_OBIAOpeningColorMapper->SetInput(m_OBIAOpeningLabelMapToLabelImageFilter->GetOutput());

    m_RenderingModel->DeleteLayerByName("OBIA Opening");

    m_OBIAOpeningGenerator->SetImage(m_OBIAOpeningColorMapper->GetOutput());

    m_OBIAOpeningGenerator->GenerateQuicklookOff();
    m_OBIAOpeningGenerator->GenerateLayer();
    m_OBIAOpeningGenerator->GetLayer()->SetName("OBIA Opening");

    m_OBIAOpeningLabelGenerator->SetImage(m_OBIAOpeningLabelMapToLabelImageFilter->GetOutput());

    m_OBIAOpeningLabelGenerator->GenerateQuicklookOff();
    m_OBIAOpeningLabelGenerator->GenerateLayer();
    m_OBIAOpeningLabelGenerator->GetLayer()->SetName("OBIA Opening Label");

    m_RenderingModel->AddLayer(m_OBIAOpeningGenerator->GetLayer());
    m_RenderingModel->AddLayer(m_OBIAOpeningLabelGenerator->GetLayer());
    itk::MetaDataDictionary blankMetaDataDictionary;
    m_OBIAOpeningColorMapper->GetOutput()->SetMetaDataDictionary(blankMetaDataDictionary);
    m_OBIAOpeningColorMapper->GetOutput()->UpdateOutputInformation();

    if (m_ObjectDescriptionActionHandler->GetAttributesLabelMap() != m_OutputLabelMap)
      {
      m_ObjectDescriptionActionHandler->SetAttributesLabelMap(m_OutputLabelMap);
      }

    m_OutputLabelMap->UpdateOutputInformation();

    m_HasToGenerateOBIAOpeningLayer = false;
    }
}

void ConnectedComponentSegmentationModule::Process()
{

  if (uiTmpOutputSelection->value() > INPUT_IMAGE)
    {
    this->UpdateMaskLayer();
    m_MaskOutputReady = true;
    }

  if (uiTmpOutputSelection->value() > MASK)
    {

    this->UpdateCCSegmentationLayer();
    m_CCSegmentationReady = true;
    }

  // relabel component
  if (uiTmpOutputSelection->value() > CONNECTED_COMPONENT_SEGMENTATION_OUTPUT)
    {

    this->UpdateRelabelLayer();
    m_RelabelOutputReady = true;
    }

  if (uiTmpOutputSelection->value() > SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION)
    {

    this->UpdateOBIAOpeningLayer();

    m_OBIAOpeningOutputReady = true;
    }

  // Layer choice
  m_MaskGenerator->GetLayer()->SetVisible(false);
  m_MaskedImageGenerator->GetLayer()->SetVisible(false);
  m_CCSegmentationGenerator->GetLayer()->SetVisible(false);
  m_CCSegmentationLabelGenerator->GetLayer()->SetVisible(false);
  m_RelabelRGBGenerator->GetLayer()->SetVisible(false);
  m_RelabelGenerator->GetLayer()->SetVisible(false);
  m_ImageGenerator->GetLayer()->SetVisible(false);
  m_OBIAOpeningGenerator->GetLayer()->SetVisible(false);
  m_OBIAOpeningLabelGenerator->GetLayer()->SetVisible(false);

  m_PixelDescriptionModel->ClearLayers();

  if (uiTmpOutputSelection->value() == INPUT_IMAGE)
    {
    m_ImageGenerator->GetLayer()->SetVisible(true);
    m_PixelDescriptionModel->AddLayer(m_ImageGenerator->GetLayer());
    }

  if (uiTmpOutputSelection->value() == MASK)
    {
    m_MaskGenerator->GetLayer()->SetVisible(true);
    m_PixelDescriptionModel->AddLayer(m_MaskGenerator->GetLayer());
    }
  if (uiTmpOutputSelection->value() == MASKED_IMAGE)
    {
    m_MaskedImageGenerator->GetLayer()->SetVisible(true);
    m_PixelDescriptionModel->AddLayer(m_MaskedImageGenerator->GetLayer());
    }
  if (uiTmpOutputSelection->value() == CONNECTED_COMPONENT_SEGMENTATION_OUTPUT)
    {
    m_CCSegmentationGenerator->GetLayer()->SetVisible(true);
    m_CCSegmentationLabelGenerator->GetLayer()->SetVisible(true);

    BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
    blender->SetAlpha(1);
    m_CCSegmentationLabelGenerator->SetBlendingFunction(blender);

    m_PixelDescriptionModel->AddLayer(m_CCSegmentationLabelGenerator->GetLayer());
    }
  if (uiTmpOutputSelection->value() == SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION)
    {
    m_RelabelRGBGenerator->GetLayer()->SetVisible(true);
    m_RelabelGenerator->GetLayer()->SetVisible(true);
    BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
    blender->SetAlpha(1);
    //m_RelabelRGBGenerator->SetRenderingFunction(NoStretchRenderingFunctionType::New());

    m_RelabelGenerator->SetBlendingFunction(blender);
    m_PixelDescriptionModel->AddLayer(m_RelabelGenerator->GetLayer());

    }
  if (uiTmpOutputSelection->value() == OUTPUT)
    {

    m_OBIAOpeningGenerator->GetLayer()->SetVisible(true);
    m_OBIAOpeningLabelGenerator->GetLayer()->SetVisible(true);

    BlendingFunctionType::Pointer blender = BlendingFunctionType::New();
    blender->SetAlpha(1);
    m_OBIAOpeningLabelGenerator->SetBlendingFunction(blender);
    m_PixelDescriptionModel->AddLayer(m_OBIAOpeningLabelGenerator->GetLayer());
    }

  m_PixelDescriptionModel->NotifyAll();
  m_RenderingModel->Update();
  this->Show();
  this->pBusyBar->value(0);
  this->pBusyBar->hide();
  Fl::check();
}

void ConnectedComponentSegmentationModule::TmpOutputSelection()
{
  this->CheckProcess();
  this->Show();
}
/**
 * Cancel CallBack
 */
void ConnectedComponentSegmentationModule::Cancel()
{
  this->ClearOutputDescriptors();
  this->BusyOff();
  this->Hide();
}
/**
 * OK CallBack
 */
void ConnectedComponentSegmentationModule::OK()
{

  // check if the expression are ok
  this->LiveCheckMask();
  this->LiveCheckOBIA();
  this->LiveCheckCC();

  this->ClearOutputDescriptors();

  //  if the pipeline is ok
  if (m_IsOBIAExpressionOK && m_IsCCExpressionOK && m_IsMaskExpressionOK)
    {
    // close the GUI
    //this->Hide();

    StreamingConnectedComponentSegmentationOBIAToVectorDataFilterType::FilterType::Pointer
        streamingFilter = StreamingConnectedComponentSegmentationOBIAToVectorDataFilterType::FilterType::New();

    streamingFilter->GetFilter()->SetInput(m_InputImage);

    if (m_NoMask)
      streamingFilter->GetFilter()->SetMaskExpression("1");
    else streamingFilter->GetFilter()->SetMaskExpression(m_MaskFilter->GetExpression());

    streamingFilter->GetFilter()->SetConnectedComponentExpression(m_CCFilter->GetFunctor().GetExpression());
    streamingFilter->GetFilter()->SetMinimumObjectSize(uiMinSize->value());
    streamingFilter->GetFilter()->SetShapeReducedSetOfAttributes(m_ShapeReducedSetOfAttributes);
    streamingFilter->GetFilter()->SetComputePolygon(m_ComputePolygon);
    streamingFilter->GetFilter()->SetComputePerimeter(m_ComputePerimeter);
    streamingFilter->GetFilter()->SetComputeFeretDiameter(m_ComputeFeretDiameter);
    streamingFilter->GetFilter()->SetComputeFlusser(m_ComputeFlusser);

    if (m_NoOBIAOpening)
      streamingFilter->GetFilter()->SetOBIAExpression("1");
    else streamingFilter->GetFilter()->SetOBIAExpression(m_OBIAOpeningFilter->GetExpression());

    FltkFilterWatcher qlwatcher(streamingFilter->GetStreamer(), 0, 0, 200, 20,
                                otbGetTextMacro("Processing entire image ..."));

    streamingFilter->Update();

    m_OutputVectorData = streamingFilter->GetFilter()->GetOutputVectorData();

    /*
     * Reprojection of the output VectorData
     *
     * The output of the Filter is in image physical coordinates,
     * projection WKT applied if the input image has one
     *
     * We need to reproject in WGS84 if the input image is in sensor model geometry
     */

    std::string projRef = m_InputImage->GetProjectionRef();
    ImageKeywordlist kwl = m_InputImage->GetImageKeywordlist();

    if (projRef.empty() && kwl.GetSize() > 0)
      {

      // image is in sensor model geometry

      // Reproject VectorData in image projection
      typedef otb::VectorDataProjectionFilter<VectorDataType, VectorDataType> VectorDataProjectionFilterType;

      VectorDataProjectionFilterType::Pointer vproj = VectorDataProjectionFilterType::New();
      vproj->SetInput(m_OutputVectorData);
      vproj->SetInputKeywordList(kwl);
      //vproj->SetInputOrigin(m_InputImage->GetOrigin());
      //vproj->SetInputSpacing(m_InputImage->GetSpacing());

      const char *cfname;
      if (!m_UseDEM)
        {
        const char* defaultPath = "";
        if ( otb::ConfigurationFile::GetInstance()->IsValid() )
          {
          defaultPath = otb::ConfigurationFile::GetInstance()->GetDEMDirectory().c_str();
          }

        cfname = flu_dir_chooser("Choose DEM directory if you want to...", defaultPath);
        Fl::check();
        }
      else
        {
        cfname = m_DEMPath.c_str();
        }

      if (cfname != NULL)
        {

        // check the directory
        typedef otb::DEMHandler DEMHandlerType;
        DEMHandlerType::Pointer DEMTest = DEMHandlerType::Instance();

        if (!(itksys::SystemTools::FileIsDirectory(cfname) && DEMTest->IsValidDEMDirectory(cfname)))
          {
          std::ostringstream oss;
          oss << "Invalid DEM directory " << cfname << "."<<std::endl;
          oss << "Output Vector Data will be created without DEM information" <<std::endl;
          MsgReporter::GetInstance()->SendError(oss.str());
          }

        DEMTest->OpenDEMDirectory(cfname);

        }

      vproj->Update();
      m_OutputVectorData = vproj->GetOutput();
      }

    /*
     m_OBIAOpeningColorMapper->SetInput(m_OBIAOpeningLabelMapToLabelImageFilter->GetOutput());

     m_OutputRGBLabelImage = m_OBIAOpeningColorMapper->GetOutput();

     this->AddOutputDescriptor(m_OutputRGBLabelImage, "OutputLabelImage", otbGetTextMacro("Output image"));
     */
    this->AddOutputDescriptor(m_OutputVectorData, "OutputVectorData", otbGetTextMacro("Output vector data"));

    this->NotifyOutputsChange();
    }
  else
    {
    std::ostringstream oss;
    oss << "At least one formula is wrong" << std::endl << " Mask expression " << m_IsMaskExpressionOK << std::endl
        << " CC Segmentation expression " << m_IsCCExpressionOK << std::endl << " Object Analysis formula "
        << m_IsOBIAExpressionOK << std::endl;

    MsgReporter::GetInstance()->SendError(oss.str());

    }
  // Once module is closed, it is no longer busy
  this->BusyOff();
  // Close the GUI
  this->Hide();
}

} // End namespace otb

#endif
