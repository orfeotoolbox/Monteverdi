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

#include "otbViewerModule.h"
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Color_Chooser.H>

namespace otb
{
/** Constructor */
ViewerModule::ViewerModule() :  m_InputImageLayer(), m_RenderingModel(),m_PixelDescriptionModel(),
				m_View(), m_PixelDescriptionView(), m_CurveWidget(),
				m_Controller(), m_RenderingFunction()
{
  // Color Definition
  m_Red.Fill(0);
  m_Green.Fill(0);
  m_Blue.Fill(0);
  m_Grey.Fill(0.5);
  m_Red[0]  = 1.;   m_Red[3]   = 0.5;
  m_Green[1]= 1.;   m_Green[3] = 0.5;
  m_Blue[2] = 1.;   m_Blue[3]  = 0.5;
  
  // Build a new rendering model
  m_RenderingModel = RenderingModelType::New();
  m_PixelDescriptionModel = PixelDescriptionModelType::New();
  m_PixelDescriptionModel->SetLayers(m_RenderingModel->GetLayers());
  
  // Build a view
  m_View                 = ViewType::New();
  m_PixelDescriptionView = PixelDescriptionViewType::New();
  
  // Build a controller
  m_Controller     = WidgetControllerType::New();
  
  // StandardRendering Function : Needed for the histogram handler
  m_StandardRenderingFunction = StandardRenderingFunctionType::New();
  
  // Vertical asymptotes for each channel
  m_BlueVaCurveR = VerticalAsymptoteCurveType::New();
  m_BlueVaCurveL = VerticalAsymptoteCurveType::New();
  
  m_GreenVaCurveR = VerticalAsymptoteCurveType::New();
  m_GreenVaCurveL = VerticalAsymptoteCurveType::New();
  
  m_RedVaCurveR = VerticalAsymptoteCurveType::New();
  m_RedVaCurveL = VerticalAsymptoteCurveType::New();
  
  // Build the curve widget
  m_CurveWidget    = CurvesWidgetType::New();
  m_CurveWidget->SetXAxisLabel("Pixels");
  m_CurveWidget->SetYAxisLabel("Frequency");
   
  // Curvet Widget Instanciation
  m_BlueCurveWidgetGroup  = CurvesWidgetType::New();
  m_GreenCurveWidgetGroup = CurvesWidgetType::New();
  m_RedCurveWidgetGroup   = CurvesWidgetType::New();
   
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
  
  // Display Window
  m_DisplayWindow   = WidgetManagerType::New();
  m_DisplayWindow->RegisterFullWidget(m_View->GetFullWidget());
  m_DisplayWindow->RegisterScrollWidget(m_View->GetScrollWidget());
  m_DisplayWindow->RegisterZoomWidget(m_View->GetZoomWidget());
  m_DisplayWindow->RegisterPixelDescriptionWidget(m_PixelDescriptionView->GetPixelDescriptionWidget());
  m_DisplayWindow->RegisterHistogramWidget(m_CurveWidget);

  // VectorData List Instance
  m_VectorDataList = VectorDataListType::New();
   
  // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage","Image to visualize :");
  this->AddInputDescriptor<VectorDataType>("VectorData","VectorData to visualize.",true,true);

  // Build GUI
  this->Build();   
}

/** Destructor */
ViewerModule::~ViewerModule()
{}

/** PrintSelf method */
void ViewerModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void ViewerModule::Run()
{
  //Get Input Image
  m_InputImage = this->GetInputData<ImageType>("InputImage");
  
  // Set the File Name as a label to the viewer window
  m_Label = this->GetInputDataDescription<ImageType>("InputImage");
  m_DisplayWindow->SetLabel(m_Label.c_str());
  
  // First check if there is actually an input image
  if(m_InputImage.IsNull())
    {
    itkExceptionMacro(<<"The image pointer is null, there is nothing to display. You probably forget to set the image.");
    }

  // Update image info for further use
  m_InputImage->UpdateOutputInformation();

  // If there is a VectorData
  for(unsigned int i = 0; i < this->GetNumberOfInputDataByKey("VectorData");i++)
    {
      VectorDataType::Pointer vdata = this->GetInputData<VectorDataType>("VectorData",i);
      m_VectorDataList->PushBack(vdata);
      m_DisplayedVectorData.push_back(true);
      this->UpdateVectorData(i);
      std::string desc = this->GetInputDataDescription<VectorDataType>("VectorData",i);
      this->AddName( desc );
    }

  // Generate the layer
  ImageLayerGeneratorPointerType generator = ImageLayerGeneratorType::New();
  generator->SetImage(m_InputImage);
  //FltkFilterWatcher qlwatcher(generator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  generator->GenerateLayer();
  m_InputImageLayer = generator->GetLayer();
  m_RenderingFunction = generator->GetRenderingFunction();

  // Work with standardrenderingFunction
  ChannelListType channels = m_InputImageLayer->GetRenderingFunction()->GetChannelList();
  m_StandardRenderingFunction->SetChannelList(channels);
  m_InputImageLayer->SetRenderingFunction(m_StandardRenderingFunction);
  
  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(generator->GetLayer());

  // Show everything
  m_DisplayWindow->Show();

  // Update the rendering model
  m_RenderingModel->Update();

  // Handle Histogram
  this->UpdateHistogramCurve();

  // Update The Histogram Tab
  this->UpdateTabHistogram();
  
  // Show the interface setup
  bSetupWindow->show();
  
  // Update the color composition window
  this->UpdateViewerSetupWindow();
}

/**
 *
 */
void ViewerModule::UpdateHistogramCurve()
{
  // Clear the widget 
  m_CurveWidget->ClearAllCurves();
  
  // Get the current rendering function 
  RenderingFunctionType::Pointer renderingFunction = m_InputImageLayer->GetRenderingFunction();
  
  // 
   if (m_InputImageLayer->GetRenderingFunction()->GetPixelRepresentationSize() >=3)
   {
     HistogramCurveType::Pointer bhistogram = HistogramCurveType::New();
     bhistogram->SetHistogramColor(m_Blue);
     bhistogram->SetLabelColor(m_Blue);
     bhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(2));
     m_CurveWidget->AddCurve(bhistogram);
   }

