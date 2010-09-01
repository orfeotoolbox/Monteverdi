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
#ifndef __otbObjectLabelingView_cxx
#define __otbObjectLabelingView_cxx

#include "otbObjectLabelingView.h"

#include "otbMsgReporter.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

namespace otb
{

ObjectLabelingView::ObjectLabelingView() : m_Controller(), m_Model(), m_ImageView(), m_PixelView(), m_VectorDataGlComponents()
{
  // Get the model
  m_Model = ObjectLabelingModel::GetInstance();

  // Build the visualization part
  m_ImageView = ImageViewType::New();
  m_PixelView = PixelViewType::New();
}

ObjectLabelingView::~ObjectLabelingView()
{
  // Remove registered visualization components from the interface
  gFullResolution->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
  gZoom->remove(m_ImageView->GetZoomWidget());
  gPixelDescription->remove(m_PixelView->GetPixelDescriptionWidget());
}

void ObjectLabelingView::Build()
{
  m_Model->RegisterListener(this);

  // Build the visu part
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_PixelView->SetModel(m_Model->GetPixelDescriptionModel());

  // Add the GlComponent for ROI Selection
  VectorDataGlComponentType::Pointer glComp = VectorDataGlComponentType::New();
  glComp->SetVectorData(m_Model->GetSelectedPolygon());
  glComp->RenderPolygonBoundariesOnlyOn();
  m_ImageView->GetFullWidget()->AddGlComponent(glComp);
  m_ImageView->GetScrollWidget()->AddGlComponent(glComp);
  m_ImageView->GetZoomWidget()->AddGlComponent(glComp);

  // Add the GlComponent for Margin sampling
  VectorDataGlComponentType::Pointer glComp2 = VectorDataGlComponentType::New();
  VectorDataGlComponentType::ColorType color;
  color[0]=1.;
  color[1]=0.;
  color[2]=1.;
  color[3]=0.75;
  glComp2->SetVectorData(m_Model->GetMarginSampledPolygon());
  glComp2->RenderPolygonBoundariesOnlyOn();
  glComp2->SetColor(color);
  m_ImageView->GetFullWidget()->AddGlComponent(glComp2);
  m_ImageView->GetScrollWidget()->AddGlComponent(glComp2);
  m_ImageView->GetZoomWidget()->AddGlComponent(glComp2);

  // Add the GlComponent for neighboring polygons
//  VectorDataGlComponentType::Pointer glComp3 = VectorDataGlComponentType::New();
//  VectorDataGlComponentType::ColorType color3;
//  color3[0]=0.;
//  color3[1]=1.;
//  color3[2]=0.;
//  color3[3]=0.5;
//  glComp3->SetVectorData(m_Model->GetNeighborsPolygon());
//  glComp3->RenderPolygonBoundariesOnlyOn();
//  glComp3->SetColor(color3);
//  m_ImageView->GetFullWidget()->AddGlComponent(glComp3);
//  m_ImageView->GetScrollWidget()->AddGlComponent(glComp3);
//  m_ImageView->GetZoomWidget()->AddGlComponent(glComp3);
  
  if(!m_Controller)
    {
    itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  if(!m_WidgetsController)
    {
    itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }

  // Build the fltk code
  ObjectLabelingGUI::Build();

  // Register controllers
  m_ImageView->SetController(m_WidgetsController);

  // Remove registered visualization components from the interface
  gFullResolution->add(m_ImageView->GetFullWidget());
  gScroll->add(m_ImageView->GetScrollWidget());
  gZoom->add(m_ImageView->GetZoomWidget());
  gPixelDescription->add(m_PixelView->GetPixelDescriptionWidget());

  gFullResolution->resizable(m_ImageView->GetFullWidget());
  gScroll->resizable(m_ImageView->GetScrollWidget());
  gZoom->resizable(m_ImageView->GetZoomWidget());
  gPixelDescription->resizable(m_PixelView->GetPixelDescriptionWidget());

  m_ImageView->GetZoomWidget()->resize(gZoom->x(),gZoom->y(),gZoom->w(),gZoom->h());
  m_ImageView->GetFullWidget()->resize(gFullResolution->x(),gFullResolution->y(),gFullResolution->w(),gFullResolution->h());
  m_ImageView->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),gScroll->w(),gScroll->h());
  m_PixelView->GetPixelDescriptionWidget()->resize(gPixelDescription->x(),gPixelDescription->y(),gPixelDescription->w(),gPixelDescription->h());

 // Show and refresh the interface
  this->wMainWindow->show();
  //this->wBandSetupMainWindow->show();

