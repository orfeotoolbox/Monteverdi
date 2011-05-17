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

#include "otbGCPToSensorModelModel.h"
#include "otbFltkFilterWatcher.h"
#include "otbTileMapTransform.h"
#include "otbCachingPathManager.h"

#include "tinyxml.h"

#ifdef OTB_USE_CURL
#include "otbPlaceNameToLonLat.h"
#include "otbCoordinateToName.h"
#endif

namespace otb
{

GCPToSensorModelModel::
GCPToSensorModelModel() : m_GCPsToRPCSensorModelImageFilter(), m_GCPsContainer(), m_ErrorsContainer(),
  m_GroundError(0.), m_MeanError(0.), m_VisualizationModel(), m_ImageGenerator(),
  m_BlendingFunction(), m_InputImage(), m_Output(), m_OutputChanged(false),
  m_DEMPath(""), m_ElevMgt(GCP), m_MeanElevation(0.), m_DEMHandler(),
  m_GCPsContainerHasChanged(false), m_MapReader(), m_TileIO(), m_PlaceName(""),
  m_Latitude(0.), m_Longitude(0.), m_Depth(2), m_SelectedLatitude(0.),
  m_SelectedLongitude(0.), m_SizeX(0), m_SizeY(0), m_Region(),
  m_ServerName("http://tile.openstreetmap.org/"),
  m_CacheDirectory(""), m_MapVisualizationModel(), m_MapImageGenerator(),
  m_MapBlendingFunction(), m_PlaceNameChanged(false), m_LatLongChanged(false),
  m_DepthChanged(false), m_HasNewMap(false), m_SelectedPointChanged(false),
  m_CrossIndexesContainer()
{
  // Visualization
  m_VisualizationModel  = VisualizationModelType::New();
  m_BlendingFunction    = BlendingFunctionType::New();
  m_ImageGenerator      = LayerGeneratorType::New();

  m_BlendingFunction->SetAlpha(0.6);

  // Input & Output
  m_InputImage = VectorImageType::New();
  m_Output     = VectorImageType::New();

  // DEM
  m_DEMHandler = DEMHandler::New();

  // Filter
  m_GCPsToRPCSensorModelImageFilter = GCPsToRPCSensorModelImageFilterType::New();

  // Map
  m_MapVisualizationModel = VisualizationModelType::New();
  m_MapBlendingFunction   = BlendingFunctionType::New();
  m_MapImageGenerator     = MapLayerGeneratorType::New();

  m_MapBlendingFunction->SetAlpha(0.6);

  // Use expand because CurlHelper can have pb when the path is relative
  CachingPathManager::GetInstance()->GetAValidCachingPath();
  m_CacheDirectory = CachingPathManager::GetInstance()->GetFullCachingPath();
  CachingPathManager::GetInstance()->CreateCachingDirectory();

  // Configure Tile
  m_TileIO = TileMapType::New();
}

GCPToSensorModelModel
::~GCPToSensorModelModel()
{
}

void GCPToSensorModelModel::
Notify(ListenerBase * listener)
{
  listener->Notify();
}

void
GCPToSensorModelModel
::SetImage(VectorImagePointerType image)
{
  m_OutputChanged = false;

  image->UpdateOutputInformation();
  m_InputImage = image;

  // Add input image to the filter
  m_GCPsToRPCSensorModelImageFilter->SetInput(m_InputImage);
  m_GCPsToRPCSensorModelImageFilter->UpdateOutputInformation();

  // Load GCPs from input image
  m_GCPsToRPCSensorModelImageFilter->SetUseImageGCPs(true);

  // Generate the layer
  m_ImageGenerator->SetImage(image);
  m_ImageGenerator->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(m_ImageGenerator->GetProgressSource(), 0, 0, 200, 20, "Generating QuickLook ...");
  m_ImageGenerator->GenerateLayer();
 
  m_ImageGenerator->GetLayer()->SetName("InputImage");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());
  m_VisualizationModel->Update();

  // Update the GCP & Error container
  this->UpdateContainer();
}