   if (m_InputImageLayer->GetRenderingFunction()->GetPixelRepresentationSize() >=2)
   {
     HistogramCurveType::Pointer ghistogram = HistogramCurveType::New();
     ghistogram->SetHistogramColor(m_Green);
     ghistogram->SetLabelColor(m_Green);
     ghistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(1));
     m_CurveWidget->AddCurve(ghistogram);
   }

   HistogramCurveType::Pointer rhistogram = HistogramCurveType::New();
   if (m_InputImageLayer->GetRenderingFunction()->GetPixelRepresentationSize() == 1)
   {
     rhistogram->SetHistogramColor(m_Grey);
     rhistogram->SetLabelColor(m_Grey);
   }
   else
   {
     rhistogram->SetHistogramColor(m_Red);
     rhistogram->SetLabelColor(m_Red);
   }
   rhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(0));
   m_CurveWidget->AddCurve(rhistogram);
}


/**
 * Project the vector Data in the image projection.
 * Extract the Remotesensing region corresponding to the image.
 */
void ViewerModule::UpdateVectorData(unsigned int index)
{
  VectorDataProjectionFilterType::Pointer vproj;
  VectorDataExtractROIType::Pointer vdextract;
  
  // Extract The part of the VectorData that actually overlaps with
  // the image extent
  vdextract = VectorDataExtractROIType::New();
  vdextract->SetInput(m_VectorDataList->GetNthElement(index));

  // Find the geographic region of interest

  // Ge the index of the corner of the image
  ImageType::IndexType ul, ur, ll, lr;
  ImageType::PointType pul,pur,pll,plr;
  ul = m_InputImage->GetLargestPossibleRegion().GetIndex();
  ur = ul;
  ll = ul;
  lr = ul;
  ur[0]+=m_InputImage->GetLargestPossibleRegion().GetSize()[0];
  lr[0]+=m_InputImage->GetLargestPossibleRegion().GetSize()[0];
  lr[1]+=m_InputImage->GetLargestPossibleRegion().GetSize()[1];
  ll[1]+=m_InputImage->GetLargestPossibleRegion().GetSize()[1];

  // Transform to physical point
  m_InputImage->TransformIndexToPhysicalPoint(ul,pul);
  m_InputImage->TransformIndexToPhysicalPoint(ur,pur);
  m_InputImage->TransformIndexToPhysicalPoint(ll,pll);
  m_InputImage->TransformIndexToPhysicalPoint(lr,plr);

  // Build the cartographic region
  RemoteSensingRegionType rsRegion;
  RemoteSensingRegionType::IndexType rsOrigin;
  RemoteSensingRegionType::SizeType  rsSize;
  rsOrigin[0]= min(pul[0],plr[0]);
  rsOrigin[1]= min(pul[1],plr[1]);
  rsSize[0]=vcl_abs(pul[0]-plr[0]);
  rsSize[1]=vcl_abs(pul[1]-plr[1]);

  rsRegion.SetOrigin(rsOrigin);
  rsRegion.SetSize(rsSize);
  rsRegion.SetRegionProjection(m_InputImage->GetProjectionRef());
  rsRegion.SetKeywordList(m_InputImage->GetImageKeywordlist());

  // Set the cartographic region to the extract roi filter
  vdextract->SetRegion(rsRegion);
  if(!m_DEMDirectory.empty())
    vdextract->SetDEMDirectory(m_DEMDirectory);

  // Reproject VectorData in image projection
  vproj = VectorDataProjectionFilterType::New();
  vproj->SetInput(vdextract->GetOutput());
  vproj->SetInputProjectionRef(m_VectorDataList->GetNthElement(index)->GetProjectionRef());
  vproj->SetOutputKeywordList(m_InputImage->GetImageKeywordlist());
  vproj->SetOutputProjectionRef(m_InputImage->GetProjectionRef());
  vproj->SetOutputOrigin(m_InputImage->GetOrigin());
  vproj->SetOutputSpacing(m_InputImage->GetSpacing());
  if(!m_DEMDirectory.empty())
    vproj->SetDEMDirectory(m_DEMDirectory);
  vproj->Update();

  // Create a VectorData gl component
  VectorDataGlComponentType::Pointer vgl = VectorDataGlComponentType::New();
  vgl->SetVectorData(vproj->GetOutput());
  vgl->SetColor(this->SetRandomColor());
    
  // Add it to the image view
  m_View->GetScrollWidget()->AddGlComponent(vgl);
  m_View->GetFullWidget()->AddGlComponent(vgl);
  m_View->GetZoomWidget()->AddGlComponent(vgl);
}

