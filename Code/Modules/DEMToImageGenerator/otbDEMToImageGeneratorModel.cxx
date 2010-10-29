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
#include "otbDEMToImageGeneratorModel.h"
#include <ogr_spatialref.h>
#include "otbMapProjection.h"
#include <string>
#include "otbImageFileReader.h"

namespace otb
{

/**
 * Constructor
 */
DEMToImageGeneratorModel::DEMToImageGeneratorModel()
{
  m_OutputChanged         = false;
  m_TransformChanged      = false;
  m_TempTransformChanged  = false;
  m_UseImageFileName      = false;
  m_EstimateInputRPCModel = false;
  m_SRTMDEM               = false;
  m_UseInputImage         = false;

  m_InputImage = InputImageType::New();

  m_Transform        = TransformType::New();
  m_InverseTransform = TransformType::New();

  m_DEMToImageGenerator = DEMToImageGeneratorType::New();
}

/**
 * Destructor
 */
DEMToImageGeneratorModel::
~DEMToImageGeneratorModel()
{}


/**
 * Compute the new Origin, Spacing & Size of the output
 */
void DEMToImageGeneratorModel
::UpdateOutputParameters()
{
  m_DEMToImageGenerator->SetOutputParametersFromImage(m_InputImage.GetPointer());
}

/**
 *
 */
void
DEMToImageGeneratorModel
::ProjectRegion(unsigned int sizeX,
                unsigned int sizeY,
                double spacingX,
                double spacingY,
                double originX,
                double originY,
                bool isUl)
{
  // Edit the size
  m_OutputSize[0] = sizeX;
  m_OutputSize[1] = sizeY;

  // Edit the spacing
  m_OutputSpacing[0] = spacingX;
  m_OutputSpacing[1] = spacingY;

  // Edit the origin in the cartographic projection
  OutputPointType geoPoint, newCartoPoint;
  geoPoint[0] = originX;
  geoPoint[1] = originY;
  newCartoPoint = m_Transform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);

  if (isUl)
    {
    m_OutputOrigin[0] = newCartoPoint[0] - m_OutputSpacing[0] * m_OutputSize[0] / 2;
    m_OutputOrigin[1] = newCartoPoint[1] - m_OutputSpacing[1] * m_OutputSize[1] / 2;
    }
  else
    {
    m_OutputOrigin[0] = newCartoPoint[0];
    m_OutputOrigin[1] = newCartoPoint[1];
    }
}

/**
 *
 */
void
DEMToImageGeneratorModel
::ReprojectImage()
{
  m_Transform->InstanciateTransform();
#if 0
  if(this->GetUseInputImage() == true )
    {
      m_Resampler->SetInput(m_InputImage);
      //resampler->SetDeformationFieldSpacing(gridSpacing);
      m_Resampler->SetOutputOrigin(m_OutputOrigin);
      m_Resampler->SetOutputSize(m_OutputSize);
      m_Resampler->SetOutputSpacing(m_OutputSpacing);
      m_Resampler->SetOutputProjectionRef(m_Transform->GetOutputProjectionRef());

      // Default padding value
      InputImageType::PixelType defaultValue;
      itk::PixelBuilder<InputImageType::PixelType>::Zero(defaultValue,
                                                     m_InputImage->GetNumberOfComponentsPerPixel());
      m_Resampler->SetEdgePaddingValue(defaultValue);

      m_Output        = m_Resampler->GetOutput();
    }
  else
    {
    }
#endif
//  m_DEMToImageGenerator->SetTransform(m_Transform);
  m_DEMToImageGenerator->SetOutputOrigin(m_OutputOrigin);
  m_DEMToImageGenerator->SetOutputSize(m_OutputSize);
  m_DEMToImageGenerator->SetOutputSpacing(m_OutputSpacing);
  // Default padding value
  InputImageType::PixelType defaultValue;
  itk::PixelBuilder<InputImageType::PixelType>::Zero(defaultValue,
                                                 m_InputImage->GetNumberOfComponentsPerPixel());
  m_DEMToImageGenerator->SetDefaultUnknownValue(defaultValue);

  m_Output        = m_DEMToImageGenerator->GetOutput();

  m_OutputChanged = true;
  this->NotifyAll();
  m_OutputChanged = false;

}



/**
 *
 */
void
DEMToImageGeneratorModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}

/**
 *
 */
void
DEMToImageGeneratorModel
::SetDEMDirectoryPath(const char* DEMDirectory)
{
  m_DEMToImageGenerator->SetDEMDirectoryPath(DEMDirectory);
}

} // End namespace
