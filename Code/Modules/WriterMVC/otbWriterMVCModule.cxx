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
  m_Model      = WriterModel::New();
  m_View       = WriterViewGUI::New();
  m_Controller = WriterController::New();
  
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

/** The custom run command */
void WriterModule::Run()
{
  FPVImageType::Pointer vectorImage = this->GetInputData<FPVImageType>("InputDataSet");
  
  if(vectorImage.IsNotNull())
    {
    m_Model->SetInputImage(vectorImage);

/*
  this->BuildGUI();
  wFileChooserWindow->show();*/
    m_View->Show();
    m_Model->GenerateLayers();
    }
  else
    {
    itkExceptionMacro(<<"InputDataSet is NULL");
    }
}




} // End namespace otb

#endif