/**
 *
 */
void ViewerModule::AddName(std::string name)
{
  dVDList->add(name.c_str());
  dVDList->redraw();
}

/**
 *
 */
void ViewerModule::UpdateListSelectionColor()
{
  //selectedIndex
  unsigned int selectedIndex = dVDList->value();

  if(selectedIndex && m_DisplayedVectorData[selectedIndex-1])
    {
      VectorDataGlComponentType*  selecedVectorDataGlComponent = dynamic_cast<VectorDataGlComponentType*>(m_View->GetZoomWidget()->GetNthGlComponent(selectedIndex-1));
      ColorType                   curColor                     = selecedVectorDataGlComponent->GetColor();
      
      // color To fl_color
      fl_color(static_cast<unsigned char>(255*curColor[0]),
	       static_cast<unsigned char>(255*curColor[1]),
	       static_cast<unsigned char>(255*curColor[2]));
      
      //Update the ROIColorItem
      dROIColor->color(fl_color());
      dROIColor->redraw();
      //Update the text color
      dVDList->selection_color(fl_color());
      dVDList->redraw();
    }
}

/**
 *
 */
void ViewerModule::DeleteVectorData()
{
  unsigned int selectedIndex = dVDList->value();
  
  if(selectedIndex)
    {
      // Remove the VectorDataGl Compenent
      m_View->GetScrollWidget()->GetNthGlComponent(selectedIndex)->SetVisible(false);
      m_View->GetFullWidget()->GetNthGlComponent(selectedIndex)->SetVisible(false);
      m_View->GetZoomWidget()->GetNthGlComponent(selectedIndex-1)->SetVisible(false);  //-1 because in the Zoom Widget there is no red square wich is a GlComponent 

      // Update the status of the selectedItem vector data
      m_DisplayedVectorData[selectedIndex-1] = false;
  
      //Redraw all the widgets
      this->RedrawWidget();
    }
}

/**
 * Hide all the vector datas
 **/
void ViewerModule::ClearAll()
{
  for(unsigned int i = 0 ; i< m_VectorDataList->Size(); i++ )
    {
      // Hide the Gl Componenent int the view
      m_View->GetScrollWidget()->GetNthGlComponent(i+1)->SetVisible(false);
      m_View->GetFullWidget()->GetNthGlComponent(i+1)->SetVisible(false);
      m_View->GetZoomWidget()->GetNthGlComponent(i)->SetVisible(false);
      
      // Update the dispaly status of each vector
      m_DisplayedVectorData[i] = false;
    }
  //Redraw all the widgets
  this->RedrawWidget();
}

