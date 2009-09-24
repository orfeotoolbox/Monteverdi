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

namespace otb
{
/** Constructor */
ViewerModule::ViewerModule()
{
  //m_FPVViewer = FPVViewerType::New();
  //m_VectorViewer = VectorViewerType::New();

  m_StandardViewer = ViewerType::New();

  
  // Describe inputs
  this->AddInputDescriptor("Floating_Point_VectorImage","InputImage","Image to visualize :");
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
    InputImageType::Pointer image = dynamic_cast<InputImageType *>(data.GetDataObject());
    m_StandardViewer->SetImage(image);
    }
}


/** The custom run command */
void ViewerModule::Run()
{
  m_StandardViewer->Update();
 typedef ViewerModule::ImageType InputImageType;
  std::cout<<"Image recieved in the viewer module: "<<(InputImageType::Pointer)m_StandardViewer->GetImage()<<std::endl;
}

} // End namespace otb

#endif
