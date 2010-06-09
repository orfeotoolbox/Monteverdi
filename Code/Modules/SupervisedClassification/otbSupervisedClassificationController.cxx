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
    m_Model->SetValidationTrainingProportion(m_View->slRepartition->value());
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
  m_Model->GetModelEstimator()->SetKernelType(m_View->svmKernelType->value());
  m_Model->GetModelEstimator()->SetSVMType(m_View->svmSVMType->value());
  m_Model->GetModelEstimator()->SetPolynomialKernelDegree(
                                           m_View->svmKernelDegree->value());
  m_Model->GetModelEstimator()->SetCacheSize(m_View->svmCacheSize->value());
  m_Model->GetModelEstimator()->DoProbabilityEstimates(m_View->svmPE->value());
  m_Model->GetModelEstimator()->SetKernelGamma(m_View->svmGamma->value());
  m_Model->GetModelEstimator()->SetNu(m_View->svmNu->value());
  m_Model->GetModelEstimator()->SetKernelCoef0(m_View->svmCoef0->value());
  m_Model->GetModelEstimator()->SetEpsilon(m_View->svmEpsilon->value());
  m_Model->GetModelEstimator()->SetP(m_View->svmP->value());
    //FIXME wire up all the rest
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    return;
  }
}

}//end namespace otb