  m_ImageView->GetZoomWidget()->show();
  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_PixelView->GetPixelDescriptionWidget()->show();

  // Updating svm kernel list
  bSVMKernel->add("Linear");
  bSVMKernel->add("Polynomial");
  bSVMKernel->add("Radial Basis Function");
  bSVMKernel->add("Sigmoid");

  this->RefreshInterface();
}

void ObjectLabelingView::UpdateViewerSetup()
{
  if(!m_Model->GetVectorImage())
    {
      return;
    }
  
  unsigned int lNbComponent = m_Model->GetVectorImage()->GetNumberOfComponentsPerPixel();
  itk::OStringStream oss;
  oss.str("");
  
  //Clear all the choices
  iGrayscaleChannelChoice->clear();
  iRChannelChoice->clear();
  iGChannelChoice->clear();
  iBChannelChoice->clear();
  
  for (unsigned int i = 0;i<lNbComponent;++i)
    {
      oss.str("");
      oss<<i+1;
      iGrayscaleChannelChoice->add(oss.str().c_str());
      iRChannelChoice->add(oss.str().c_str());
      iGChannelChoice->add(oss.str().c_str());
      iBChannelChoice->add(oss.str().c_str());
    }
  wViewerSetupWindow->redraw();
  //iGrayscaleChannelChoice->redraw();
  
  std::vector<unsigned int> channels = m_Model->GetChannels();
  if(lNbComponent > 2)
    {
      rViewerSetupColorMode->do_callback();
      iRChannelChoice->value(std::min(channels[0],lNbComponent-1));
      iGChannelChoice->value(std::min(channels[1],lNbComponent-1));
      iBChannelChoice->value(std::min(channels[2],lNbComponent-1));
    }
  else
    {
      rViewerSetupGrayscaleMode->do_callback();
      iGrayscaleChannelChoice->value(std::min(channels[0],lNbComponent-1));
    }
  wViewerSetupWindow->redraw();
}

void ObjectLabelingView::SetController(ObjectLabelingControllerInterface * controller)
{
  m_Controller = controller;
}

void ObjectLabelingView::SetWidgetsController(ImageWidgetController * controller)
{
  m_WidgetsController = controller; 
}

void ObjectLabelingView::Notify(const std::string & event)
{
  if(event == "Update")
    {
    this->RefreshInterface();
    this->RefreshVisualization();
    Fl::flush();
    }

}

void ObjectLabelingView::RefreshVisualization()
{
  // Apply spacing to all components
  dynamic_cast<VectorDataGlComponentType *>(m_ImageView->GetFullWidget()->GetNthGlComponent(1))->SetOrigin(m_Model->GetOrigin());
  dynamic_cast<VectorDataGlComponentType *>(m_ImageView->GetFullWidget()->GetNthGlComponent(1))->SetSpacing(m_Model->GetSpacing());
  dynamic_cast<VectorDataGlComponentType *>(m_ImageView->GetFullWidget()->GetNthGlComponent(2))->SetOrigin(m_Model->GetOrigin());
  dynamic_cast<VectorDataGlComponentType *>(m_ImageView->GetFullWidget()->GetNthGlComponent(2))->SetSpacing(m_Model->GetSpacing());
//  dynamic_cast<VectorDataGlComponentType *>(m_ImageView->GetFullWidget()->GetNthGlComponent(3))->SetOrigin(m_Model->GetOrigin());
//  dynamic_cast<VectorDataGlComponentType *>(m_ImageView->GetFullWidget()->GetNthGlComponent(3))->SetSpacing(m_Model->GetSpacing());

  // Remove all GL components from views except for the 4 first ones which are always here
  unsigned int nbComponents = m_ImageView->GetFullWidget()->GetNumberOfGlComponents();

  for(unsigned int compIndex = nbComponents-1; compIndex > 3; --compIndex)
    {
      m_ImageView->GetFullWidget()->RemoveGlComponent(compIndex);
      m_ImageView->GetScrollWidget()->RemoveGlComponent(compIndex);
      // ZoomWidget does not have the ROI GL Component, so it has only 3 fixed GlComponents
      m_ImageView->GetZoomWidget()->RemoveGlComponent(compIndex - 1);
    }

  // Recreate all VectorDataGlComponentType
  m_VectorDataGlComponents.clear();


  // Create all VectorDataGlComponentType to reflect the model
  for(unsigned int classIndex = 0; classIndex < m_Model->GetNumberOfClasses();++classIndex)
    {
      VectorDataGlComponentType::Pointer glComp = VectorDataGlComponentType::New();
      glComp->SetVectorData(m_Model->GetClass(classIndex).m_VectorData);
      glComp->SetColor(m_Model->GetClass(classIndex).m_Color);
      glComp->SetOrigin(m_Model->GetOrigin());
      glComp->SetSpacing(m_Model->GetSpacing());

      m_ImageView->GetFullWidget()->AddGlComponent(glComp);
      m_ImageView->GetScrollWidget()->AddGlComponent(glComp);
      m_ImageView->GetZoomWidget()->AddGlComponent(glComp);
      m_VectorDataGlComponents.push_back(glComp);
    }

  m_ImageView->Update();
}