/**
 *  Dispaly if Hidden VectorData
 */
void 
ViewerModule::DisplaySelectedVectorData()
{
  // Get the selected vectordata
  unsigned int selectedIndex = dVDList->value();
  
  // Dispaly the selected VD if not already dispalyed
  if(!m_DisplayedVectorData[selectedIndex-1])
    {
      // Hide all displayed vector datas
      m_View->GetScrollWidget()->GetNthGlComponent(selectedIndex)->SetVisible(true);
      m_View->GetFullWidget()->GetNthGlComponent(selectedIndex)->SetVisible(true);
      m_View->GetZoomWidget()->GetNthGlComponent(selectedIndex-1)->SetVisible(true);
      // Set visible status to selected VectorDataGl Compenents
      m_DisplayedVectorData[selectedIndex-1] = true;
    }
  
  //Redraw all the widgets
  this->RedrawWidget();
}


/**
 *  Dispaly if Hidden VectorData
 */
void 
ViewerModule::DisplayVectorData()
{
  // loop over all the vector datas
  for(unsigned int i = 0 ; i< m_VectorDataList->Size(); i++ )
    {
      // Dispaly the selected VD if not already displayed
      if(!m_DisplayedVectorData[i])
	{
	  m_View->GetScrollWidget()->GetNthGlComponent(i+1)->SetVisible(true);
	  m_View->GetFullWidget()->GetNthGlComponent(i+1)->SetVisible(true);
	  m_View->GetZoomWidget()->GetNthGlComponent(i)->SetVisible(true);
	  m_DisplayedVectorData[i] = true;
	}
    }
  //Redraw all the widgets
  this->RedrawWidget();
}

/**
 *
 */
ViewerModule::ColorType  ViewerModule::SetRandomColor()
{
  ColorType   color;
  // Update Vector Data color 
  color[0]=rand()/(RAND_MAX+1.0);
  color[1]=rand()/(RAND_MAX+1.0);
  color[2]=rand()/(RAND_MAX+1.0);
  color[3]=1.0;

  return color;
}

/**
 *
 */
