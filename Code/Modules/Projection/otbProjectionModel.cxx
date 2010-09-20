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
#include "otbProjectionModel.h"
#include <ogr_spatialref.h>
#include "otbMapProjections.h"
#include <string>

namespace otb
{

/**
 * Constructor
 */
ProjectionModel::ProjectionModel()
{
  m_OutputChanged    = false;
  m_TransformChanged = false;
  m_TempTransformChanged = false;
  m_UseDEM               = false;

  m_Resampler = ResampleFilterType::New();

  m_Transform        = TransformType::New();
  m_InverseTransform = TransformType::New();
}

/**
 * Destructor
 */
ProjectionModel::
~ProjectionModel()
{}

///////////////////////////////////////////////////////////////
/**     CASE : input transform is not allowed */
/////////////// /////////////// /////////////// /////////////// 

/**
 * Update the input Projection Ref
 */
void
ProjectionModel
::UpdateInputUTMTransform(int zone, bool north)
{
  typedef UtmInverseProjection UtmProjectionType;
  UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();
  
  utmProjection->SetZone(zone);
  utmProjection->SetHemisphere(north);
  std::string utmRef = utmProjection->GetWkt();
  
  // Report projection ref (not done by the resample filter)
  itk::MetaDataDictionary& dict = m_InputImage->GetMetaDataDictionary();
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, utmRef);

  // Up To date the transform
  m_Transform->SetInputProjectionRef(utmRef);
  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();

  m_TempTransformChanged = true;
  this->NotifyAll();
  m_TempTransformChanged = false;
}

/**
 * Update the input Projection LambertII Projection Ref
 */
void ProjectionModel
::InitializeInputLambertIITransform()
{
  typedef Lambert2EtenduForwardProjection Lambert2Type;
  Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();
  std::string  lambertRef = lambert2Projection->GetWkt();
  
  // Report projection ref (not done by the resample filter)
  itk::MetaDataDictionary& dict = m_InputImage->GetMetaDataDictionary();
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, lambertRef);

  // Update the transform
  m_Transform->SetInputProjectionRef(lambertRef);
  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();

  m_TempTransformChanged = true;
  this->NotifyAll();
  m_TempTransformChanged = false;
}

/**
 * Initialize the Transmercator Projection and Transform
 */
void ProjectionModel
::UpdateInputTMTransform(double scale, double falseEasting, double falseNorthing)
{
  typedef otb::TransMercatorInverseProjection TransMercatorProjectionType;
  TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();
  transMercatorProjection->SetParameters(falseEasting,falseNorthing,scale);
  std::string tmRef =   transMercatorProjection->GetWkt();
  
  // Report projection ref (not done by the resample filter)
  itk::MetaDataDictionary& dict = m_InputImage->GetMetaDataDictionary();
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, tmRef);

  // Up To date the transform
  m_Transform->SetInputProjectionRef(tmRef);
  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();

  m_TempTransformChanged = true;
  this->NotifyAll();
  m_TempTransformChanged = false;
}

//////////////////////////////////////////////////////////////////
/**     END CASE : input transform is not accessible */
//////////////////////////////////////////////////////////////////

/**
 *
 */
void
ProjectionModel
::UpdateUTMTransform(int zone, bool north)
{
  typedef UtmInverseProjection UtmProjectionType;
  UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();
  
  utmProjection->SetZone(zone);
  utmProjection->SetHemisphere(north);
  m_OutputProjectionRef = utmProjection->GetWkt();
  
  // Build the Generic RS transform
  m_Transform->SetInputProjectionRef(m_InputImage->GetProjectionRef());
  m_Transform->SetInputDictionary(m_InputImage->GetMetaDataDictionary());

  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();
  
  // Get the transform
  m_Transform->GetInverse(m_InverseTransform);

  // Update the output image parameters
  this->UpdateOutputParameters();

  //Notify the view that the transform has changed
  m_TransformChanged = true;
  this->NotifyAll();
  m_TransformChanged = false;
}

/**
 * Initialize the Remote Sensing Transform
 */