void
GCPToSensorModelModel
::LoadGCP()
{
  // Load GCPs from Input Image
  m_GCPsToRPCSensorModelImageFilter->ClearGCPs();
  m_GCPsToRPCSensorModelImageFilter->LoadImageGCPs();

  m_GCPsContainerHasChanged = true;
  this->UpdateContainer();
  m_GCPsContainerHasChanged = false;
}

void GCPToSensorModelModel
::ImportGCPsFromXmlFile(const char * fname)
{
  // Build an xml document handler
  TiXmlDocument doc(fname);

  // If the document fails to lead
  if (!doc.LoadFile())
    {
    itkExceptionMacro(<< "Can't open file " << fname);
    }

  // Build an xml handle
  TiXmlHandle hDoc(&doc);

  // Look for the GroundControlPoints tag
  TiXmlHandle gcps = hDoc.FirstChildElement("GroundControlPoints");

  // Iterate on each ground control point
  for (TiXmlElement* currentGcp = gcps.FirstChildElement("GroundControlPoint").ToElement();
       currentGcp != NULL;
       currentGcp = currentGcp->NextSiblingElement())
    {
    Point2DType p2d;
    Point3DType p3d;

    // Decode gcp column
    TiXmlElement* currentElement = currentGcp->FirstChildElement("Column");
    if (!currentElement)
      {
      itkExceptionMacro(<< "Bad XML file (<Column> not found): " << fname);
      }
    p2d[0] = atof(currentElement->GetText());

    // Decode gcp row
    currentElement = currentGcp->FirstChildElement("Row");
    if (!currentElement)
      {
      itkExceptionMacro(<< "Bad XML file (<Row> not found): " << fname);
      }
    p2d[1] = atof(currentElement->GetText());

    // Decode gcp latitude
    currentElement = currentGcp->FirstChildElement("Latitude");
    if (!currentElement)
      {
      itkExceptionMacro(<< "Bad XML file (<Latitude> not found): " << fname);
      }
    p3d[1] = atof(currentElement->GetText());

    // Decode gcp longitude
    currentElement = currentGcp->FirstChildElement("Longitude");
    if (!currentElement)
      {
      itkExceptionMacro(<< "Bad XML file (<Longitude> not found): " << fname);
      }
    p3d[0] = atof(currentElement->GetText());

    // Decode gcp elevation
    currentElement = currentGcp->FirstChildElement("Elevation");
    if (!currentElement)
      {
      itkExceptionMacro(<< "Bad XML file (<Elevation> not found): " << fname);
      }
    p3d[2] = atof(currentElement->GetText());

    // Finally, add the gcp
    m_GCPsToRPCSensorModelImageFilter->AddGCP(p2d, p3d);
    }
  // Update GCPs container
  this->UpdateContainer();
}

void GCPToSensorModelModel
::ExportGCPsToXmlFile(const char * fname) const
{
  // Declare a stringstream to be used later
  itk::OStringStream oss;
  oss << std::fixed << std::setprecision(6);

  // Build an xml document
  TiXmlDocument     doc;
  TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
  doc.LinkEndChild(decl);

  // Build root node
  TiXmlElement * root = new TiXmlElement("GroundControlPoints");
  doc.LinkEndChild(root);

  // Iterate on each gcp
  for (unsigned int i = 0; i < m_GCPsContainer.size(); i++)
    {
    // Add a new entry
    TiXmlElement * currentGCP = new TiXmlElement("GroundControlPoint");
    root->LinkEndChild(currentGCP);

    // Store the row index
    TiXmlElement * column = new TiXmlElement("Column");
    oss.str("");
    oss << m_GCPsContainer[i].first[0];
    column->LinkEndChild(new TiXmlText(oss.str().c_str()));
    currentGCP->LinkEndChild(column);

    // Store the row index
    TiXmlElement * row = new TiXmlElement("Row");
    oss.str("");
    oss << m_GCPsContainer[i].first[1];
    row->LinkEndChild(new TiXmlText(oss.str().c_str()));
    currentGCP->LinkEndChild(row);

    // Store the latitude
    TiXmlElement * latitude = new TiXmlElement("Latitude");
    oss.str("");
    oss << m_GCPsContainer[i].second[1];
    latitude->LinkEndChild(new TiXmlText(oss.str().c_str()));
    currentGCP->LinkEndChild(latitude);

    // Store the longitude
    TiXmlElement * longitude = new TiXmlElement("Longitude");
    oss.str("");
    oss << m_GCPsContainer[i].second[0];
    longitude->LinkEndChild(new TiXmlText(oss.str().c_str()));
    currentGCP->LinkEndChild(longitude);

    // Store the elevation
    TiXmlElement * elevation = new TiXmlElement("Elevation");
    oss.str("");
    oss << m_GCPsContainer[i].second[2];
    elevation->LinkEndChild(new TiXmlText(oss.str().c_str()));
    currentGCP->LinkEndChild(elevation);
    }
  // Finally, write the file
  doc.SaveFile(fname);
}