void ObjectLabelingView::RefreshInterface()
{
  typedef ObjectLabelingModel::ObjectClassVectorType::const_iterator ClassesIteratorType;

  ClassesIteratorType it = m_Model->GetClasses().begin();

  // Update the classes list
  lClasses->clear();

  itk::OStringStream oss;

  while(it!=m_Model->GetClasses().end())
    {
    oss.str("");
    oss<<it->m_Name<<" ("<<it->m_Samples.size()<<" objects)";
    lClasses->add(oss.str().c_str());
    ++it;
    }
  
  // Select the previously selected class
  if(m_Model->HasSelectedClass())
    {
    lClasses->value(m_Model->GetSelectedClass()+1);
    }
  else
    {
    lClasses->value(0);
    }
  this->UpdateClassInformation();

  // Buttons activation/deactivation
  if(m_Model->HasClasses())
    {
    bRemoveClass->activate();
    bClearClasses->activate();
    }
  else
    {
    bRemoveClass->deactivate();
    bClearClasses->deactivate();
    }

  // Features update 
  lFeatures->clear();
  ObjectLabelingModel::AvailableFeaturesMapType myvector = m_Model->GetAvailableFeatures();
  ObjectLabelingModel::AvailableFeaturesMapType::const_iterator fit = myvector.begin();

  while(fit != myvector.end())
    {
    lFeatures->add(fit->first.c_str(),fit->second);
    ++fit;
    }

  // Margin samples update
  int lvalue = lMarginSamples->value();
  lMarginSamples->clear();

  ObjectLabelingModel::LabelVectorType::const_iterator lit = m_Model->GetMarginSamples().begin();
  

  while(lit != m_Model->GetMarginSamples().end())
    {
    oss.str("");
    oss<<*lit;
    lMarginSamples->add(oss.str().c_str());
    ++lit;
    }

  lMarginSamples->value(lvalue);

  // Read the accuracy value 
  vAccuracy->value(m_Model->GetAccuracy());

  // Read parameters
  bSVMKernel->value(m_Model->GetSVMEstimator()->GetKernelType());
  bNbCvFolders->value(m_Model->GetSVMEstimator()->GetNumberOfCrossValidationFolders());
  bSVMNbCoarseSteps->value(m_Model->GetSVMEstimator()->GetCoarseOptimizationNumberOfSteps());
  bSVMNbFineSteps->value(m_Model->GetSVMEstimator()->GetFineOptimizationNumberOfSteps());
  bSVMOptimization->value(m_Model->GetSVMEstimator()->GetParametersOptimization());
  bALNumberOfSamples->value(m_Model->GetMarginSampler()->GetNumberOfCandidates());

  bUseContext->value(m_Model->GetUseContext());
}

void ObjectLabelingView::UpdateClassInformation()
{
  if(m_Model->HasSelectedClass())
    {
    unsigned int classIndex = m_Model->GetSelectedClass();
    itk::OStringStream oss;
    oss<<m_Model->GetClass(classIndex).m_Label;
    vClassLabel->activate();
    vClassLabel->value(oss.str().c_str());
    vClassName->activate();
    vClassName->value(m_Model->GetClass(classIndex).m_Name.c_str());
    Fl_Color flColor = fl_color_cube(static_cast<int>((FL_NUM_RED - 1) *m_Model->GetClass(classIndex).m_Color[0]),
				     static_cast<int>((FL_NUM_GREEN - 1) *m_Model->GetClass(classIndex).m_Color[1]),
				     static_cast<int>((FL_NUM_BLUE - 1) *m_Model->GetClass(classIndex).m_Color[2]));
    bClassColor->activate();
    lClasses->selection_color(flColor);
    bClassColor->color(flColor);    

    // Update the sample list
    ObjectLabelingModel::ObjectClassType::LabelVectorType::const_iterator sit = m_Model->GetClass(classIndex).m_Samples.begin();
    
    int objectIndex = lObjects->value();

    lObjects->clear();
    
    while(sit != m_Model->GetClass(classIndex).m_Samples.end())
      {
      oss.str("");
      oss<<(*sit);
      lObjects->add(oss.str().c_str());
      ++sit;
      }

    if(objectIndex <= lObjects->size())
      {
      lObjects->value(objectIndex);
      }
    }
  else
    {
    lObjects->clear();
    vClassLabel->deactivate();
    vClassLabel->value("");
    vClassName->deactivate();
    vClassName->value("");
    bClassColor->deactivate();
    lClasses->selection_color(gObjectsInterface->color());
    bClassColor->color(gObjectsInterface->color());
    }
  bClassColor->redraw();
  lClasses->redraw();  
}

