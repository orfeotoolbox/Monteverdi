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

#include "otbForwardSensorModel.h"

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
  m_Transform        = TransformType::New();
  m_InverseTransform = TransformType::New();
  this->BuildGUI();
}

/** Destructor */
ExtractROIModule::~ExtractROIModule()
{
    this->Cancel();
}

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

  typedef ForwardSensorModel<double>                ForwardSensorType;
  
  roundLongLat->activate();
  
  if(!image.IsNull() && vectorImage.IsNull())
  {
      image->UpdateOutputInformation();
      imageRegion = image->GetLargestPossibleRegion();
      
      try
      {
        const itk::MetaDataDictionary & inputDict = image->GetMetaDataDictionary();
        m_Transform->SetInputDictionary(inputDict);
        m_Transform->SetInputOrigin(image->GetOrigin());
        m_Transform->SetInputSpacing(image->GetSpacing());
        m_Transform->InstanciateTransform();
        
        if (m_Transform->GetTransformAccuracy() == Projection::UNKNOWN)
        {
          roundLongLat->deactivate();
        }
      }
      catch ( itk::ExceptionObject &  e)
      {
        roundLongLat->deactivate();
      }
  }
  else if(image.IsNull() && !vectorImage.IsNull())
  {
      vectorImage->UpdateOutputInformation();
      imageRegion = vectorImage->GetLargestPossibleRegion();
      
      try
      {
        const itk::MetaDataDictionary & inputDict = vectorImage->GetMetaDataDictionary();
        m_Transform->SetInputDictionary(inputDict);
        m_Transform->SetInputOrigin(vectorImage->GetOrigin());
        m_Transform->SetInputSpacing(vectorImage->GetSpacing());
        m_Transform->InstanciateTransform();
        
        if (m_Transform->GetTransformAccuracy() == Projection::UNKNOWN)
        {
          roundLongLat->deactivate();
        }
      }
      catch ( itk::ExceptionObject &e)
      {
        roundLongLat->deactivate();
      }
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


void ExtractROIModule::LongLatSelection()
{
//   std::cout << "toto" << std::endl;
  if ( roundLongLat->value() == 1 )
  {
    guiPixel->hide();
    guiLongLat->show();
  }
  else
  {
    guiPixel->show();
    guiLongLat->hide();
  }  
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

      IndexType idxInit;
      OffsetType offSize;
      if(!image.IsNull() && vectorImage.IsNull())
      {
        // Get input image
        FloatingImageType::Pointer image = this->GetInputData<FloatingImageType>("InputImage");

        //TODO Add case of long/lat input
        if ( roundLongLat->value() == 1 )
        {
          OutputPointType pt1;
          OutputPointType pt2;
          
          pt1[0] = static_cast<InternalPixelType>( vLong1->value() );
          pt1[1] = static_cast<InternalPixelType>( vLatitude1->value() );
          pt2[0] = static_cast<InternalPixelType>( vLong2->value() );
          pt2[1] = static_cast<InternalPixelType>( vLatitude2->value() );
          
          OutputPointType pto1;
          OutputPointType pto2;
          m_Transform->GetInverse(m_InverseTransform);
          pto1 = m_InverseTransform->TransformPoint(pt1);
          pto2 = m_InverseTransform->TransformPoint(pt2);
          
          IndexType index1;
          IndexType index2;
                    
          image->TransformPhysicalPointToIndex(pto1, index1);
          image->TransformPhysicalPointToIndex(pto2, index2);
          
          idxInit = index1;
          offSize = index2 - index1;
          
        }
        else
        {
          idxInit[0] = static_cast<unsigned long>(vStartX->value());
          idxInit[1] = static_cast<unsigned long>(vStartY->value()); 
           
          offSize[0] = static_cast<unsigned long>(vSizeX->value());
          offSize[1] = static_cast<unsigned long>(vSizeY->value()); 
        }
        
        m_ImageExtractROIFilter->SetStartX(idxInit[0]);
        m_ImageExtractROIFilter->SetStartY(idxInit[1]);
        m_ImageExtractROIFilter->SetSizeX(offSize[0]);
        m_ImageExtractROIFilter->SetSizeY(offSize[1]);
        m_ImageExtractROIFilter->SetInput(image);
        this->ClearOutputDescriptors();
        this->AddOutputDescriptor(m_ImageExtractROIFilter->GetOutput(),"OutputImage","Image extracted." );
        this->NotifyOutputsChange();
      }
      else if(image.IsNull() && !vectorImage.IsNull())

      {
        // Get Input Vector Image
        FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");
        
        //TODO Add case of long/lat input
        if ( roundLongLat->value() == 1 )
        {
          OutputPointType pt1;
          OutputPointType pt2;
          
          pt1[0] = static_cast<InternalPixelType>( vLong1->value() );
          pt1[1] = static_cast<InternalPixelType>( vLatitude1->value() );
          pt2[0] = static_cast<InternalPixelType>( vLong2->value() );
          pt2[1] = static_cast<InternalPixelType>( vLatitude2->value() );
          
          OutputPointType pto1;
          OutputPointType pto2;
          m_Transform->GetInverse(m_InverseTransform);
          pto1 = m_InverseTransform->TransformPoint(pt1);
          pto2 = m_InverseTransform->TransformPoint(pt2);
          
          IndexType index1;
          IndexType index2;
                    
          vectorImage->TransformPhysicalPointToIndex(pto1, index1);
          vectorImage->TransformPhysicalPointToIndex(pto2, index2);
          
          idxInit = index1;
          offSize = index2 - index1;
        }
        else
        {
          idxInit[0] = static_cast<unsigned long>(vStartX->value());
          idxInit[1] = static_cast<unsigned long>(vStartY->value()); 
           
          offSize[0] = static_cast<unsigned long>(vSizeX->value());
          offSize[1] = static_cast<unsigned long>(vSizeY->value()); 
        }
        
        m_VectorImageExtractROIFilter->SetStartX(idxInit[0]);
        m_VectorImageExtractROIFilter->SetStartY(idxInit[1]);
        m_VectorImageExtractROIFilter->SetSizeX(offSize[0]);
        m_VectorImageExtractROIFilter->SetSizeY(offSize[1]);
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


