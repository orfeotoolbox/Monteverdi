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
#include "otbRasterizationModule.h"


#include "otbMsgReporter.h"


namespace otb
{
/** Constructor */
RasterizationModule::RasterizationModule()
{
       // First, do constructor stuffs

       // Then, describe inputs needed by the module

       // Add a vector and image input

       this->AddInputDescriptor<VectorDataType>("VectorData", otbGetTextMacro("Vector data to rasterize"));
       this->AddInputDescriptor<ImageType>("InputImage", otbGetTextMacro("Support Image"));


       m_InputImage=ImageType::New();
       m_InputVectorData=VectorDataType::New();
       m_VectorDataProjFilter = VectorDataProjectionFilterType::New();
       m_VectorDataProperties = VectorDataPropertiesType::New();
       m_VectorDataExtractROI = VectorDataExtractROIType::New();
       m_VectorDataRendering = VectorDataToImageFilterType::New();
       m_OutputImage=ImageType::New();
}

/** Destructor */
RasterizationModule::~RasterizationModule()
{}

/** PrintSelf method */
void RasterizationModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
       // Call superclass implementation
       Superclass::PrintSelf(os, indent);
}

void RasterizationModule::Run()
{
       // Here is the body of the module.
       // When the Run() method is called, necessary inputs have been
       // passed to the module.

       // First step is to retrieve the inputs

       // Image input
       m_InputImage = this->GetInputData<ImageType>("InputImage");


       if (m_InputImage.IsNotNull())
       {
              m_InputImage->UpdateOutputInformation();
       }
       else
       {
              itkExceptionMacro(<< "Input image is NULL.");
       }

       // Vector Data input
       m_InputVectorData= this->GetInputData<VectorDataType>("VectorData");


       if (m_InputVectorData.IsNotNull())
       {
              m_InputVectorData->UpdateOutputInformation();
       }
       else
       {
              itkExceptionMacro(<< "Vector data is NULL.");
       }

       // Reprojecting the VectorData
       std::string imageProjectionRef;
       imageProjectionRef = m_InputImage->GetProjectionRef();

       std::string vectorDataProjectionRef;
       vectorDataProjectionRef=m_InputVectorData->GetProjectionRef();

       m_VectorDataProjFilter->SetInput(m_InputVectorData);
       m_VectorDataProjFilter->SetInputProjectionRef(m_InputVectorData->GetProjectionRef());
       // set the projection ref with Image projection ref
       m_VectorDataProjFilter->SetOutputProjectionRef(imageProjectionRef);
       m_VectorDataProjFilter->SetOutputKeywordList(m_InputImage->GetImageKeywordlist());

       // image support information extraction //

       // Collecting Image support region

       SizeType size;
       size[0] = m_InputImage->GetLargestPossibleRegion().GetSize(0);
       size[1] = m_InputImage->GetLargestPossibleRegion().GetSize(1);

       PointType origin;
       origin=m_InputImage->GetOrigin();

       SpacingType spacing;
       spacing=m_InputImage->GetSpacing();


       SizePhyType  sizePhy;
       sizePhy[0] = size[0] * spacing[0];
       sizePhy[1] = size[1] * spacing[1];

       // extract ROI
       RemoteSensingRegionType   region;
       region.SetSize(sizePhy);
       region.SetOrigin(origin);
       region.SetRegionProjection(imageProjectionRef);
       region.SetKeywordList(m_InputImage->GetImageKeywordlist());

       m_VectorDataExtractROI->SetRegion(region);
       m_VectorDataExtractROI->SetInput(m_VectorDataProjFilter->GetOutput());
       // rendering
       m_VectorDataRendering->SetInput(m_VectorDataExtractROI->GetOutput());
       m_VectorDataRendering->SetSize(size);
       m_VectorDataRendering->SetOrigin(origin);
       m_VectorDataRendering->SetSpacing(spacing);
       m_VectorDataRendering->SetVectorDataProjectionWKT(imageProjectionRef);
       m_VectorDataRendering->SetRenderingStyleType(VectorDataToImageFilterType::Binary);

       // Output
       m_OutputImage=m_VectorDataRendering->GetOutput();
       m_OutputImage->SetMetaDataDictionary(m_InputImage->GetMetaDataDictionary());
       m_OutputImage->UpdateOutputInformation();

       // First, clear any previous output
       this->ClearOutputDescriptors();

       //this->AddOutputDescriptor(m_VectorDataRendering->GetOutput(), "OutputImage",
       this->AddOutputDescriptor(m_OutputImage, "OutputImage",
       otbGetTextMacro("Vector data rasterized"));

       this->NotifyOutputsChange();


}

} // End namespace otb