void ViewerModule::ChangeROIColor()
{
  unsigned int selectedIndex = dVDList->value();
  
  if(selectedIndex && m_DisplayedVectorData[selectedIndex-1])
    {
      VectorDataGlComponentType*  selecedVectorDataGlComponent = dynamic_cast<VectorDataGlComponentType*>(m_View->GetZoomWidget()->GetNthGlComponent(selectedIndex-1));
      ColorType curColor  = selecedVectorDataGlComponent->GetColor();
        
      double r = (double)curColor[0];
      double g = (double)curColor[1];
      double b = (double)curColor[2];

      int ok = fl_color_chooser("Changed class color:",r,g,b);

      if (ok)
	{
	  curColor[0]=(float)r;
	  curColor[1]=(float)g;
	  curColor[2]=(float)b;
      
	  fl_color(static_cast<unsigned char>(255*curColor[0]),
		   static_cast<unsigned char>(255*curColor[1]),
		   static_cast<unsigned char>(255*curColor[2]));
	  
	  // Change the color of the RoiButton 
	  dROIColor->color(fl_color());
	  // Change the color of the text
	  dVDList->selection_color(fl_color());
      	  dVDList->redraw();
	  // Change the color of the VectorData
	  selecedVectorDataGlComponent->SetColor(curColor);
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

void ViewerModule::UseDEM()
{
  // build the GUI
  this->BuildDEM();
  
  // show the DEM GUI
  if(bDEM->value())
    wDEM->show();
}

/**
 *
 */
void ViewerModule::UpdateDEMSettings()
{
  if(gDEMPath->value())
    {
      // Copy the DEM pathname
      m_DEMDirectory = gDEMPath->value();
  
      // Delete the vector data
      for(unsigned int i = 0 ; i< m_VectorDataList->Size(); i++ )
	{
	  m_View->GetScrollWidget()->RemoveGlComponent(1);
	  m_View->GetFullWidget()->RemoveGlComponent(1);
	  m_View->GetZoomWidget()->RemoveGlComponent(0);
	}
  
      // Reproject using the DEM this time
      for(unsigned int i = 0; i < m_VectorDataList->Size();i++)
	{
	  this->UpdateVectorData(i);
	}

      // Refresh widgets
      this->RedrawWidget();
    }
}

/**
 * Edit the viewer setup ---------------------
 */

 /**
  *
  */
 void ViewerModule::UpdateViewerSetupWindow()
 {
   //Get the number of components per pixel
   unsigned int nbComponent = m_InputImage->GetNumberOfComponentsPerPixel();
   
   itk::OStringStream oss;
   oss.str("");

   //Clear all the choices
   guiGrayscaleChannelChoice->clear();
   guiRedChannelChoice->clear();
   guiGreenChannelChoice->clear();
   guiBlueChannelChoice->clear();
   guiRealChannelChoice->clear();
   guiImaginaryChannelChoice->clear();

   for (unsigned int i = 0;i<nbComponent;++i)
   {
     oss.str("");
     oss<<i+1;
     guiGrayscaleChannelChoice->add(oss.str().c_str());
     guiRedChannelChoice->add(oss.str().c_str());
     guiGreenChannelChoice->add(oss.str().c_str());
     guiBlueChannelChoice->add(oss.str().c_str());
     guiGrayscaleChannelChoice->add(oss.str().c_str());
     guiRealChannelChoice->add(oss.str().c_str());
     guiImaginaryChannelChoice->add(oss.str().c_str());
   }
   guiViewerSetupWindow->redraw();

   switch(nbComponent){
   case 1 :
     this->GrayScaleSet();
     break;
   case 4 :
     this->RGBSet();
     break;
   case 3 :
     this->RGBSet();
     break;
   case 2:
     this->ComplexSet();
     break;
   default :
     break;
   }
 }


 /**
  * RGBSet();
  */
 void ViewerModule::RGBSet()
 {
   unsigned int nbComponent = m_InputImage->GetNumberOfComponentsPerPixel();

   guiViewerSetupColorMode->set();
   guiViewerSetupComplexMode->clear();
   guiViewerSetupGrayscaleMode->clear();
   guiGrayscaleChannelChoice->deactivate();
   guiRealChannelChoice->deactivate();
   guiImaginaryChannelChoice->deactivate();
   bAmplitude->deactivate();
   bPhase->deactivate();

   guiRedChannelChoice->activate();
   guiGreenChannelChoice->activate();
   guiBlueChannelChoice->activate();

   ChannelListType channels = m_StandardRenderingFunction->GetChannelList();
   unsigned int i=0;
   while (channels.size() < 3)
   {
     channels.push_back(i);
     ++i;
   }

   guiRedChannelChoice->value(std::min(channels[0],nbComponent-1));
   guiGreenChannelChoice->value(std::min(channels[1],nbComponent-1));
   guiBlueChannelChoice->value(std::min(channels[2],nbComponent-1));
 }



/**
 * Change the render function to gray scale one
 */
void ViewerModule::GrayScaleSet()
{
  unsigned int nbComponent = m_InputImage->GetNumberOfComponentsPerPixel();

  guiViewerSetupGrayscaleMode->set();
  guiViewerSetupComplexMode->clear();
  guiViewerSetupColorMode->clear();

  guiRealChannelChoice->deactivate();
  guiImaginaryChannelChoice->deactivate();
  bAmplitude->deactivate();
  bPhase->deactivate();
  guiRedChannelChoice->deactivate();
  guiGreenChannelChoice->deactivate();
  guiBlueChannelChoice->deactivate();

  guiGrayscaleChannelChoice->activate();
  
  ChannelListType channels = m_StandardRenderingFunction->GetChannelList();
  if (channels.size() < 1)
    {
      channels.push_back(0);
    }
  guiGrayscaleChannelChoice->value(std::min(channels[0],nbComponent-1));
}

/**
 *
 */
void ViewerModule::ComplexSet()
 {
   unsigned int nbComponent = m_InputImage->GetNumberOfComponentsPerPixel();

   guiViewerSetupComplexMode->set();
   guiViewerSetupColorMode->clear();
   guiViewerSetupGrayscaleMode->clear();
   guiGrayscaleChannelChoice->deactivate();
   guiRedChannelChoice->deactivate();
   guiGreenChannelChoice->deactivate();
   guiBlueChannelChoice->deactivate();
   guiRealChannelChoice->activate();
   guiImaginaryChannelChoice->activate();
   bAmplitude->activate();
   bPhase->activate();
   
   ChannelListType channels = m_StandardRenderingFunction->GetChannelList();
   unsigned int i=0;
   while (channels.size() < 2)
   {
     channels.push_back(i);
     ++i;
   }
   guiRealChannelChoice->value(std::min(channels[0],nbComponent-1));
   guiImaginaryChannelChoice->value(std::min(channels[1],nbComponent-1));
 }

/**
 *
 */
void ViewerModule::ViewerSetupOk()
 {
   if (guiViewerSetupColorMode->value())
     {
       this->UpdateRGBChannelOrder(atoi(guiRedChannelChoice->value())-1,
				   atoi(guiGreenChannelChoice->value())-1,
				   atoi(guiBlueChannelChoice->value())-1);
     }
   else if (guiViewerSetupGrayscaleMode->value())
     {
      this->UpdateGrayScaleChannelOrder(atoi(guiGrayscaleChannelChoice->value())-1);
     }
   else if (guiViewerSetupComplexMode->value())
     {
       if (bAmplitude->value())
	 {
	   this->UpdateAmplitudeChannelOrder(atoi(guiRealChannelChoice->value())-1,
								       atoi(guiImaginaryChannelChoice->value())-1);
	 }
       else
	 {
	   this->UpdatePhaseChannelOrder(atoi(guiRealChannelChoice->value())-1,
					 atoi(guiImaginaryChannelChoice->value())-1);
	 }
     }
     
  // Refresh widgets
   this->RedrawWidget();

   // Update Histogram 
   this->UpdateHistogramCurve();   
   
   //
   this->UpdateTabHistogram();
 }

/**
 * Gray Scale channel order
 */
void ViewerModule::UpdateGrayScaleChannelOrder(int choice)
{
  // Build the appropriate rendring function
  StandardRenderingFunctionPointerType  gsRenderingFunction = StandardRenderingFunctionType::New();
  ChannelListType                       channels;
  
  channels.resize(3);
  channels[0]=choice;
  channels[1]=choice;
  channels[2]=choice;
  gsRenderingFunction->SetChannelList(channels);
  
  // Apply the new rendering function to the Image layer
  m_InputImageLayer->SetRenderingFunction(gsRenderingFunction);
  m_RenderingModel->Update();  
}

/**
 * RGB Channel Order
 */

void ViewerModule::UpdateRGBChannelOrder(int red, int green , int blue)
{
  // Build the appropriate rendring function
  StandardRenderingFunctionPointerType  gsRenderingFunction = StandardRenderingFunctionType::New();
  ChannelListType                       channels;
  
  channels.resize(3);
  channels[0]=red;
  channels[1]=green;
  channels[2]=blue;
  gsRenderingFunction->SetChannelList(channels);
  
  // Apply the new rendering function to the Image layer
  m_InputImageLayer->SetRenderingFunction(gsRenderingFunction);
  m_RenderingModel->Update();
}

/**
 * Amplitude 
 */

void ViewerModule::UpdateAmplitudeChannelOrder(int realChoice, int imChoice)
{
  AmplitudeRenderingFunction::PixelRepresentationFunctionType::ChannelListType channels;
  channels.push_back(realChoice);
  channels.push_back(imChoice);

  AmplitudeRenderingFunction::Pointer modulusFunction =  AmplitudeRenderingFunction::New();
  modulusFunction->SetChannelList(channels);
  
  // Apply the new rendering function to the Image layer
  m_InputImageLayer->SetRenderingFunction(modulusFunction);
  m_RenderingModel->Update();
}

/**
 *
 */
void ViewerModule::UpdatePhaseChannelOrder(int realChoice, int imChoice)
{
  PhaseRenderingFunction::PixelRepresentationFunctionType::ChannelListType channels;
  channels.push_back(realChoice);
  channels.push_back(imChoice);

  PhaseRenderingFunction::Pointer phaseFunction =  PhaseRenderingFunction::New();
  phaseFunction->SetChannelList(channels);
  
  // Apply the new rendering function to the Image layer
  m_InputImageLayer->SetRenderingFunction(phaseFunction);
  m_RenderingModel->Update();
}

/**
 *
 */
void ViewerModule::UpdateTabHistogram()
{
  //Get the number of subdivisons in the GUI 
 double width  = gHistogram->w()/2;
 double height = gHistogram->h()/2;
 double blank = 2.;
 
  // Clear the widget 
  m_BlueCurveWidgetGroup->ClearAllCurves();
  m_GreenCurveWidgetGroup->ClearAllCurves();
  m_RedCurveWidgetGroup->ClearAllCurves();
    
  /// Update the StandardRenderingFunction
  ChannelListType channels = m_InputImageLayer->GetRenderingFunction()->GetChannelList();
  m_StandardRenderingFunction->SetChannelList(channels);
  m_InputImageLayer->SetRenderingFunction(m_StandardRenderingFunction);
   
  //Set the rendering to the histogram handlers
  m_BlueHistogramHandler->SetRenderingFunction(m_StandardRenderingFunction);
  m_GreenHistogramHandler->SetRenderingFunction(m_StandardRenderingFunction);
  m_RedHistogramHandler->SetRenderingFunction(m_StandardRenderingFunction);
  
  /// BLUE band : draw the BLUE band histogram and the asymptote
  if (m_InputImageLayer->GetRenderingFunction()->GetPixelRepresentationSize() >=3)
    {
      HistogramCurveType::Pointer bhistogram = HistogramCurveType::New();
      bhistogram->SetHistogramColor(m_Blue);
      bhistogram->SetLabelColor(m_Blue);
      bhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(channels[2]));
      m_BlueCurveWidgetGroup->AddCurve(bhistogram);
      
      //Add to the gHistogram group
      gHistogram->add(m_BlueCurveWidgetGroup);
      gHistogram->resizable(m_BlueCurveWidgetGroup);
      m_BlueCurveWidgetGroup->resize(gHistogram->x()+blank,gHistogram->y()+height+blank,
				     width-blank ,height-blank);
      
      // Right Asymptote
      m_BlueVaCurveR->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(2*channels[2]));
      m_BlueVaCurveR->SetVerticalAsymptoteColor(m_Blue);
      
      // Left Asymptote
      m_BlueVaCurveL->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(2*channels[2]+1));
      m_BlueVaCurveL->SetVerticalAsymptoteColor(m_Blue);
      
      // Add the asymptote to the curve
      m_BlueCurveWidgetGroup->AddCurve(m_BlueVaCurveR);
      m_BlueCurveWidgetGroup->AddCurve(m_BlueVaCurveL);
      
      // Edit the channel we're changing
      m_BlueHistogramHandler->SetChannel(channels[2]);
    }
  
  /// GREEN band : draw the GREEN band histogram and the asymptote
  if (m_InputImageLayer->GetRenderingFunction()->GetPixelRepresentationSize() >=2)
    {
      HistogramCurveType::Pointer ghistogram = HistogramCurveType::New();
      ghistogram->SetHistogramColor(m_Green);
      ghistogram->SetLabelColor(m_Green);
      ghistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(channels[1]));
      m_GreenCurveWidgetGroup->AddCurve(ghistogram);
      
      //Add to the gHistogram group
      gHistogram->add(m_GreenCurveWidgetGroup);
      gHistogram->resizable(m_GreenCurveWidgetGroup);
      m_GreenCurveWidgetGroup->resize(gHistogram->x() + width + blank ,gHistogram->y()+blank,
				      width-blank, height-blank);
      
      // Right Asymptote
      m_GreenVaCurveR->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(2*channels[1]));
      m_GreenVaCurveR->SetVerticalAsymptoteColor(m_Green); 
      
      // Left Asymptote
      m_GreenVaCurveL->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(2*channels[1]+1));
      m_GreenVaCurveL->SetVerticalAsymptoteColor(m_Green);

      // Add the asymptote to the curve
      m_GreenCurveWidgetGroup->AddCurve(m_GreenVaCurveR);
      m_GreenCurveWidgetGroup->AddCurve(m_GreenVaCurveL);
      
      // Edit the channel we're changing
      m_GreenHistogramHandler->SetChannel(channels[1]);
    }
  
  /// RED band : draw the RED band histogram and the asymptote
  HistogramCurveType::Pointer rhistogram = HistogramCurveType::New();
  if (m_InputImageLayer->GetRenderingFunction()->GetPixelRepresentationSize() == 1)
    {
      rhistogram->SetHistogramColor(m_Grey);
      rhistogram->SetLabelColor(m_Grey);
      
      //Add to the gHistogram group
      gHistogram->add(m_RedCurveWidgetGroup);
      gHistogram->resizable(m_RedCurveWidgetGroup);
      m_RedCurveWidgetGroup->resize(gHistogram->x()+blank,gHistogram->y()+blank,
				    gHistogram->w()-blank,height-blank);
    }
  else
    {
      rhistogram->SetHistogramColor(m_Red);
      rhistogram->SetLabelColor(m_Red);
	
      //Add to the gHistogram group
      gHistogram->add(m_RedCurveWidgetGroup);
      gHistogram->resizable(m_RedCurveWidgetGroup);
      m_RedCurveWidgetGroup->resize(gHistogram->x()+blank,gHistogram->y()+blank,
				    width-blank, height-blank);
    }
    
  rhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(channels[0])); 
  m_RedCurveWidgetGroup->AddCurve(rhistogram);
  // Right Asymptote
  m_RedVaCurveR->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(2*channels[0]));
  m_RedVaCurveR->SetVerticalAsymptoteColor(rhistogram->GetLabelColor()); 
    
  // Left Asymptote
  m_RedVaCurveL->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(2*channels[0]+1));
  m_RedVaCurveL->SetVerticalAsymptoteColor(rhistogram->GetLabelColor());
    
  // Add the asymptote to the curve
  m_RedCurveWidgetGroup->AddCurve(m_RedVaCurveR);
  m_RedCurveWidgetGroup->AddCurve(m_RedVaCurveL);
    
  // Edit the channel we're changing
  m_RedHistogramHandler->SetChannel(channels[0]);
}


