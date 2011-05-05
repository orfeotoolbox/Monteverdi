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

#include <string>

#include <ogr_spatialref.h>
#include "otbImageFileReader.h"
#include "otbMath.h"

namespace otb
{

/**
 * Constructor
 */
DEMToImageGeneratorModel::DEMToImageGeneratorModel():
  m_UseInputImage(false),  m_HillShadingProcess(false), m_ReliefProcess(false)
{
  m_DEMToImageGenerator = DEMToImageGeneratorType::New();
  m_HillShading = HillShadingFilterType::New();
  m_ReliefColored = VectorImageType::New();

  m_Colormapper = ColorMapFilterType::New();
  m_Colormap = ColorMapFunctorType::New();
  m_Multiply = MultiplyFilterType::New();
  m_RGBtoVectorImageCastFilter = RGBtoVectorImageCastFilterType::New();

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
  m_DEMToImageGenerator->InstanciateTransform();
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
  m_DEMToImageGenerator->SetOutputOrigin(m_OutputOrigin);
  m_DEMToImageGenerator->SetOutputSize(m_OutputSize);
  m_DEMToImageGenerator->SetOutputSpacing(m_OutputSpacing);
  m_DEMToImageGenerator->InstanciateTransform();
  m_Output        = m_DEMToImageGenerator->GetOutput();

  this->NotifyAll();
}

/**
 *
 */
void
DEMToImageGeneratorModel
::ProcessHillShading(double azimutAngle, double elevationAngle, unsigned int radius)
{
  //Compute the resolution (Vincenty formula)
  double lon1 = m_OutputOrigin[0];
  double lon2 = m_OutputOrigin[0] + m_OutputSize[0] * m_OutputSpacing[0];
  double lat1 = m_OutputOrigin[1];
  double lat2 = m_OutputOrigin[1] + m_OutputSize[1] * m_OutputSpacing[1];
  double R = 6371; // km
  double d = vcl_acos(vcl_sin(lat1) * vcl_sin(lat2) +
                      vcl_cos(lat1) * vcl_cos(lat2) * vcl_cos(lon2 - lon1)) * R;
//double res = d / vcl_sqrt(2.0);

  m_HillShading->SetRadius(radius);
  m_HillShading->SetInput(m_DEMToImageGenerator->GetOutput());
  m_HillShading->SetAzimuthLight(azimutAngle* CONST_PI_180);
  m_HillShading->SetElevationLight(elevationAngle * CONST_PI_180);

//  m_HillShading->GetFunctor().SetXRes(res);
//  m_HillShading->GetFunctor().SetYRes(res);

  m_HillShadingProcess = true;
  this->NotifyAll();
}


/**
 *
 */
void
DEMToImageGeneratorModel
::ProcessColorRelief(double min, double max, bool withHillShading)
{

  m_Colormapper->UseInputImageExtremaForScalingOff();

  m_Colormap->SetMinimumInputValue(min);
  m_Colormap->SetMaximumInputValue(max);
  m_Colormapper->SetColormap(m_Colormap);

  m_Colormapper->SetInput(m_DEMToImageGenerator->GetOutput());

  m_Multiply->SetInput1(m_Colormapper->GetOutput());
  m_Multiply->SetInput2(m_HillShading->GetOutput());

  if(withHillShading)
    {
    m_RGBtoVectorImageCastFilter->SetInput( m_Multiply->GetOutput() );
    }
  else
    {
    m_RGBtoVectorImageCastFilter->SetInput( m_Colormapper->GetOutput() );
    }

  m_ReliefColored = m_RGBtoVectorImageCastFilter->GetOutput();
  m_ReliefProcess = true;
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
