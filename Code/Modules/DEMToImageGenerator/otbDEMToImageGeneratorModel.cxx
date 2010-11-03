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
  m_UseInputImage = false;
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
::UpdateOutputParametersFromImage(InputImageType::Pointer inputImage)
{
  m_DEMToImageGenerator->SetOutputParametersFromImage(inputImage.GetPointer());
  m_DEMToImageGenerator->GetTransform()->InstanciateTransform();
  this->SetUseInputImage(true);

  InputImageType::IndexType index;
  InputImageType::PointType point, geoPoint;
  index.Fill(0);
  inputImage->TransformIndexToPhysicalPoint(index, point);
  // Transform to geo and to Choosen projection
  m_OutputOrigin = m_DEMToImageGenerator->GetTransform()->GetTransform()->GetFirstTransform()->TransformPoint(point);

  index[0]= inputImage->GetLargestPossibleRegion().GetSize()[0];
  index[1]= inputImage->GetLargestPossibleRegion().GetSize()[1];

  m_OutputSize[0] = index[0];
  m_OutputSize[1] = index[1];

  inputImage->TransformIndexToPhysicalPoint(index, point);
  // Transform to geo and to Choosen projection
  geoPoint = m_DEMToImageGenerator->GetTransform()->GetTransform()->GetFirstTransform()->TransformPoint(point);
  m_OutputSpacing = inputImage->GetSpacing();
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
                double originY)
{
  // Edit the size
  m_OutputSize[0] = sizeX;
  m_OutputSize[1] = sizeY;

  // Edit the spacing
  m_OutputSpacing[0] = spacingX;
  m_OutputSpacing[1] = spacingY;

  // Edit the origin in the cartographic projection
  OutputPointType geoPoint;
  geoPoint[0] = originX;
  geoPoint[1] = originY;
  m_OutputOrigin = m_DEMToImageGenerator->GetTransform()->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);

}

/**
 *
 */
void
DEMToImageGeneratorModel
::ReprojectImage()
{
//  m_Transform->InstanciateTransform();
//  m_DEMToImageGenerator->SetTransform(m_Transform);
  m_DEMToImageGenerator->SetOutputOrigin(m_OutputOrigin);
  m_DEMToImageGenerator->SetOutputSize(m_OutputSize);
  m_DEMToImageGenerator->SetOutputSpacing(m_OutputSpacing);
  m_Output        = m_DEMToImageGenerator->GetOutput();

  this->NotifyAll();
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
