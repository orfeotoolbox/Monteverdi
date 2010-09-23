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
#include "otbOrthorectification.h"

#include <FLU/Flu_File_Chooser.h>
#include <FL/Fl.H>

#include "otbI18n.h"
// #include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#include <FL/Enumerations.H>

#include "otbMacro.h"
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
Orthorectification::Orthorectification()
{
  this->CreateGUI();
  guiAvElev->value("0");
  guiTRANSMERCATOREast->value("0");
  guiTRANSMERCATORNorth->value("0");
  guiTRANSMERCATORScale->value("1");
  m_MainWindowInitSize[0] = guiMainWindow->w();
  m_MainWindowInitSize[1] = guiMainWindow->h();
  m_OutputSize.Fill(0);
  m_OutputOrigin.Fill(0);
  m_OutputSpacing.Fill(0);
  m_DEMSpacing.Fill(0);
  m_EastNorth.push_back(0);
  m_EastNorth.push_back(0);
  m_InterpType = LINEAR_;
  m_MapType = UTM;
  m_UTMZoneRef = 0;
  m_UTMHemRef = 'N';
  m_TileNumber = 0;
  m_MaxTileSize = 256.;
  guiShowDEM->hide();

  m_OrthoFilter = OrthorectificationFilterType::New();
  
  m_HasOutput = false;
}
void
Orthorectification
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}