void
GCPToSensorModelModel
::SetDEMPath(const std::string& DEMPath)
{
  // Elevation Management
  m_ElevMgt = DEM;

  // Generate DEMHandler
  m_DEMPath = DEMPath;
  m_DEMHandler->OpenDEMDirectory(DEMPath.c_str());

  // Activate use DEM
  m_GCPsToRPCSensorModelImageFilter->SetUseDEM(true);

  // Add DEM to filter
  m_GCPsToRPCSensorModelImageFilter->SetDEMHandler(m_DEMHandler);

  // Clear GCPsContainer
  m_GCPsToRPCSensorModelImageFilter->ClearGCPs();

  // Add GCPs with no elevation
  for (unsigned int i = 0; i < m_GCPsContainer.size(); i++)
    {
    Point2DType sensorPoint, groundPoint;
    sensorPoint = m_GCPsContainer[i].first;
    groundPoint[0] = m_GCPsContainer[i].second[0];
    groundPoint[1] = m_GCPsContainer[i].second[1];

    m_GCPsToRPCSensorModelImageFilter->AddGCP(sensorPoint, groundPoint);
    }

  // Update GCPsContainer
  this->UpdateContainer();
}

void
GCPToSensorModelModel
::SetGCPElevation()
{
  // Clear GCPsContainer
  m_GCPsToRPCSensorModelImageFilter->ClearGCPs();

  // Add GCPs with no elevation
  for (unsigned int i = 0; i < m_GCPsContainer.size(); i++)
    {
    Point2DType sensorPoint, groundPoint;
    sensorPoint = m_GCPsContainer[i].first;
    groundPoint[0] = m_GCPsContainer[i].second[0];
    groundPoint[1] = m_GCPsContainer[i].second[1];

    m_GCPsToRPCSensorModelImageFilter->AddGCP(sensorPoint, groundPoint);
    }

  // Update GCPsContainer
  this->UpdateContainer();
}

void
GCPToSensorModelModel
::SetMeanElevation(double meanElev)
{
  m_MeanElevation = meanElev;
  m_ElevMgt = MEAN;

  // Set mean elevation to the filter
  m_GCPsToRPCSensorModelImageFilter->SetMeanElevation(m_MeanElevation);

  // Don't use DEM
  m_GCPsToRPCSensorModelImageFilter->SetUseDEM(false);

  // Clear GCPsContainer
  m_GCPsToRPCSensorModelImageFilter->ClearGCPs();

  // Add GCPs with no elevation
  for (unsigned int i = 0; i < m_GCPsContainer.size(); i++)
    {
    Point2DType sensorPoint, groundPoint;
    sensorPoint = m_GCPsContainer[i].first;
    groundPoint[0] = m_GCPsContainer[i].second[0];
    groundPoint[1] = m_GCPsContainer[i].second[1];

    m_GCPsToRPCSensorModelImageFilter->AddGCP(sensorPoint, groundPoint);
    }

  // Update GCPsContainer
  this->UpdateContainer();
}

