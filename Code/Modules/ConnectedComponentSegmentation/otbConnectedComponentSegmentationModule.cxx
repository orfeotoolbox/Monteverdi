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

#include "otbFltkFilterWatcher.h"

#include "otbVectorImageToImageListFilter.h"

#include <time.h>

double diffclock(clock_t clock1,clock_t clock2)
{
  double diffticks=clock1-clock2;
  double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
  return diffms;
}




namespace otb
{
/**
 * Constructor
 */
ConnectedComponentSegmentationModule::ConnectedComponentSegmentationModule()
{
  // Build a view
  m_View                 = ViewType::New();

  // Build a controller
  m_WidgetsController           = WidgetControllerType::New();

  // Layer Generators
  m_ImageGenerator          = VectorImageLayerGeneratorType::New();
  m_MaskGenerator          = ImageLayerGeneratorType::New();
  m_CCSegmentationGenerator = RGBImageLayerGeneratorType::New();
  m_RelabelGenerator = RGBImageLayerGeneratorType::New();
  m_OBIAOpeningGenerator = RGBImageLayerGeneratorType::New();

  // Build a new rendering model
  m_RenderingModel       = RenderingModelType::New();

  // Build the handlers
  m_ResizingHandler       = ResizingHandlerType::New();
  m_ChangeRegionHandler   = ChangeRegionHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler = ChangeScaleHandlerType::New();


  // Add the action handlers to the widgets controller
  m_WidgetsController->AddActionHandler(m_ResizingHandler);
  m_WidgetsController->AddActionHandler(m_ChangeRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_WidgetsController->AddActionHandler(m_ChangeScaleHandler);



  // Wire the MVC
  m_View->SetModel(m_RenderingModel);
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

  m_NoMask=true;

  // reduced set of attributes
  m_ShapeReducedSetOfAttributes=true;
  m_StatsReducedSetOfAttributes=true;

  // output not available
  m_MaskOutputReady=false;
  m_CCSegmentationReady=false;
  m_RelabelOutputReady=false;
  m_OBIAOpeningOutputReady=false;

  // Build the GUI
  this->CreateGUI();


  // Remove registered visualization components from the interface
  gImageViewer->add(m_View->GetFullWidget());
  gScroll->add(m_View->GetScrollWidget());
  gZoom->add(m_View->GetZoomWidget());

  gImageViewer->resizable(m_View->GetFullWidget());
  gScroll->resizable(m_View->GetScrollWidget());
  gScroll->resizable(m_View->GetZoomWidget());

  m_View->GetFullWidget()->resize(gImageViewer->x(), gImageViewer->y(), gImageViewer->w(), gImageViewer->h());
  m_View->GetScrollWidget()->resize(gScroll->x(), gScroll->y(), gScroll->w(), gScroll->h());
  m_View->GetZoomWidget()->resize(gZoom->x(), gZoom->y(), gZoom->w(), gZoom->h());

  // Show and refresh the interface
  // this->wMainWindow->show();

  m_View->GetFullWidget()->show();
  m_View->GetScrollWidget()->show();
  m_View->GetZoomWidget()->show();



  // Describe inputs
  this->AddInputDescriptor<VectorImageType>("InputImage", otbGetTextMacro("Image to process"), false, false);


}

/**
 * Destructor
 */
ConnectedComponentSegmentationModule::~ConnectedComponentSegmentationModule()
{}

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
  ui_Help->value(0);
}

void ConnectedComponentSegmentationModule::Show()
{
  // Show the main window
  guiMainWindow->show();
  m_View->GetScrollWidget()->show();
  m_View->GetFullWidget()->show();
  m_View->GetZoomWidget()->show();
}

void ConnectedComponentSegmentationModule::OBIA_functor_init()
{

  m_ShapeAttributes.push_back("Elongation");
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
  m_ShapeAttributes.push_back("PhysicalSize");

  //TO DO JGU
  if(!m_ShapeReducedSetOfAttributes)
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


  m_StatAttributes.push_back("Kurtosis");
  m_StatAttributes.push_back("Mean");
  m_StatAttributes.push_back("Skewness");
  m_StatAttributes.push_back("Variance");
  //TO DO JGU
   if(!m_StatsReducedSetOfAttributes)
     {
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

  m_OBIAOpeningFilter->SetAttributes(m_ShapeAttributes,m_StatAttributes,m_NumberOfInputBands);
}


/** 
 * The custom run command 
 */
void ConnectedComponentSegmentationModule::Run()
{
  // Until window closing, module will be busy
  this->BusyOn();


  // Get input image
  m_InputImage = this->GetInputData<VectorImageType>("InputImage");


  if (m_InputImage.IsNull())
    {
    itkExceptionMacro(<< "Input image is NULL");
    }

  m_NumberOfInputBands=m_InputImage->GetNumberOfComponentsPerPixel();
  m_InputMask = ImageType::New();

  // Instantiate filters
  m_CCFilter=ConnectedComponentFilterType::New();
  m_MaskFilter=MaskMuParserFilterType::New();
  //m_OpeningFilter=LabelObjectOpeningFilterType::New();
  m_CCRelabelFilter= RelabelComponentFilterType::New();
  m_OBIAOpeningFilter= LabelObjectOpeningFilterType::New();
  m_OBIAOpeningLabelMapToLabelImageFilter = LabelMapToLabelImageFilterType::New();
  m_CCImageToCCLabelMapFilter  = LabelImageToLabelMapFilterType::New();
  m_ShapeLabelMapFilter = ShapeLabelMapFilterType::New();
  m_RadiometricLabelMapFilter = RadiometricLabelMapFilterType::New();
  m_OBIAOpeningLabelMapToVectorDataFilter = LabelMapToVectorDataFilterType::New();


  // instantiate colormapper
  m_CCColorMapper = ColorMapFilterType::New();
  m_RelabelColorMapper = ColorMapFilterType::New();
  m_OBIAOpeningColorMapper = ColorMapFilterType::New();

  itk::ImageRegion<2> imageRegion;
  imageRegion.SetSize(1,1);
  imageRegion.SetIndex(1,1);

  m_InputImage->SetRequestedRegion(imageRegion);
  m_InputImage->Update();

  // if no data we don't use mask
  m_MaskFilter->SetInput(m_InputImage);
  this->UpdateMaskFormulaVariablesList();
  this->LiveCheckMask();
  //
  m_CCFilter->SetInput(m_InputImage);

  this->UpdateCCFormulaVariablesList();
  this->LiveCheckCC();

  this->OBIA_functor_init();
  this->UpdateOBIAFormulaVariablesList();
  this->LiveCheckOBIA();

  // Generate the layer
  m_ImageGenerator->SetImage(m_InputImage);
  FltkFilterWatcher qlwatcher(m_ImageGenerator->GetProgressSource(), 0, 0, 200, 20, otbGetTextMacro("Generating QuickLook ..."));
  m_ImageGenerator->GenerateLayer();

  m_InputImageLayer = m_ImageGenerator->GetLayer();
  m_InputImageLayer->SetName("ImageLayer");

  // Clear previous layers
  m_RenderingModel->ClearLayers();
  // m_InputImageLayer->SetVisible(false);

  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(m_InputImageLayer);
  m_RenderingModel->Update();

  // other layers has to be generated
  m_HasToGenerateMaskLayer=true;
  m_HasToGenerateCCSegmentationLayer=true;
  m_HasToGenerateRelabelLayer=true;
  m_HasToGenerateOBIAOpeningLayer=true;


  // INPUT_IMAGE
  uiTmpOutputSelection->add("Input image");

  // MASK_OUTPUT
  uiTmpOutputSelection->add("Mask Output");

  //  CONNECTED_COMPONENT_SEGMENTATION_OUTPUT,

  uiTmpOutputSelection->add("Segmentation output");

  //  SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION,
  uiTmpOutputSelection->add("Segmentation after small objects rejection");

  //   OUTPUT
  uiTmpOutputSelection->add("Filter Output");

  // Default value : INPUT_IMAGE
  uiTmpOutputSelection->value(INPUT_IMAGE);

  // Initialize the help window
  this->InitHelp();

  // Show the GUI
  this->Show();
}

void ConnectedComponentSegmentationModule::UpdateMaskFormulaVariablesList()
{

  const std::map<std::string,double*>& variables = m_MaskFilter->GetVar();

  // Get the number of variables
  std::map<std::string,double*>::const_iterator item = variables.begin();

  // Query the variables
  for (; item!=variables.end(); ++item)
    {
    ui_VarNamesMask->add(item->first.c_str());
    }

}



void ConnectedComponentSegmentationModule::UpdateCCFormulaVariablesList()
{


  CCFunctorType& tempFunctor= m_CCFilter->GetFunctor();
  tempFunctor.SetExpression(ui_CCExpression->value());

  // Define the iterators
  itk::ImageConstIterator<VectorImageType> inputIt(m_InputImage ,m_InputImage->GetRequestedRegion());
  inputIt.GoToBegin();



  try
  {
    tempFunctor(inputIt.Get(),inputIt.Get());
  }
  catch(itk::ExceptionObject& err)
  {
    itkWarningMacro(<< err);
  }

  const std::map<std::string,double*> & variables= tempFunctor.GetVar();

  std::map<std::string,double*>::const_iterator item = variables.begin();

  // Query the variables
  for (; item!=variables.end(); ++item)
    {
    ui_VarNamesCC->add(item->first.c_str());
    }



}

void ConnectedComponentSegmentationModule::UpdateOBIAFormulaVariablesList()
{

  const std::map<std::string,double*>& variables = m_OBIAOpeningFilter->GetVar();

  // Get the number of variables
  std::map<std::string,double*>::const_iterator item = variables.begin();

  // Query the variables
  for (; item!=variables.end(); ++item)
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
  helpContent << "- Connected Component Segmentation module "<< std::endl<< std::endl;
  helpContent << "Fill each formula, choose your visualization output and click on Update button" << std::endl;
  helpContent << "If Mask formula is left blank, no mask used"<< std::endl;
  helpContent << "If OBIA formula is left blank, no post processing is applied " << std::endl;

  ui_HelpText->value(helpContent.str().c_str());
}


/**
 * Quick add a variable name into the expression
 */
void ConnectedComponentSegmentationModule::QuickAddMask()
{
  std::ostringstream tmpExpression;
  // ui_VarNamesMask->select(idx);

  tmpExpression << ui_MaskExpression->value() << " " <<ui_VarNamesMask->text() ;
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
  // ui_VarNamesMask->select(idx);

  tmpExpression << ui_CCExpression->value() << " " <<ui_VarNamesCC->text() ;
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
  // ui_VarNamesMask->select(idx);

  tmpExpression << ui_OBIAExpression->value() << " " <<ui_VarNamesOBIA->text() ;
  ui_OBIAExpression->value(tmpExpression.str().c_str());
  ui_OBIAExpression->take_focus();

  LiveCheckOBIA();
}


void ConnectedComponentSegmentationModule::CheckProcess()
{

  if(uiTmpOutputSelection->value()==INPUT_IMAGE)
    {
    ui_Update->activate();
    }

  if(uiTmpOutputSelection->value()==MASK_IMAGE)
    {
    if(m_IsMaskExpressionOK)
      ui_Update->activate();
    else
      ui_Update->deactivate();
    }

  if(uiTmpOutputSelection->value()==CONNECTED_COMPONENT_SEGMENTATION_OUTPUT)
    {
    if(m_IsCCExpressionOK && m_IsMaskExpressionOK )
      ui_Update->activate();
    else
      ui_Update->deactivate();
    }

  if(uiTmpOutputSelection->value()==SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION)
    {
    if(m_IsCCExpressionOK && m_IsMaskExpressionOK )
      ui_Update->activate();
    else
      ui_Update->deactivate();
    }

  if(uiTmpOutputSelection->value()==OUTPUT)
    {
    if(m_IsCCExpressionOK && m_IsMaskExpressionOK &&  m_IsOBIAExpressionOK)
      ui_Update->activate();
    else
      ui_Update->deactivate();
    }

}




void ConnectedComponentSegmentationModule::LiveCheckMask()
{
  m_IsMaskExpressionOK = true;
  m_NoMask=false;

  ui_MaskExpression->color(FL_GREEN);
  ui_MaskExpression->tooltip("The Expression is Valid");

  if(strcmp(ui_MaskExpression->value(),""))
    {
    m_MaskFilter->SetExpression(ui_MaskExpression->value());
    //m_MaskFilter->UpdateOutputInformation();
    if(!(m_IsMaskExpressionOK = m_MaskFilter->CheckExpression()))
      {
      ui_MaskExpression->color(FL_RED);
      m_NoMask=true;
      //ui_MaskExpression->tooltip(err.GetDescription());
      //ui_Ok->deactivate();
      }

    }
  else
    {
    m_NoMask=true;

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

  CCFunctorType& checkFunctor= m_CCFilter->GetFunctor();
  checkFunctor.SetExpression(ui_CCExpression->value());
  // Define the iterators
  itk::ImageConstIterator<VectorImageType> inputIt(m_InputImage ,m_InputImage->GetRequestedRegion());
  inputIt.GoToBegin();



  try
  {
    checkFunctor(inputIt.Get(),inputIt.Get());
    m_IsCCExpressionOK = true;
    ui_CCExpression->color(FL_GREEN);
    ui_CCExpression->tooltip("The Expression is Not Valid");
  }
  catch(itk::ExceptionObject& err)
  {

    ui_CCExpression->tooltip(err.GetDescription());

  }


  ui_CCExpression->redraw();

  this->CheckProcess();

}


void ConnectedComponentSegmentationModule::LiveCheckOBIA()
{
  m_NoOBIAOpening=false;
  m_IsOBIAExpressionOK = true;
  if(strcmp(ui_OBIAExpression->value(),""))
    {
    ui_OBIAExpression->color(FL_RED);
    ui_OBIAExpression->tooltip("The Expression is not Valid");


    m_OBIAOpeningFilter->SetExpression(ui_OBIAExpression->value());
    //m_MaskFilter->UpdateOutputInformation();

    if((m_IsOBIAExpressionOK = m_OBIAOpeningFilter->CheckExpression()))
      {
      ui_OBIAExpression->color(FL_GREEN);
      ui_OBIAExpression->tooltip("The Expression is Valid");

      }
    }
  else
    {
    ui_OBIAExpression->color(FL_GREEN);
    m_NoOBIAOpening=true;
    ui_OBIAExpression->tooltip("No OBIA Opening, all labeled object are valid");
    }

  ui_OBIAExpression->redraw();
  this->CheckProcess();


}

void ConnectedComponentSegmentationModule::SetObjectMinArea()
{

}

void ConnectedComponentSegmentationModule::UpdateTmpOutput()
{
  this->Process();
}



void ConnectedComponentSegmentationModule::UpdateMaskLayer()
{
  //test layer generation

  m_HasToGenerateMaskLayer = false;
  string mask_expression;
  if(m_NoMask)
    {
    mask_expression="1";
    }
  else
    {
    mask_expression=ui_MaskExpression->value();
    }
  //std::cout<<"mask_expression "<<mask_expression<<std::endl;
  if(m_MaskFilter->GetExpression()!=mask_expression)
    {

    m_HasToGenerateMaskLayer =true;

    // mask has to be regenerated, all processing has to be regenerated
    m_CCSegmentationReady=false;
    m_RelabelOutputReady=false;
    m_OBIAOpeningOutputReady=false;


    }

  if(m_HasToGenerateMaskLayer || !m_MaskOutputReady)
    {
    m_RenderingModel->DeleteLayerByName("Mask");

    m_MaskFilter->SetExpression(mask_expression);
    m_MaskGenerator->SetImage(m_MaskFilter->GetOutput());
    m_MaskGenerator->GenerateQuicklookOff();
    m_MaskGenerator->GenerateLayer();
    m_MaskGenerator->GetLayer()->SetName("Mask");

    m_RenderingModel->AddLayer(m_MaskGenerator->GetLayer());


    // m_RenderingModel->Update();
    m_MaskFilter->GetOutput()->UpdateOutputInformation();
    // m_HasToGenerateMaskLayer=false;


    }
}


void ConnectedComponentSegmentationModule::UpdateCCSegmentationLayer()
{

  if(!m_CCSegmentationReady || m_CCFilter->GetFunctor().GetExpression()!=ui_CCExpression->value())
     {
     m_HasToGenerateCCSegmentationLayer =true;
     // CC segmentation has to be regenerated, further processing has to be regenerated
     m_RelabelOutputReady=false;
     m_OBIAOpeningOutputReady=false;
     }


  if(m_HasToGenerateCCSegmentationLayer)
    {
      m_CCFilter->SetMaskImage(m_MaskFilter->GetOutput());
      m_CCFilter->GetFunctor().SetExpression(ui_CCExpression->value());
      m_CCColorMapper->SetInput(m_CCFilter->GetOutput());


      m_RenderingModel->DeleteLayerByName("CCSegmentation");

      m_CCSegmentationGenerator->SetImage(m_CCColorMapper->GetOutput());

      m_CCSegmentationGenerator->GenerateQuicklookOff();
      m_CCSegmentationGenerator->GenerateLayer();
      m_CCSegmentationGenerator->GetLayer()->SetName("CCSegmentation");

      m_RenderingModel->AddLayer(m_CCSegmentationGenerator->GetLayer());


      // m_RenderingModel->Update();
      m_CCFilter->GetOutput()->UpdateOutputInformation();
      m_CCColorMapper->GetOutput()->UpdateOutputInformation();
      m_HasToGenerateCCSegmentationLayer=false;
    }
}

void ConnectedComponentSegmentationModule::UpdateRelabelLayer()
{
  double minObjectSize=uiMinSize->value();
  if(!m_RelabelOutputReady || m_CCRelabelFilter->GetMinimumObjectSize()!=minObjectSize)
    {
        m_HasToGenerateRelabelLayer =true;
        // CC segmentation has to be regenerated, further processing has to be regenerated
        m_OBIAOpeningOutputReady=false;
    }


  if(m_HasToGenerateRelabelLayer)
    {
    m_CCRelabelFilter->SetInput(m_CCFilter->GetOutput());
    m_CCRelabelFilter->SetMinimumObjectSize(minObjectSize);
    //
    m_RelabelColorMapper->SetInput( m_CCRelabelFilter->GetOutput());

    m_RenderingModel->DeleteLayerByName("Relabel");

    m_RelabelGenerator->SetImage( m_RelabelColorMapper->GetOutput());

    m_RelabelGenerator->GenerateQuicklookOff();
    m_RelabelGenerator->GenerateLayer();
    m_RelabelGenerator->GetLayer()->SetName("Relabel");

    m_RenderingModel->AddLayer(m_RelabelGenerator->GetLayer());


    // m_RenderingModel->Update();
    m_CCRelabelFilter->GetOutput()->UpdateOutputInformation();
    m_RelabelColorMapper->GetOutput()->UpdateOutputInformation();
    m_HasToGenerateRelabelLayer=false;
    }
}


void ConnectedComponentSegmentationModule::UpdateOBIAOpeningLayer()
{
  if(!m_OBIAOpeningOutputReady || m_OBIAOpeningFilter->GetExpression()!=ui_OBIAExpression->value())
    {
    m_HasToGenerateOBIAOpeningLayer =true;
    // CC segmentation has to be regenerated, further processing has to be regenerated
    m_OBIAOpeningOutputReady=false;
    }

  if(m_HasToGenerateOBIAOpeningLayer)
    {
    m_RenderingModel->DeleteLayerByName("OBIA Opening");

    m_OBIAOpeningGenerator->SetImage( m_OBIAOpeningColorMapper->GetOutput());

    m_OBIAOpeningGenerator->GenerateQuicklookOff();
    m_OBIAOpeningGenerator->GenerateLayer();
    m_OBIAOpeningGenerator->GetLayer()->SetName("OBIA Opening");

    m_RenderingModel->AddLayer(m_OBIAOpeningGenerator->GetLayer());


    // m_RenderingModel->Update();
    m_OBIAOpeningFilter->GetOutput()->UpdateOutputInformation();
    m_OBIAOpeningColorMapper->GetOutput()->UpdateOutputInformation();
    m_HasToGenerateOBIAOpeningLayer=false;
    }
}




void ConnectedComponentSegmentationModule::Process()
{
  //std::cout<<"process  ...."<<std::endl;

  // mask creation, if formula is left blank, no mask used

  //std::cout<<"create mask ";
  //time_t t_begin;
  //time_t t_end;

  //time(&t_begin);
  if(uiTmpOutputSelection->value()>INPUT_IMAGE)
  {
    this->UpdateMaskLayer();
    m_MaskOutputReady=true;
  }

  if(uiTmpOutputSelection->value()>MASK_IMAGE)
    {

      this->UpdateCCSegmentationLayer();
      m_CCFilter->Update();
      m_CCSegmentationReady=true;
    }

  // relabel component
  if(uiTmpOutputSelection->value()>CONNECTED_COMPONENT_SEGMENTATION_OUTPUT)
     {

      this->UpdateRelabelLayer();
      m_RelabelOutputReady=true;
     }

  if(uiTmpOutputSelection->value()>SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION)
    {

    m_CCImageToCCLabelMapFilter->SetInput(m_CCRelabelFilter->GetOutput());
    m_CCImageToCCLabelMapFilter->SetBackgroundValue(0);

    // intermediate step : Fusion
    // TBD

    // shape attributes computation
    m_ShapeLabelMapFilter->SetInput(m_CCImageToCCLabelMapFilter->GetOutput());
    //m_ShapeLabelMapFilter->SetReducedAttributeSet(false);
    m_ShapeLabelMapFilter->SetReducedAttributeSet(m_ShapeReducedSetOfAttributes);
    // band stat attributes computation

    m_RadiometricLabelMapFilter->SetInput(m_ShapeLabelMapFilter->GetOutput());
    // m_RadiometricLabelMapFilter->SetInput(m_CCImageToCCLabelMapFilter->GetOutput());

    m_RadiometricLabelMapFilter->SetFeatureImage(m_InputImage);
    //m_RadiometricLabelMapFilter->SetReducedAttributeSet(false);
    m_RadiometricLabelMapFilter->SetReducedAttributeSet(m_StatsReducedSetOfAttributes);
    m_RadiometricLabelMapFilter->Update();


    m_AttributesLabelMap=m_RadiometricLabelMapFilter->GetOutput();
    //AttributesLabelMap->SetAdjacencyMap(labelImageToLabelMap->GetOutput()->GetAdjacencyMap());

    // TODO JGU  code clean is necessary
    // DBG OUTPUT

    // step 4 OBIA Filtering using shape and radiometric object characteristics


    //unsigned int nb_obj=m_AttributesLabelMap->GetNumberOfLabelObjects();
    //std::cout<<"number of attributes map label objects : "<<nb_obj<<std::endl;

    //std::cout<<"OBIA opening ";
    //time(&t_begin);
    //LabelObjectOpeningFilterType::Pointer opening= LabelObjectOpeningFilterType::New();
    if( m_NoOBIAOpening)
      {
      m_OutputLabelMap=m_RadiometricLabelMapFilter->GetOutput();
      }
    else
      {
      m_OBIAOpeningFilter->SetExpression(ui_OBIAExpression->value());
      m_OBIAOpeningFilter->SetInput(m_RadiometricLabelMapFilter->GetOutput());
      m_OBIAOpeningFilter->Update();
      m_OutputLabelMap=m_OBIAOpeningFilter->GetOutput();
      }



    //nb_obj=m_OutputLabelMap->GetNumberOfLabelObjects();
    //std::cout<<"number of attributes map label objects after opening : "<<nb_obj<<std::endl;


    /*   AttributesMapLabelObjectType::Pointer Object_3941=m_OutputLabelMap->GetLabelObject(3941);

    std::vector<std::string> m_AttributesName=Object_3941->GetAvailableAttributes();

    for (unsigned int i=0;i<Object_3941->GetNumberOfAttributes();i++)
      {

      std::cout<<" "<<(m_AttributesName.at(i)).c_str()<<" "<<Object_3941->GetAttribute((m_AttributesName.at(i)).c_str())<<std::endl;


      }*/

    m_OBIAOpeningLabelMapToLabelImageFilter->SetInput(m_OutputLabelMap);
    m_OBIAOpeningLabelMapToLabelImageFilter->Update();

    m_OBIAOpeningColorMapper->SetInput(m_OBIAOpeningLabelMapToLabelImageFilter->GetOutput());

    this->UpdateOBIAOpeningLayer();

    m_OBIAOpeningOutputReady=true;
     }

  // Layer choice
  m_MaskGenerator->GetLayer()->SetVisible(false);
  m_CCSegmentationGenerator->GetLayer()->SetVisible(false);
  m_RelabelGenerator->GetLayer()->SetVisible(false);
  m_ImageGenerator->GetLayer()->SetVisible(false);
  m_OBIAOpeningGenerator->GetLayer()->SetVisible(false);

  if(uiTmpOutputSelection->value()==INPUT_IMAGE)
    {
    m_ImageGenerator->GetLayer()->SetVisible(true);
    }

  if(uiTmpOutputSelection->value()==MASK_IMAGE)
    {
    m_MaskGenerator->GetLayer()->SetVisible(true);
    }
  if(uiTmpOutputSelection->value()==CONNECTED_COMPONENT_SEGMENTATION_OUTPUT)
    {
    m_CCSegmentationGenerator->GetLayer()->SetVisible(true);
    }
  if(uiTmpOutputSelection->value()==SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION)
    {
    m_RelabelGenerator->GetLayer()->SetVisible(true);
    }
  if(uiTmpOutputSelection->value()==OUTPUT)
    {
    m_OBIAOpeningGenerator->GetLayer()->SetVisible(true);
    }


  m_RenderingModel->Update();
  this->Show();
  //time(&t_end);
  //std::cout<< double(difftime(t_end,t_begin)) <<"s"<<std::endl;

}




void ConnectedComponentSegmentationModule::ProcessCC()
{
  //std::cout<<"process  ...."<<std::endl;



  // launch CC //


  // update tmpOutput //

}


void ConnectedComponentSegmentationModule::TmpOutputSelection()
{


  this->CheckProcess();
  this->Show();
}

/**
 * OK CallBack
 */
void ConnectedComponentSegmentationModule::OK()
{
  this->CheckProcess();
  // Apply the filter
  //m_BandMathFilter->SetExpression(ui_CCExpression->value());
  //m_Output = m_BandMathFilter->GetOutput();
  this->ClearOutputDescriptors();
  // this->AddOutputDescriptor(m_BandMathFilter->GetOutput(), "OutputImage", otbGetTextMacro("Result image"));
  if(m_MaskOutputReady)
    this->AddOutputDescriptor(m_MaskFilter->GetOutput(), "OutputMask", otbGetTextMacro("Mask image"));
  else
    MsgReporter::GetInstance()->SendWarning("No mask generation");
  if(m_CCSegmentationReady)
    this->AddOutputDescriptor(m_CCFilter->GetOutput(), "CCSegmentationOutput", otbGetTextMacro("Connected Component Segmentation labeled output"));
  else
    MsgReporter::GetInstance()->SendWarning("No CC Segmentation has been applied");
  if(m_RelabelOutputReady)
    this->AddOutputDescriptor(m_CCRelabelFilter->GetOutput(), "RelabelOutput", otbGetTextMacro("Relabeled Image with small objects rejection"));
  else
    MsgReporter::GetInstance()->SendWarning("Segmentation output relabeling not available");
  if(m_OBIAOpeningOutputReady)
    this->AddOutputDescriptor(m_OutputLabelMap, "OBIAOpeningOutput", otbGetTextMacro("OBIA Opening output"));
  else
    MsgReporter::GetInstance()->SendWarning("OBIA output not available");

  // TODO JGU : change the flg to process complete
  if(m_OBIAOpeningOutputReady)
    {

    // transform OBIA opening output to vector data

    m_OBIAOpeningLabelMapToVectorDataFilter->SetInput(m_OutputLabelMap);
    m_OBIAOpeningLabelMapToVectorDataFilter->Update();

    m_OutputVectorData=m_OBIAOpeningLabelMapToVectorDataFilter->GetOutput();

    m_OutputLabelImage=m_OBIAOpeningLabelMapToLabelImageFilter->GetOutput();

    this->AddOutputDescriptor(m_OutputLabelImage, "OutputLabelImage", otbGetTextMacro("Output image"));
    this->AddOutputDescriptor(m_OutputVectorData, "OutputVectorData", otbGetTextMacro("Output vector data"));

    }
  this->NotifyOutputsChange();
  // Once module is closed, it is no longer busy
  this->BusyOff();

  // close the GUI
  this->Hide();
}

} // End namespace otb

#endif
