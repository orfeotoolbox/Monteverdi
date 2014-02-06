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
#include "otbBayesianFusionModule.h"

namespace otb
{
/** Constructor */
BayesianFusionModule::BayesianFusionModule()
{
  // First, do constructor stuffs

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<FloatingImageType>("PanImage", "Panchromatic image");
  this->AddInputDescriptor<FloatingVectorImageType>("ZoomXsImage", "XS image superimposed to panchromatic image with cubic resampling");
  this->AddInputDescriptor<FloatingVectorImageType>("XsImage", "XS image superimposed to panchromatic image with nearest neighbors resampling");

  m_BayesianFusionFilter = BayesianFusionFilterType::New();

}

/** Destructor */
BayesianFusionModule::~BayesianFusionModule()
{}

/** PrintSelf method */
void BayesianFusionModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void BayesianFusionModule::Run()
{
  // Here is the body of the module.
  // When the Run() method is called, necessary inputs have been
  // passed to the module.

  // First step is to retrieve the inputs
  FloatingImageType::Pointer       panImage = this->GetInputData<FloatingImageType>("PanImage");
  FloatingVectorImageType::Pointer zoomXsImage = this->GetInputData<FloatingVectorImageType>("ZoomXsImage");
  FloatingVectorImageType::Pointer xsImage = this->GetInputData<FloatingVectorImageType>("XsImage");


  // Once all inputs have been properly retrieved, do what the module
  // should do : show a gui, start an MVC model, trigger processing...

  m_BayesianFusionFilter->SetPanchro(panImage);
  m_BayesianFusionFilter->SetMultiSpect(xsImage);
  m_BayesianFusionFilter->SetMultiSpectInterp(zoomXsImage);

  // Then, when the module did actually produce some outputs, declare
  // them.

  // First, clear any previous output
  this->ClearOutputDescriptors();

  // Add an output (single version)
  this->AddOutputDescriptor(m_BayesianFusionFilter->GetOutput(), "PanSharpenedImageOutput",
                            "Pansharpened image");

  // Last, when all outputs where declared, notify listeners
  this->NotifyOutputsChange();
}
} // End namespace otb