void
GCPToSensorModelModel
::AddGCP(float x, float y, float lon, float lat, float elev)
{
  // Create Index
  IndexType index;
  index[0] = static_cast<long int>(x);
  index[1] = static_cast<long int>(y);

  // Check if the point is inside the image region
  if (!m_InputImage->GetLargestPossibleRegion().IsInside(index))
    {
    itkExceptionMacro(
      << "Index " << index << " is outside the image size " << m_InputImage->GetLargestPossibleRegion().GetSize() <<
      ".");
    }

  // Transform index to physical point
  ImagePointType phyPoint;
  m_InputImage->TransformIndexToPhysicalPoint(index, phyPoint);

  // Sensor point
  Point2DType sensorPoint;
  sensorPoint[0] = phyPoint[0];
  sensorPoint[1] = phyPoint[1];

  // Ground point
  Point2DType groundPoint;
  groundPoint[0] = lon;
  groundPoint[1] = lat;

  // Check if GCPs allready exists
  bool         found = false;
  unsigned int j = 0;
  while (j < m_GCPsContainer.size() && !found)
    {
    if (m_GCPsContainer[j].first == sensorPoint ||
        ((m_GCPsContainer[j].second[0] == groundPoint[0]) &&
         (m_GCPsContainer[j].second[1] == groundPoint[1])
        )
        )
      {
      found = true;
      itkExceptionMacro(
        << "At most one of the 2 given index " << sensorPoint << " or " << groundPoint <<
        " already appears in the list.");
      }
    j++;
    }

  // Check if elevation management is set to GCP
  if (m_ElevMgt == GCP)
    {
    // Use elev for elevation value
    Point3DType groundPoint3D;
    groundPoint3D[0] = groundPoint[0];
    groundPoint3D[1] = groundPoint[1];
    groundPoint3D[2] = elev;

    // Add GCP To the filter
    m_GCPsToRPCSensorModelImageFilter->AddGCP(sensorPoint, groundPoint3D);
    }
  // Else use internal elevation manager
  else
    {
    m_GCPsToRPCSensorModelImageFilter->AddGCP(sensorPoint, groundPoint);
    }

  // Update GCPsContainer
  this->UpdateContainer();
}

void
GCPToSensorModelModel
::RemovePointFromGCPsContainer(unsigned int id)
{
  if (id >= m_GCPsContainer.size()) itkExceptionMacro(
      << "Impossible to erase the " << id << " element. Out of vector size (" << m_GCPsContainer.size() << ").");

  m_GCPsToRPCSensorModelImageFilter->RemoveGCP(id);
  this->UpdateContainer();
}

void
GCPToSensorModelModel
::ClearGCPsContainer()
{
  // Clear GCPs
  m_GCPsToRPCSensorModelImageFilter->ClearGCPs();

  // Update local GCP container
  this->UpdateContainer();
}

void
GCPToSensorModelModel
::UpdateContainer()
{
  // Update the container and image informations
  m_GCPsToRPCSensorModelImageFilter->UpdateOutputInformation();
  m_GCPsContainer = m_GCPsToRPCSensorModelImageFilter->GetGCPsContainer();
  m_GroundError = m_GCPsToRPCSensorModelImageFilter->GetRMSGroundError();
  m_ErrorsContainer = m_GCPsToRPCSensorModelImageFilter->GetErrorsContainer();
  m_MeanError = m_GCPsToRPCSensorModelImageFilter->GetMeanError();

  // Generate cross point
  m_CrossIndexesContainer.clear();
  for (unsigned int i = 0; i < m_GCPsContainer.size(); i++)
    {
    // Transform index to physical point
    IndexType      index;
    ImagePointType phyPoint;
    phyPoint[0] = m_GCPsContainer[i].first[0];
    phyPoint[1] = m_GCPsContainer[i].first[1];
    m_InputImage->TransformPhysicalPointToIndex(phyPoint, index);

    // Add Point to the container
    m_CrossIndexesContainer.push_back(index);
    }

  // Notify it to the view
  m_GCPsContainerHasChanged = true;
  this->NotifyAll();
  m_GCPsContainerHasChanged = false;
}

void
GCPToSensorModelModel
::OK()
{
  // Set Output
  m_Output = m_GCPsToRPCSensorModelImageFilter->GetOutput();
  m_Output->UpdateOutputInformation();

  m_OutputChanged = true;
  this->NotifyAll();
}

/** get scale rfom depth */
const std::string GCPToSensorModelModel::ConvertDepthToScale(const unsigned int depth) const
{
  TileMapImageIOHelper::Pointer myTileMapImageIOHelper;
  myTileMapImageIOHelper = TileMapImageIOHelper::New();
  return myTileMapImageIOHelper->ConvertDepthToScale(depth);
}