void ProjectionModel
::InitializeLambertIITransform()
{
  typedef Lambert2EtenduForwardProjection Lambert2Type;
  Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();
  m_OutputProjectionRef = lambert2Projection->GetWkt();

  // Build the Generic RS transform
  m_Transform->SetInputProjectionRef(m_InputImage->GetProjectionRef());
  m_Transform->SetInputDictionary(m_InputImage->GetMetaDataDictionary());

  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();

  // Get the inverse
  m_Transform->GetInverse(m_InverseTransform);

  this->UpdateOutputParameters();

  m_TransformChanged = true;
  this->NotifyAll();
  m_TransformChanged = false;
}

/**
 * Initialize the Transmercator Projection and Transform
 */
void ProjectionModel
::UpdateTMTransform(double scale, double falseEasting, double falseNorthing)
{
  typedef otb::TransMercatorInverseProjection TransMercatorProjectionType;
  TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();
  transMercatorProjection->SetParameters(falseEasting,falseNorthing,scale);
  m_OutputProjectionRef = transMercatorProjection->GetWkt();
  
  // Build the Generic RS transform
  m_Transform->SetInputProjectionRef(m_InputImage->GetProjectionRef());
  m_Transform->SetInputDictionary(m_InputImage->GetMetaDataDictionary());

  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();

  // Instanciate the inverse RS transform
  m_Transform->GetInverse(m_InverseTransform);

  this->UpdateOutputParameters();

  m_TransformChanged = true;
  this->NotifyAll();
  m_TransformChanged = false;
}

/**
 *
 */
void
ProjectionModel
::UpdateWGS84Transform()
{
  // Build the Output Projection Ref
  // WGS84
  char * wgs84Ref;
  OGRSpatialReference oSRS;
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.exportToWkt(&wgs84Ref);
  m_OutputProjectionRef = wgs84Ref;
  
  // Build the Generic RS transform
  m_Transform->SetInputProjectionRef(m_InputImage->GetProjectionRef());
  m_Transform->SetInputDictionary(m_InputImage->GetMetaDataDictionary());

  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();

  // Get the transform
  m_Transform->GetInverse(m_InverseTransform);

  // Update the output image parameters
  this->UpdateOutputParameters();

  //Notify the view that the transform has changed
  m_TransformChanged = true;
  this->NotifyAll();
  m_TransformChanged = false;
}

/**
 * Compute the new Origin, Spacing & Size of the output
 */
