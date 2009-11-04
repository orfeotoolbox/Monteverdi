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
        ForwardSensorType::Pointer sensor = ForwardSensorType::New();
        sensor->SetImageGeometry( image->GetImageKeywordlist() );
      }
      catch ( itk::ExceptionObject &  )
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
        ForwardSensorType::Pointer sensor = ForwardSensorType::New();
        sensor->SetImageGeometry( vectorImage->GetImageKeywordlist() );
      }
      catch ( itk::ExceptionObject &  )
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

/** Test if user can select lat/long selection*/
/*
bool ExtractROIModule::HaveKeyWordList()
{
  try
  {
    ForwardSensorType::Pointer sensor = ForwardSensorType::New();
    sensor->SetImageGeometry( image->GetImageKeywordlist() );
  }
  catch ( itk::ExceptionObject &  )
  {
    fl_alert("Invalid image : No ImageKeywordlist found");
    return;
  }
}
*/
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
          InverseSensorInputPointType pt1;
          InverseSensorInputPointType pt2;
          pt1[0] = static_cast<InternalPixelType>( vLong1->value() );
          pt1[1] = static_cast<InternalPixelType>( vLatitude1->value() );
          pt2[0] = static_cast<InternalPixelType>( vLong2->value() );
          pt2[1] = static_cast<InternalPixelType>( vLatitude2->value() );

          InverseSensorOutputPointType pto1;
          InverseSensorOutputPointType pto2;
          
          InverseSensorType::Pointer sensor = InverseSensorType::New();
          sensor->SetImageGeometry( image->GetImageKeywordlist() );
          
          pto1 = sensor->TransformPoint (pt1);
          pto2 = sensor->TransformPoint (pt2);
          
          IndexType index1;
          IndexType index2;
          
          image->TransformPhysicalPointToIndex(pto1, index1);
          image->TransformPhysicalPointToIndex(pto2, index2);
          
          idxInit = index1;
          offSize = index2 - index1;
          
          std::cout << "pt1"<<  pt1 << std::endl;
          std::cout << "pt2"<<  pt2 << std::endl;
          std::cout << "pto1"<<  pto1 << std::endl;
          std::cout << "pto2"<<  pto2 << std::endl;
          std::cout << "index1"<<  index1 << std::endl;
          std::cout << "index2"<<  index2 << std::endl;
          std::cout << "idxInit"<<  idxInit << std::endl;
          std::cout << "offSize"<<  offSize << std::endl;
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
          InverseSensorInputPointType pt1;
          InverseSensorInputPointType pt2;
          pt1[0] = static_cast<InternalPixelType>( vLong1->value() );
          pt1[1] = static_cast<InternalPixelType>( vLatitude1->value() );
          pt2[0] = static_cast<InternalPixelType>( vLong2->value() );
          pt2[1] = static_cast<InternalPixelType>( vLatitude2->value() );

          InverseSensorOutputPointType pto1;
          InverseSensorOutputPointType pto2;
          
          InverseSensorType::Pointer sensor = InverseSensorType::New();
          sensor->SetImageGeometry( vectorImage->GetImageKeywordlist() );
          
          pto1 = sensor->TransformPoint (pt1);
          pto2 = sensor->TransformPoint (pt2);
          
          IndexType index1;
          IndexType index2;
          
          vectorImage->TransformPhysicalPointToIndex(pto1, index1);
          vectorImage->TransformPhysicalPointToIndex(pto2, index2);
          
          idxInit = index1;
          offSize = index2 - index1;
          
          std::cout << "pt1"<<  pt1 << std::endl;
          std::cout << "pt2"<<  pt2 << std::endl;
          std::cout << "pto1"<<  pto1 << std::endl;
          std::cout << "pto2"<<  pto2 << std::endl;
          std::cout << "index1"<<  index1 << std::endl;
          std::cout << "index2"<<  index2 << std::endl;
          std::cout << "idxInit"<<  idxInit << std::endl;
          std::cout << "offSize"<<  offSize << std::endl;
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


