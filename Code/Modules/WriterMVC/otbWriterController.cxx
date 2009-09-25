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
#ifndef __otbWriterController_cxx
#define __otbWriterController_cxx

#include "otbWriterController.h"


namespace otb
{
/** Constructor */
WriterController::WriterController()
{
  
}

/** Destructor */
WriterController::~WriterController()
{}

void
    WriterController
  ::OpenInputImage(const char * filename)
{
  try
  {
//     std::string strFilename = filename;
//     this->ClearSelectedChannels();
//     m_Model->SetInputImage( strFilename );
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}



} // End namespace otb

#endif
