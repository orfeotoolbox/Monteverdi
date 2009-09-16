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


#include "otbSpeckleFilteringModel.h"

namespace otb
{


/** Initialize the singleton */
SpeckleFilteringModel::Pointer SpeckleFilteringModel::Instance = NULL;

/**
 * Constructor
 */
SpeckleFilteringModel::SpeckleFilteringModel()
{
  m_InputImage  = InputImageType::New();
  m_LeeFilter   = LeeFilterType::New();
  m_FrostFilter = FrostFilterType::New();
}

/**
 * Destructor
 */
SpeckleFilteringModel::
~SpeckleFilteringModel(){}


/** Manage the singleton */
SpeckleFilteringModel::Pointer SpeckleFilteringModel::GetInstance()
{
  if (!Instance)
  {
    Instance = SpeckleFilteringModel::New();
  }
  return Instance;
}

/**
 * Apply Lee to the inputImage
 */
void
SpeckleFilteringModel
::LeeFiltering(unsigned int radius)
{
  std::cout <<"Model : Begin Computation Lee " << std::endl;
  LeeFilterType::SizeType     lradius;
  lradius.Fill(radius);
  m_LeeFilter->SetInput(m_InputImage);
  m_LeeFilter->SetRadius(lradius);
  std::cout <<"Model : Computation Lee Done ... " << std::endl;
  m_Output = m_LeeFilter->GetOutput();
}

/**
 * ApplyLee to the inputImage
 */
void
SpeckleFilteringModel
::FrostFiltering(unsigned int radius, double deRamp)
{
  FrostFilterType::SizeType     fradius;
  fradius.Fill(radius);

  m_FrostFilter->SetInput(m_InputImage);
  m_FrostFilter->SetRadius(fradius);
  m_FrostFilter->SetDeramp(deRamp);
  m_Output = m_FrostFilter->GetOutput();
}

void
SpeckleFilteringModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}


}// End namespac
