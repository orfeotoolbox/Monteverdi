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
#include "otbConcatenateModule.h"

namespace otb
{
/** Constructor */
ConcatenateModule::ConcatenateModule()
{
  // First, do constructor stuffs

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<ImageType>("InputImage", "Image to concatenate", false, true);
  m_ImageListToVectorImageFilter = ImageListToVectorImageFilterType::New();
  m_ImageList = ImageListType::New();
}

/** Destructor */
ConcatenateModule::~ConcatenateModule()
{}

/** PrintSelf method */
void ConcatenateModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void ConcatenateModule::Run()
{
  // First step is to retrieve the inputs
  // Get the number of input image
  unsigned int numberOfInputsImages = this->GetNumberOfInputDataByKey("InputImage");
  if (numberOfInputsImages == 0)
    {
    itkExceptionMacro(<< "The number of inputs images is Null.");
    }
  // Init the list of inputs image
  for (unsigned int i = 0; i < numberOfInputsImages; i++)
    {
    ImageType::Pointer image = this->GetInputData<ImageType>("InputImage", i);
    if (image.IsNull())
      {
      itkExceptionMacro(<< "The input image number " << i << " is Null.");
      }
    image->UpdateOutputInformation();
    m_ImageList->PushBack(image);
    }

  // Wire filters
  m_ImageListToVectorImageFilter->SetInput(m_ImageList);

  // First, clear any previous output
  this->ClearOutputDescriptors();
  // Add an output (single version)
  this->AddOutputDescriptor(m_ImageListToVectorImageFilter->GetOutput(), "OutputImage",
                            "Image concatenated");
  // Last, when all outputs where declared, notify listeners
  this->NotifyOutputsChange();
}

} // End namespace otb
