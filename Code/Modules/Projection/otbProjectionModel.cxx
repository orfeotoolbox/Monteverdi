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

  // 
  m_Resampler = ResampleFilterType::New();
  m_PerBander = PerBandFilterType::New();
}

/**
 * Destructor
 */
ProjectionModel::
~ProjectionModel()
{}

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
  oSRS.SetUTM(zone,north);
  
  char * utmRef = NULL;
  oSRS.exportToWkt(&utmRef);

  // Build the Generic RS transform 
  RSTransformType::Pointer     transformFilter = RSTransformType::New();
  transformFilter->SetInputProjectionRef(m_InputImage->GetProjectionRef());
  transformFilter->SetInputSpacing(m_InputImage->GetSpacing());
  transformFilter->SetInputOrigin(m_InputImage->GetOrigin());
  transformFilter->SetInputDictionary(m_InputImage->GetMetaDataDictionary());
  
  transformFilter->SetOutputProjectionRef(utmRef);
  transformFilter->InstanciateTransform();

  m_Transform = transformFilter->GetTransform();

  this->UpdateOutputParameters();

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
  //oSRS.SetWellKnownGeogCS("WGS84");
  oSRS.SetLCC(stdParallel1,stdParallel2,originLat,originLong,falseEasting,falseNorthing);
  
  char * lambertRef = NULL;
  oSRS.exportToWkt(&lambertRef);

  // Build the Generic RS transform 
  RSTransformType::Pointer     transformFilter = RSTransformType::New();
  transformFilter->SetInputProjectionRef(m_InputImage->GetProjectionRef());
  transformFilter->SetInputSpacing(m_InputImage->GetSpacing());
  transformFilter->SetInputOrigin(m_InputImage->GetOrigin());
  transformFilter->SetInputDictionary(m_InputImage->GetMetaDataDictionary());
  
  transformFilter->SetOutputProjectionRef(lambertRef);
  transformFilter->InstanciateTransform();
  m_Transform = transformFilter->GetTransform();
  
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
  
  char * lambertRef = NULL;
  oSRS.exportToWkt(&lambertRef);

  // Build the Generic RS transform 
  RSTransformType::Pointer     transformFilter = RSTransformType::New();
  transformFilter->SetInputProjectionRef(m_InputImage->GetProjectionRef());
  transformFilter->SetInputSpacing(m_InputImage->GetSpacing());
  transformFilter->SetInputOrigin(m_InputImage->GetOrigin());
  transformFilter->SetInputDictionary(m_InputImage->GetMetaDataDictionary());
  
  transformFilter->SetOutputProjectionRef(lambertRef);
  transformFilter->InstanciateTransform();

  m_Transform = transformFilter->GetTransform();

  this->UpdateOutputParameters();

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
  index1.Fill(0);
  index2[0] = size[0];
  index2[1] = 0;
  index3[0] = 0;
  index3[1] = size[1];
  index4[0] = size[0];
  index4[1] = size[1];
  
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
  // size image in carto coordinate :
  double sizeXcarto =  maxX-minX;
  // - because of the difference of origin for Y (image vs. carto)
  double sizeYcarto =  -(maxY-minY);
  
  // X 
  double alphaX1 = vcl_atan2( vcl_abs(voutput[right][1]-voutput[up][1]), vcl_abs(voutput[right][0]-voutput[up][0]) );
  double alphaX2 = vcl_atan2( vcl_abs(voutput[left][1]-voutput[up][1]), vcl_abs(voutput[left][0]-voutput[up][0]) );

  m_OutputSpacing[0] = sizeXcarto/((static_cast<double>(size[0])* vcl_cos( alphaX1 )) + (static_cast<double>(size[1])* vcl_cos( alphaX2 )));
  m_OutputSize[0]    = static_cast<unsigned int>(vcl_floor(std::abs(sizeXcarto/m_OutputSpacing[0])) );

  // Y
  double alphaY1 = vcl_atan2( vcl_abs(voutput[up][1]-voutput[left][1]), vcl_abs(voutput[up][0]-voutput[left][0]) );
  double alphaY2 = vcl_atan2( vcl_abs(voutput[down][1]-voutput[left][1]), vcl_abs(voutput[down][0]-voutput[left][0]) );

  m_OutputSpacing[1] = sizeYcarto/((static_cast<double>(size[1])* vcl_cos( alphaY1 )) + (static_cast<double>(size[0])* vcl_cos( alphaY2 )));
  m_OutputSize[1]    = static_cast<unsigned int>(vcl_floor(std::abs(sizeYcarto/m_OutputSpacing[1])) );
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
  newCartoPoint = m_Transform->GetSecondTransform()->TransformPoint(geoPoint);
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
  std::cout <<"m_Origin "<< m_OutputOrigin[0] << "," << m_OutputOrigin[1]  << std::endl;
  
  m_Resampler->SetTransform(m_Transform);
  m_Resampler->SetSize(m_OutputSize);
  m_Resampler->SetOutputSpacing(m_OutputSpacing);
  m_Resampler->SetOutputOrigin(m_OutputOrigin);
  m_PerBander->SetInput(m_InputImage);
  m_PerBander->SetFilter(m_Resampler);
  m_Output = m_PerBander->GetOutput();
  m_OutputChanged = true;
  this->NotifyAll();
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
