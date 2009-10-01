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
#include "base/ossimFilename.h"
#include <FL/Fl_Color_Chooser.H>

namespace otb
{
/** Constructor */
ViewerModule::ViewerModule() :  m_InputImageLayer(), m_RenderingModel(),m_PixelDescriptionModel(),
				m_View(), m_PixelDescriptionView(), m_CurveWidget(),
				m_Controller(), m_RenderingFunction()
{
  // 
  m_Label = "Viewer Module";
  
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
   m_CurveWidgetGroup    = CurvesWidgetType::New();
   m_CurveWidgetGroup->SetIdentifier("Curve");
   m_CurveWidgetGroup->SetController(m_Controller);
   
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

  // Add the histogram handler
  HistogramActionHandlerType::Pointer histogramHandler = HistogramActionHandlerType::New();
  histogramHandler->SetModel(m_RenderingModel);
  histogramHandler->SetView(m_View);
  histogramHandler->SetCurve(m_CurveWidgetGroup);
  histogramHandler->SetLeftAsymptote(m_BlueVaCurveL);
  histogramHandler->SetRightAsymptote(m_BlueVaCurveR);
  histogramHandler->SetRenderingFunction(m_StandardRenderingFunction);
  m_Controller->AddActionHandler(histogramHandler);

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

  // Color Definition
  m_Red.Fill(0);
  m_Green.Fill(0);
  m_Blue.Fill(0);
  m_Grey.Fill(0.5);
  m_Red[0]  = 1.;   m_Red[3]   = 0.5;
  m_Green[1]= 1.;   m_Green[3] = 0.5;
  m_Blue[2] = 1.;   m_Blue[3]  = 0.5;

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
  m_InputImage = this->GetInputData<ImageType>("InputImage");
  
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
    this->AddName();
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
  std::cout <<"Channel Size " << channels.size() << std::endl;
  m_StandardRenderingFunction->SetChannelList(channels);
  m_StandardRenderingFunction->SetAutoMinMax(true);
  m_InputImageLayer->SetRenderingFunction(m_StandardRenderingFunction);
  
  // Set the window and layer label
  m_DisplayWindow->SetLabel(m_Label.c_str());
  m_InputImageLayer->SetName(m_Label);

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
  // Set a random color to the vector data
  //itk::OStringStream oss;
  //oss<<"Vector Data : "<<m_VectorDataList->Size();
  //dVDList->add(oss.str().c_str());
  dVDList->add(name.c_str());
  dVDList->redraw();
  dVDList->redraw();
}

/**
 *
 */
void ViewerModule::UpdateListSelectionColor()
{
  //selectedIndex
  unsigned int selectedIndex = dVDList->value();

  if(selectedIndex)
    {
      VectorDataGlComponentType*  selecedVectorDataGlComponent = dynamic_cast<VectorDataGlComponentType*>(m_View->GetZoomWidget()->GetNthGlComponent(selectedIndex-1));
      ColorType                   curColor                     = selecedVectorDataGlComponent->GetColor();
      
      // color To fl_color
      fl_color(static_cast<unsigned char>(255*curColor[0]),
	       static_cast<unsigned char>(255*curColor[1]),
	       static_cast<unsigned char>(255*curColor[2]));
      
      //Update the ROIColoritem
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
  
      std::cout <<"SelectedItem " << selectedIndex << std::endl;
  
      // Remove the VectorData from the list
      m_VectorDataList->Erase(selectedIndex-1);
  
      //Update the Fl_Browser
      dVDList->remove(selectedIndex);
  
      // Remove the VectorDataGl Compenent
      m_View->GetScrollWidget()->RemoveGlComponent(selectedIndex);
      m_View->GetFullWidget()->RemoveGlComponent(selectedIndex);
      m_View->GetZoomWidget()->RemoveGlComponent(selectedIndex-1);   //-1 because in the Zoom Widget there is no red square wich is a GlComponent 
  
      //Redraw all the widgets
      this->RedrawWidget();
    }
}

/**
 *
 **/
void ViewerModule::ClearAll()
{
  for(unsigned int i = 0 ; i< m_VectorDataList->Size(); i++ )
    {
      m_View->GetScrollWidget()->RemoveGlComponent(1);
      m_View->GetFullWidget()->RemoveGlComponent(1);
      m_View->GetZoomWidget()->RemoveGlComponent(0);
    }
  
  //Erase the VectorDataList
  m_VectorDataList->Clear();
  
  //Erase the Fl_Browser
  dVDList->clear();

  //
  //Update the ROIColoritem
  dROIColor->color(fl_color());
  dROIColor->redraw();
  
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
  
  if(selectedIndex)
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
  // 
  this->BuildDEM();
  
  // 
  if(bDEM->value())
    wDEM->show();
}

/**
 *
 */
void ViewerModule::UpdateDEMSettings()
{
  std::cout << "Path du DEM "<<gDEMPath->value()<< std::endl;
  
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
  else
    {
      std::cout <<"DEM path empty" << std::endl;
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
   std::cout <<"nbComponents " <<  nbComponent << std::endl;
   
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

   ChannelListType channels = m_RenderingFunction->GetChannelList();
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
  
  ChannelListType channels = m_RenderingFunction->GetChannelList();
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
   
   ChannelListType channels = m_RenderingFunction->GetChannelList();
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
  //TODO : FIXME : test for a new instance of histogram Curve
  m_CurveWidgetGroup->SetXAxisLabel("Pixels");
  m_CurveWidgetGroup->SetYAxisLabel("Frequency");
  
  // Clear the widget 
  m_CurveWidgetGroup->ClearAllCurves();

  //Update the StandardRenderingFunction
  ChannelListType channels = m_InputImageLayer->GetRenderingFunction()->GetChannelList();
  m_StandardRenderingFunction->SetChannelList(channels);
  m_StandardRenderingFunction->SetAutoMinMax(true);
  m_InputImageLayer->SetRenderingFunction(m_StandardRenderingFunction);
    
  for(unsigned int i = 0 ; i<m_StandardRenderingFunction->GetChannelList().size() ; i++)
    std::cout <<" m_StandardRenderingFunction Channel N "<< i << " Value  " << m_StandardRenderingFunction->GetChannelList()[i]<< std::endl;
  
  
  if (/*m_InputImageLayer->GetRenderingFunction()*/m_StandardRenderingFunction->GetPixelRepresentationSize() >=3)
    {
      HistogramCurveType::Pointer bhistogram = HistogramCurveType::New();
      bhistogram->SetHistogramColor(m_Blue);
      bhistogram->SetLabelColor(m_Blue);
      bhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(2));
      m_CurveWidgetGroup->AddCurve(bhistogram);
    }
  
  if (/*m_InputImageLayer->GetRenderingFunction()*/m_StandardRenderingFunction->GetPixelRepresentationSize() >=2)
    {
      HistogramCurveType::Pointer ghistogram = HistogramCurveType::New();
      ghistogram->SetHistogramColor(m_Green);
      ghistogram->SetLabelColor(m_Green);
      ghistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(1));
      //m_CurveWidgetGroup->AddCurve(ghistogram);
    }

