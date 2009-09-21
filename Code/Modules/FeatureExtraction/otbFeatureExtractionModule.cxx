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
#ifndef __otbFeatureExtractionModule_cxx
#define __otbFeatureExtractionModule_cxx

#include "otbFeatureExtractionModule.h"

namespace otb
{
/** Constructor */
FeatureExtractionModule::FeatureExtractionModule()
{
  // Build mvc
  m_View       = FeatureExtractionViewGUI::New();
  m_Controller = FeatureExtractionController::New();
  m_Model      = FeatureExtractionModel::GetInstance();
  m_Controller->SetView(m_View);
  m_View->SetFeatureExtractionController(m_Controller);

  // Describe inputs
  this->AddInputDescriptor("Floating_Point_Image","InputImage","Image to apply feature extraction.");
}

/** Destructor */
FeatureExtractionModule::~FeatureExtractionModule()
{}

/** PrintSelf method */
void FeatureExtractionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** Assign input by key. This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void FeatureExtractionModule::AssignDataByKey(const std::string & key, const DataObjectWrapper & data)
{
  typedef FeatureExtractionModel::InputImageType InputImageType;

  if(key == "InputImage")
    {
    InputImageType * image = dynamic_cast<InputImageType *>(data.GetDataObject());
    m_Model->SetInputImage(image);
    }
}

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
const DataObjectWrapper FeatureExtractionModule::RetrieveDataByKey(const std::string & key) const
{
  DataObjectWrapper wrapper;
  if(key == "OutputImage")
    {
    wrapper.Set("Floating_Point_Image",m_Model->GetOutputImage());
    }
  return wrapper;
}

/** The custom run command */
void FeatureExtractionModule::Run()
{
  m_View->Show();
  this->AddOutputDescriptor("Floating_Point_Image","OutputImage","Feature image extraction.");
}

} // End namespace otb

#endif
