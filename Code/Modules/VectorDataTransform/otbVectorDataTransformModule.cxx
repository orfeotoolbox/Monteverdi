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
#include "otbVectorDataTransformModule.h"

namespace otb
{
/** Constructor */
VectorDataTransformModule::VectorDataTransformModule()
{
  // This module needs pipeline locking
  //this->NeedsPipelineLockingOff();

  // First, do constructor stuffs
  m_View = ViewType::New();
  m_Model = ModelType::New();
  m_Controller = ControllerType::New();

  m_View->SetController(m_Controller);
  m_View->SetModel(m_Model);
  m_View->SetWidgetsController(m_Controller->GetWidgetsController());

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);

  m_Model->RegisterListener(this);

  // Instanciation of the Image To VectorImage Filter
  m_CastImageFilter = CastImageFilter::New();

  // Add a new input
  this->AddInputDescriptor<VectorImageType>("InputImage","The reference image");
  this->AddTypeToInputDescriptor<ImageType>("InputImage");
  this->AddInputDescriptor<VectorDataType>("InputVectorData","Vector Datas to transform");
}

/** Destructor */
VectorDataTransformModule::~VectorDataTransformModule()
{}

/** PrintSelf method */
void VectorDataTransformModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void VectorDataTransformModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();
  
  // Handle the input image and cast to VectorImage if necessary
  VectorImageType::Pointer fpvImage = this->GetInputData<VectorImageType>("InputImage");
  ImageType::Pointer singleImage = this->GetInputData<ImageType>("InputImage");
  
  if (!singleImage.IsNull() && fpvImage.IsNull())
    {
    m_CastImageFilter->SetInput(singleImage);
    m_CastImageFilter->UpdateOutputInformation();
    fpvImage = m_CastImageFilter->GetOutput();
    }

  // Get the vectorData
  VectorDataType::Pointer vd = this->GetInputData<VectorDataType>("InputVectorData");

  if(fpvImage.IsNotNull() && vd.IsNotNull())
    {
    // Process the input as an FloatingVectorImageType
    m_View->Build();
    m_Model->OpenInputs(fpvImage, vd);
    }
  else
    {
    itkExceptionMacro(<<"Input image or input vectorData is null");
    }
}

/** The Notify */
void VectorDataTransformModule::Notify(const std::string & event)
{
  if (event == "OutputsUpdated")
     {
     this->ClearOutputDescriptors();
     this->AddOutputDescriptor(m_Model->GetOutputVectorData(),"TransformedVectorData","The transformed VectorData");
     this->NotifyOutputsChange();
     }
   else if(event == "Quit")
     {
     // Once module is closed, it is no longer busy
     this->BusyOff();
     }
}
} // End namespace otb

