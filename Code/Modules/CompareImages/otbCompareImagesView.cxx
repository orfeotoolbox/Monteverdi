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

#include "otbCompareImagesView.h"

namespace otb
{
/**
 * Constructor
 */
CompareImagesView
::CompareImagesView()
{
  // Create the component of the GUI
  this->CreateGUI();

  pBusyBar->minimum(0);
  pBusyBar->maximum(1);
  pBusyBar->value(1);

  // Build the visualization part
  m_LeftVisuView = VisuViewType::New();
  m_RightVisuView = VisuViewType::New();
  m_LeftPixelDescriptionView = PixelDescriptionViewType::New();
  m_RightPixelDescriptionView = PixelDescriptionViewType::New();
}


void
CompareImagesView
::InitVisu()
{
  // Set the model
  m_LeftVisuView->SetModel(GetModel()->GetLeftVisuModel());

  // Set the model
  m_RightVisuView->SetModel(GetModel()->GetRightVisuModel());
  
  m_LeftPixelDescriptionView->SetModel(GetModel()->GetLeftPixelDescriptionModel());
  m_RightPixelDescriptionView->SetModel(GetModel()->GetRightPixelDescriptionModel());
  
  
  gLeftImagePixelInformation->add(m_LeftPixelDescriptionView->GetPixelDescriptionWidget());
  gLeftImagePixelInformation->resizable(m_LeftPixelDescriptionView->GetPixelDescriptionWidget());
  m_LeftPixelDescriptionView->GetPixelDescriptionWidget()->resize(gLeftImagePixelInformation->x(), gLeftImagePixelInformation->y(),
                                                              gLeftImagePixelInformation->w(), gLeftImagePixelInformation->h());
  
  
  gRightImagePixelInformation->add(m_RightPixelDescriptionView->GetPixelDescriptionWidget());
  gRightImagePixelInformation->resizable(m_RightPixelDescriptionView->GetPixelDescriptionWidget());
  m_RightPixelDescriptionView->GetPixelDescriptionWidget()->resize(gRightImagePixelInformation->x(), gRightImagePixelInformation->y(),
                                                              gRightImagePixelInformation->w(), gRightImagePixelInformation->h());
  gLeftImagePixelInformation->redraw();
  gRightImagePixelInformation->redraw();
}

CompareImagesModel*
CompareImagesView
::GetModel()
{
  return m_Controller->GetModel();
}

/**
* Process
*/
void
CompareImagesView
::Process()
{
  this->pBusyBar->show();
  Fl::check();

  m_Controller->ProcessCompareImagesFilter();

  this->pBusyBar->hide();
  Fl::check();
}

/**
* Update Stats
*/
void
CompareImagesView
::UpdateStats()
{
  unsigned int stat_element_width = (unsigned int)((double)gMatrixCompareImagess->w() / 3.);
  unsigned int stat_element_height = 20;
  
  unsigned int x0 = gMatrixCompareImagess->x();
  unsigned int y0 = gMatrixCompareImagess->y() + stat_element_height;
  
  Fl_Output * stat_element;
  
  // RMSE
  stat_element = new Fl_Output(x0 + 0 * stat_element_width , y0,
			       stat_element_width, stat_element_height);
  
  std::ostringstream ostream_rmse;
  ostream_rmse << vcl_sqrt(GetModel()->GetRMSE());

  
  stat_element->value(ostream_rmse.str().c_str());
  stat_element->textfont(FL_HELVETICA_BOLD);
  stat_element->clear_visible_focus();
  
  gMatrixCompareImagess->add(stat_element);
  
  // PSNR
  stat_element = new Fl_Output(x0 + 1 * stat_element_width , y0,
			       stat_element_width, stat_element_height);
  
  std::ostringstream ostream_psnr;
  ostream_psnr << GetModel()->GetPSNR(); 
  
  stat_element->value(ostream_psnr.str().c_str());
  stat_element->textfont(FL_HELVETICA_BOLD);
  stat_element->clear_visible_focus();
  
  gMatrixCompareImagess->add(stat_element);
  
  // MaxAbsoluteError
  stat_element = new Fl_Output(x0 + 2 * stat_element_width , y0,
			       stat_element_width, stat_element_height);
  
  std::ostringstream ostream_maxerror;
  ostream_maxerror << GetModel()->GetMaxError(); 
  
  stat_element->value(ostream_maxerror.str().c_str());
  stat_element->textfont(FL_HELVETICA_BOLD);
  stat_element->clear_visible_focus();
  
  gMatrixCompareImagess->add(stat_element);    
  
  gMatrixCompareImagess->redraw();
}

/**
 * Handle the notification of the model
 */
void
CompareImagesView
::Notify(const std::string& event)
{
   if(event == "StatsComputed")
   {
      this->UpdateStats();
   }
}




/**
 * Show the GUI
 */
void
CompareImagesView
::Show()
{
  unsigned int stat_element_width = (unsigned int)((double)gMatrixCompareImagess->w() / 3.);
  unsigned int stat_element_height = 20;
  
  unsigned int x0 = gMatrixCompareImagess->x();
  unsigned int y0 = gMatrixCompareImagess->y();
  
  Fl_Output * stat_element;
  
  // Header RMSE
  stat_element = new Fl_Output(x0 + 0 * stat_element_width , y0,
			       stat_element_width, stat_element_height);
  
  stat_element->value("RMSE");
  stat_element->textfont(FL_HELVETICA_BOLD);
  stat_element->clear_visible_focus();
  //stat_element->deactivate();
  
  gMatrixCompareImagess->add(stat_element);
  
  // Header PSNR
  stat_element = new Fl_Output(x0 + 1 * stat_element_width , y0,
			       stat_element_width, stat_element_height);
  
  stat_element->value("PSNR");
  stat_element->textfont(FL_HELVETICA_BOLD);
  stat_element->clear_visible_focus();
  //stat_element->deactivate();
  
  gMatrixCompareImagess->add(stat_element);
  
  // Header MaxError
  stat_element = new Fl_Output(x0 + 2 * stat_element_width , y0,
			       stat_element_width, stat_element_height);
  
  stat_element->value("MaxError");
  stat_element->textfont(FL_HELVETICA_BOLD);
  stat_element->clear_visible_focus();
  //stat_element->deactivate();
  
  gMatrixCompareImagess->add(stat_element);


  // Show and refresh the interface
  guiMainWindow->show();

  // Remove registered visualization components from the interface
  gViewFullLeft->add(m_LeftVisuView->GetFullWidget());
  gViewFullLeft->resizable(m_LeftVisuView->GetFullWidget());
  m_LeftVisuView->GetFullWidget()->show();
  m_LeftVisuView->GetFullWidget()->resize(gViewFullLeft->x(), gViewFullLeft->y(), gViewFullLeft->w(), gViewFullLeft->h());
  
  gViewFullRight->add(m_RightVisuView->GetFullWidget());
  gViewFullRight->resizable(m_RightVisuView->GetFullWidget());
  m_RightVisuView->GetFullWidget()->show();
  m_RightVisuView->GetFullWidget()->resize(gViewFullRight->x(), gViewFullRight->y(), gViewFullRight->w(), gViewFullRight->h());
  
  
  gViewQuicklook->add(m_RightVisuView->GetScrollWidget());
  m_RightVisuView->GetScrollWidget()->show();
  m_RightVisuView->GetScrollWidget()->resize(gViewQuicklook->x(), gViewQuicklook->y(), gViewQuicklook->w(), gViewQuicklook->h());
  
  gViewQuicklook->add(m_LeftVisuView->GetScrollWidget());
  gViewQuicklook->resizable(m_LeftVisuView->GetScrollWidget());
  m_LeftVisuView->GetScrollWidget()->show();
  m_LeftVisuView->GetScrollWidget()->resize(gViewQuicklook->x(), gViewQuicklook->y(), gViewQuicklook->w(), gViewQuicklook->h());
  
  m_LeftVisuView->GetFullWidget()->redraw();
  m_RightVisuView->GetFullWidget()->redraw();
  m_LeftVisuView->GetScrollWidget()->redraw();
  
  gViewQuicklook->show();
  gViewFullLeft->show();
  gViewFullRight->show();
  
}


/**
 * SaveQuit method
 */
void
CompareImagesView
::SaveQuit()
{
  m_Controller->SaveQuit();
  //hide the gui
  this->Hide();
}

/**
 * Quit method
 */
void
CompareImagesView
::Quit()
{
  this->Hide();
}

/// Hide the app
void
CompareImagesView
::Hide(void)
{
  // hide the main windo
  guiMainWindow->hide();

}

/**
 * PrintSelf Method
 */
void
CompareImagesView
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // End namespac