void ProjectionModel
::UpdateOutputParameters()
{
  // Compute the 4 corners in the cartographic coordinate system
  unsigned int                 up = 0,  down = 0, left = 0, right = 0;
  std::vector<IndexType>       vindex;
  std::vector<OutputPointType> voutput;

  IndexType index1, index2, index3, index4;
  SizeType  size;

  // Image size
  size = m_InputImage->GetLargestPossibleRegion().GetSize();

  // project the 4 corners
  index1 = m_InputImage->GetLargestPossibleRegion().GetIndex();
  index2 = m_InputImage->GetLargestPossibleRegion().GetIndex();
  index3 = m_InputImage->GetLargestPossibleRegion().GetIndex();
  index4 = m_InputImage->GetLargestPossibleRegion().GetIndex();

  index2[0] += size[0] - 1;
  index3[0] += size[0] - 1;
  index3[1] += size[1] - 1;
  index4[1] += size[1] - 1;

  vindex.push_back(index1);
  vindex.push_back(index2);
  vindex.push_back(index3);
  vindex.push_back(index4);

  for (unsigned int i = 0; i < vindex.size(); i++)
    {
    OutputPointType physicalPoint;
    m_InputImage->TransformIndexToPhysicalPoint(vindex[i], physicalPoint);
    voutput.push_back(m_Transform->TransformPoint(physicalPoint));
    }

  // Compute the boundaries
  double minX = voutput[0][0];
  double maxX = voutput[0][0];
  double minY = voutput[0][1];
  double maxY = voutput[0][1];

  for (unsigned int i = 0; i < voutput.size(); i++)
    {
    // Origins
    if (minX > voutput[i][0])
      {
      minX = voutput[i][0];
      left = i;
      }
    if (minY > voutput[i][1])
      {
      minY = voutput[i][1];
      down = i;
      }

    // Sizes
    if (maxX < voutput[i][0])
      {
      maxX = voutput[i][0];
      right = i;
      }
    if (maxY < voutput[i][1])
      {
      maxY = voutput[i][1];
      up = i;
      }
    }

  // Compute the output size
  double sizeCartoX = vcl_abs(maxX - minX);
  double sizeCartoY = vcl_abs(minY - maxY);

  OutputPointType o, oX, oY;

  // Initialize
  o[0] = minX;
  o[1] = maxY;
  oX = o;
  oY = o;

  m_OutputOrigin = o;

  oX[0] += sizeCartoX;
  oY[1] += sizeCartoY;

  // Transform back into the input image
  OutputPointType io = m_InverseTransform->TransformPoint(o);
  OutputPointType ioX = m_InverseTransform->TransformPoint(oX);
  OutputPointType ioY = m_InverseTransform->TransformPoint(oY);

  // Transform to indices
  IndexType ioIndex, ioXIndex, ioYIndex;
  m_InputImage->TransformPhysicalPointToIndex(io, ioIndex);
  m_InputImage->TransformPhysicalPointToIndex(ioX, ioXIndex);
  m_InputImage->TransformPhysicalPointToIndex(ioY, ioYIndex);

  // Evaluate Ox and Oy length in number of pixels
  double OxLength, OyLength;

  OxLength = vcl_sqrt(vcl_pow((double) ioIndex[0] - (double) ioXIndex[0], 2)
                      +  vcl_pow((double) ioIndex[1] - (double) ioXIndex[1], 2));

  OyLength = vcl_sqrt(vcl_pow((double) ioIndex[0] - (double) ioYIndex[0], 2)
                      +  vcl_pow((double) ioIndex[1] - (double) ioYIndex[1], 2));

  // Evaluate spacing
  m_OutputSpacing[0] = sizeCartoX / OxLength;
  m_OutputSpacing[1] = -sizeCartoY / OyLength;

  // Evaluate size
  m_OutputSize[0] = static_cast<unsigned int>(vcl_floor(vcl_abs(sizeCartoX / m_OutputSpacing[0])));
  m_OutputSize[1] = static_cast<unsigned int>(vcl_floor(vcl_abs(sizeCartoY / m_OutputSpacing[1])));
}

/**
 *
 */
void
ProjectionModel
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
ProjectionModel
::ReprojectImage()
{
  // Fill the resampler parameters
  m_Resampler->SetInput(m_InputImage);
  m_Resampler->SetOutputSize(m_OutputSize);
  m_Resampler->SetOutputSpacing(m_OutputSpacing);
  m_Resampler->SetOutputOrigin(m_OutputOrigin);
  m_Resampler->SetOutputProjectionRef(m_Transform->GetOutputProjectionRef());
  // Deformation grid spacing : 4 times the output spacing
  m_Resampler->SetDeformationFieldSpacing(4.*m_OutputSpacing);
  
  // Use the DEM
  if(m_UseDEM)
    {
    m_Resampler->SetDEMDirectory(m_Transform->GetDEMDirectory());
    }
  
  // Default padding value 
  InputImageType::PixelType defaultValue;
  itk::PixelBuilder<InputImageType::PixelType>::Zero(defaultValue,
                                                     m_InputImage->GetNumberOfComponentsPerPixel());
  m_Resampler->SetEdgePaddingValue(defaultValue);

  m_Output        = m_Resampler->GetOutput();
  m_OutputChanged = true;
  this->NotifyAll();
  m_OutputChanged = false;
}

/**
 *
 */
void
ProjectionModel
::SetDEMPath(std::string dem)
{
  if(m_UseDEM)
    {
    // Update the transform and the inverse one
    m_Transform->SetDEMDirectory(dem);
    m_Transform->InstanciateTransform();
    m_Transform->GetInverse(m_InverseTransform);
    }
  else
    {
    // Update the transform and the inverse one
    m_Transform->SetDEMDirectory("");
    m_Transform->InstanciateTransform();
    m_Transform->GetInverse(m_InverseTransform);
    }
}


/**
 *
 */
void
ProjectionModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}

} // End namespace
