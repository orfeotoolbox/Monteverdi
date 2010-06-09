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

  //Set the slider with the model value
  slRepartition->value(m_Controller->GetModel()->GetValidationTrainingProportion());

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
  this->CleanUp();
}

void
SupervisedClassificationView
::Ok()
{
  this->CleanUp();
}

void
SupervisedClassificationView
::CleanUp()
{
  m_Controller->Ok();
  free(tDescription->buffer());
  this->HideAll();
}

void
SupervisedClassificationView
::Notify()
{
  tDescription->buffer()->text(m_Controller->GetModel()->GetDescription());
}

}
