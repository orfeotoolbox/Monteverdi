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
#ifndef __otbSpeckleFilteringModule_cxx
#define __otbSpeckleFilteringModule_cxx

#include "otbSpeckleFilteringModule.h"

namespace otb
{
/** Constructor */
SpeckleFilteringModule::SpeckleFilteringModule()
{
  // Build mvc
  m_View       = SpeckleFilteringView::New();
  m_Controller = SpeckleFilteringController::New();
  m_Model      = SpeckleFilteringModel::GetInstance();
  m_Controller->SetView(m_View);
  m_View->SetController(m_Controller);

  // Describe inputs
  this->AddInputDescriptor("Floating_Point_Image","InputImage","Image to apply speckle filtering on.");
}

/** Destructor */
SpeckleFilteringModule::~SpeckleFilteringModule()
{}

/** PrintSelf method */
void SpeckleFilteringModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** Assign input by key. This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void SpeckleFilteringModule::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  typedef SpeckleFilteringModel::InputImageType InputImageType;

  if(key == "InputImage")
    {
    InputImageType * image = dynamic_cast<InputImageType *>(data.GetDataObject());
    m_Model->SetInputImage(image);
    }
}

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
const DataObjectWrapper SpeckleFilteringModule::RetrieveOutputByKey(const std::string & key) const
{
  DataObjectWrapper wrapper;
  if(key == "OutputImage")
    {
    wrapper.Set("Floating_Point_Image",m_Model->GetOutput());
    }
  return wrapper;
}

/** The custom run command */
void SpeckleFilteringModule::Run()
{
  m_View->Show();
  this->AddOutputDescriptor("Floating_Point_Image","OutputImage","Speckle filtered image.");
}

} // End namespace otb

#endif