void ViewerModule::TabSetupPosition()
{
  if(strcmp(gVectorData->value()->label(),"Histogram" ))
    {
      m_BlueCurveWidgetGroup->hide();
      m_GreenCurveWidgetGroup->hide();
      m_RedCurveWidgetGroup->hide();
    }
  else
    {
      m_BlueCurveWidgetGroup->show();
      m_GreenCurveWidgetGroup->show();
      m_RedCurveWidgetGroup->show();
    }
}

/**
 * 
 */
void ViewerModule::UpdateUpperQuantile()
{
  double upperQuantile = bUpperQuantile->value() / 100.;

  // Cancel the automatic computation of the quantile
  m_StandardRenderingFunction->SetAutoMinMax(false);
  
  //  Get the extrema value for all the bands
  ParametersType    params;
  params = m_StandardRenderingFunction->GetParameters();
  
  for(unsigned int i = 1 ; i< params.Size() ; i = i+2)
    {
      double max = m_StandardRenderingFunction->GetHistogramList()->GetNthElement((unsigned int)(i/2))->Quantile(0,1-upperQuantile);
      params.SetElement(i,max);
    }
  
  // Update the layer
  m_StandardRenderingFunction->SetParameters(params);
  m_RenderingModel->Update();
   
  // Redraw the hitogram curves
  this->UpdateTabHistogram();
  
  // Tab Setup Position
  m_BlueCurveWidgetGroup->redraw();
  m_GreenCurveWidgetGroup->redraw();
  m_RedCurveWidgetGroup->redraw();
}


