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

#include "otbPolarimetricSynthesisModule.h"

namespace otb
{
/** Constructor */
PolarimetricSynthesisModule::PolarimetricSynthesisModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOff();

  realToComplexFilter.resize(4);
  imaginaryToComplexFilter.resize(4);
  realAndImaginaryToComplexFilter.resize(4);
  for(unsigned int i = 0 ; i < 4 ; ++i)
    {
      realToComplexFilter[i] = RealImageToComplexImageFilterType::New();
      imaginaryToComplexFilter[i] = ImaginaryImageToComplexImageFilterType::New();
      realAndImaginaryToComplexFilter[i] = RealAndImaginaryImageToComplexImageFilterType::New();
    }

  // First, do constructor stuffs
  m_Controller = ControllerType::New();
  m_View = ViewType::New();
  m_Model = ModelType::New();

  m_View->SetController(m_Controller);
  m_View->SetModel(m_Model);
  m_View->SetWidgetsController(m_Controller->GetWidgetsController());

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);

  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module
  this->AddInputDescriptor<ImageType>("InputImageHHReal","HH Image (Real Part)",true,false);
  this->AddInputDescriptor<ImageType>("InputImageHHImag","HH Image (Imaginary Part)",true,false);
  this->AddInputDescriptor<ImageType>("InputImageHVReal","HV Image (Real Part)",true,false);
  this->AddInputDescriptor<ImageType>("InputImageHVImag","HV Image (Imaginary Part)",true,false);
  this->AddInputDescriptor<ImageType>("InputImageVHReal","VH Image (Real Part)",true,false);
  this->AddInputDescriptor<ImageType>("InputImageVHImag","VH Image (Imaginary Part)",true,false);
  this->AddInputDescriptor<ImageType>("InputImageVVReal","VV Image (Real Part)",true,false);
  this->AddInputDescriptor<ImageType>("InputImageVVImag","VV Image (Imaginary Part)",true,false);
}

/** Destructor */
PolarimetricSynthesisModule::~PolarimetricSynthesisModule()
{}

/** PrintSelf method */
void PolarimetricSynthesisModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}
 /** Combine real and imaginary image to create a complex image*/
PolarimetricSynthesisModule::ComplexImageType::Pointer
PolarimetricSynthesisModule::CombineData(unsigned int i,
                                         ImageType::Pointer & realImage,
                                         ImageType::Pointer & imagImage)
{
  if(realImage.IsNotNull() && imagImage.IsNotNull())
   {
      realAndImaginaryToComplexFilter[i]->SetInputRealPart(realImage);
      realAndImaginaryToComplexFilter[i]->SetInputRealPart(imagImage);
      return realAndImaginaryToComplexFilter[i]->GetOutput();
   }
 else
   {
     if(realImage.IsNotNull())
       {
         realToComplexFilter[i]->SetInput(realImage);
         return realToComplexFilter[i]->GetOutput();
       }
     else
       {
       if(imagImage.IsNotNull())
         {
           imaginaryToComplexFilter[i]->SetInput(imagImage);
           return imaginaryToComplexFilter[i]->GetOutput();
         }
       else
         {
           return NULL;
         }
       }
   }
}


/** The custom run command */
void PolarimetricSynthesisModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();

  ImageType::Pointer hhRealImage = this->GetInputData<ImageType> ("InputImageHHReal");
  ImageType::Pointer hhImagImage = this->GetInputData<ImageType> ("InputImageHHImag");
  ImageType::Pointer hvRealImage = this->GetInputData<ImageType> ("InputImageHVReal");
  ImageType::Pointer hvImagImage = this->GetInputData<ImageType> ("InputImageHVImag");
  ImageType::Pointer vhRealImage = this->GetInputData<ImageType> ("InputImageVHReal");
  ImageType::Pointer vhImagImage = this->GetInputData<ImageType> ("InputImageVHImag");
  ImageType::Pointer vvRealImage = this->GetInputData<ImageType> ("InputImageVVReal");
  ImageType::Pointer vvImagImage = this->GetInputData<ImageType> ("InputImageVVImag");

  ComplexImageType::Pointer hhImage;
  ComplexImageType::Pointer hvImage;
  ComplexImageType::Pointer vhImage;
  ComplexImageType::Pointer vvImage;

  hhImage = CombineData(0,hhRealImage,hhImagImage);
  hvImage = CombineData(1,hvRealImage,hvImagImage);
  vhImage = CombineData(2,vhRealImage,vhImagImage);
  vvImage = CombineData(3,vvRealImage,vvImagImage);

  if( hhImage.IsNull() && hvImage.IsNull() &&
      vhImage.IsNull() && vvImage.IsNull() )
    {
      itkExceptionMacro(<<"Need at least one input image ");
    }

  if(hhImage.IsNotNull() || hvImage.IsNotNull())
    {
      m_Model->SetHEmissionMode(true);
      if(hhImage.IsNull() || hvImage.IsNull())
        {
        itkExceptionMacro(<<"Need at least two inputs images: hh and hv");
        }
    }
  if(vhImage.IsNotNull() || vvImage.IsNotNull())
    {
      m_Model->SetVEmissionMode(true);
      if(vhImage.IsNull() || vvImage.IsNull())
        {
        itkExceptionMacro(<<"Need at least two inputs images: vh and vv");
        }
    }

  m_Model->SetImageHH(hhImage);
  m_Model->SetImageHV(hvImage);
  m_Model->SetImageVH(vhImage);
  m_Model->SetImageVV(vvImage);

  m_View->Build();
  m_Controller->LoadImages();
}

/** The Notify */
void PolarimetricSynthesisModule::Notify(const std::string & event)
{
  if (event == "OutputsUpdated")
    {
    this->ClearOutputDescriptors();
    if( m_Model->GetRGB() )
      {
      this->AddOutputDescriptor(m_Model->GetOutputVectorImage(),"PolarimetricSynthesisVectorImage","The synthetized images");
      }
    else
      {
      this->AddOutputDescriptor(m_Model->GetOutputImage(),"PolarimetricSynthesisImage","The synthetized images");
      }
    this->NotifyOutputsChange();
    }
  else if(event == "Quit")
    {
    // Once module is closed, it is no longer busy
    this->BusyOff();
    }
}
} // End namespace otb

