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

  m_Resampler = ResampleFilterType::New();
  m_PerBander = PerBandFilterType::New();
  
  m_Transform        = TransformType::New();
  m_InverseTransform = TransformType::New();
}

/**
 * Destructor
 */
ProjectionModel::
~ProjectionModel()
{}

/**
 * Update the input Projection Ref
 */
void
ProjectionModel
::UpdateInputUTMTransform(int zone,bool north)
{
  // Build the Output Projection Ref
  // Suppose that the transformation choosen is UTM : 31 , North:
  OGRSpatialReference oSRS;
  oSRS.SetProjCS("UTM");
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.SetUTM(zone,north);
  
  char * utmRef = NULL;
  oSRS.exportToWkt(&utmRef);

  // Report projection ref (not done by the resample filter)
  itk::MetaDataDictionary & dict = m_InputImage->GetMetaDataDictionary();
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, utmRef);

  // Up To date the transform
  m_Transform->SetInputProjectionRef(utmRef);
  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();

  m_TempTransformChanged = true;
  //m_TransformChanged = true;
  this->NotifyAll();
  m_TempTransformChanged = false;
  //m_TransformChanged = false;
}

/**
 * Update the input Projection LambertII Projection Ref
 */
void ProjectionModel
::InitializeInputLambertIITransform()
{
  // Projection Parameters
  double stdParallel1  = 45.89891944;
  double stdParallel2  = 47.69601389;
  double originLat     = 46.8;
  double originLong    = 2.33722778;
  double falseEasting  = 600000;
  double falseNorthing = 2200000;

  // Build the Output Projection Ref
  OGRSpatialReference oSRS;
  oSRS.SetProjCS("Lambert II ");
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.SetLCC(stdParallel1,stdParallel2,originLat,originLong,falseEasting,falseNorthing);
  
  char * lambertRef = NULL;
  oSRS.exportToWkt(&lambertRef);
  
  // Report projection ref (not done by the resample filter)
  itk::MetaDataDictionary & dict = m_InputImage->GetMetaDataDictionary();
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey,lambertRef );
  
  // Update the transform
  m_Transform->SetInputProjectionRef(lambertRef);
  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();
  
  m_TempTransformChanged = true;
  //m_TransformChanged = true;
  this->NotifyAll();
  m_TempTransformChanged = false;
  //m_TransformChanged = false;
}


/**
 * Initialize the Transmercator Projection and Transform
 */
void ProjectionModel
::UpdateInputTMTransform(double scale, double falseEasting , double falseNorthing )
{
  //Get the parameters
  double originLat   = 49.83;
  double originLong  = 6.16;

  // Build the Output Projection Ref
  // Suppose that the transformation choosen is UTM : 31 , North:
  OGRSpatialReference oSRS;
  oSRS.SetProjCS("Transmercator ");
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.SetTM(originLat,originLong,scale,falseEasting,falseNorthing);
  
  char * tmRef = NULL;
  oSRS.exportToWkt(&tmRef);
  
  // Report projection ref (not done by the resample filter)
  itk::MetaDataDictionary & dict = m_InputImage->GetMetaDataDictionary();
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey,tmRef);

  // Up To date the transform
  m_Transform->SetOutputProjectionRef(m_OutputProjectionRef);
  m_Transform->InstanciateTransform();
  
  m_TempTransformChanged = true;
  //  m_TransformChanged = true;
  this->NotifyAll();
  m_TempTransformChanged = false;
  //m_TransformChanged = false;
}

/**
 *
 */
