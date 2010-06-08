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
//  m_Controller->OK();
  this->HideAll();
}

void
SupervisedClassificationView
::HideAll()
{
  wMainWindow->hide();
  wSVMSetup->hide();
  wValidationWindow->hide();
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
  MsgReporter::GetInstance()->Hide();
  wMainWindow->hide();
}

}
