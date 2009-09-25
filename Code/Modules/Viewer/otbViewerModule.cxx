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
ViewerModule::ViewerModule() : m_Label("Default label"), m_InputImage(), m_VectorDataList(),
			       m_InputImageLayer(), m_RenderingModel(),m_PixelDescriptionModel(),
			       m_View(), m_PixelDescriptionView(), m_CurveWidget(),
			       m_Controller(), m_RenderingFunction(), m_DisplayWindow(),m_GLComponentList()
{
  // Build a new rendering model
  m_RenderingModel = RenderingModelType::New();
  m_PixelDescriptionModel = PixelDescriptionModelType::New();
  m_PixelDescriptionModel->SetLayers(m_RenderingModel->GetLayers());

  // Build a view
  m_View                 = ViewType::New();
  m_PixelDescriptionView = PixelDescriptionViewType::New();

  // Build a controller
  m_Controller     = WidgetControllerType::New();

  // Build the curve widget
  m_CurveWidget    = CurvesWidgetType::New();
  
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
  
  // Display Window
  m_DisplayWindow   = WidgetManagerType::New();
  m_DisplayWindow->RegisterFullWidget(m_View->GetFullWidget());
  m_DisplayWindow->RegisterScrollWidget(m_View->GetScrollWidget());
  m_DisplayWindow->RegisterZoomWidget(m_View->GetZoomWidget());
  m_DisplayWindow->RegisterPixelDescriptionWidget(m_PixelDescriptionView->GetPixelDescriptionWidget());
  m_DisplayWindow->RegisterHistogramWidget(m_CurveWidget);

  // VectorData List Instance
  m_VectorDataList = VectorDataListType::New();
  
  // GlComponent List
  m_GLComponentList = VectorDataGlComponentListType::New();
  
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

/** Assign input by key.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void ViewerModule::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  typedef ViewerModule::ImageType InputImageType;

  if(key == "InputImage")
    {
      m_InputImage = dynamic_cast<InputImageType *>(data.GetDataObject());
    }

  if(key == "VectorData")
    {
      VectorDataType * vdata = dynamic_cast<VectorDataType*>(data.GetDataObject());
      m_VectorDataList->PushBack(vdata);
      this->AddName();
    }
}

/** The custom run command */
void ViewerModule::Run()
{
    // First check if there is actually an input image
  if(m_InputImage.IsNull())
    {
    itkExceptionMacro(<<"The image pointer is null, there is nothing to display. You probably forget to set the image.");
    }

  // Update image info for further use
  m_InputImage->UpdateOutputInformation();

  // If there is a VectorData
  for(unsigned int i = 0; i < m_VectorDataList->Size();i++)
  {
    this->UpdateVectorData(i);
  }

  // Colors
  HistogramCurveType::ColorType red,green,blue;
  red.Fill(0);
  red[0]=1.;
  red[3]=0.5;

  green.Fill(0);
  green[1]=1.;
  green[3]=0.5;

  blue.Fill(0);
  blue[2]=1.;
  blue[3]=0.5;
  
  // Generate the layer
  ImageLayerGeneratorPointerType generator = ImageLayerGeneratorType::New();
  generator->SetImage(m_InputImage);
  //FltkFilterWatcher qlwatcher(generator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  generator->GenerateLayer();
  m_InputImageLayer = generator->GetLayer();
  m_RenderingFunction = generator->GetRenderingFunction();

  // Set the window and layer label
  m_DisplayWindow->SetLabel(m_Label.c_str());
  m_InputImageLayer->SetName(m_Label);

  // Add the generated layer to the rendering model
  m_RenderingModel->AddLayer(generator->GetLayer());

  // Show everything
  m_DisplayWindow->Show();

  // Update the rendering model
  m_RenderingModel->Update();

  // adding histograms rendering
   HistogramCurveType::Pointer rhistogram = HistogramCurveType::New();
  //   rhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(m_RenderingFunction->GetPixelRepresentationFunction().GetRedChannelIndex()));
  rhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(0));
  rhistogram->SetHistogramColor(red);
  rhistogram->SetLabelColor(red);
  
   HistogramCurveType::Pointer ghistogram = HistogramCurveType::New();
  //   ghistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(m_RenderingFunction->GetPixelRepresentationFunction().GetGreenChannelIndex()));
  ghistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(1));
  ghistogram->SetHistogramColor(green);
  ghistogram->SetLabelColor(green);
  
   HistogramCurveType::Pointer bhistogram = HistogramCurveType::New();
  //   bhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(m_RenderingFunction->GetPixelRepresentationFunction().GetBlueChannelIndex()));
  bhistogram->SetHistogram(m_InputImageLayer->GetHistogramList()->GetNthElement(2));
  bhistogram->SetHistogramColor(blue);
  bhistogram->SetLabelColor(blue);
  m_CurveWidget->AddCurve(rhistogram);
  m_CurveWidget->AddCurve(ghistogram);
  m_CurveWidget->AddCurve(bhistogram);
  m_CurveWidget->SetXAxisLabel("Pixels");
  m_CurveWidget->SetYAxisLabel("Frequency");
  
  // Show the interface setup
  bSetupWindow->show();
  // Update the color composition window
  this->UpdateViewerSetupWindow();
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
  vdextract->SetDEMDirectory(m_DEMDirectory);

  // Reproject VectorData in image projection
  vproj = VectorDataProjectionFilterType::New();
  vproj->SetInput(vdextract->GetOutput());
  vproj->SetInputProjectionRef(m_VectorDataList->GetNthElement(index)->GetProjectionRef());
  vproj->SetOutputKeywordList(m_InputImage->GetImageKeywordlist());
  vproj->SetOutputProjectionRef(m_InputImage->GetProjectionRef());
  vproj->SetOutputOrigin(m_InputImage->GetOrigin());
  vproj->SetOutputSpacing(m_InputImage->GetSpacing());
  vproj->SetDEMDirectory(m_DEMDirectory);
  vproj->Update();

  // Create a VectorData gl component
  VectorDataGlComponentType::Pointer vgl = VectorDataGlComponentType::New();
  vgl->SetVectorData(vproj->GetOutput());
  vgl->SetColor(this->SetRandomColor());
  
  // Add the GlComponent to the list 
  m_GLComponentList->PushBack(vgl);
  
  // Add it to the image view
  m_View->GetScrollWidget()->AddGlComponent(vgl);
  m_View->GetFullWidget()->AddGlComponent(vgl);
  m_View->GetZoomWidget()->AddGlComponent(vgl);
}

