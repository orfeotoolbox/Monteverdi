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

#include "otbTileMapImportModule.h"
#include "otbTileMapImageIOHelper.h"

namespace otb
{
/** Constructor */
TileMapImportModule::TileMapImportModule()
{
  // Init
  m_PlaceName = "";
  m_CountryName = "";

  m_Latitude = -1000;
  m_Longitude = -1000;

  m_Depth = 1;

  m_SizeX = 500;
  m_SizeY = 500;

  m_ServerName = "http://tile.openstreetmap.org/";

  // Use expand because CurlHelper can have pb when the path is relative
  ossimFilename changeDir = "./Caching";
  m_CacheDirectory = changeDir.expand();

  // Create the caching dir if not already created
  ossimFilename cachingDir(m_CacheDirectory);
  cachingDir.createDirectory();

  m_OutputKey = "OutputImage";

  // Build GUI
  this->BuildGUI();

  // Initialize Input Placename textedit
  std::string empty_str = "";
  vPlacename->value(empty_str.c_str());

  // Initialize Depth choice
  TileMapImageIOHelper::Pointer myTileMapImageIOHelper;
  myTileMapImageIOHelper = TileMapImageIOHelper::New();
  for (int i = 1; i <= 18; i++)
    {
    std::ostringstream label;
    label << i;
    vDepth->add(myTileMapImageIOHelper->ConvertDepthToScale(i).c_str(), "", NULL);
    }

  // Initialize server choice
  std::string servername;
  servername = "Open Street Map";    // http://tile.openstreetmap.org/
  vServername->add(servername.c_str(), "", NULL);
  servername = "Near Map";   // http://www.nearmap.com/maps/
  vServername->add(servername.c_str(), "", NULL);
  servername = "Hill Shading";   // http://tile.orfeo-toolbox.org/hillShade/
  vServername->add(servername.c_str(), "", NULL);
  vServername->value(0);

  vDepth->value(m_Depth - 1);
  vSizeX->value(m_SizeX);
  vSizeY->value(m_SizeY);

}

/** Destructor */
TileMapImportModule::~TileMapImportModule()
{
  this->Cancel();
}

/** PrintSelf method */
void TileMapImportModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void TileMapImportModule::Run()
{
  wTileMapImportWindow->show();
}

/** Cancel */
void TileMapImportModule::Cancel()
{
  wTileMapImportWindow->hide();
}
/** Ok */
void TileMapImportModule::Ok()
{
  try
    {
    std::string lFileSuffix;
    std::string lAddressMode;

    // Get ServerName
    switch (vServername->value())
      {
      // Open street map
      case 0:
        {
        m_ServerName = "http://tile.openstreetmap.org/";
        lFileSuffix = "png";
        lAddressMode = "1";
        m_CacheDirectory += "/OSM";
        m_OutputKey = "OpenStreetMap-";
        break;
        }
      case 1:
        {
        m_ServerName = "http://www.nearmap.com/maps/";
        lFileSuffix = "jpg";
        lAddressMode = "2";
        m_CacheDirectory += "/NM";
        m_OutputKey = "NearMap-";
        break;
        }
      case 2:
        {
        m_ServerName = "http://tile.orfeo-toolbox.org/hillShade/";
        lFileSuffix = "jpg";
        lAddressMode = "3";
        m_CacheDirectory += "/OTBHS";
        m_OutputKey = "otbHS-";
        break;
        }
      // Open street map par défaut
      default:
        {
        m_ServerName = "http://tile.openstreetmap.org/";
        lFileSuffix = "png";
        lAddressMode = "1";
        m_CacheDirectory += "/OSM";
        m_OutputKey = "OpenStreetMap-";
        break;
        }
      }

    // Get parameters
    m_Depth = static_cast<unsigned int>(vDepth->value() + 1);   // when vDepth = 0 Depth = 1;
    m_SizeX = static_cast<unsigned int>(vSizeX->value());
    m_SizeY = static_cast<unsigned int>(vSizeY->value());

    if (m_Longitude == -1000 && m_Latitude == -1000)
      {
      m_Longitude = static_cast<double> (vLongitude->value());
      m_Latitude = static_cast<double> (vLatitude->value());

      this->UpdatePlace();
      }

    if (m_PlaceName == "")
      {
      this->UpdatePlace();
      }

    // Generate Output Key
    if (m_PlaceName != "")
      {
      m_OutputKey += m_PlaceName;
      }
    else
      {
      std::ostringstream latlon;
      latlon << m_Longitude;
      latlon << ",";
      latlon << m_Latitude;
      m_OutputKey += latlon.str();
      }

    std::ostringstream sdepth;
    sdepth << m_Depth;

    m_OutputKey += "-";
    m_OutputKey += sdepth.str();

    // Create ImageIO and Reader
    m_TileIO      = ImageIOType::New();
    m_ReaderTile  = ReaderType::New();

    // Configure TileIO
    m_TileIO->SetDepth(m_Depth);
    m_TileIO->SetCacheDirectory(m_CacheDirectory);

    // Configure reader
    m_ReaderTile->SetFileName(m_ServerName);
    m_ReaderTile->SetImageIO(m_TileIO);
    m_ReaderTile->UpdateOutputInformation();

    // Create Model
    m_Model = ModelType::New();

    // Configure m_Model
    bool resModel = m_Model->SetImageGeometry(m_ReaderTile->GetOutput()->GetImageKeywordlist());
    if (!resModel)
      {
      itkExceptionMacro(<< "Unable to create projection.");
      }

    dynamic_cast<ossimplugins::ossimTileMapModel*>(m_Model->GetOssimModel())->setDepth(m_Depth);
    // Set lon/lat
    PointType lonLatPoint;
    lonLatPoint[0]  = m_Longitude;
    lonLatPoint[1]  = m_Latitude;

    // Transform lon/lat to pixel
    PointType tilePoint;
    tilePoint = m_Model->TransformPoint(lonLatPoint);

    // Set parameters for extract ROI
    long int startX = static_cast<long int>(tilePoint[0]);
    long int startY = static_cast<long int>(tilePoint[1]);
    long int sizeX = m_SizeX;
    long int sizeY = m_SizeY;

    // Create extract ROI
    m_ExtractROIOsmFilter = ExtractROIFilterType::New();

    RegionType           region;
    ImageType::IndexType index;
    ImageType::SizeType  size;

    // Configure extract ROI
    m_ExtractROIOsmFilter->SetStartX(startX - sizeX / 2);
    m_ExtractROIOsmFilter->SetStartY(startY - sizeY / 2);
    m_ExtractROIOsmFilter->SetSizeX(sizeX);
    m_ExtractROIOsmFilter->SetSizeY(sizeY);

    size[0] = sizeX;
    size[1] = sizeY;
    index[0] = startX - sizeX / 2;
    index[1] = startY - sizeY / 2;

    region.SetSize(size);
    region.SetIndex(index);

    region.Crop(m_ReaderTile->GetOutput()->GetLargestPossibleRegion());

    m_ExtractROIOsmFilter->SetExtractionRegion(region);
    m_ExtractROIOsmFilter->SetInput(m_ReaderTile->GetOutput());

    this->ClearOutputDescriptors();
    this->AddOutputDescriptor(m_ExtractROIOsmFilter->GetOutput(), m_OutputKey, otbGetTextMacro("Image extracted"));
    this->NotifyOutputsChange();

    wTileMapImportWindow->hide();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

/** SearchPlace */
void TileMapImportModule::SearchPlace()
{
  // Get placename from fluid
  m_PlaceName = vPlacename->value();

  // Check placename
  if (m_PlaceName == "")
    {
    itkDebugMacro(<< "Placename is empty");
    }
  else
    {
    // Create placenameToLonLat object
    m_PlaceNameToLonLat = PlaceNameToLonLatType::New();

    // Set placename & evaluate lat/long
    m_PlaceNameToLonLat->SetPlaceName(m_PlaceName);
    if (m_PlaceNameToLonLat->Evaluate())
      {
      // Get Lat/Lon
      m_Longitude = m_PlaceNameToLonLat->GetLon();
      m_Latitude  = m_PlaceNameToLonLat->GetLat();

      // Set Lat/Lon in UI
      vLongitude->value(m_Longitude);
      vLatitude->value(m_Latitude);
      }
    else
      {
      itkDebugMacro(<< "Error when fetching lat/lon.");
      }
    }
}

/** Update place */
void TileMapImportModule::UpdatePlace()
{
  // Get coordinates
  m_Longitude = static_cast<double>(vLongitude->value());
  m_Latitude  = static_cast<double>(vLatitude->value());

  if ((m_Longitude < -1000) || (m_Longitude > 1000) || (m_Latitude < -1000) || (m_Latitude > 1000))
    {
    itkDebugMacro(<< "Latitude or Longitude is wrong.");
    }
  else
    {
    // Create coordinateToName object
    m_CoordinateToName  = CoordinateToNameType::New();

    // Set lon/lat & evaluate name
    m_CoordinateToName->SetLon(m_Longitude);
    m_CoordinateToName->SetLat(m_Latitude);
    if (m_CoordinateToName->Evaluate())
      {
      // Get placename and country name
      m_PlaceName   = m_CoordinateToName->GetPlaceName();
      m_CountryName = m_CoordinateToName->GetCountryName();

      // Set placename in UI
      std::ostringstream l_PlaceAndCountryName;

      l_PlaceAndCountryName << m_PlaceName;
      l_PlaceAndCountryName << " ";
      l_PlaceAndCountryName << m_CountryName;
      vPlacename->value(l_PlaceAndCountryName.str().c_str());
      }
    else
      {
      itkDebugMacro(<< "No resuslts for this Lat/Lon.");
      }
    }
}

} // End namespace otb
