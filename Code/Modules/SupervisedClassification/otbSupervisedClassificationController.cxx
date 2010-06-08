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

#include "otbSupervisedClassificationController.h"

#include "otbMsgReporter.h"

namespace otb
{

void SupervisedClassificationController::Train()
{
  try
  {
    m_Model->Train();
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
  }
}

void SupervisedClassificationController::Validate()
{
  try
  {
    m_Model->Validate();
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
  }
}

void SupervisedClassificationController::Quit()
{
  try
  {
    m_Model->Quit();
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
  }
}

void SupervisedClassificationController::SVMSetupOk()
{
  try
  {
  m_Model->GetModelEstimator()->SetC(m_View->svmC->value());
    //FIXME wire up all the rest
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
  }
}

}//end namespace otb
