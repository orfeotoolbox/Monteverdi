#include "otbMeanShiftModuleView.h"

#include "otbMsgReporter.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

namespace otb
{

MeanShiftModuleView::MeanShiftModuleView(): m_Controller(), m_Model(), m_ImageView()
{
  m_Model = MeanShiftModuleModel::New();

  // Build the visualization part
  m_ImageView = ImageViewType::New();


}

MeanShiftModuleView::~MeanShiftModuleView()
{
  // Remove registered visualization components from the interface
  gImageViewer->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
}

void MeanShiftModuleView::SetModel(MeanShiftModuleModel* model)
{
  m_Model = model;
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_Model->RegisterListener(this);
}

void MeanShiftModuleView::Build()
{
  if(!m_Controller)
    {
    itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  if(!m_WidgetsController)
    {
    itkExceptionMacro(<<"Widgets controller is not set, can not build view.");
    }
  // Build the fltk code
  MeanShiftModuleViewGroup::Build();

  // Register controllers
   m_ImageView->SetController(m_WidgetsController);

   // Remove registered visualization components from the interface
   gImageViewer->add(m_ImageView->GetFullWidget());
   gScroll->add(m_ImageView->GetScrollWidget());

   gImageViewer->resizable(m_ImageView->GetFullWidget());
   gScroll->resizable(m_ImageView->GetScrollWidget());

   m_ImageView->GetFullWidget()->resize(gImageViewer->x(),gImageViewer->y(),gImageViewer->w(),gImageViewer->h());
   m_ImageView->GetScrollWidget()->resize(gScroll->x(),gScroll->y(),gScroll->w(),gScroll->h());

   // Show and refresh the interface
    this->wMainWindow->show();

    m_ImageView->GetFullWidget()->show();
    m_ImageView->GetScrollWidget()->show();


    this->RefreshInterface();
    SetSpatialRadius(static_cast<unsigned int>(this->mSpatialRadius->value()));
    SetSpectralRadius(static_cast<unsigned int>(this->mSpectralRadius->value()));
    SetMinRegionSize(static_cast<unsigned int>(this->mMinRegionSize->value()));
}
void MeanShiftModuleView::Notify()
{
  this->RefreshInterface();
  m_Mutex.Lock();
  this->RefreshVisualization();
  m_Mutex.Unlock();
  Fl::flush();

}

void MeanShiftModuleView::RefreshVisualization()
{
  if (!m_Controller->IsModelUpdating())
  {
    m_ImageView->Update();
  }
}

void MeanShiftModuleView::RefreshInterface()
{

}

void MeanShiftModuleView::Exit()
{
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}

void MeanShiftModuleView::OpenImage()
{
  const char * cfname = fl_file_chooser("Select an Input Image", "*.*",".");
  Fl::check();
  wMainWindow->redraw();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  m_Controller->OpenImage( cfname );
}

void MeanShiftModuleView::SaveLabelImage()
{
  const char * cfname = fl_file_chooser("Select Image Name", "*.*",".");
  Fl::check();
  wMainWindow->redraw();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  m_Controller->SaveLabelImage( cfname );
}

void MeanShiftModuleView::SaveClusterImage()
{
  const char * cfname = fl_file_chooser("Select Image Name", "*.*",".");
  Fl::check();
  wMainWindow->redraw();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  m_Controller->SaveClusterImage( cfname );
}



void MeanShiftModuleView::RunSegmentation()
{
       m_Controller->RunSegmentation();
       static_cast<bool>(this->mClustersButton->value(1));
}

void MeanShiftModuleView::SetSpatialRadius(unsigned int sr)
{
  m_Controller->SetSpatialRadius(sr);
}

void MeanShiftModuleView::SetSpectralRadius(unsigned int sr)
{
       m_Controller->SetSpectralRadius(sr);
}

void MeanShiftModuleView::SetMinRegionSize(unsigned int mr)
{
       m_Controller->SetMinRegionSize(mr);
}


void MeanShiftModuleView::SwitchClusters()
{
       m_Controller->SwitchClusters(
         static_cast<bool>(this->mClustersButton->value()));
}

void MeanShiftModuleView::SwitchBoundaries()
{

}


}// end namespace