  HistogramCurveType::Pointer rhistogram = HistogramCurveType::New();
  if (/*m_InputImageLayer->GetRenderingFunction()*/m_StandardRenderingFunction->GetPixelRepresentationSize() == 1)
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
  //m_CurveWidgetGroup->AddCurve(rhistogram);
  
  // Add the verticalAsymptotes Curve
  // Right Asymptote
  m_BlueVaCurveR->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(4) );
  m_BlueVaCurveR->SetVerticalAsymptoteColor(m_Red);
  // Left Asymptote
  m_BlueVaCurveL->SetAbcisse(m_StandardRenderingFunction->GetParameters().GetElement(5) );
  m_BlueVaCurveL->SetVerticalAsymptoteColor(m_Red);
  
  m_CurveWidgetGroup->AddCurve(m_BlueVaCurveR);
  m_CurveWidgetGroup->AddCurve(m_BlueVaCurveL);
  
  //Add to the gHistogram group
  gHistogram->add(m_CurveWidgetGroup);
  gHistogram->resizable(m_CurveWidgetGroup);
  m_CurveWidgetGroup->resize(gHistogram->x(),gHistogram->y(),
			     gHistogram->w(),gHistogram->h());
}


void ViewerModule::TabSetupPosition()
{
  std::cout << "position " << gVectorData->value()->label()<< std::endl;
  
  if(strcmp(gVectorData->value()->label(),"Histogram" ))
   {
     m_CurveWidgetGroup->hide();
   }
  else
   {
     m_CurveWidgetGroup->show();
   }
}


} // End namespace otb

#endif
