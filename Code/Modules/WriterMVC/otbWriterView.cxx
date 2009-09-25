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


#include "otbWriterView.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Text_Buffer.H>

#include "otbMacro.h"
#include "itkExceptionObject.h"

namespace otb
{
/** Constructor */
WriterView::WriterView()
{
  // Model pointer and listener registration
  m_WriterModel = WriterModel::GetInstance();
  m_WriterModel->RegisterListener(this);
  this->BuildGUI();
}

/** Destructor */
WriterView::~WriterView()
{}

void
    WriterView
  ::Notify()
{
//   if (m_FeatureExtractionModel->GetHasInput())
//   {
//     InitWidgets();
//     this->UpdateChannelSelection();
//     this->UpdateInformation();
//   }
}

void WriterView::Show()
{
  std::cout << "show" << std::endl;
}



void WriterView::OK()
{
  std::string filepath = vFilePath->value();
  m_WriterModel->UpdateWriter(filepath);
}

void WriterView::Browse()
{
  const char * filename = NULL;

  filename = fl_file_chooser("Choose the dataset file ...", "*.*",".");
  Fl::check();
  if (filename == NULL)
  {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
  }
  vFilePath->value(filename);
  
  m_WriterController->OpenInputImage(filename);
  
}

void WriterView::Quit()
{
  guiMainWindow->hide();
}

} // End namespace otb