void
ProjectionModel
::UpdateUTMTransform(int zone,bool north)
{
  // Build the Output Projection Ref
  // Suppose that the transformation choosen is UTM : 31 , North:
  OGRSpatialReference oSRS;
  oSRS.SetProjCS("UTM");
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.SetUTM(zone,north);
    
  oSRS.exportToWkt(&m_OutputProjectionRef/*utmRef*/);

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
  // Projection Parameters
  double stdParallel1  = 45.89891944;
  double stdParallel2  = 47.69601389;
  double originLat     = 46.8;
  double originLong    = 2.33722778;
  double falseEasting  = 600000;
  double falseNorthing = 2200000;

  // Build the Output Projection Ref
  OGRSpatialReference oSRS;
  oSRS.SetProjCS("Lambert II ");
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.SetLCC(stdParallel1,stdParallel2,originLat,originLong,falseEasting,falseNorthing);
  oSRS.exportToWkt(&m_OutputProjectionRef);

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
::UpdateTMTransform(double scale, double falseEasting , double falseNorthing )
{
  //Get the parameters
  double originLat   = 49.83;
  double originLong  = 6.16;

  // Build the Output Projection Ref
  // Suppose that the transformation choosen is UTM : 31 , North:
  OGRSpatialReference oSRS;
  oSRS.SetProjCS("Transmercator ");
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.SetTM(originLat,originLong,scale,falseEasting,falseNorthing);
  oSRS.exportToWkt(&m_OutputProjectionRef);

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
  OGRSpatialReference oSRS;
  oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.exportToWkt(&m_OutputProjectionRef);
  std::cout <<"m_OutputProjectionRef " << m_OutputProjectionRef << std::endl;

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
  unsigned int up = 0,  down = 0, left = 0, right = 0;
  std::vector<IndexType>        vindex;
  std::vector<OutputPointType>   voutput;
  
  IndexType        index1,index2,index3,index4;
  SizeType         size;
  
  // Image size
  size = m_InputImage->GetLargestPossibleRegion().GetSize();
  
  // project the 4 corners
  index1 = m_InputImage->GetLargestPossibleRegion().GetIndex();
  index2 = m_InputImage->GetLargestPossibleRegion().GetIndex();
  index3 = m_InputImage->GetLargestPossibleRegion().GetIndex();
  index4 = m_InputImage->GetLargestPossibleRegion().GetIndex();

  index2[0]+= size[0]-1;
  index3[0]+= size[0]-1;
  index3[1]+= size[1]-1;
  index4[1]+= size[1]-1;
  
  vindex.push_back(index1);
  vindex.push_back(index2);
  vindex.push_back(index3);
  vindex.push_back(index4);
  
  for(unsigned int i = 0; i<vindex.size() ; i++)
    {
      OutputPointType physicalPoint;
      m_InputImage->TransformIndexToPhysicalPoint(vindex[i],physicalPoint);
      voutput.push_back(m_Transform->TransformPoint(physicalPoint));
    }

  // Compute the boundaries
  double minX = voutput[0][0];
  double maxX = voutput[0][0];
  double minY = voutput[0][1];
  double maxY = voutput[0][1];

  for(unsigned int i = 0; i<voutput.size() ; i++)
    {
      // Origins
      if ( minX > voutput[i][0] )
       {
         minX = voutput[i][0];
         left = i;
       }
      if ( minY > voutput[i][1] )
       {
         minY = voutput[i][1];
         down = i;
       }

      // Sizes
      if ( maxX < voutput[i][0] )
       {
         maxX = voutput[i][0];
         right = i;
       }
      if ( maxY < voutput[i][1] )
       {
         maxY = voutput[i][1];
         up = i;
       }
    }
//   // size image in carto coordinate :
//   double sizeXcarto =  maxX-minX;
//   // - because of the difference of origin for Y (image vs. carto)
//   double sizeYcarto =  -(maxY-minY);
  
//   // X
//   double alphaX1 = vcl_atan2( vcl_abs(voutput[right][1]-voutput[up][1]), vcl_abs(voutput[right][0]-voutput[up][0]) );
//   double alphaX2 = vcl_atan2( vcl_abs(voutput[left][1]-voutput[up][1]), vcl_abs(voutput[left][0]-voutput[up][0]) );
  
//   m_OutputSpacing[0] = sizeXcarto/((static_cast<double>(size[0])* vcl_cos( alphaX1 )) + (static_cast<double>(size[1])* vcl_cos( alphaX2 )));
//   m_OutputSize[0]    = static_cast<unsigned int>(vcl_floor(std::abs(sizeXcarto/m_OutputSpacing[0])) );
  
//   // Y
//   double alphaY1 = vcl_atan2( vcl_abs(voutput[up][1]-voutput[left][1]), vcl_abs(voutput[up][0]-voutput[left][0]) );
//   double alphaY2 = vcl_atan2( vcl_abs(voutput[down][1]-voutput[left][1]), vcl_abs(voutput[down][0]-voutput[left][0]) );

//   m_OutputSpacing[1] = sizeYcarto/((static_cast<double>(size[1])* vcl_cos( alphaY1 )) + (static_cast<double>(size[0])* vcl_cos( alphaY2 )));
//   m_OutputSize[1]    = static_cast<unsigned int>(vcl_floor(std::abs(sizeYcarto/m_OutputSpacing[1])) );


  // Compute the output size
  double sizeCartoX = vcl_abs(maxX - minX);
  double sizeCartoY = vcl_abs(minY - maxY);

  OutputPointType o,oX,oY;
  
  // Initialize
  o[0] = minX;
  o[1] = maxY;
  oX = o;
  oY = o;

  m_OutputOrigin = o;

  oX[0]+=sizeCartoX;
  oY[1]+=sizeCartoY;

  // Transform back into the input image
  OutputPointType io = m_InverseTransform->TransformPoint(o);
  OutputPointType ioX = m_InverseTransform->TransformPoint(oX);
  OutputPointType ioY = m_InverseTransform->TransformPoint(oY);
  
  // Transform to indices
  IndexType ioIndex, ioXIndex, ioYIndex;
  m_InputImage->TransformPhysicalPointToIndex(io,ioIndex);
  m_InputImage->TransformPhysicalPointToIndex(ioX,ioXIndex);
  m_InputImage->TransformPhysicalPointToIndex(ioY,ioYIndex);

  // Evaluate Ox and Oy length in number of pixels
  double OxLength,OyLength;
  
  OxLength = vcl_sqrt( vcl_pow((double)ioIndex[0] - (double)ioXIndex[0],2)
                  +  vcl_pow((double)ioIndex[1] - (double)ioXIndex[1],2));

  OyLength = vcl_sqrt( vcl_pow((double)ioIndex[0] - (double)ioYIndex[0],2)
                  +  vcl_pow((double)ioIndex[1] - (double)ioYIndex[1],2));

  // Evaluate spacing
  m_OutputSpacing[0] = sizeCartoX/OxLength;
  m_OutputSpacing[1] = -sizeCartoY/OyLength;
  
  // Evaluate size
  m_OutputSize[0] = static_cast<unsigned int>(vcl_floor(vcl_abs(sizeCartoX/m_OutputSpacing[0])));
  m_OutputSize[1] = static_cast<unsigned int>(vcl_floor(vcl_abs(sizeCartoY/m_OutputSpacing[1])));
}

/**
 *
 */
void
ProjectionModel
::ProjectRegion(unsigned int sizeX, unsigned int sizeY, double spacingX, double spacingY, double originX, double originY, bool isUl)
{
  // Edit the size
  m_OutputSize[0] = sizeX;
  m_OutputSize[1] = sizeY;
  
  // Edit the spacing
  m_OutputSpacing[0] = spacingX;
  m_OutputSpacing[1] = spacingY;
  
  // Edit the origin in the cartographic projection
  OutputPointType      geoPoint, newCartoPoint;
  geoPoint[0] = originX;
  geoPoint[1] = originY;
  newCartoPoint = m_Transform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
  if(isUl)
    {
      m_OutputOrigin[0] = newCartoPoint[0]-m_OutputSpacing[0]*m_OutputSize[0]/2;
      m_OutputOrigin[1] = newCartoPoint[1]-m_OutputSpacing[1]*m_OutputSize[1]/2;
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
  m_Resampler->SetTransform(m_InverseTransform);
  m_Resampler->SetSize(m_OutputSize);
  m_Resampler->SetOutputSpacing(m_OutputSpacing);
  m_Resampler->SetOutputOrigin(m_OutputOrigin);
  m_PerBander->SetInput(m_InputImage);
  m_PerBander->SetFilter(m_Resampler);
  m_Output = m_PerBander->GetOutput();
  
  m_Output->UpdateOutputInformation();
  // Report projection ref (not done by the resample filter)
  itk::MetaDataDictionary & dict = m_Output->GetMetaDataDictionary();
  std::string projectionRef = m_Transform->GetOutputProjectionRef();
  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, projectionRef);
  m_Output->SetMetaDataDictionary(dict);
  m_Output->UpdateOutputInformation();
  
  m_OutputChanged = true;
  this->NotifyAll();
  
  // The SetSizeMacro doesn't recall the update on the resampler when size is changed
  m_Resampler = ResampleFilterType::New();
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

}// End namespace