/**
 *  
 */
void ViewerModule::UpdateLowerQuantile()
{
  double lowerQuantile = bLowerQuantile->value() / 100.;
   
  // Cancel the automatic computation of the quantile
  m_StandardRenderingFunction->SetAutoMinMax(false);
  
  // Get the standardRenderingFunction extrema parameters 
  ParametersType    params;
  params = m_StandardRenderingFunction->GetParameters();
  
  // Update the parameters
  for(unsigned int i = 0 ; i< params.Size() ; i = i+2)
    {
      double min = m_StandardRenderingFunction->GetHistogramList()->GetNthElement((unsigned int)(i/2))->Quantile(0,lowerQuantile);
      params.SetElement(i,min);
    }
  
  // Update the layer
  m_StandardRenderingFunction->SetParameters(params);
  m_RenderingModel->Update();
  
  // Redraw the hitogram curves
  this->UpdateTabHistogram();

  // Tab Setup Position
  m_BlueCurveWidgetGroup->redraw();
  m_GreenCurveWidgetGroup->redraw();
  m_RedCurveWidgetGroup->redraw();
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
  // Hide the main window
  m_DisplayWindow->Hide();
  
  // Hide the Setup Propreties Window
  bSetupWindow->hide();
  
  // Hide the DEM Window
  wDEM->hide();
}

} // End namespace otb

#endif


