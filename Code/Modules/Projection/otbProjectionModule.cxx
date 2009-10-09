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
#include "otbProjectionModule.h"

#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#include <FL/Enumerations.H>

#include "otbMacro.h"
#include "otbPerBandVectorImageFilter.h"
#include "itkExceptionObject.h"


#include "elevation/ossimElevManager.h"
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"

// Maps include
#include "otbMapProjections.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "otbWindowedSincInterpolateImageBlackmanFunction.h"
#include "otbWindowedSincInterpolateImageHammingFunction.h"
#include "otbWindowedSincInterpolateImageCosineFunction.h"
#include "otbWindowedSincInterpolateImageWelchFunction.h"
#include "otbWindowedSincInterpolateImageLanczosFunction.h"
#include "otbBSplineInterpolateImageFunction.h"
#include "otbMsgReporter.h"

namespace otb
{
/**
 * Constructor
 */
ProjectionModule::ProjectionModule()
{
  this->CreateGUI();
//   guiAvElev->value("0");
//   guiTRANSMERCATOREast->value("0");
//   guiTRANSMERCATORNorth->value("0");
//   guiTRANSMERCATORScale->value("1");
//   m_MainWindowInitSize[0] = guiMainWindow->w();
//   m_MainWindowInitSize[1] = guiMainWindow->h();
//   m_OutputSize.Fill(0);
//   m_OutputOrigin.Fill(0);
//   m_OutputSpacing.Fill(0);
//   m_DEMSpacing.Fill(0);
//   m_EastNorth.push_back(0);
//   m_EastNorth.push_back(0);
//   m_InterpType = LINEAR_;
//   m_MapType = UTM;
//   m_UTMZoneRef = 0;
//   m_UTMHemRef = 'N';
//   m_TileNumber = 0;
//   m_MaxTileSize = 256.;
//   guiShowDEM->hide();
  
//   m_HasOutput = false;

//   //Instanciate Filter 
//   //m_ProjectionModuleFilter = ProjectionModuleFilterType::New();
//   //m_PerBandFilter     = PerBandFilterType::New();
    // Describe inputs
  this->AddInputDescriptor<ImageType>("InputImage","Image to project :");
}

void
ProjectionModule
::Run()
{
  try
  {
//     this->SelectAction();
  
//     // test if the good parameters are available
//     int resCheckImageParameters = this->CheckImageParameters();
    int resCheckImageParameters = 0;
    if (resCheckImageParameters == 1)
      {
	itkExceptionMacro(<<"Invalid image parameters");
      }
    
    guiMainWindow->show();
  }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

void
ProjectionModule
::OpenDEM()
{
//   // Choose directory
//   const char * cfname = fl_dir_chooser("Pick a directory", "*.*");
//   // Check path
//   if (cfname == NULL || strlen(cfname)<1)
//   {
//     return;
//   }
//   // Check directoy avaibility
//   ossimElevManager * elevManager = ossimElevManager::instance();
//   ossimFilename ossimDEMDir;
//   ossimDEMDir=ossimFilename(cfname);
//   ossimDirectory od(cfname);
//   if (!elevManager->loadElevationPath(ossimDEMDir))
//   {
//     fl_alert("Invalid directory, no DEM files found!");
//     this->OpenDEM();
//     return;

//   }
//   guiDEMPath->value(cfname);
}

void
ProjectionModule
::Quit()
{
  guiMainWindow->hide();
}



void
ProjectionModule
::OK()
{
//   try
//   {
//     int resCheckImageParameters = this->CheckImageParameters();
//     if (resCheckImageParameters == 1)
//     {
//     itkExceptionMacro(<<"Invalid image parameters");
//     }

//     this->ComputeTileNumber();
//     int resCheckMapParameters = this->CheckMapParameters();

//     if (resCheckMapParameters == 1)
//     {
//     itkExceptionMacro(<<"Invalid map parameters");
//     }

//     guiMainWindow->hide();

//     m_HasOutput = true;
//     this->NotifyAll();
//   }
//   catch (itk::ExceptionObject & err)
//   {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
//   }
}






void
ProjectionModule
::UpdateUTMParam()
{
//   ImagePointerType image = m_InputImage;
//   ForwardSensorInputPointType point;
//   // Take the mid image
//   point[0] = image->GetLargestPossibleRegion().GetSize()[0]/2 + 1;
//   point[1] = image->GetLargestPossibleRegion().GetSize()[1]/2 + 1;

//   DoubleVectorType latLongPointDouble = this->PointToLongLat( point );
//   ForwardSensorOutputPointType latLongPoint;
//   latLongPoint[0] = latLongPointDouble[0];
//   latLongPoint[1] = latLongPointDouble[1];

//   typedef UtmForwardProjection UtmProjectionType;
//   UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();
//   UtmProjectionType::InputPointType geoPoint;

//   int utmZone = utmProjection->GetZoneFromGeoPoint(latLongPoint);

//   itk::OStringStream oss;
//   oss.str("");
//   oss<<utmZone;
//   guiUTMZone->value(oss.str().c_str());
//   if ( latLongPoint[1] >= 0  )
//   {
//     guiUTMNorth->value(1);
//     guiUTMSouth->value(0);
//   }
//   else
//   {
//     guiUTMSouth->value(1);
//     guiUTMNorth->value(0);
//   }

//   oss.str("");
//   oss<<setiosflags(std::ios_base::fixed);
//   oss<<latLongPoint[1];
//   guiLongSelection->value(oss.str().c_str());
//   oss.str("");
//   oss<<latLongPoint[0];
//   guiLatSelection->value(oss.str().c_str());
}



void
ProjectionModule
::UpdateMapParam()
{

//   ImagePointerType image = m_InputImage;

//   DoubleVectorType latLongPointDouble(2,0);
//   latLongPointDouble[0] = strtod(guiLongSelection->value(), NULL);
//   latLongPointDouble[1] = strtod(guiLatSelection->value(), NULL);

//   ForwardSensorOutputPointType latLongPoint;
//   latLongPoint[0] = latLongPointDouble[0];
//   latLongPoint[1] = latLongPointDouble[1];

//   itk::OStringStream oss;
//   oss<<setiosflags(std::ios_base::fixed);
//   oss.str("");

//   switch (this->GetMapType())
//   {
//   case UTM:
//   {
//     typedef UtmForwardProjection UtmProjectionType;
//     UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();

//     int utmZone = utmProjection->GetZoneFromGeoPoint(latLongPoint);

//     oss<<utmZone;
//     guiUTMZone->value(oss.str().c_str());
//     if ( latLongPoint[1] >= 0  )
//     {
//       guiUTMNorth->value(1);
//       guiUTMSouth->value(0);
//     }
//     else
//     {
//       guiUTMSouth->value(1);
//       guiUTMNorth->value(0);
//     }

//     m_EastNorth = this->LongLatPointToCarto( latLongPoint );
//     oss.str("");
//     oss<<m_EastNorth[1];
//     guiUTMNorthSelection->value(oss.str().c_str());
//     oss.str("");
//     oss<<m_EastNorth[0];
//     guiUTMEastSelection->value(oss.str().c_str());
//     break;
//   }
//   case LAMBERT2:
//   {
//     m_EastNorth = this->LongLatPointToCarto( latLongPoint );
//     oss.str("");
//     oss<<m_EastNorth[1];
//     guiLambertNorthSelection->value(oss.str().c_str());
//     oss.str("");
//     oss<<m_EastNorth[0];
//     guiLambertEastSelection->value(oss.str().c_str());
//     break;
//   }
//   case TRANSMERCATOR:
//   {
//     m_EastNorth = this->LongLatPointToCarto( latLongPoint );
//     oss.str("");
//     oss<<m_EastNorth[1];
//     guiTransmercatorNorthSelection->value(oss.str().c_str());
//     oss.str("");
//     oss<<m_EastNorth[0];
//     guiTransmercatorEastSelection->value(oss.str().c_str());
//     break;
//   }
//   default:
//     fl_alert("Problem with map projection type, please contact developpers");
//     break;
//   }
}



void
ProjectionModule
::UpdateLongLat()
{
//   ForwardSensorInputPointType point;
//   point[0] = m_EastNorth[0];
//   point[1] = m_EastNorth[1];
//   DoubleVectorType longLat = this->CartoPointToLongLat(point);
//   itk::OStringStream oss;
//   oss<<setiosflags(std::ios_base::fixed);
//   oss.str("");
//   oss<<longLat[0];
//   guiLongSelection->value(oss.str().c_str());
//   oss.str("");
//   oss<<longLat[1];
//   guiLatSelection->value(oss.str().c_str());

  
}

void
ProjectionModule
::UpdateOutputParameters()
{
//   ImagePointerType image = m_InputImage;
//   ForwardSensorInputPointType point;
//   DoubleVectorType coord;
//   itk::OStringStream oss;

//   int imageSizeX = image->GetLargestPossibleRegion().GetSize()[0];
//   int imageSizeY = image->GetLargestPossibleRegion().GetSize()[1];

//   point[0] = 0;
//   point[1] = 0;

//   coord = this->ImageToCarto( point );
//   // Store image corner coordinates
//   std::vector<DoubleVectorType> coordList(4, coord);
//   // Compute Sizes
//   point[0] = imageSizeX - 1;
//   point[1] = 0;
//   coordList[1] = this->ImageToCarto( point );
//   point[0] = imageSizeX - 1;
//   point[1] = imageSizeY - 1;
//   coordList[2] = this->ImageToCarto( point );
//   point[0] = 0;
//   point[1] = imageSizeY - 1;
//   coordList[3] = this->ImageToCarto( point );

//   // Compute min/max coordinate values
//   double minX = coordList[0][0];
//   double maxX = coordList[0][0];
//   double minY = coordList[0][1];
//   double maxY = coordList[0][1];

//   // Store point position in the output image
//   // calculation will be done from those indexeces to cares all possible rotations
//   unsigned int up = 0;
//   unsigned int down = 0;
//   unsigned int left = 0;
//   unsigned int right = 0;

//   for (unsigned int i=1; i<coordList.size(); i++)
//   {
//     // Origins
//     if ( minX > coordList[i][0] )
//       {
// 	minX = coordList[i][0];
// 	left = i;
//       }

//     if ( minY > coordList[i][1] )
//       {
// 	minY = coordList[i][1];
// 	down = i;
//       }

//     // Sizes
//     if ( maxX < coordList[i][0] )
//       {
// 	maxX = coordList[i][0];
// 	right = i;
//       }

//     if ( maxY < coordList[i][1] )
//       {
// 	maxY = coordList[i][1];
// 	up = i;
//       }
//   }
//   // size image in carto coordinate :
//   double sizeXcarto =  maxX-minX;
//   // - because of the difference of origin for Y (image vs. carto)
//   double sizeYcarto =  -(maxY-minY);

//   double XfSpacing = 0.;
//   double YfSpacing = 0.;
//   int sizeX = 0;
//   int sizeY = 0;
//   //////////////////////////////// for X
//   double alphaX1 = vcl_atan2( vcl_abs(coordList[right][1]-coordList[up][1]), vcl_abs(coordList[right][0]-coordList[up][0]) );
//   double alphaX2 = vcl_atan2( vcl_abs(coordList[left][1]-coordList[up][1]), vcl_abs(coordList[left][0]-coordList[up][0]) );
//   // spacing X
//   XfSpacing = sizeXcarto/((static_cast<double>(imageSizeX)* vcl_cos( alphaX1 )) + (static_cast<double>(imageSizeY)* vcl_cos( alphaX2 )));
//   // sizeX
//   sizeX = static_cast<int>(vcl_floor(std::abs(sizeXcarto/XfSpacing)) );

//   //////////////////////////////// for Y
//   double alphaY1 = vcl_atan2( vcl_abs(coordList[up][1]-coordList[left][1]), vcl_abs(coordList[up][0]-coordList[left][0]) );
//   double alphaY2 = vcl_atan2( vcl_abs(coordList[down][1]-coordList[left][1]), vcl_abs(coordList[down][0]-coordList[left][0]) );
//   // spacing X
//   // - because of the difference of origin for Y (image vs. carto)
//   YfSpacing = sizeYcarto/((static_cast<double>(imageSizeY)* vcl_cos( alphaY1 )) + (static_cast<double>(imageSizeX)* vcl_cos( alphaY2 )));
//   // sizeY
//   sizeY = static_cast<int>(vcl_floor(std::abs(sizeYcarto/YfSpacing)) );


//   oss.str("");
//   oss<<XfSpacing;
//   guiSpacingX->value(oss.str().c_str());
//   oss.str("");
//   oss<<YfSpacing;
//   guiSpacingY->value(oss.str().c_str());
 
//   oss.str("");
//   oss<<sizeX;
//   guiSizeX->value(oss.str().c_str());
//   oss.str("");
//   oss<<sizeY;
//   guiSizeY->value(oss.str().c_str());
}


void
ProjectionModule::UpdateDEMSpacing()
{
//   double cartoSpacingX = strtod( guiSpacingX->value(), NULL);
//   double cartoSpacingY = strtod( guiSpacingY->value(), NULL);
//   double sizeX = strtod( guiSizeX->value(), NULL);
//   double sizeY = strtod( guiSizeY->value(), NULL);

//   ForwardSensorInputPointType point;
//   point[0] = m_OutputOrigin[0];
//   point[1] = m_OutputOrigin[1];
//   DoubleVectorType ULLongLat = this->CartoPointToLongLat( point );
//   point[0] += cartoSpacingX*sizeX;
//   DoubleVectorType ULLongLatX = this->CartoPointToLongLat( point );

//   point[0] = m_OutputOrigin[0];
//   point[1] += cartoSpacingY*sizeY;
//   DoubleVectorType ULLongLatY = this->CartoPointToLongLat( point );

//   m_DEMSpacing[0] = (ULLongLatX[0]-ULLongLat[0])/sizeX;
//   m_DEMSpacing[1] = (ULLongLatY[1]-ULLongLat[1])/sizeY;
}

int
ProjectionModule::CheckImageParameters()
{
//   try
//   {
//     ForwardSensorType::Pointer sensor = ForwardSensorType::New();
//     sensor->SetImageGeometry( m_InputImage->GetImageKeywordlist() );
//   }
//   catch ( itk::ExceptionObject & )
//   {
//   return 1;
//   }
//   if (strcmp("",guiSizeX->value()) == 0 || strcmp("",guiSizeY->value()) == 0)
//   {
//     fl_alert("Please, set Size values...");
//     return 1;
//   }
//   if (strcmp("",guiSpacingX->value()) == 0 || strcmp("",guiSpacingY->value()) == 0)
//   {
//     fl_alert("Please, set Spacing values...");
//     return 1;
//   }

//   m_OutputSize[0] = atoi(guiSizeX->value());
//   m_OutputSize[1] = atoi(guiSizeY->value());
//   m_OutputSpacing[0] = atof(guiSpacingX->value());
//   m_OutputSpacing[1] = atof(guiSpacingY->value());

//   ForwardSensorInputPointType longLatPoint;
//   longLatPoint[0] = strtod(guiLongSelection->value(), NULL);
//   longLatPoint[1] = strtod(guiLatSelection->value(), NULL);
//   DoubleVectorType cartoPoint = LongLatPointToCartoInOriginRef/*LongLatPointToCarto*/(longLatPoint);

//   // upper left
//   if ( guiCenterPixel->value()==1 )
//   {
//     m_OutputOrigin[0] = cartoPoint[0]-m_OutputSpacing[0]*m_OutputSize[0]/2;
//     m_OutputOrigin[1] = cartoPoint[1]-m_OutputSpacing[1]*m_OutputSize[1]/2;
//   }
//   else
//   {
//     if ( guiULPixel->value()==1 )
//     {
//       m_OutputOrigin[0] = cartoPoint[0];
//       m_OutputOrigin[1] = cartoPoint[1];
//     }
//   }

//   return 0;
}


int
ProjectionModule
::CheckMapParameters()
{
//   int res = 0;
//   switch (this->GetMapType())
//   {
//   case UTM:
//   {
//     if (strcmp("",guiUTMZone->value()) )
//     {
//       char hem = 'N';
//       if ( guiUTMSouth->value() == 1 )
//         hem = 'S';
//       typedef UtmInverseProjection UtmProjectionType;
//       UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();
//       utmProjection->SetZone(atoi(guiUTMZone->value()));
//       utmProjection->SetHemisphere(hem);
//       res = this->CreateOutput<UtmProjectionType>(utmProjection);
//     }
//     else
//     {
//       itk::OStringStream oss;
//       oss.str("Invalid Zone parameter: ");
//       oss<<guiUTMZone->value();
//       fl_alert(oss.str().c_str());
//     }
//     break;
//   }
//   case LAMBERT2:
//   {
//     typedef Lambert2EtenduInverseProjection Lambert2Type;
//     Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();
//     res = this->CreateOutput<Lambert2Type>(lambert2Projection);

//     break;
//   }
//   case TRANSMERCATOR:
//   {
//     if (strcmp("",guiTRANSMERCATOREast->value()) != 0 && strcmp("",guiTRANSMERCATORNorth->value()) != 0 )
//     {
//       if (strcmp("",guiTRANSMERCATORScale->value()) != 0 )
//       {
//         typedef otb::TransMercatorInverseProjection TransMercatorProjectionType;
//         TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();

//         transMercatorProjection->SetParameters(strtod(guiTRANSMERCATOREast->value(), NULL),
//                                                strtod(guiTRANSMERCATORNorth->value(), NULL),
//                                                strtod(guiTRANSMERCATORScale->value(), NULL));
//         res = this->CreateOutput<TransMercatorProjectionType>(transMercatorProjection);
//       }
//       else
//       {
//         fl_alert("Invalid Scale Factor...");
//       }
//     }
//     else
//     {
//       fl_alert("Please, set Easting and Northing values...");
//     }
//     break;
//   }
//   default:
//     fl_alert("Problem with map projection type, please contact developpers");
//     res = 1;
//     break;
//   }
//   return res;
}



int
ProjectionModule
::UpdateInterpolator()
{
//   int res = 0;

//   switch (this->GetInterpolatorType())
//   {
//   case LINEAR_:
//   {
//     typedef itk::LinearInterpolateImageFunction<SingleImageType, double> LinearType;
//     LinearType::Pointer interp = LinearType::New();
//     m_Interp = interp;
//     break;
//   }
//   case NEAREST:
//   {
//     typedef itk::NearestNeighborInterpolateImageFunction<SingleImageType, double> NearestType;
//     NearestType::Pointer interp = NearestType::New();
//     m_Interp = interp;
//     break;
//   }
//   case SINC:
//   {
//     itk::OStringStream oss;
//     oss.str("");
//     oss<<guiSincRadius->value();
//     if ( strcmp("",oss.str().c_str()) != 0 )
//     {
//       oss.str("");
//       oss<<guiSincWindow->value();
//       switch ( guiSincWindow->value() )
//       {
//       case 0:
//       {
//         typedef WindowedSincInterpolateImageBlackmanFunction<SingleImageType> BlackmanType;
//         BlackmanType::Pointer interp = BlackmanType::New();
//         interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()) );
//         m_Interp = interp;
//         break;
//       }
//       case 1:
//       {

//         typedef WindowedSincInterpolateImageCosineFunction<SingleImageType> CosineType;
//         CosineType::Pointer interp = CosineType::New();
//         interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//         m_Interp = interp;
//         break;
//       }
//       case 2:
//       {
//         typedef WindowedSincInterpolateImageGaussianFunction<SingleImageType> GaussianType;
//         GaussianType::Pointer interp = GaussianType::New();
//         interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//         m_Interp = interp;
//         break;
//       }
//       case 3:
//       {
//         typedef WindowedSincInterpolateImageHammingFunction<SingleImageType> HammingType;
//         HammingType::Pointer interp = HammingType::New();
//         interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//         m_Interp = interp;
//         break;
//       }
//       case 4:
//       {
//         typedef WindowedSincInterpolateImageLanczosFunction<SingleImageType> LanczosType;
//         LanczosType::Pointer interp = LanczosType::New();
//         interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//         m_Interp = interp;
//         break;
//       }
//       case 5:
//       {
//         typedef WindowedSincInterpolateImageWelchFunction<SingleImageType> WelchType;
//         WelchType::Pointer interp = WelchType::New();
//         interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//         m_Interp = interp;
//         break;
//       }
//       default:
//         fl_alert("Problem with interpolator type, please contact developpers");
//         break;
//       }
//     }
//     else
//     {
//       fl_alert("Invalid ionterpolator Radius...");
//     }
//     break;
//   }
//   case SPLINES:
//   {
//     typedef otb::BSplineInterpolateImageFunction<SingleImageType, double, double> SplineType;
//     SplineType::Pointer interp = SplineType::New();
//     interp->SetSplineOrder(static_cast<unsigned int>(guiSplineOrder->value()) );
//     m_Interp = interp;

//     break;
//   }
//   default:
//     fl_alert("Problem with map projection type, please contact developpers");
//     res = 1;
//     break;
//   }
//   return res;
}


void
ProjectionModule::SelectAction()
{
//   bool outputDone = false;
//   bool containsKeywordlist = true;

//   ImagePointerType image = m_InputImage;
//   // test if the image has KeywordList
//   try
//   {
//     ForwardSensorType::Pointer sensor = ForwardSensorType::New();
//     sensor->SetImageGeometry( image->GetImageKeywordlist() );
//   }
//   catch ( itk::ExceptionObject & )
//   {
//     containsKeywordlist = false;
//   }

//   // If orthorectifed
//   if (outputDone==false && containsKeywordlist == true)
//   {
//     // Update Easting and northing values
//     ForwardSensorInputPointType point;
//     point[0] = image->GetLargestPossibleRegion().GetSize()[0]/2 + 1;
//     point[1] = image->GetLargestPossibleRegion().GetSize()[1]/2 + 1;
//     DoubleVectorType longLat = this->PointToLongLat( point );

//     itk::OStringStream oss;
//     oss<<setiosflags(std::ios_base::fixed);
//     oss.str("");
//     oss<<longLat[0];
//     guiLongSelection->value(oss.str().c_str());
//     oss.str("");
//     oss<<longLat[1];
//     guiLatSelection->value(oss.str().c_str());

//     // Upadate map parameters
//     this->UpdateMapParam();
//     // Store ref Zone and Hemisphere
//     m_UTMZoneRef = atoi(guiUTMZone->value());
//     m_UTMHemRef = 'N';
//     if (guiUTMSouth->value() == 1)
//       m_UTMHemRef = 'S';

//     // Update outputinformation
//     this->UpdateOutputParameters();

//     // Compute extent image
//     this->GenerateImageExtent();
//     // Show insight image
//     this->InsightExtent();

//     // Load the default interpolator
//     this->UpdateInterpolator();

//   }
//   else
//   {
//     guiUTMNorthSelection->value("");
//     guiLambertNorthSelection->value("");
//     guiTransmercatorNorthSelection->value("");
//     guiUTMEastSelection->value("");
//     guiLambertEastSelection->value("");
//     guiTransmercatorEastSelection->value("");

//     // Clean extent image
//     guiImageExtent->Clear();
//   }

}



void
ProjectionModule
::UpdateEastNorth()
{
//   switch (this->GetMapType())
//   {
//   case UTM:
//   {
//     m_EastNorth[1] = strtod(guiUTMNorthSelection->value(), NULL);
//     m_EastNorth[0] = strtod(guiUTMEastSelection->value(), NULL);
//     break;
//   }
//   case LAMBERT2:
//   {
//     m_EastNorth[1] = strtod(guiLambertNorthSelection->value(), NULL);
//     m_EastNorth[0] = strtod(guiLambertEastSelection->value(), NULL);
//   }
//   case TRANSMERCATOR:
//   {
//     m_EastNorth[1] = strtod(guiTransmercatorNorthSelection->value(), NULL);
//     m_EastNorth[0] = strtod(guiTransmercatorEastSelection->value(), NULL);
//     break;
//   }
//   default:
//     break;
//   }

//   this->Modified();
}


void
ProjectionModule::SetMapType(MapType map)
{
//   m_MapType = map;
//   this->SelectAction();
//   this->Modified();
}


MapType
ProjectionModule::GetMapType()
{
  return m_MapType;
}


void
ProjectionModule::SetInterpolatorType(InterpolatorType interp)
{
  m_InterpType = interp;
  this->UpdateInterpolator();
  this->Modified();
}


InterpolatorType
ProjectionModule::GetInterpolatorType()
{
  return m_InterpType;
}


// void
// ProjectionModule
// ::GenerateImageExtent()
// {
//   guiImageExtent->ClearPointList();
//   guiImageExtent->ClearLongLatList();
//   ImagePointerType image =  m_InputImage;
//   // test if the image has Keywordlist
//   try
//   {
//     ForwardSensorType::Pointer sensor = ForwardSensorType::New();
//     sensor->SetImageGeometry( image->GetImageKeywordlist() );
//   }
//   catch ( itk::ExceptionObject &  )
//   {
//     fl_alert("Invalid image : No ImageKeywordlist found");
//     return;
//   }
//   ForwardSensorInputPointType  point;

//   // upper left
//   point[0] = 0;
//   point[1] = 0;
//   guiImageExtent->AddPointToList( this->ImageToCarto(point) );
//   guiImageExtent->AddPointToLatLongList( this->PointToLongLat(point) );

//   // upper right
//   point[0] = image->GetLargestPossibleRegion().GetSize()[0]-1;
//   guiImageExtent->AddPointToList( this->ImageToCarto(point) );
//   guiImageExtent->AddPointToLatLongList( this->PointToLongLat( point) );

//   // lower right
//   point[1] = image->GetLargestPossibleRegion().GetSize()[1]-1;
//   guiImageExtent->AddPointToList( this->ImageToCarto(point) );
//   guiImageExtent->AddPointToLatLongList( this->PointToLongLat(point) );

//   // lower left
//   point[0] = 0;
//   guiImageExtent->AddPointToList( this->ImageToCarto(point) );
//   guiImageExtent->AddPointToLatLongList( this->PointToLongLat(point) );

//   guiImageExtent->ComputeBoundingBox();
//   guiImageExtent->redraw();
// }


// ProjectionModule::DoubleVectorType
// ProjectionModule::ImageToCarto(ForwardSensorInputPointType point)
// {
//   ForwardSensorInputPointType longLatDouble;
//   DoubleVectorType longLat = this->PointToLongLat(point);
//   longLatDouble[0] = longLat[0];
//   longLatDouble[1] = longLat[1];

//   return ( this->LongLatPointToCarto(longLatDouble) );

// }

} // End namespace otb

