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

#include "otbExtractROIModule.h"
#include "otbMsgReporter.h"
#include "itkImageRegion.h"

namespace otb
{
/** Constructor */
ExtractROIModule::ExtractROIModule()
{
  // Describe inputs
  this->AddInputDescriptor<FloatingImageType>("InputImage","Image to read.");
  this->AddTypeToInputDescriptor<FloatingVectorImageType>("InputImage");
  m_VectorImageExtractROIFilter = VectorImageExtractROIFilterType::New();
  m_ImageExtractROIFilter = ImageExtractROIFilterType::New();
  this->BuildGUI();
}

/** Destructor */
ExtractROIModule::~ExtractROIModule()
{}

/** PrintSelf method */
void ExtractROIModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}

/** The custom run command */
void ExtractROIModule::Run()
{
  FloatingImageType::Pointer image = this->GetInputData<FloatingImageType>("InputImage");
  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");


  itk::ImageRegion<2> imageRegion;

  if(!image.IsNull() && vectorImage.IsNull())
  {
      image->UpdateOutputInformation();
      imageRegion = image->GetLargestPossibleRegion();
  }
  else if(image.IsNull() && !vectorImage.IsNull())
  {
      vectorImage->UpdateOutputInformation();
      imageRegion = vectorImage->GetLargestPossibleRegion();
  }
  else
  {
    itkExceptionMacro("The image pointer is not initialized!!");
  }

  vInputImageSizeX->value(imageRegion.GetSize()[0]);
  vInputImageSizeY->value(imageRegion.GetSize()[1]);

  vStartX->minimum(static_cast<double>(imageRegion.GetIndex()[0]));
  vStartX->maximum(static_cast<double>(imageRegion.GetSize()[0]));
  vStartX->value(vStartX->minimum());
  vStartY->minimum(static_cast<double>(imageRegion.GetIndex()[1]));
  vStartY->maximum(static_cast<double>(imageRegion.GetSize()[1]));
  vStartY->value(vStartY->minimum());

  vSizeX->minimum(vStartX->minimum());
  vSizeX->maximum(vStartX->maximum());
  vSizeX->value(vSizeX->maximum());
  vSizeY->minimum(vStartY->minimum());
  vSizeY->maximum(vStartY->maximum());
  vSizeY->value(vSizeY->maximum());
  wExtractROIWindow->show();
}


/** Cancel */
void ExtractROIModule::Cancel()
{
    wExtractROIWindow->hide();
}
/** Ok */
void ExtractROIModule::Ok()
{
  try
    {
      FloatingImageType::Pointer image = this->GetInputData<FloatingImageType>("InputImage");
      FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");


      if(!image.IsNull() && vectorImage.IsNull())
      {
        // Get Input Image
        FloatingImageType::Pointer image = this->GetInputData<FloatingImageType>("InputImage");

        m_ImageExtractROIFilter->SetStartX(static_cast<unsigned long>(vStartX->value()));
        m_ImageExtractROIFilter->SetStartY(static_cast<unsigned long>(vStartY->value()));
        m_ImageExtractROIFilter->SetSizeX(static_cast<unsigned long>(vSizeX->value()));
        m_ImageExtractROIFilter->SetSizeY(static_cast<unsigned long>(vSizeY->value()));
        m_ImageExtractROIFilter->SetInput(image);
        this->ClearOutputDescriptors();
        this->AddOutputDescriptor(m_ImageExtractROIFilter->GetOutput(),"OutputImage","Image extracted." );
        this->NotifyOutputsChange();
      }
      else if(image.IsNull() && !vectorImage.IsNull())

      {
        // Get Input Vector Image
        FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");

        m_VectorImageExtractROIFilter->SetStartX(static_cast<unsigned long>(vStartX->value()));
        m_VectorImageExtractROIFilter->SetStartY(static_cast<unsigned long>(vStartY->value()));
        m_VectorImageExtractROIFilter->SetSizeX(static_cast<unsigned long>(vSizeX->value()));
        m_VectorImageExtractROIFilter->SetSizeY(static_cast<unsigned long>(vSizeY->value()));
        m_VectorImageExtractROIFilter->SetInput(vectorImage);
        this->ClearOutputDescriptors();
        this->AddOutputDescriptor(m_VectorImageExtractROIFilter->GetOutput(),"OutputImage","Image extracted." );
        this->NotifyOutputsChange();
      }

     wExtractROIWindow->hide();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


} // End namespace otb


