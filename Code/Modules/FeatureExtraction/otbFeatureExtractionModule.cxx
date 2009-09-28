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
  m_Model      = FeatureExtractionModel::New();
  m_View       = FeatureExtractionViewGUI::New();
  m_Controller = FeatureExtractionController::New();

  m_View->SetFeatureExtractionModel(m_Model);
  m_View->InitVisu();
  m_View->SetFeatureExtractionController(m_Controller);

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);


  // Describe inputs
  this->AddInputDescriptor<FeatureExtractionModel::InputImageType>("InputImage","Image to apply feature extraction.");

//   this->AddInputDescriptor<FeatureExtractionModel::InputImageType>("InputImage2","Optionnal image to apply feature extraction.",true,false);
//   
//   this->AddInputDescriptor<FeatureExtractionModel::InputImageType>("InputImage3","Multiple images to apply feature extraction.",false,true);
//   
//   this->AddInputDescriptor<FeatureExtractionModel::InputImageType>("InputImage4","Image to apply feature extraction.",true,true);

  // the FeatureExtractionModel registers its module
  m_Model->RegisterListener(this);
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
void FeatureExtractionModule::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  typedef FeatureExtractionModel::InputImageType InputImageType;

  if(key == "InputImage")
    {
    InputImageType * image = dynamic_cast<InputImageType *>(data.GetDataObject());
//     std::cout<<"set model.."<<std::endl;
    m_Model->SetInputImage(image);
    }
}

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
const DataObjectWrapper FeatureExtractionModule::RetrieveOutputByKey(const std::string & key) const
{
  DataObjectWrapper wrapper;
  if(key == "OutputImage")
    {
    wrapper.Set(m_Model->GetOutputImage());
    }
  return wrapper;
}

/** The custom run command */
void FeatureExtractionModule::Run()
{
  m_View->Show();
  m_Model->GenerateLayers();
  
  std::cout << "end of Feature GUI"<< std::endl;
  //Output descriptor
  this->AddOutputDescriptor<FeatureExtractionModel::InputImageType>("OutputImage","Feature image extraction.");
  
  // Notify all listener
  this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
}

/** The Notify */
void FeatureExtractionModule::Notify()
{
  if (m_Model->GetHasChanged())
  {
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("OutputsUpdated",m_InstanceId));
  }
}

} // End namespace otb

#endif