void ObjectLabelingView::Classes()
{
  if(lClasses->value() == 0)
    {
    m_Controller->ClearSelectedClass();
    }
  else
    {
    m_Controller->ClassSelected(lClasses->value()-1);
    }
}

void ObjectLabelingView::AddClass()
{
//  VectorDataGlComponentType::Pointer glComp = VectorDataGlComponentType::New();
//  m_ImageView->GetFullWidget()->AddGlComponent(glComp);
//  m_ImageView->GetScrollWidget()->AddGlComponent(glComp);
//  m_ImageView->GetZoomWidget()->AddGlComponent(glComp);
//  m_VectorDataGlComponents.push_back(glComp);
  m_Controller->AddClass();

}

void ObjectLabelingView::RemoveClass()
{
  if(m_Model->HasSelectedClass())
    {
//    m_ImageView->GetFullWidget()->RemoveGlComponent(3+m_Model->GetSelectedClass());
//    m_ImageView->GetScrollWidget()->RemoveGlComponent(3+m_Model->GetSelectedClass());
//    m_ImageView->GetZoomWidget()->RemoveGlComponent(3+m_Model->GetSelectedClass());
//    m_VectorDataGlComponents.erase(m_VectorDataGlComponents.begin()+m_Model->GetSelectedClass());
    m_Controller->RemoveClass(m_Model->GetSelectedClass());
    }
}

void ObjectLabelingView::ClearClasses()
{
//  for(unsigned int i = 0; i<m_Model->GetNumberOfClasses();++i)
//    {
//    m_ImageView->GetFullWidget()->RemoveGlComponent(3+i);
//    m_ImageView->GetScrollWidget()->RemoveGlComponent(3+i);
//    m_ImageView->GetZoomWidget()->RemoveGlComponent(3+i);
//    }
//  m_VectorDataGlComponents.clear();
  m_Controller->ClearClasses();
}

void ObjectLabelingView::ClassColor()
{
   if(m_Model->HasSelectedClass())
    {
     double r = m_Model->GetClass(m_Model->GetSelectedClass()).m_Color[0];
     double g = m_Model->GetClass(m_Model->GetSelectedClass()).m_Color[1];
     double b = m_Model->GetClass(m_Model->GetSelectedClass()).m_Color[2];
     double a = m_Model->GetClass(m_Model->GetSelectedClass()).m_Color[3];
     
     bool ok = fl_color_chooser("Changed class color:",r,g,b);
     
     if (ok)
       {
       ObjectLabelingModel::ColorType color;
       color[0]=r;
       color[1]=g;
       color[2]=b;
       color[3]=a;
       m_Controller->ChangeClassColor(m_Model->GetSelectedClass(),color);
       }
    }
  
}
void ObjectLabelingView::ClassLabel()
{
  if(m_Model->HasSelectedClass())
    {
    m_Controller->ChangeClassLabel(m_Model->GetSelectedClass(),atoi(vClassLabel->value()));
    }
}
void ObjectLabelingView::ClassName()
{
  if(m_Model->HasSelectedClass())
    {
    m_Controller->ChangeClassName(m_Model->GetSelectedClass(),vClassName->value());
    } 
}
void ObjectLabelingView::Objects()
{
  m_Controller->SelectObject(lObjects->value()-1);
}
void ObjectLabelingView::RemoveObject()
{
  if(lObjects->value()>0)
    {
    m_Controller->RemoveObject(lObjects->value()-1);

    }
}

void ObjectLabelingView::ClearObjects()
{
  m_Controller->ClearObjects();
}

void ObjectLabelingView::ObjectFocus(){}
void ObjectLabelingView::ImageFileChooser()
{
  const char * filename = NULL;

  filename = fl_file_chooser("Pick an image file", "*.*",".");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }
  vImagePath->value(filename);
}

void ObjectLabelingView::LabelsFileChooser()
{
  const char * filename = NULL;
  
  filename = fl_file_chooser("Pick a labeled image file", "*.*",".");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }
  vLabelsPath->value(filename);

}

