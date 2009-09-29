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
#ifndef __otbWriterMVCModule_cxx
#define __otbWriterMVCModule_cxx

#include "otbWriterMVCModule.h"


namespace otb
{
/** Constructor */
WriterModule::WriterModule()
{
//   m_FPVWriter = FPVWriterType::New();
//   m_VectorWriter = VectorWriterType::New();
  
  //Build MVC
  m_View       = WriterViewGUI::New();
  m_Controller = WriterController::New();
  m_Model      = WriterModel::New();
  m_Controller->SetView(m_View);
  m_View->SetWriterController(m_Controller);
  
  m_View->SetWriterModel(m_Model);
  m_View->InitVisu();
  m_View->SetWriterController(m_Controller);

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);
   // Describe inputs
  this->AddInputDescriptor<FPVImageType>("InputDataSet","Dataset to write.");
  this->AddTypeToInputDescriptor<VectorType>("InputDataSet");

}

/** Destructor */
WriterModule::~WriterModule()
{}

/** PrintSelf method */
void WriterModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** Assign input by key. This method must be reimplemented in subclasses.
 *  When this method is called, key checking and data type matching
 *  is already done. */
void WriterModule::AssignInputByKey(const std::string & key, const DataObjectWrapper & data)
{
  const Superclass::InputDataDescriptorMapType inMap = this->GetInputsMap();
  if(key == "InputDataSet")
  {
    FPVImageType * image = dynamic_cast<FPVImageType *>(data.GetDataObject());
//     m_FPVWriter->SetInput(image);
    m_Model->SetInputImage(image);
  }
//   else if (key == "InputVectorDataSet")
//   {
  // COMMENTED OUT BECAUSE UNUSED VectorType * vector = dynamic_cast<VectorType *>(data.GetDataObject());
//     m_VectorWriter->SetInput(vector);  
    //TODO manage vector data in the model
//   }
}

/** The custom run command */
void WriterModule::Run()
{/*
  this->BuildGUI();
  wFileChooserWindow->show();*/
  m_View->Show();
  m_Model->GenerateLayers();
  std::cout << "end of Feature GUI"<< std::endl;
}




} // End namespace otb

#endif