#ifdef OTB_USE_CURL

void
GCPToSensorModelModel
::SetMap(long int sizeX, long int sizeY)
{
  m_SizeX = sizeX;
  m_SizeY = sizeY;

  m_TileIO->SetDepth(m_Depth);
  m_TileIO->SetCacheDirectory(m_CacheDirectory);

  // Configure reader
  m_MapReader = MapReaderType::New();
  m_MapReader->SetFileName(m_ServerName);
  m_MapReader->SetImageIO(m_TileIO);
  m_MapReader->UpdateOutputInformation();

  // Create Transform To transform coordinates
  typedef otb::TileMapTransform<otb::TransformDirection::FORWARD> TransformType;
  TransformType::Pointer transform = TransformType::New();
  transform->SetDepth(m_Depth);

  // Set lon/lat
  OutPointType lonLatPoint;
  lonLatPoint[0]  = m_Longitude;
  lonLatPoint[1]  = m_Latitude;

  // Transform lon/lat to pixel
  OutPointType tilePoint;
  tilePoint = transform->TransformPoint(lonLatPoint);

  VisualizationModelType::RegionType::IndexType index;
  VisualizationModelType::RegionType::SizeType  size;

  // Set parameters
  long int startX = static_cast<long int>(tilePoint[0]);
  long int startY = static_cast<long int>(tilePoint[1]);

  size[0] = sizeX;
  size[1] = sizeY;
  index[0] = startX - sizeX / 2;
  index[1] = startY - sizeY / 2;

  m_Region.SetSize(size);
  m_Region.SetIndex(index);

  m_MapVisualizationModel->SetExtractRegion(m_Region);

  // Generate Image
  std::vector<unsigned int> channels;
  if (m_MapReader->GetOutput()->GetNumberOfComponentsPerPixel() == 3)
    {
    channels.push_back(0);
    channels.push_back(1);
    channels.push_back(2);
    }
  else if (m_MapReader->GetOutput()->GetNumberOfComponentsPerPixel() > 3)
    {
    channels.push_back(2);
    channels.push_back(1);
    channels.push_back(0);
    }
  if (channels.size() == 3) m_MapImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

  m_MapImageGenerator->GetLayer()->GetRenderingFunction()->SetAutoMinMax(false);

  m_MapImageGenerator->GetLayer()->SetName("MapImage");

  m_MapImageGenerator->SetImage(m_MapReader->GetOutput());
  m_MapImageGenerator->GenerateQuicklookOff();

  // Clear previous layers
  m_MapVisualizationModel->ClearLayers();

  // Add the layer to the models
  m_MapVisualizationModel->AddLayer(m_MapImageGenerator->GetLayer());

  // Generate the layer
  m_MapImageGenerator->GenerateLayer();

  m_MapVisualizationModel->Update();

  m_LatLongChanged = true;
  m_PlaceNameChanged = true;
  m_DepthChanged = true;
  m_HasNewMap = true;
  this->NotifyAll();
  m_LatLongChanged = false;
  m_PlaceNameChanged = false;
  m_DepthChanged = false;
  m_HasNewMap = false;
}

void
GCPToSensorModelModel
::SearchPlaceName(double longitude, double latitude)
{
  // Get coordinates
  m_Longitude = longitude;
  m_Latitude  = latitude;

  if ((m_Longitude < -180) || (m_Longitude > 180) || (m_Latitude < -90) || (m_Latitude > 90))
    {
    itkDebugMacro(<< "Latitude or Longitude is wrong.");
    }
  else
    {
    // Create coordinateToName object
    otb::CoordinateToName::Pointer m_CoordinateToName  = otb::CoordinateToName::New();

    // Set lon/lat & evaluate name
    m_CoordinateToName->SetLon(m_Longitude);
    m_CoordinateToName->SetLat(m_Latitude);
    if (m_CoordinateToName->Evaluate())
      {
      // Get placename and country name
      std::string l_PlaceName   = m_CoordinateToName->GetPlaceName();
      std::string l_CountryName = m_CoordinateToName->GetCountryName();

      // Set placename in UI
      std::ostringstream l_PlaceAndCountryName;

      l_PlaceAndCountryName << l_PlaceName;
      l_PlaceAndCountryName << " ";
      l_PlaceAndCountryName << l_CountryName;

      m_PlaceName = l_PlaceAndCountryName.str();

      m_PlaceNameChanged = true;

      this->NotifyAll();

      m_PlaceNameChanged = false;
      }
    else
      {
      itkDebugMacro(<< "No resuslts for this Lat/Lon.");
      }
    }
}