void
Orthorectification
::Show()
{
  try
    {
    this->SelectAction();

    // test if the good parameters are available
    int resCheckImageParameters = this->CheckImageParameters();
    if (resCheckImageParameters == 1)
      {
      itkExceptionMacro(<< "Invalid image parameters");
      }

    guiMainWindow->show();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

void
Orthorectification
::OpenDEM()
{

  const char * cfname = NULL;

  // Choose directory
  cfname = flu_dir_chooser(otbGetTextMacro("Choose the dataset dir..."), "");

  if (cfname == NULL)
    {
    otbMsgDebugMacro(<< "Empty directory!");
    return;
    }

  // Check directory availability
  ossimElevManager * elevManager = ossimElevManager::instance();
  ossimFilename      ossimDEMDir;
  ossimDEMDir = ossimFilename(cfname);
  ossimDirectory od(cfname);
  if (!elevManager->loadElevationPath(ossimDEMDir))
    {
    MsgReporter::GetInstance()->SendError("Invalid directory, no DEM files found!");
    this->OpenDEM();
    return;

    }
  guiDEMPath->value(cfname);
}

void
Orthorectification
::Quit()
{
  guiMainWindow->hide();
}

void
Orthorectification
::OK()
{
  try
    {
    int resCheckImageParameters = this->CheckImageParameters();
    if (resCheckImageParameters == 1)
      {
      itkExceptionMacro(<< "Invalid image parameters");
      }

    this->ComputeTileNumber();
    int resCheckMapParameters = this->CheckMapParameters();

    if (resCheckMapParameters == 1)
      {
      itkExceptionMacro(<< "Invalid map parameters");
      }

    //m_HasOutput = true done in GenericCreateOutput
    this->NotifyAll();
    m_HasOutput = false;

    guiShowDEM->hide();
    guiMainWindow->hide();

    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
Orthorectification::ComputeTileNumber()
{
  // Compute the output image bounding box IN the image repere
  GUIRectangle::VectorVectorType pointList =  guiImageExtent->GetExtentList();
  GUIRectangle::IntVectorType    XBoundingBoxIdList = guiImageExtent->GetXBoundingBoxIdList();
  GUIRectangle::IntVectorType    YBoundingBoxIdList = guiImageExtent->GetYBoundingBoxIdList();

  //--- UL
  InverseSensorInputPointType cartoPoint0;
  cartoPoint0[0] = pointList[0][0];
  cartoPoint0[1] = pointList[0][1];
  //--- UR
  InverseSensorInputPointType cartoPoint1;
  cartoPoint1[0] = pointList[1][0];
  cartoPoint1[1] = pointList[1][1];
  //--- LR
  InverseSensorInputPointType cartoPoint2;
  cartoPoint2[0] = pointList[2][0];
  cartoPoint2[1] = pointList[2][1];
  //--- LL
  InverseSensorInputPointType cartoPoint3;
  cartoPoint3[0] = pointList[3][0];
  cartoPoint3[1] = pointList[3][1];

  std::vector<IntVectorType> imagePointList(4, this->CartoToImagePoint(cartoPoint0));
  imagePointList[1] = this->CartoToImagePoint(cartoPoint1);
  imagePointList[2] = this->CartoToImagePoint(cartoPoint2);
  imagePointList[3] = this->CartoToImagePoint(cartoPoint3);

  // Compute the bounding box of the output image bounding box IN the image repere
  int xmin = imagePointList[0][0];
  int ymin = imagePointList[0][1];
  int xmax = imagePointList[0][0];
  int ymax = imagePointList[0][1];

  for (unsigned int i = 1; i < imagePointList.size(); i++)
    {
    // Origins
    if (xmin > imagePointList[i][0]) xmin = imagePointList[i][0];

    if (ymin > imagePointList[i][1]) ymin = imagePointList[i][1];

    // Sizes
    if (xmax < imagePointList[i][0]) xmax = imagePointList[i][0];

    if (ymax < imagePointList[i][1]) ymax = imagePointList[i][1];
    }
  int w = std::abs(xmax - xmin);
  int h = std::abs(ymax - ymin);

  double outputInImageSize = static_cast<double>(w * h);
  double outputSize = strtod(guiSizeX->value(), NULL) * strtod(guiSizeY->value(), NULL);
  double bandNb = m_InputImage->GetNumberOfComponentsPerPixel();
  double internalPixelSize = 2 * 1e-6;   // 16 bits = 2 octet = 0.0000002 MO
  // note : Only 16 bits image are proposed in Monteverdi

  m_TileNumber =
    static_cast<int>(std::floor(2 * bandNb * internalPixelSize * (outputInImageSize + outputSize) / m_MaxTileSize + 0.5));

  if (m_TileNumber == 0)
    {
    m_TileNumber = 1;
    }
}

void
Orthorectification
::InsightExtent()
{
  if (this->CheckImageParameters() != 0)
    {
    return;
    }

  guiImageExtent->ClearExtentList();
  guiImageExtent->ClearExtentLongLatList();

  GUIRectangle::VectorType    extendBoxPoint(2, 0);
  ForwardSensorInputPointType longLatPoint, cartoPoint;

  longLatPoint[0] = strtod(guiLongSelection->value(), NULL);
  longLatPoint[1] = strtod(guiLatSelection->value(), NULL);
  DoubleVectorType cartoPt = LongLatPointToCartoInOriginRef(longLatPoint);

  // upper left
  if (guiCenterPixel->value() == 1)
    {
    extendBoxPoint[0] = cartoPt[0] - strtod(guiSizeX->value(), NULL) * strtod(guiSpacingX->value(), NULL) / 2;
    extendBoxPoint[1] = cartoPt[1] - strtod(guiSizeY->value(), NULL) * strtod(guiSpacingY->value(), NULL) / 2;
    }
  else
    {
    if (guiULPixel->value() == 1)
      {
      extendBoxPoint[0] = cartoPt[0];
      extendBoxPoint[1] = cartoPt[1];
      }
    }

  guiImageExtent->AddPointToExtentList(extendBoxPoint);
  cartoPoint[0] = extendBoxPoint[0];
  cartoPoint[1] = extendBoxPoint[1];
  guiImageExtent->AddPointToExtentLongLatList(this->CartoPointToLongLat(cartoPoint));

  extendBoxPoint[0] += strtod(guiSizeX->value(), NULL) * strtod(guiSpacingX->value(), NULL);
  guiImageExtent->AddPointToExtentList(extendBoxPoint);
  cartoPoint[0] = extendBoxPoint[0];
  guiImageExtent->AddPointToExtentLongLatList(this->CartoPointToLongLat(cartoPoint));

  extendBoxPoint[1] +=  strtod(guiSizeY->value(), NULL) * strtod(guiSpacingY->value(), NULL);
  guiImageExtent->AddPointToExtentList(extendBoxPoint);
  cartoPoint[1] = extendBoxPoint[1];
  guiImageExtent->AddPointToExtentLongLatList(this->CartoPointToLongLat(cartoPoint));

  extendBoxPoint[0] -= strtod(guiSizeX->value(), NULL) * strtod(guiSpacingX->value(), NULL);
  guiImageExtent->AddPointToExtentList(extendBoxPoint);
  cartoPoint[0] = extendBoxPoint[0];
  guiImageExtent->AddPointToExtentLongLatList(this->CartoPointToLongLat(cartoPoint));

  guiImageExtent->CreateInsightTooltip();
  guiImageExtent->redraw();
}

Orthorectification::DoubleVectorType
Orthorectification::PointToLongLat(ForwardSensorInputPointType point)
{
  ForwardSensorOutputPointType latLongPoint;
  DoubleVectorType             outVectPoint(2, 0);
  latLongPoint[0] = 0;
  latLongPoint[1] = 0;
  ImagePointerType           image = m_InputImage;
  ForwardSensorType::Pointer sensor = ForwardSensorType::New();
  sensor->SetImageGeometry(image->GetImageKeywordlist());

  latLongPoint = sensor->TransformPoint(point);
  outVectPoint[0] = static_cast<double>(latLongPoint[0]);
  outVectPoint[1] = static_cast<double>(latLongPoint[1]);

  return outVectPoint;
}

void
Orthorectification
::UpdateUTMParam()
{
  ImagePointerType            image = m_InputImage;
  ForwardSensorInputPointType point;
  // Take the mid image
  point[0] = image->GetLargestPossibleRegion().GetSize()[0] / 2 + 1;
  point[1] = image->GetLargestPossibleRegion().GetSize()[1] / 2 + 1;

  DoubleVectorType             latLongPointDouble = this->PointToLongLat(point);
  ForwardSensorOutputPointType latLongPoint;
  latLongPoint[0] = latLongPointDouble[0];
  latLongPoint[1] = latLongPointDouble[1];

  typedef UtmForwardProjection UtmProjectionType;
  UtmProjectionType::Pointer        utmProjection = UtmProjectionType::New();
  UtmProjectionType::InputPointType geoPoint;

  int utmZone = utmProjection->GetZoneFromGeoPoint(latLongPoint);

  itk::OStringStream oss;
  oss.str("");
  oss << utmZone;
  guiUTMZone->value(oss.str().c_str());
  if (latLongPoint[1] >= 0)
    {
    guiUTMNorth->value(1);
    guiUTMSouth->value(0);
    }
  else
    {
    guiUTMSouth->value(1);
    guiUTMNorth->value(0);
    }

  oss.str("");
  oss << setiosflags(std::ios_base::fixed);
  oss << latLongPoint[1];
  guiLongSelection->value(oss.str().c_str());
  oss.str("");
  oss << latLongPoint[0];
  guiLatSelection->value(oss.str().c_str());
}

void
Orthorectification
::UpdateMapParam()
{

  ImagePointerType image = m_InputImage;

  DoubleVectorType latLongPointDouble(2, 0);
  latLongPointDouble[0] = strtod(guiLongSelection->value(), NULL);
  latLongPointDouble[1] = strtod(guiLatSelection->value(), NULL);

  ForwardSensorOutputPointType latLongPoint;
  latLongPoint[0] = latLongPointDouble[0];
  latLongPoint[1] = latLongPointDouble[1];

  itk::OStringStream oss;
  oss << setiosflags(std::ios_base::fixed);
  oss.str("");

  switch (this->GetMapType())
    {
    case UTM:
      {
      typedef UtmForwardProjection UtmProjectionType;
      UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();

      int utmZone = utmProjection->GetZoneFromGeoPoint(latLongPoint);

      oss << utmZone;
      guiUTMZone->value(oss.str().c_str());
      if (latLongPoint[1] >= 0)
        {
        guiUTMNorth->value(1);
        guiUTMSouth->value(0);
        }
      else
        {
        guiUTMSouth->value(1);
        guiUTMNorth->value(0);
        }

      m_EastNorth = this->LongLatPointToCarto(latLongPoint);
      oss.str("");
      oss << m_EastNorth[1];
      guiUTMNorthSelection->value(oss.str().c_str());
      oss.str("");
      oss << m_EastNorth[0];
      guiUTMEastSelection->value(oss.str().c_str());
      break;
      }
    case LAMBERT2:
      {
      m_EastNorth = this->LongLatPointToCarto(latLongPoint);
      oss.str("");
      oss << m_EastNorth[1];
      guiLambertNorthSelection->value(oss.str().c_str());
      oss.str("");
      oss << m_EastNorth[0];
      guiLambertEastSelection->value(oss.str().c_str());
      break;
      }
    case LAMBERT3CARTOSUD:
      {
      m_EastNorth = this->LongLatPointToCarto(latLongPoint);
      oss.str("");
      oss << m_EastNorth[1];
      guiLambert3NorthSelection->value(oss.str().c_str());
      oss.str("");
      oss << m_EastNorth[0];
      guiLambert3EastSelection->value(oss.str().c_str());
      break;
      }
    case TRANSMERCATOR:
      {
      m_EastNorth = this->LongLatPointToCarto(latLongPoint);
      oss.str("");
      oss << m_EastNorth[1];
      guiTransmercatorNorthSelection->value(oss.str().c_str());
      oss.str("");
      oss << m_EastNorth[0];
      guiTransmercatorEastSelection->value(oss.str().c_str());
      break;
      }
    default:
      MsgReporter::GetInstance()->SendError("Problem with map projection type, please contact developpers");
      break;
    }
}

void
Orthorectification
::UpdateLongLat()
{
  ForwardSensorInputPointType point;
  point[0] = m_EastNorth[0];
  point[1] = m_EastNorth[1];
  DoubleVectorType   longLat = this->CartoPointToLongLat(point);
  itk::OStringStream oss;
  oss << setiosflags(std::ios_base::fixed);
  oss.str("");
  oss << longLat[0];
  guiLongSelection->value(oss.str().c_str());
  oss.str("");
  oss << longLat[1];
  guiLatSelection->value(oss.str().c_str());

  this->InsightExtent();
}

void
Orthorectification
::UpdateOutputParameters()
{
  ImagePointerType            image = m_InputImage;
  ForwardSensorInputPointType point;
  DoubleVectorType            coord;
  itk::OStringStream          oss;

  int imageSizeX = image->GetLargestPossibleRegion().GetSize()[0];
  int imageSizeY = image->GetLargestPossibleRegion().GetSize()[1];

  point[0] = 0;
  point[1] = 0;

  coord = this->ImageToCarto(point);
  // Store image corner coordinates
  std::vector<DoubleVectorType> coordList(4, coord);
  // Compute Sizes
  point[0] = imageSizeX - 1;
  point[1] = 0;
  coordList[1] = this->ImageToCarto(point);
  point[0] = imageSizeX - 1;
  point[1] = imageSizeY - 1;
  coordList[2] = this->ImageToCarto(point);
  point[0] = 0;
  point[1] = imageSizeY - 1;
  coordList[3] = this->ImageToCarto(point);

  // Compute min/max coordinate values
  double minX = coordList[0][0];
  double maxX = coordList[0][0];
  double minY = coordList[0][1];
  double maxY = coordList[0][1];

  // Store point position in the output image
  // calculation will be done from those indexeces to cares all possible rotations
  unsigned int up = 0;
  unsigned int down = 0;
  unsigned int left = 0;
  unsigned int right = 0;

  for (unsigned int i = 1; i < coordList.size(); i++)
    {
    // Origins
    if (minX > coordList[i][0])
      {
      minX = coordList[i][0];
      left = i;
      }

    if (minY > coordList[i][1])
      {
      minY = coordList[i][1];
      down = i;
      }

    // Sizes
    if (maxX < coordList[i][0])
      {
      maxX = coordList[i][0];
      right = i;
      }

    if (maxY < coordList[i][1])
      {
      maxY = coordList[i][1];
      up = i;
      }
    }
  // size image in carto coordinate :
  double sizeXcarto =  maxX - minX;
  // - because of the difference of origin for Y (image vs. carto)
  double sizeYcarto =  -(maxY - minY);

  double XfSpacing = 0.;
  double YfSpacing = 0.;
  int    sizeX = 0;
  int    sizeY = 0;
  //////////////////////////////// for X
  double alphaX1 =
    vcl_atan2(vcl_abs(coordList[right][1] - coordList[up][1]), vcl_abs(coordList[right][0] - coordList[up][0]));
  double alphaX2 =
    vcl_atan2(vcl_abs(coordList[left][1] - coordList[up][1]), vcl_abs(coordList[left][0] - coordList[up][0]));
  // spacing X
  XfSpacing = sizeXcarto /
              ((static_cast<double>(imageSizeX) *
                vcl_cos(alphaX1)) + (static_cast<double>(imageSizeY) * vcl_cos(alphaX2)));
  // sizeX
  sizeX = static_cast<int>(vcl_floor(std::abs(sizeXcarto / XfSpacing)));

  //////////////////////////////// for Y
  double alphaY1 =
    vcl_atan2(vcl_abs(coordList[up][1] - coordList[left][1]), vcl_abs(coordList[up][0] - coordList[left][0]));
  double alphaY2 =
    vcl_atan2(vcl_abs(coordList[down][1] - coordList[left][1]), vcl_abs(coordList[down][0] - coordList[left][0]));
  // spacing X
  // - because of the difference of origin for Y (image vs. carto)
  YfSpacing = sizeYcarto /
              ((static_cast<double>(imageSizeY) *
                vcl_cos(alphaY1)) + (static_cast<double>(imageSizeX) * vcl_cos(alphaY2)));
  // sizeY
  sizeY = static_cast<int>(vcl_floor(std::abs(sizeYcarto / YfSpacing)));

  oss.str("");
  oss << XfSpacing;
  guiSpacingX->value(oss.str().c_str());
  oss.str("");
  oss << YfSpacing;
  guiSpacingY->value(oss.str().c_str());

  oss.str("");
  oss << sizeX;
  guiSizeX->value(oss.str().c_str());
  oss.str("");
  oss << sizeY;
  guiSizeY->value(oss.str().c_str());
}

void
Orthorectification::UpdateDEMSpacing()
{
  double cartoSpacingX = strtod(guiSpacingX->value(), NULL);
  double cartoSpacingY = strtod(guiSpacingY->value(), NULL);
  double sizeX = strtod(guiSizeX->value(), NULL);
  double sizeY = strtod(guiSizeY->value(), NULL);

  ForwardSensorInputPointType point;
  point[0] = m_OutputOrigin[0];
  point[1] = m_OutputOrigin[1];
  DoubleVectorType ULLongLat = this->CartoPointToLongLat(point);
  point[0] += cartoSpacingX * sizeX;
  DoubleVectorType ULLongLatX = this->CartoPointToLongLat(point);

  point[0] = m_OutputOrigin[0];
  point[1] += cartoSpacingY * sizeY;
  DoubleVectorType ULLongLatY = this->CartoPointToLongLat(point);

  m_DEMSpacing[0] = (ULLongLatX[0] - ULLongLat[0]) / sizeX;
  m_DEMSpacing[1] = (ULLongLatY[1] - ULLongLat[1]) / sizeY;
}

int
Orthorectification::CheckImageParameters()
{
  try
    {
    ForwardSensorType::Pointer sensor = ForwardSensorType::New();
    sensor->SetImageGeometry(m_InputImage->GetImageKeywordlist());
    }
  catch (itk::ExceptionObject&)
    {
    return 1;
    }
  if (strcmp("", guiSizeX->value()) == 0 || strcmp("", guiSizeY->value()) == 0)
    {
    MsgReporter::GetInstance()->SendError("Please, set Size values...");
    return 1;
    }
  if (strcmp("", guiSpacingX->value()) == 0 || strcmp("", guiSpacingY->value()) == 0)
    {
    MsgReporter::GetInstance()->SendError("Please, set Spacing values...");
    return 1;
    }

  m_OutputSize[0] = static_cast<unsigned int>(strtod(guiSizeX->value(), NULL));
  m_OutputSize[1] = static_cast<unsigned int>(strtod(guiSizeY->value(), NULL));
  m_OutputSpacing[0] = strtod(guiSpacingX->value(), NULL);
  m_OutputSpacing[1] = strtod(guiSpacingY->value(), NULL);

  ForwardSensorInputPointType longLatPoint;
  longLatPoint[0] = strtod(guiLongSelection->value(), NULL);
  longLatPoint[1] = strtod(guiLatSelection->value(), NULL);
  DoubleVectorType cartoPoint = LongLatPointToCartoInOriginRef/*LongLatPointToCarto*/ (longLatPoint);

  // upper left
  if (guiCenterPixel->value() == 1)
    {
    m_OutputOrigin[0] = cartoPoint[0] - m_OutputSpacing[0] * m_OutputSize[0] / 2;
    m_OutputOrigin[1] = cartoPoint[1] - m_OutputSpacing[1] * m_OutputSize[1] / 2;
    }
  else
    {
    if (guiULPixel->value() == 1)
      {
      m_OutputOrigin[0] = cartoPoint[0];
      m_OutputOrigin[1] = cartoPoint[1];
      }
    }

  return 0;
}

int
Orthorectification
::CheckMapParameters()
{
  int res = 0;
  switch (this->GetMapType())
    {
    case UTM:
      {
      if (strcmp("", guiUTMZone->value()))
        {
        char hem = 'N';
        if (guiUTMSouth->value() == 1) hem = 'S';
        typedef UtmInverseProjection UtmProjectionType;
        UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();
        utmProjection->SetZone(atoi(guiUTMZone->value()));
        utmProjection->SetHemisphere(hem);
        m_CartographicProjectionRef = utmProjection->GetWkt();
        }
      else
        {
        itk::OStringStream oss;
        oss.str("Invalid Zone parameter: ");
        oss << guiUTMZone->value();
        MsgReporter::GetInstance()->SendError(oss.str().c_str());
        }
      break;
      }
    case LAMBERT2:
      {
      typedef Lambert2EtenduInverseProjection Lambert2Type;
      Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();
      m_CartographicProjectionRef = lambert2Projection->GetWkt();
      break;
      }
    case LAMBERT3CARTOSUD:
      {
      typedef Lambert3CartoSudInverseProjection Lambert3CartoSudType;
      Lambert3CartoSudType::Pointer lambert3CartoSudProjection = Lambert3CartoSudType::New();
      m_CartographicProjectionRef = lambert3CartoSudProjection->GetWkt();
      break;
      }
    case TRANSMERCATOR:
      {
      if (strcmp("", guiTRANSMERCATOREast->value()) != 0 && strcmp("", guiTRANSMERCATORNorth->value()) != 0)
        {
        if (strcmp("", guiTRANSMERCATORScale->value()) != 0)
          {
          typedef otb::TransMercatorInverseProjection TransMercatorProjectionType;
          TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();

          transMercatorProjection->SetParameters(strtod(guiTRANSMERCATOREast->value(), NULL),
                                                 strtod(guiTRANSMERCATORNorth->value(), NULL),
                                                 strtod(guiTRANSMERCATORScale->value(), NULL));
          m_CartographicProjectionRef = transMercatorProjection->GetWkt();
          }
        else
          {
          MsgReporter::GetInstance()->SendError("Invalid Scale Factor...");
          }
        }
      else
        {
        MsgReporter::GetInstance()->SendError("Please, set Easting and Northing values...");
        }
      break;
      }
    default:
      MsgReporter::GetInstance()->SendError("Problem with map projection type, please contact developpers");
      res = 1;
      break;
    } 
  
  // lauch the process
  this->CreateOutput();
  
  return res;
}

int
Orthorectification
::UpdateInterpolator()
{
  int res = 0;

  switch (this->GetInterpolatorType())
    {
    case LINEAR_:
      {
      typedef itk::LinearInterpolateImageFunction<ImageType, double> LinearType;
      LinearType::Pointer interp = LinearType::New();
      m_Interp = interp;
      break;
      }
    case NEAREST:
      {
      typedef itk::NearestNeighborInterpolateImageFunction<ImageType, double> NearestType;
      NearestType::Pointer interp = NearestType::New();
      m_Interp = interp;
      break;
      }
//     case SINC:
//       {
//       itk::OStringStream oss;
//       oss.str("");
//       oss << guiSincRadius->value();
//       if (strcmp("", oss.str().c_str()) != 0)
//         {
//         oss.str("");
//         oss << guiSincWindow->value();
//         switch (guiSincWindow->value())
//           {
//           case 0:
//             {
//             typedef WindowedSincInterpolateImageBlackmanFunction<SingleImageType> BlackmanType;
//             BlackmanType::Pointer interp = BlackmanType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             m_Interp = interp;
//             break;
//             }
//           case 1:
//             {

//             typedef WindowedSincInterpolateImageCosineFunction<SingleImageType> CosineType;
//             CosineType::Pointer interp = CosineType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             m_Interp = interp;
//             break;
//             }
//           case 2:
//             {
//             typedef WindowedSincInterpolateImageGaussianFunction<SingleImageType> GaussianType;
//             GaussianType::Pointer interp = GaussianType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             m_Interp = interp;
//             break;
//             }
//           case 3:
//             {
//             typedef WindowedSincInterpolateImageHammingFunction<SingleImageType> HammingType;
//             HammingType::Pointer interp = HammingType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             m_Interp = interp;
//             break;
//             }
//           case 4:
//             {
//             typedef WindowedSincInterpolateImageLanczosFunction<SingleImageType> LanczosType;
//             LanczosType::Pointer interp = LanczosType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             m_Interp = interp;
//             break;
//             }
//           case 5:
//             {
//             typedef WindowedSincInterpolateImageWelchFunction<SingleImageType> WelchType;
//             WelchType::Pointer interp = WelchType::New();
//             interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
//             m_Interp = interp;
//             break;
//             }
//           default:
//             MsgReporter::GetInstance()->SendError("Problem with interpolator type, please contact developpers");
//             break;
//           }
//         }
//       else
//         {
//         MsgReporter::GetInstance()->SendError("Invalid ionterpolator Radius...");
//         }
//       break;
//       }
//     case SPLINES:
//       {
//       typedef otb::BSplineInterpolateImageFunction<SingleImageType, double, double> SplineType;
//       SplineType::Pointer interp = SplineType::New();
//       interp->SetSplineOrder(static_cast<unsigned int>(guiSplineOrder->value()));
//       m_Interp = interp;

//       break;
//       }
    default:
      MsgReporter::GetInstance()->SendError("Problem with map projection type, please contact developpers");
      res = 1;
      break;
    }
  return res;
}

void
Orthorectification::
CreateOutput()
{
  IndexType start;
  start[0] = 0;
  start[1] = 0;

  OriginPointType origin = static_cast<OriginPointType>(m_OutputOrigin); 

  m_OrthoFilter->SetInput(m_InputImage);
  m_OrthoFilter->SetOutputStartIndex(start);
  m_OrthoFilter->SetOutputSize(m_OutputSize);
  m_OrthoFilter->SetOutputSpacing(m_OutputSpacing);
  m_OrthoFilter->SetOutputOrigin(origin);
  m_OrthoFilter->SetOutputProjectionRef(m_CartographicProjectionRef);
  m_OrthoFilter->SetInterpolator(m_Interp);

  // Estimate the input rpc model
  if(guiRPCEstimator->value())
    {
    m_OrthoFilter->EstimateInputRpcModelOn();
    m_OrthoFilter->SetInputRpcGridSize(20);
    }
  
  // Set DEM or elvation average value.
  if (guiUseAverageElevation->value() == 1 && guiUseDEM->value() == 0)
    {
    m_OrthoFilter->SetAverageElevation(strtod(guiAvElev->value(), NULL));
    }
  if (guiUseDEM->value() == 1 && guiUseAverageElevation->value() == 0)
    {
    if (strcmp("", guiDEMPath->value()) == 0)
      {
      MsgReporter::GetInstance()->SendError("Please, select DEM directory.");
      }
    else
      {
      m_OrthoFilter->SetDEMDirectory(guiDEMPath->value());
      }
    }

  m_Output = m_OrthoFilter->GetOutput();
  m_HasOutput = true;
}

void
Orthorectification::SelectAction()
{
  bool outputDone = false;
  bool containsKeywordlist = true;

  ImagePointerType image = m_InputImage;
  // test if the image has KeywordList
  try
    {
    ForwardSensorType::Pointer sensor = ForwardSensorType::New();
    sensor->SetImageGeometry(image->GetImageKeywordlist());
    }
  catch (itk::ExceptionObject&)
    {
    containsKeywordlist = false;
    }

  // If orthorectifed
  if (outputDone == false && containsKeywordlist == true)
    {
    // Update Easting and northing values
    ForwardSensorInputPointType point;
    point[0] = image->GetLargestPossibleRegion().GetSize()[0] / 2 + 1;
    point[1] = image->GetLargestPossibleRegion().GetSize()[1] / 2 + 1;
    DoubleVectorType longLat = this->PointToLongLat(point);

    itk::OStringStream oss;
    oss << setiosflags(std::ios_base::fixed);
    oss.str("");
    oss << longLat[0];
    guiLongSelection->value(oss.str().c_str());
    oss.str("");
    oss << longLat[1];
    guiLatSelection->value(oss.str().c_str());

    // Upadate map parameters
    this->UpdateMapParam();
    // Store ref Zone and Hemisphere
    m_UTMZoneRef = atoi(guiUTMZone->value());
    m_UTMHemRef = 'N';
    if (guiUTMSouth->value() == 1) m_UTMHemRef = 'S';

    // Update outputinformation
    this->UpdateOutputParameters();

    // Compute extent image
    this->GenerateImageExtent();
    // Show insight image
    this->InsightExtent();

    // Load the default interpolator
    this->UpdateInterpolator();

    }
  else
    {
    guiUTMNorthSelection->value("");
    guiLambertNorthSelection->value("");
    guiLambert3NorthSelection->value("");
    guiTransmercatorNorthSelection->value("");
    guiUTMEastSelection->value("");
    guiLambertEastSelection->value("");
    guiLambert3EastSelection->value("");
    guiTransmercatorEastSelection->value("");

    // Clean extent image
    guiImageExtent->Clear();
    }

}

void
Orthorectification
::UpdateEastNorth()
{
  switch (this->GetMapType())
    {
    case UTM:
      {
      m_EastNorth[1] = strtod(guiUTMNorthSelection->value(), NULL);
      m_EastNorth[0] = strtod(guiUTMEastSelection->value(), NULL);
      break;
      }
    case LAMBERT2:
      {
      m_EastNorth[1] = strtod(guiLambertNorthSelection->value(), NULL);
      m_EastNorth[0] = strtod(guiLambertEastSelection->value(), NULL);
      }
    case LAMBERT3CARTOSUD:
      {
      m_EastNorth[1] = strtod(guiLambert3NorthSelection->value(), NULL);
      m_EastNorth[0] = strtod(guiLambert3EastSelection->value(), NULL);
      }
    case TRANSMERCATOR:
      {
      m_EastNorth[1] = strtod(guiTransmercatorNorthSelection->value(), NULL);
      m_EastNorth[0] = strtod(guiTransmercatorEastSelection->value(), NULL);
      break;
      }
    default:
      break;
    }

  this->Modified();
}

void
Orthorectification::SetMapType(MapType map)
{
  m_MapType = map;
  this->SelectAction();
  this->Modified();
}

MapType
Orthorectification::GetMapType()
{
  return m_MapType;
}

void
Orthorectification::SetInterpolatorType(InterpolatorType interp)
{
  m_InterpType = interp;
  this->UpdateInterpolator();
  this->Modified();
}

InterpolatorType
Orthorectification::GetInterpolatorType()
{
  return m_InterpType;
}

void
Orthorectification
::GenerateImageExtent()
{
  guiImageExtent->ClearPointList();
  guiImageExtent->ClearLongLatList();
  ImagePointerType image =  m_InputImage;
  // test if the image has Keywordlist
  try
    {
    ForwardSensorType::Pointer sensor = ForwardSensorType::New();
    sensor->SetImageGeometry(image->GetImageKeywordlist());
    }
  catch (itk::ExceptionObject&)
    {
    MsgReporter::GetInstance()->SendError("Invalid image : No ImageKeywordlist found");
    return;
    }
  ForwardSensorInputPointType point;

  // upper left
  point[0] = 0;
  point[1] = 0;
  guiImageExtent->AddPointToList(this->ImageToCarto(point));
  guiImageExtent->AddPointToLatLongList(this->PointToLongLat(point));

  // upper right
  point[0] = image->GetLargestPossibleRegion().GetSize()[0] - 1;
  guiImageExtent->AddPointToList(this->ImageToCarto(point));
  guiImageExtent->AddPointToLatLongList(this->PointToLongLat(point));

  // lower right
  point[1] = image->GetLargestPossibleRegion().GetSize()[1] - 1;
  guiImageExtent->AddPointToList(this->ImageToCarto(point));
  guiImageExtent->AddPointToLatLongList(this->PointToLongLat(point));

  // lower left
  point[0] = 0;
  guiImageExtent->AddPointToList(this->ImageToCarto(point));
  guiImageExtent->AddPointToLatLongList(this->PointToLongLat(point));

  guiImageExtent->ComputeBoundingBox();
  guiImageExtent->redraw();
}

Orthorectification::DoubleVectorType
Orthorectification::ImageToCarto(ForwardSensorInputPointType point)
{
  ForwardSensorInputPointType longLatDouble;
  DoubleVectorType            longLat = this->PointToLongLat(point);
  longLatDouble[0] = longLat[0];
  longLatDouble[1] = longLat[1];

  return (this->LongLatPointToCarto(longLatDouble));

}

Orthorectification::DoubleVectorType
Orthorectification::LongLatPointToCarto(ForwardSensorInputPointType latLongPoint)
{
  DoubleVectorType outVectPoint(2, 0);

  // geo to carto point
  switch (this->GetMapType())
    {
    case UTM:
      {
      if (strcmp("", guiUTMZone->value()))
        {
        char hem = 'N';
        if (guiUTMSouth->value() == 1) hem = 'S';
        typedef UtmForwardProjection UtmForwardProjectionType;
        UtmForwardProjectionType::Pointer utmProjection = UtmForwardProjectionType::New();
        utmProjection->SetZone(atoi(guiUTMZone->value()));
        utmProjection->SetHemisphere(hem);

        UtmForwardProjectionType::OutputPointType cartoPoint;
        cartoPoint = utmProjection->TransformPoint(latLongPoint);

        outVectPoint[0] = static_cast<double>(cartoPoint[0]);
        outVectPoint[1] = static_cast<double>(cartoPoint[1]);
        }
      break;
      }
    case LAMBERT2:
      {
      typedef Lambert2EtenduForwardProjection Lambert2Type;
      Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();

      Lambert2Type::OutputPointType cartoPoint;
      cartoPoint = lambert2Projection->TransformPoint(latLongPoint);
      
      outVectPoint[0] = static_cast<double>(cartoPoint[0]);
      outVectPoint[1] = static_cast<double>(cartoPoint[1]);

      break;
      }
    case LAMBERT3CARTOSUD:
      {
      typedef Lambert3CartoSudForwardProjection Lambert3CartoSudType;
      Lambert3CartoSudType::Pointer lambert3CartoSudProjection = Lambert3CartoSudType::New();

      Lambert3CartoSudType::OutputPointType cartoPoint;
      cartoPoint = lambert3CartoSudProjection->TransformPoint(latLongPoint);

      outVectPoint[0] = static_cast<double>(cartoPoint[0]);
      outVectPoint[1] = static_cast<double>(cartoPoint[1]);

      break;
      }
    case TRANSMERCATOR:
      {
      if (strcmp("", guiTRANSMERCATOREast->value()) != 0 && strcmp("", guiTRANSMERCATORNorth->value()) != 0)
        {
        if (strcmp("", guiTRANSMERCATORScale->value()) != 0)
          {
          typedef otb::TransMercatorForwardProjection TransMercatorProjectionType;
          TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();

          transMercatorProjection->SetParameters(strtod(guiTRANSMERCATOREast->value(), NULL),
                                                 strtod(guiTRANSMERCATORNorth->value(), NULL),
                                                 strtod(guiTRANSMERCATORScale->value(), NULL));

          TransMercatorProjectionType::OutputPointType cartoPoint;
          cartoPoint = transMercatorProjection->TransformPoint(latLongPoint);
          outVectPoint[0] = static_cast<double>(cartoPoint[0]);
          outVectPoint[1] = static_cast<double>(cartoPoint[1]);
          }
        }
      break;
      }
    default:
      MsgReporter::GetInstance()->SendError("Problem with map projection type, please contact developpers");
      break;
    }

  return outVectPoint;
}

Orthorectification::DoubleVectorType
Orthorectification::LongLatPointToCartoInOriginRef(ForwardSensorInputPointType latLongPoint)
{
  DoubleVectorType outVectPoint(2, 0);

  // geo to carto point
  if (this->GetMapType() == UTM)
    {
    typedef UtmForwardProjection UtmForwardProjectionType;
    UtmForwardProjectionType::Pointer utmProjection = UtmForwardProjectionType::New();
    utmProjection->SetZone(m_UTMZoneRef);
    utmProjection->SetHemisphere(m_UTMHemRef);

    UtmForwardProjectionType::OutputPointType cartoPoint;
    cartoPoint = utmProjection->TransformPoint(latLongPoint);

    outVectPoint[0] = static_cast<double>(cartoPoint[0]);
    outVectPoint[1] = static_cast<double>(cartoPoint[1]);
    }
  else
    {
    outVectPoint = this->LongLatPointToCarto(latLongPoint);
    }

  return outVectPoint;
}

Orthorectification::IntVectorType
Orthorectification::CartoToImagePoint(InverseSensorInputPointType cartoPoint)
{
  ImagePointerType image =  m_InputImage;

  IntVectorType    outIntVectPoint(2, 0);
  DoubleVectorType outVectPoint(2, 0);

  InverseSensorType::Pointer sensor = InverseSensorType::New();
  sensor->SetImageGeometry(image->GetImageKeywordlist());

  // geo to carto point
  switch (this->GetMapType())
    {
    case UTM:
      {
      if (strcmp("", guiUTMZone->value()))
        {
        char hem = 'N';
        if (guiUTMSouth->value() == 1) hem = 'S';
        typedef UtmInverseProjection UtmInverseProjectionType;
        UtmInverseProjectionType::Pointer utmProjection = UtmInverseProjectionType::New();
        utmProjection->SetZone(atoi(guiUTMZone->value()));
        utmProjection->SetHemisphere(hem);

        typedef CompositeTransform<UtmInverseProjectionType, InverseSensorType> TransformType;
        TransformType::Pointer transform = TransformType::New();
        transform->SetFirstTransform(utmProjection);
        transform->SetSecondTransform(sensor);
        TransformType::SecondTransformOutputPointType outPoint;
        outPoint = transform->TransformPoint(cartoPoint);

        outVectPoint[0] = static_cast<double>(std::floor(outPoint[0]));
        outVectPoint[1] = static_cast<double>(std::floor(outPoint[1]));
        }
      break;
      }
    case LAMBERT2:
      {
      typedef Lambert2EtenduInverseProjection Lambert2Type;
      Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();

      typedef CompositeTransform<Lambert2Type, InverseSensorType> TransformType;
      TransformType::Pointer transform = TransformType::New();
      transform->SetFirstTransform(lambert2Projection);
      transform->SetSecondTransform(sensor);
      TransformType::SecondTransformOutputPointType outPoint;
      outPoint = transform->TransformPoint(cartoPoint);

      outVectPoint[0] = static_cast<double>(std::floor(outPoint[0]));
      outVectPoint[1] = static_cast<double>(std::floor(outPoint[1]));

      break;
      }
    case LAMBERT3CARTOSUD:
      {
      typedef Lambert3CartoSudInverseProjection Lambert3CartoSudType;
      Lambert3CartoSudType::Pointer lambert3CartoSudProjection = Lambert3CartoSudType::New();

      typedef CompositeTransform<Lambert3CartoSudType, InverseSensorType> TransformType;
      TransformType::Pointer transform = TransformType::New();
      transform->SetFirstTransform(lambert3CartoSudProjection);
      transform->SetSecondTransform(sensor);
      TransformType::SecondTransformOutputPointType outPoint;
      outPoint = transform->TransformPoint(cartoPoint);

      outVectPoint[0] = static_cast<double>(std::floor(outPoint[0]));
      outVectPoint[1] = static_cast<double>(std::floor(outPoint[1]));

      break;
      }
    case TRANSMERCATOR:
      {
      if (strcmp("", guiTRANSMERCATOREast->value()) != 0 && strcmp("", guiTRANSMERCATORNorth->value()) != 0)
        {
        if (strcmp("", guiTRANSMERCATORScale->value()) != 0)
          {
          typedef otb::TransMercatorInverseProjection TransMercatorProjectionType;
          TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();

          transMercatorProjection->SetParameters(strtod(guiTRANSMERCATOREast->value(), NULL),
                                                 strtod(guiTRANSMERCATORNorth->value(), NULL),
                                                 strtod(guiTRANSMERCATORScale->value(), NULL));

          typedef CompositeTransform<TransMercatorProjectionType, InverseSensorType> TransformType;
          TransformType::Pointer transform = TransformType::New();
          transform->SetFirstTransform(transMercatorProjection);
          transform->SetSecondTransform(sensor);
          TransformType::SecondTransformOutputPointType outPoint;
          outPoint = transform->TransformPoint(cartoPoint);

          outVectPoint[0] = static_cast<double>(std::floor(outPoint[0]));
          outVectPoint[1] = static_cast<double>(std::floor(outPoint[1]));
          }
        }
      break;
      }
    default:
      MsgReporter::GetInstance()->SendError("Problem with map projection type, please contact developpers");
      break;
    }

  outIntVectPoint[0] = std::max(0, static_cast<int>(outVectPoint[0]));
  outIntVectPoint[0] = std::min(outIntVectPoint[0], static_cast<int>(image->GetLargestPossibleRegion().GetSize()[0] - 1));
  outIntVectPoint[1] = std::max(0, static_cast<int>(outVectPoint[1]));
  outIntVectPoint[1] = std::min(outIntVectPoint[1], static_cast<int>(image->GetLargestPossibleRegion().GetSize()[1] - 1));

  return outIntVectPoint;
}

Orthorectification::DoubleVectorType
Orthorectification::CartoPointToLongLat(ForwardSensorInputPointType cartoPoint)
{
  DoubleVectorType outVectPoint(2, 0);

  // geo to carto point
  switch (this->GetMapType())
    {
    case UTM:
      {
      if (strcmp("", guiUTMZone->value()))
        {
        char hem = 'N';
        if (guiUTMSouth->value() == 1) hem = 'S';
        typedef UtmInverseProjection UtmInverseProjectionType;
        UtmInverseProjectionType::Pointer utmProjection = UtmInverseProjectionType::New();
        utmProjection->SetZone(atoi(guiUTMZone->value()));
        utmProjection->SetHemisphere(hem);

        UtmInverseProjectionType::OutputPointType latLongPoint;
        latLongPoint = utmProjection->TransformPoint(cartoPoint);
        outVectPoint[0] = static_cast<double>(latLongPoint[0]);
        outVectPoint[1] = static_cast<double>(latLongPoint[1]);
        }
      break;
      }
    case LAMBERT2:
      {
      typedef Lambert2EtenduInverseProjection Lambert2Type;
      Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();

      Lambert2Type::OutputPointType latLongPoint;
      latLongPoint = lambert2Projection->TransformPoint(cartoPoint);

      outVectPoint[0] = static_cast<double>(latLongPoint[0]);
      outVectPoint[1] = static_cast<double>(latLongPoint[1]);

      break;
      }
    case LAMBERT3CARTOSUD:
      {
      typedef Lambert3CartoSudInverseProjection Lambert3CartoSudType;
      Lambert3CartoSudType::Pointer lambert3CartoSudProjection = Lambert3CartoSudType::New();

      Lambert3CartoSudType::OutputPointType latLongPoint;
      latLongPoint = lambert3CartoSudProjection->TransformPoint(cartoPoint);

      outVectPoint[0] = static_cast<double>(latLongPoint[0]);
      outVectPoint[1] = static_cast<double>(latLongPoint[1]);

      break;
      }
    case TRANSMERCATOR:
      {
      if (strcmp("", guiTRANSMERCATOREast->value()) != 0 && strcmp("", guiTRANSMERCATORNorth->value()) != 0)
        {
        if (strcmp("", guiTRANSMERCATORScale->value()) != 0)
          {
          typedef otb::TransMercatorInverseProjection TransMercatorProjectionType;
          TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();

          transMercatorProjection->SetParameters(strtod(guiTRANSMERCATOREast->value(), NULL),
                                                 strtod(guiTRANSMERCATORNorth->value(), NULL),
                                                 strtod(guiTRANSMERCATORScale->value(), NULL));

          TransMercatorProjectionType::OutputPointType latLongPoint;
          latLongPoint = transMercatorProjection->TransformPoint(cartoPoint);
          outVectPoint[0] = static_cast<double>(latLongPoint[0]);
          outVectPoint[1] = static_cast<double>(latLongPoint[1]);
          }
        }
      break;
      }
    default:
      MsgReporter::GetInstance()->SendError("Problem with map projection type, please contact developpers");
      break;
    }

  return outVectPoint;
}

/**
 * PrintSelf Method
 */
void
Orthorectification::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // End namespace otb