/**
 *
 */
void ViewerModule::AddName()
{
  // Set a random color to the vector data
  itk::OStringStream oss;
  oss<<"Vector Data : "<<m_VectorDataList->Size();
  dVDList->add(oss.str().c_str());
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
  m_View->GetScrollWidget()->redraw();
  m_View->GetFullWidget()->redraw();
  m_View->GetZoomWidget()->redraw();
}

void ViewerModule::UseDEM()
{
  std::cout <<"Selected " << bDEM->value() << std::flush <<std::endl;
  
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
   //guiGrayscaleChannelChoice->redraw();

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

   //guiViewerSetupName->value(this->CutFileName(selectedItem-1));
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

   //RenderingFunctionType::Pointer renderingFunction = m_ImageViewerManagerModel->GetObjectList().at(selectedItem-1).pRenderFunction;

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
  //RenderingFunctionType::Pointer renderingFunction = m_ImageViewerManagerModel->GetObjectList().at(selectedItem-1).pRenderFunction;

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
   
   //RenderingFunctionType::Pointer renderingFunction = m_ImageViewerManagerModel->GetObjectList().at(selectedItem-1).pRenderFunction;
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
  
  // Refresh widgets
  this->RedrawWidget();
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
  
  // Refresh widgets
  this->RedrawWidget();
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
  
  // Refresh widgets
  this->RedrawWidget();
}


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
  
  // Refresh widgets
  this->RedrawWidget();
}


} // End namespace otb

#endif
