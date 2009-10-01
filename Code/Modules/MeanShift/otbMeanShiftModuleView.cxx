#include "otbMeanShiftApplicationView.h"

#include "otbMsgReporter.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

namespace otb
{

MeanShiftApplicationView::MeanShiftApplicationView(): m_Controller(), m_Model(), m_ImageView()
{
  m_Model = MeanShiftApplicationModel::GetInstance();
  m_Model->RegisterListener(this);

  // Build the visualization part
  m_ImageView = ImageViewType::New();
  m_ImageView->SetModel(m_Model->GetVisualizationModel());


}

MeanShiftApplicationView::~MeanShiftApplicationView()
{
  // Remove registered visualization components from the interface
  gImageViewer->remove(m_ImageView->GetFullWidget());
  gScroll->remove(m_ImageView->GetScrollWidget());
}

void MeanShiftApplicationView::Build()
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
  MeanShiftApplicationViewGUI::Build();

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
void MeanShiftApplicationView::Notify()
{
  this->RefreshInterface();
  m_Mutex.Lock();
  this->RefreshVisualization();
  m_Mutex.Unlock();
  Fl::flush();

}

void MeanShiftApplicationView::RefreshVisualization()
{
  if (!m_Controller->IsModelUpdating())
  {
    m_ImageView->Update();
  }
}

void MeanShiftApplicationView::RefreshInterface()
{

}

void MeanShiftApplicationView::Exit()
{
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}

void MeanShiftApplicationView::OpenImage()
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

void MeanShiftApplicationView::SaveLabelImage()
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

void MeanShiftApplicationView::SaveClusterImage()
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



void MeanShiftApplicationView::RunSegmentation()
{
	m_Controller->RunSegmentation();
	static_cast<bool>(this->mClustersButton->value(1));
}

void MeanShiftApplicationView::SetSpatialRadius(unsigned int sr)
{
  m_Controller->SetSpatialRadius(sr);
}

void MeanShiftApplicationView::SetSpectralRadius(unsigned int sr)
{
	m_Controller->SetSpectralRadius(sr);
}

void MeanShiftApplicationView::SetMinRegionSize(unsigned int mr)
{
	m_Controller->SetMinRegionSize(mr);
}


void MeanShiftApplicationView::SwitchClusters()
{
	m_Controller->SwitchClusters(
	  static_cast<bool>(this->mClustersButton->value()));
}

void MeanShiftApplicationView::SwitchBoundaries()
{

}


}// end namespace
