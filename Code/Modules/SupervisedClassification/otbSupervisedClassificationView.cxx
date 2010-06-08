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

}
