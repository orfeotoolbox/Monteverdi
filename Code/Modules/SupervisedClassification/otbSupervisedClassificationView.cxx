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

#include "otbSupervisedClassificationView.h"

#include "otbMsgReporter.h"

namespace otb
{

void
SupervisedClassificationView
::Quit()
{
  this->CleanUp();
}

void
SupervisedClassificationView
::HideAll()
{
  wMainWindow->hide();
  wSVMSetup->hide();
  wValidationWindow->hide();
  MsgReporter::GetInstance()->Hide();
}

void
SupervisedClassificationView
::Show()
{
  wMainWindow->show();
}


void
SupervisedClassificationView
::BuildInterface()
{
  if(!m_Controller)
    {
      itkExceptionMacro(<<"Controller is not set, can not build view.");
    }

  // Build the fltk code
  this->CreateGUI();

  //Create the buffer for the text display
  Fl_Text_Buffer* buffer = new Fl_Text_Buffer();
  tDescription->buffer(buffer);

  //Create the buffer for the confusion matrix display
  Fl_Text_Buffer* buffer2 = new Fl_Text_Buffer();
  tConfusionMatrix->buffer(buffer2);

  //Set the slider with the model value
  slRepartition->value(m_Controller->GetModel()->GetValidationTrainingProportion());
  slMaxTraining->value(m_Controller->GetModel()->GetMaxTrainingSize());
  slMaxValidation->value(m_Controller->GetModel()->GetMaxValidationSize());

  icClassKey->clear();

  // Show
  this->Show();
}

void
SupervisedClassificationView
::SVMSetup()
{
  //FIXME get the current values from the model?
  wSVMSetup->show();
}

void
SupervisedClassificationView
::SVMSetupOk()
{
  m_Controller->SVMSetupOk();
  wSVMSetup->hide();
}

void
SupervisedClassificationView
::SVMSetupCancel()
{
  wSVMSetup->hide();
}

void
SupervisedClassificationView
::Train()
{
  m_Controller->Train();
}

void
SupervisedClassificationView
::Validate()
{
  m_Controller->Validate();
  wValidationWindow->show();
}

void
SupervisedClassificationView
::Cancel()
{
  m_Controller->Quit();
  this->CleanUp();
}

void
SupervisedClassificationView
::Ok()
{
  m_Controller->Ok();
  this->CleanUp();
}

void
SupervisedClassificationView
::CleanUp()
{
  delete tDescription->buffer();
  delete tConfusionMatrix->buffer();
  this->HideAll();
}

void
SupervisedClassificationView
::SetClassKey(const char * key)
{
  m_Controller->SetClassKey(key);
}

void
SupervisedClassificationView
::Notify()
{
  slMaxTraining->maximum(m_Controller->GetModel()->GetClassMinSize());
  slMaxValidation->maximum(m_Controller->GetModel()->GetClassMinSize());

  tDescription->buffer()->text(m_Controller->GetModel()->GetDescription());
  tConfusionMatrix->buffer()->text(m_Controller->GetModel()->GetMatrixString());

  std::vector<std::string> classKeyList =  m_Controller->GetModel()->GetClassKeyList();
  for (unsigned int i = 0; i < classKeyList.size(); ++i)
  {
    icClassKey->add(classKeyList[i].c_str());
  }
  icClassKey->value(m_Controller->GetModel()->GetClassKey());
}

}
