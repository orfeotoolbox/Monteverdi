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
WriterMVCModule::WriterMVCModule()
{
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
  //this->AddInputDescriptor<FloatingVectorImageType>("InputDataSet","Dataset to write.");
  this->AddInputDescriptor<FloatingVectorImageType>("InputDataSet","Dataset to write.");
//   this->AddTypeToInputDescriptor<VectorType>("InputDataSet");
  
  m_Model->RegisterListener(this);
}

/** Destructor */
WriterMVCModule::~WriterMVCModule()
{}

/** PrintSelf method */
void WriterMVCModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void WriterMVCModule::Run()
{
  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputDataSet");
//   VectorType::Pointer vectorData = this->GetInputData<VectorType>("InputDataSet");
  
  if(vectorImage.IsNotNull())
  {
    m_Model->SetInputImage(vectorImage);
    m_View->Show();
    m_Model->GenerateLayers();
  }
//   else if (vectorData.IsNotNull())
//   {
//     //TODO write vectordata
//     m_Model->GetVectorWriter()->SetInput(vectorData);
//   }
  else
  {
    itkExceptionMacro(<<"Input image is NULL.");
  }
}

/** The Notify */
void WriterMVCModule::Notify()
{
  if (m_Model->GetHasChanged())
  {
    // Send an event to Monteverdi application
    this->NotifyAll(MonteverdiEvent("Dataset written",m_InstanceId));
  }
}

} // End namespace otb

#endif