void ObjectLabelingView::SaveSamplesToXMLFile()
{
  
  const char * filename = NULL;
  
  filename = fl_file_chooser("Pick a file", "*.*",".");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }

  m_Controller->SaveSamplesToXMLFile(filename);
}

void ObjectLabelingView::SaveClassificationParametersToXMLFile()
{
  
  const char * filename = NULL;
  
  filename = fl_file_chooser("Pick a file", "*.*",".");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }

  m_Controller->SaveClassificationParametersToXMLFile(filename);
}

void ObjectLabelingView::LoadSamplesFromXMLFile()
{
  
  const char * filename = NULL;
  
  filename = fl_file_chooser("Pick a file", "*.*",".");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }

  m_Controller->LoadSamplesFromXMLFile(filename);
}

void ObjectLabelingView::SaveClassification()
{
  m_Controller->SaveClassification();
}

void ObjectLabelingView::Exit()
{
  this->SaveClassification();
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
  wImagesSelectionWindow->hide();
}

void ObjectLabelingView::SampleMargin()
{
  bBusy->show();
  Fl::flush();
  m_Controller->SampleMargin();
  bBusy->hide();
}

void ObjectLabelingView::ChangeKernelType()
{
  m_Controller->ChangeKernelType(bSVMKernel->value());
}

void ObjectLabelingView::ChangeNumberOfCrossValidationFolders()
{
  m_Controller->ChangeNumberOfCrossValidationFolders(bNbCvFolders->value());
}

void ObjectLabelingView::ChangeParametersOptimisation()
{
  m_Controller->ChangeParametersOptimisation(bSVMOptimization->value());
}

void ObjectLabelingView::ChangeNumberOfCoarseSteps()
{
  m_Controller->ChangeNumberOfCoarseSteps(bSVMNbCoarseSteps->value());
}

void ObjectLabelingView::ChangeNumberOfFineSteps()
{
  m_Controller->ChangeNumberOfFineSteps(bSVMNbFineSteps->value());
}

void ObjectLabelingView::ChangeNumberOfMarginSamples()
{
  m_Controller->ChangeNumberOfMarginSamples(bALNumberOfSamples->value());
}

void ObjectLabelingView::ChangeMarginColor()
{
// TODO: Implement this
}

void ObjectLabelingView::FocusOnMarginSample()
{
  m_Controller->FocusOnMarginSample(lMarginSamples->value());
}

void ObjectLabelingView::ClearMarginSamples()
{
  m_Controller->ClearMarginSamples();
}

void ObjectLabelingView::Classify()
{
  bBusy->show();
  Fl::flush();
  for(VectorDataGlComponentVectorType::iterator  git = m_VectorDataGlComponents.begin();
      git!=m_VectorDataGlComponents.end();++git)
    {
    (*git)->SetVisible(false);
    }
  m_Controller->Classify();
  bBusy->hide();
}

void ObjectLabelingView::ClearClassification()
{
  for(VectorDataGlComponentVectorType::iterator  git = m_VectorDataGlComponents.begin();
      git!=m_VectorDataGlComponents.end();++git)
    {
    (*git)->SetVisible(true);
    }
  m_Controller->ClearClassification();
}

void ObjectLabelingView::Features()
{
  // Walk the feature list
  for(int i = 1; i<=lFeatures->nitems();++i)
    {
    std::string fname = lFeatures->text(i);
    if(lFeatures->checked(i))
      {
      m_Controller->ChangeFeatureState(fname,true);
      }
    else
      {
      m_Controller->ChangeFeatureState(fname,false);
      }
    }
}

void ObjectLabelingView::ViewerSetupOkCallback()
{
  m_Controller->UpdateViewerDisplay();
}

void ObjectLabelingView::BandSetupOkCallback()
{
  //m_Controller->
  std::cout<<"B: "<<this->iBChannelId->value()<<std::endl;
  std::cout<<"G: "<<this->iGChannelId->value()<<std::endl;
  std::cout<<"R: "<<this->iRChannelId->value()<<std::endl;
  std::cout<<"NIR: "<<this->iNIRChannelId->value()<<std::endl;
  m_Controller->UpdateBandId();
  // hide the window
  this->wBandSetup->hide();
  // show the main window
  this->wMainWindow->show();
}

void ObjectLabelingView::UseContext()
{
  m_Controller->SetUseContext(bUseContext->value());
}

void ObjectLabelingView::ClassificationOpacity()
{
  m_Controller->ChangeClassificationOpacity(bClassificationOpacity->value());
}

}
#endif
