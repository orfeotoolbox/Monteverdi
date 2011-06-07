#include "otbMeanShiftModuleView.h"

#include "otbMsgReporter.h"
#include "otbI18n.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_draw.H>

namespace otb
{

MeanShiftModuleView::MeanShiftModuleView() : m_Controller(), m_Model(), m_ImageView()
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
  gZoom->remove(m_ImageView->GetZoomWidget());
  this->Exit();
}

void MeanShiftModuleView::SetModel(MeanShiftModuleModel* model)
{
  m_Model = model;
  m_ImageView->SetModel(m_Model->GetVisualizationModel());
  m_Model->RegisterListener(this);
}

void MeanShiftModuleView::Build()
{
  if (!m_Controller)
    {
    itkExceptionMacro(<< "Controller is not set, can not build view.");
    }

  if (!m_WidgetsController)
    {
    itkExceptionMacro(<< "Widgets controller is not set, can not build view.");
    }
  // Build the fltk code
  MeanShiftModuleViewGroup::Build();

  // Register controllers
  m_ImageView->SetController(m_WidgetsController);

  // Remove registered visualization components from the interface
  gImageViewer->add(m_ImageView->GetFullWidget());
  gScroll->add(m_ImageView->GetScrollWidget());
  gZoom->add(m_ImageView->GetZoomWidget());

  gImageViewer->resizable(m_ImageView->GetFullWidget());
  gScroll->resizable(m_ImageView->GetScrollWidget());
  gScroll->resizable(m_ImageView->GetZoomWidget());

  m_ImageView->GetFullWidget()->resize(gImageViewer->x(), gImageViewer->y(), gImageViewer->w(), gImageViewer->h());
  m_ImageView->GetScrollWidget()->resize(gScroll->x(), gScroll->y(), gScroll->w(), gScroll->h());
  m_ImageView->GetZoomWidget()->resize(gZoom->x(), gZoom->y(), gZoom->w(), gZoom->h());

  // Show and refresh the interface
  this->wMainWindow->show();

  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_ImageView->GetZoomWidget()->show();

  this->RefreshInterface();
  SetSpatialRadius(static_cast<unsigned int>(this->mSpatialRadius->value()));
  SetSpectralRadius(static_cast<double>(this->mSpectralRadius->value()));
  SetMinRegionSize(static_cast<unsigned int>(this->mMinRegionSize->value()));

  this->UpdateViewerSetup();

}
void MeanShiftModuleView::Notify(const std::string& event)
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
  m_Controller->Quit();
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}

void MeanShiftModuleView::RunSegmentation()
{
  m_Controller->RunSegmentation();
  this->mClustersButton->value(1);
  this->mClustersButton->do_callback();
}

void MeanShiftModuleView::SetSpatialRadius(unsigned int sr)
{
  m_Controller->SetSpatialRadius(sr);
}

void MeanShiftModuleView::SetSpectralRadius(double sr)
{
  m_Controller->SetSpectralRadius(sr);
}

void MeanShiftModuleView::SetMinRegionSize(unsigned int mr)
{
  m_Controller->SetMinRegionSize(mr);
}

void MeanShiftModuleView::SwitchClusters()
{
  m_Controller->SwitchClusters(static_cast<bool>(this->mClustersButton->value()));
}

void MeanShiftModuleView::SwitchBoundaries()
{
  bool val = true;
  if (mBoundButton->value() == 0)
    {
    val = false;
    mOpacity->deactivate();
    }
  else
    {
    mOpacity->activate();
    }

  m_Controller->SwitchBoundaries(val);
}

void MeanShiftModuleView::SetOpacity(double op)
{
  m_Controller->SetOpacity(op);
}

/**
 * Update the viewer setup window
 */
void MeanShiftModuleView::UpdateViewerSetup()
{
  if (!m_Model->GetIsImageReady())
    {
    // no image selected, return
    return;
    }

  unsigned int       lNbComponent = m_Model->GetInputImage()->GetNumberOfComponentsPerPixel();
  itk::OStringStream oss;
  oss.str("");

  //Clear all the choices
  iGrayscaleChannelChoice->clear();
  iRChannelChoice->clear();
  iGChannelChoice->clear();
  iBChannelChoice->clear();

  for (unsigned int i = 0; i < lNbComponent; ++i)
    {
    oss.str("");
    oss << i + 1;
    iGrayscaleChannelChoice->add(oss.str().c_str());
    iRChannelChoice->add(oss.str().c_str());
    iGChannelChoice->add(oss.str().c_str());
    iBChannelChoice->add(oss.str().c_str());
    }
  wViewerSetupWindow->redraw();
  //iGrayscaleChannelChoice->redraw();

  std::vector<unsigned int> channels = m_Model->GetChannels();
  if (lNbComponent > 2)
    {
    rViewerSetupColorMode->do_callback();
    iRChannelChoice->value(std::min(channels[0], lNbComponent - 1));
    iGChannelChoice->value(std::min(channels[1], lNbComponent - 1));
    iBChannelChoice->value(std::min(channels[2], lNbComponent - 1));
    }
  else
    {
    rViewerSetupGrayscaleMode->do_callback();
    iGrayscaleChannelChoice->value(std::min(channels[0], lNbComponent - 1));
    }
  wViewerSetupWindow->redraw();
}

void MeanShiftModuleView::ViewerSetupOkCallback()
{
  m_Controller->UpdateViewerDisplay();
}

void MeanShiftModuleView::Show()
{
  this->wMainWindow->show();

  m_ImageView->GetFullWidget()->show();
  m_ImageView->GetScrollWidget()->show();
  m_ImageView->GetZoomWidget()->show();
}

void MeanShiftModuleView::Hide()
{
  this->wMainWindow->hide();
}

} // end namespace
