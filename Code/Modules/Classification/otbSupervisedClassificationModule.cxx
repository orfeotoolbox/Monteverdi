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
#ifndef __otbSupervisedClassificationModule_cxx
#define __otbSupervisedClassificationModule_cxx

#include "otbSupervisedClassificationModule.h"

namespace otb
{
/** Constructor */
SupervisedClassificationModule::SupervisedClassificationModule()
{
  m_SupervisedClassification = SupervisedClassificationAppli::New();

  // Describe inputs
  this->AddInputDescriptor("Floating_Point_VectorImage","InputImage","Image to apply Classification on.");
}

/** Destructor */
SupervisedClassificationModule::~SupervisedClassificationModule()
{}

/** PrintSelf method */
void SupervisedClassificationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** Assign input by key. This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void SupervisedClassificationModule::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  typedef SupervisedClassificationAppli::ImageType InputImageType;

  if(key == "InputImage")
    {
    InputImageType * image = dynamic_cast<InputImageType *>(data.GetDataObject());
    m_SupervisedClassification->SetInputImage(image);
    }
}

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
const DataObjectWrapper SupervisedClassificationModule::RetrieveOutputByKey(const std::string & key) const
{
  DataObjectWrapper wrapper;
  if(key == "OutputImage")
    {
      wrapper.Set("UI_Point_VectorImage",m_SupervisedClassification->GetOutput());
    }
  return wrapper;
}

/** The custom run command */
void SupervisedClassificationModule::Run()
{
  m_SupervisedClassification->Build();
  m_SupervisedClassification->Show();
  
  m_SupervisedClassification->LoadImage();
  
  // Check For SVMModel
  if(!m_Model.empty())
    {
      m_SupervisedClassification->SetModelFileName(m_Model);
      m_SupervisedClassification->LoadSVMModel();
    }
  this->AddOutputDescriptor("UI_Point_VectorImage","OutputImage","SupervisedClassification image.");
}

} // End namespace otb

#endif