void
GCPToSensorModelModel
::SearchLonLat(std::string placename)
{
  // Get placename from fluid
  m_PlaceName = placename;

  // Check placename
  if (m_PlaceName == "")
    {
    itkDebugMacro(<< "Placename is empty");
    }
  else
    {
    // Create placenameToLonLat object
    otb::PlaceNameToLonLat::Pointer m_PlaceNameToLonLat = otb::PlaceNameToLonLat::New();

    // Set placename & evaluate lat/long
    m_PlaceNameToLonLat->SetPlaceName(m_PlaceName);
    if (m_PlaceNameToLonLat->Evaluate())
      {
      // Get Lat/Lon
      m_Longitude = m_PlaceNameToLonLat->GetLon();
      m_Latitude  = m_PlaceNameToLonLat->GetLat();

      m_LatLongChanged = true;

      this->NotifyAll();

      m_LatLongChanged = false;
      }
    else
      {
      itkDebugMacro(<< "Error when fetching lat/lon.");
      }
    }
}

void
GCPToSensorModelModel
::DisplayMap(std::string placename,
             double longitude,
             double latitude,
             unsigned int depth,
             long int sizeX,
             long int sizeY)
{
  m_SizeX = sizeX;
  m_SizeY = sizeY;

  // Update display
  if (placename != m_PlaceName)
    {
    this->SearchLonLat(placename);
    }
  else if (latitude != m_Latitude || longitude != m_Longitude)
    {
    this->SearchPlaceName(longitude, latitude);
    }

  // Update Depth
  if (depth != m_Depth)
    {
    m_Depth = depth;

    this->SetMap(sizeX, sizeY);
    }
  else
    {
    // Create Model To transform coordinates
    typedef otb::TileMapTransform<otb::TransformDirection::FORWARD> TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetDepth(m_Depth);

    // Set lon/lat
    OutPointType lonLatPoint;
    lonLatPoint[0]  = m_Longitude;
    lonLatPoint[1]  = m_Latitude;

    // Transform lon/lat to pixel
    OutPointType tilePoint;
    tilePoint = transform->TransformPoint(lonLatPoint);

    // Set parameters for extract ROI
    long int startX = static_cast<long int>(tilePoint[0]);
    long int startY = static_cast<long int>(tilePoint[1]);

    RegionType::IndexType index;
    RegionType::SizeType  size;

    size[0] = sizeX;
    size[1] = sizeY;
    index[0] = startX - sizeX / 2;
    index[1] = startY - sizeY / 2;

    m_Region.SetSize(size);
    m_Region.SetIndex(index);

    m_MapVisualizationModel->SetExtractRegion(m_Region);
    m_MapVisualizationModel->Update();

    m_HasNewMap = true;
    this->NotifyAll();
    m_HasNewMap = false;
    }
}

void
GCPToSensorModelModel
::IncreaseDepth(int value, long int x, long int y)
{
  // Get current index
  RegionType::IndexType index = m_Region.GetIndex();

  // Modify index
  index[0] = x - m_SizeX / 2;
  index[1] = y - m_SizeX / 2;

  // Refresh region
  m_Region.SetIndex(index);

  // Create Model
  ForwardModelPointerType forwardModel = ForwardModelType::New();

  // Configure m_Model
  typedef otb::TileMapTransform<otb::TransformDirection::INVERSE> TransformType;
  TransformType::Pointer transform = TransformType::New();
  transform->SetDepth(m_Depth);

  OutPointType point, latlong;
  point[0] = index[0] + m_SizeX / 2;
  point[1] = index[1] + m_SizeY / 2;
  latlong = transform->TransformPoint(point);

  // Refresh LatLong
  m_Longitude = static_cast<double>(latlong[0]);
  m_Latitude = static_cast<double>(latlong[1]);

  this->SearchPlaceName(m_Longitude, m_Latitude);

  int tempDepth = m_Depth + value;
  if (tempDepth < 1)
    {
    tempDepth = 1;
    }
  else if (tempDepth > 18)
    {
    tempDepth = 18;
    }

  m_Depth = tempDepth;

  this->SetMap(m_SizeX, m_SizeY);
}

