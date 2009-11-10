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

#include "otbSupervisedClassificationModule.h"

namespace otb
{
/** Constructor */
SupervisedClassificationModule::SupervisedClassificationModule()
{
  // This module needs pipeline locking (because of visualization)
  this->NeedsPipelineLockingOn();

  m_SupervisedClassification = SupervisedClassificationAppli::New();

  m_SupervisedClassification->RegisterListener(this);

  // Describe inputs
  this->AddInputDescriptor<SupervisedClassificationAppli::ImageType>("InputImage", otbGetTextMacro("Image to apply Classification on"));

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

/** The custom run command */
void SupervisedClassificationModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  InputImageType::Pointer input = this->GetInputData<InputImageType>("InputImage");
  std::string desc = this->GetInputDataDescription<InputImageType>("InputImage",0);

  if(input.IsNotNull())
    {
      m_SupervisedClassification->SetImageFileName(desc);
    m_SupervisedClassification->SetInputImage(input);
    m_SupervisedClassification->Build();
    m_SupervisedClassification->Show();

    m_SupervisedClassification->LoadImage();

    // Check For SVMModel
    if(!m_Model.empty())
      {
      m_SupervisedClassification->SetModelFileName(m_Model);
      m_SupervisedClassification->LoadSVMModel();
      }

    }
  else
    {
    itkExceptionMacro(<<"InputImage is NULL");
    }
}

/** The notify */
void SupervisedClassificationModule::Notify()
{
  if(m_SupervisedClassification->GetHasOutput())
    {
      //this->ClearOutputDescriptors();
      this->EraseOutputByKey("OutputImage");
      this->AddOutputDescriptor(m_SupervisedClassification->GetOutput(),"OutputImage", otbGetTextMacro("Classified image"));
      this->NotifyOutputsChange();
      // Once module is closed, it is no longer busy
      this->BusyOff();
    }

  if(m_SupervisedClassification->GetHasOutputVector())
    {
      //this->ClearOutputDescriptors();
      for(unsigned int i=0; i<m_LabelsList.size(); i++)
       {
         this->EraseOutputByKey(m_LabelsList[i].c_str());
       }

      m_LabelsList.clear();
      if( m_SupervisedClassification->GetOutputVector().size() != m_SupervisedClassification->GetClassesMap().size() )
       {
         itkExceptionMacro(<<"Invalid outputs size");
       }

      unsigned int i = 0;
      ClassesMapType myMap = m_SupervisedClassification->GetClassesMap();
      for ( ClassesMapType::iterator it = myMap.begin();it!=myMap.end();++it)
       {
          itk::OStringStream oss;
          oss<<"OutputVector "<<(*it)->GetName();
          this->AddOutputDescriptor(m_SupervisedClassification->GetOutputVector()[i], oss.str().c_str(), otbGetTextMacro("Vectors of classified image"));
          m_LabelsList.push_back(oss.str());
          i++;
       }

      this->NotifyOutputsChange();
      // Once module is closed, it is no longer busy
      this->BusyOff();
    }
}

} // End namespace otb

