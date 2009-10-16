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
#ifndef __otbPointSetRegister_txx
#define __otbPointSetRegister_txx

#include "otbPointSetRegister.h"



namespace otb
{
template <class TTransform>
PointSetRegister<TTransform>
::PointSetRegister() : m_FixPointSet(), m_MovingPointSet(), m_Transform(), m_TransformParameters()
{
  m_FixPointSet = PointSetType::New();
  m_MovingPointSet = PointSetType::New();
  m_Transform = TransformationType::New();
}


template <class TTransform>
void
PointSetRegister<TTransform>
::ComputeTransform()
{
  if(m_FixPointSet->GetNumberOfPoints() == 0 || m_MovingPointSet->GetNumberOfPoints() == 0)
    {
      itkExceptionMacro(<<"Input set is empty");
    }
  if(m_FixPointSet->GetNumberOfPoints() != m_MovingPointSet->GetNumberOfPoints())
    {
      itkExceptionMacro(<<"Input point sets set size differs.");
    }
  
  MetricType::Pointer metric = MetricType::New();
  TransformationPointerType invTransform = TransformationType::New();
  // Optimizer Type
  OptimizerType::Pointer optimizer = OptimizerType::New();
  optimizer->SetUseCostFunctionGradient(false);
  invTransform->SetIdentity();
  RegistrationType::Pointer registration = RegistrationType::New();
  // Scale the translation components of the Transform in the Optimizer
  
  unsigned long numberOfIterations = 2000;
  double gradientTolerance = 1e-5; // convergence criterion
  double valueTolerance = 1e-5; // convergence criterion
  double epsilonFunction = 1e-6; // convergence criterion
  optimizer->SetScales( m_Scales );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetValueTolerance( valueTolerance );
  optimizer->SetGradientTolerance( gradientTolerance );
  optimizer->SetEpsilonFunction( epsilonFunction );

  registration->SetInitialTransformParameters( invTransform->GetParameters() );

  //------------------------------------------------------
  // Connect all the components required for Registration
  //------------------------------------------------------
  registration->SetMetric( metric );
  registration->SetOptimizer( optimizer );
  registration->SetTransform( invTransform );
  registration->SetFixedPointSet( m_FixPointSet ); 
  registration->SetMovingPointSet( m_MovingPointSet );

  registration->StartRegistration();

 invTransform->GetInverse( m_Transform );

 m_TransformParameters = m_Transform->GetParameters();
}


}// namespace otb

#endif