void
GCPToSensorModelModel
::DragMap(long int x, long int y)
{
  // Get current index
  RegionType::IndexType index = m_Region.GetIndex();

  // Modify index
  index[0] += x;
  index[1] += y;

  // Refresh region
  m_Region.SetIndex(index);

  // Create Model
  typedef otb::TileMapTransform<otb::TransformDirection::INVERSE> TransformType;
  TransformType::Pointer transform = TransformType::New();
  transform->SetDepth(m_Depth);

  OutPointType point, latlong;
  point[0] = index[0] + m_SizeX / 2;
  point[1] = index[1] + m_SizeY / 2;
  latlong = transform->TransformPoint(point);

  // Refresh LatLong
  m_Longitude = static_cast<double>(latlong[0]);
  m_Latitude = static_cast<double>(latlong[1]);

  this->SearchPlaceName(m_Longitude, m_Latitude);

  // Refresh Map
  m_MapVisualizationModel->SetExtractRegion(m_Region);
  m_MapVisualizationModel->Update();

  m_LatLongChanged = true;
  m_HasNewMap = true;
  this->NotifyAll();
  m_LatLongChanged = false;
  m_HasNewMap = false;
}

void
GCPToSensorModelModel
::SetSelectedPoint(long int x, long int y)
{

  typedef otb::TileMapTransform<otb::TransformDirection::INVERSE> TransformType;
  TransformType::Pointer transform = TransformType::New();
  transform->SetDepth(m_Depth);

  OutPointType point, latlong;
  point[0] = x;
  point[1] = y;
  latlong = transform->TransformPoint(point);

  // Refresh LatLong
  m_SelectedLongitude = static_cast<double>(latlong[0]);
  m_SelectedLatitude = static_cast<double>(latlong[1]);

  m_SelectedPointChanged = true;
  this->NotifyAll();
  m_SelectedPointChanged = false;

}

/** Center Map On selected Point */
void
GCPToSensorModelModel::CenterMapOnSelectedPoint(long int x, long int y, int depth)
{
  // Check if there is enought point to have a Keyword list
  if (m_GCPsContainer.size() < 3)
    {
    itkExceptionMacro(<< "There is not enough GCPs to generate a keywordlist.");
    }

  // Transform coordinates to physical point
  IndexType index;
  index[0] = x;
  index[1] = y;

  ImagePointType phyPoint;
  m_InputImage->TransformIndexToPhysicalPoint(index, phyPoint);

  // Create model
  ForwardModelPointerType forwardModel = ForwardModelType::New();

  // Configure model
  forwardModel->SetImageGeometry(m_GCPsToRPCSensorModelImageFilter->GetKeywordlist());

  if (forwardModel->IsValidSensorModel() == false )
    {
    itkExceptionMacro(<< "Unable to create projection.");
    }

  OutPointType point, latlong;
  point[0] = phyPoint[0];
  point[1] = phyPoint[1];
  latlong = forwardModel->TransformPoint(point);

  // Center map on this point
  double lon = static_cast<double>(latlong[0]);
  double lat = static_cast<double>(latlong[1]);

  SizeType size = m_Region.GetSize();

  this->DisplayMap(m_PlaceName, lon, lat, depth, size[0], size[1]);
}

#else
void
GCPToSensorModelModel
::SetMap(long int sizeX, long int sizeY)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::DragMap(long int x, long int y)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::SetSelectedPoint(long int x, long int y)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::IncreaseDepth(int value, long int x, long int y)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::SearchPlaceName(double longitude, double latitude)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::SearchLonLat(std::string placename)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::DisplayMap(std::string placename,
             double longitude,
             double latitude,
             unsigned int depth,
             long int sizeX,
             long int sizeY)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

/** Center Map On selected Point */
void
GCPToSensorModelModel
::CenterMapOnSelectedPoint(long int x, long int y, int depth)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

#endif

} // namespace otb
