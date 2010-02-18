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

#include "elevation/ossimElevManager.h"
#include "base/ossimDirectory.h"
#include "projection/ossimRpcSolver.h"

#ifdef OTB_USE_CURL
#include "otbPlaceNameToLonLat.h"
#include "otbCoordinateToName.h"
#endif

namespace otb
{
/** Initialize the singleton */
GCPToSensorModelModel::Pointer GCPToSensorModelModel::Instance = NULL;
/** Manage the singleton */
GCPToSensorModelModel::Pointer
GCPToSensorModelModel::GetInstance()
{
  if (!Instance)
  {
    Instance = GCPToSensorModelModel::New();
  }
  return Instance;
}

void GCPToSensorModelModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

  GCPToSensorModelModel::GCPToSensorModelModel() : m_VisualizationModel(), m_BlendingFunction(), m_Output(),
                                             m_Projection(), m_DEMPath(), m_ElevMgt(), m_MeanElevation(),
                                             m_DEMHandler(), m_GCPsElevation(), m_UsedElevation(), m_DEMsElevation(),
                                             m_HasNewImage(), m_ProjectionUpdated(), m_ProjectionType(),
                                             m_GroundError(), m_MapVisualizationModel(), m_MapBlendingFunction()
{
  m_VisualizationModel = VisualizationModelType::New();
  m_BlendingFunction = BlendingFunctionType::New();
  m_BlendingFunction->SetAlpha(0.6);
  m_ImageGenerator = LayerGeneratorType::New();
  m_InputImage = VectorImageType::New();
  m_Output = VectorImageType::New();
  m_IndexesList.clear();

  m_Projection = NULL;
  m_RpcProjection = NULL;
  m_RpcModel = NULL;


  m_DEMPath = "";

  m_MeanElevation = 0;
  m_OutputChanged = false;
  m_ElevMgt = GCP;
  m_DEMHandler = DEMHandler::New();
  m_GCPsElevation.clear();
  m_DEMsElevation.clear();
  m_UsedElevation.clear();
  m_HasNewImage = false;
  m_ProjectionUpdated = false;
  m_ProjectionType = BILINEAR;
  m_GroundError = 0.;
  
  m_ServerName = "http://tile.openstreetmap.org/";
  m_CacheDirectory = "./Caching";
  
  ossimFilename cachingDir(m_CacheDirectory);
  cachingDir.createDirectory();
  
  m_PlaceName = "Toulouse";
  m_Latitude = 43.6043;
  m_Longitude = 1.4433;
  m_Depth = 1;
  m_PlaceNameChanged = false;
  m_LatLongChanged = false;
  m_MapVisualizationModel = VisualizationModelType::New();
  m_MapBlendingFunction = BlendingFunctionType::New();
  m_MapBlendingFunction->SetAlpha(0.6);
  m_MapImageGenerator = MapLayerGeneratorType::New();
}

GCPToSensorModelModel
::~GCPToSensorModelModel()
{
  // deleted with ossimRefPtr
  //delete m_RpcProjection;
  //delete m_RpcModel;
  //delete m_Projection
}


void
GCPToSensorModelModel
::SetImage(VectorImagePointerType image)
{
  m_OutputChanged = false;

  image->UpdateOutputInformation();
  m_InputImage = image;

  // Generate the layer
  m_ImageGenerator->SetImage(image);
  m_ImageGenerator->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(m_ImageGenerator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  m_ImageGenerator->GenerateLayer();

  std::vector<unsigned int> channels;
  if(image->GetNumberOfComponentsPerPixel()==3)
    {
      channels.push_back(0);
      channels.push_back(1);
      channels.push_back(2);
    }
  else if(image->GetNumberOfComponentsPerPixel()>3)
    {
      channels.push_back(2);
      channels.push_back(1);
      channels.push_back(0);
    }
  if( channels.size() == 3 )
    m_ImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

    m_ImageGenerator->GetLayer()->SetName("InputImage");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());
  m_VisualizationModel->Update();

  m_HasNewImage = true;
  this->NotifyAll();
  m_HasNewImage = false;
}

void
GCPToSensorModelModel
::LoadGCP()
{
  if( m_InputImage.IsNull())
    itkExceptionMacro(<<"No input image detected");

  ContinuousIndexType index1, index2;
  double height;
  this->ClearIndexesList();

  for( unsigned int i=0; i<m_InputImage->GetGCPCount(); i++ )
    {
      index1[0] = m_InputImage->GetGCPCol(i);
      index1[1] = m_InputImage->GetGCPRow(i);
      index2[0] = m_InputImage->GetGCPX(i);
      index2[1] = m_InputImage->GetGCPY(i);
      height =  m_InputImage->GetGCPZ(i);
      this->AddIndexesToList( index1, index2, height );
    }
}

void
GCPToSensorModelModel
::SetDEMPath( const std::string & DEMPath )
{
  ossimElevManager * elevManager = ossimElevManager::instance();
  ossimFilename ossimDEMDir;
  ossimDEMDir=ossimFilename(DEMPath.c_str());
  ossimDirectory od(DEMPath.c_str());
  if (!elevManager->loadElevationPath(ossimDEMDir))
   {
     itkExceptionMacro("Invalid directory \""<<DEMPath<<"\", no DEM files found!");
   }
  m_DEMPath = DEMPath;
  m_ElevMgt = DEM;
  m_DEMHandler->OpenDEMDirectory(DEMPath.c_str());

  m_DEMsElevation.clear();
  for(unsigned int i = 0; i<m_IndexesList.size(); i++)
    {
      ImagePointType phyPoint;
      phyPoint[0] = m_IndexesList[i].second[0];
      phyPoint[1] = m_IndexesList[i].second[1];
      m_DEMsElevation.push_back( m_DEMHandler->GetHeightAboveMSL(phyPoint) );
    }

  this->Modified();
}

void
GCPToSensorModelModel
::SetMeanElevation( double meanElev )
{
  m_MeanElevation = meanElev;
  m_ElevMgt = MEAN;
  this->Modified();
}


void
GCPToSensorModelModel
::AddIndexesToList( ContinuousIndexType id1, ContinuousIndexType id2, double elev )
{
  bool found = false;
  unsigned int j=0;


  while( j<m_IndexesList.size() && !found )
    {
      if( m_IndexesList[j].first == id1 || m_IndexesList[j].second == id2 )
       {
         found = true;
         itkExceptionMacro(<<"At most one of the 2 given index "<<id1<<" or "<<id2<<" already appears in the list.");
       }
      j++;
    }

  IndexCoupleType paire(id1, id2);
  m_IndexesList.push_back(paire);
  m_GCPsElevation.push_back(elev);
  ImagePointType phyPoint;
  phyPoint[0] = id2[0];
  phyPoint[1] = id2[1];
  m_DEMsElevation.push_back( m_DEMHandler->GetHeightAboveMSL(phyPoint) );
}

void
GCPToSensorModelModel
::RemovePointFromList( unsigned int id )
{
  if( id>=m_IndexesList.size() )
    itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size ("<<m_IndexesList.size()<<").");
  if( id>=m_GCPsElevation.size() )
    itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size ("<<m_GCPsElevation.size()<<").");
  if( id>=m_UsedElevation.size() )
    itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size ("<<m_UsedElevation.size()<<").");
  if( id>=m_DEMsElevation.size() && m_DEMsElevation.size() != 0 )
    itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size ("<<m_DEMsElevation.size()<<").");

  m_IndexesList.erase(m_IndexesList.begin()+id);
  m_GCPsElevation.erase(m_GCPsElevation.begin()+id);
  m_UsedElevation.erase(m_UsedElevation.begin()+id);
  m_DEMsElevation.erase(m_DEMsElevation.begin()+id);
}



void
GCPToSensorModelModel
::ComputeBilinearProjection()
{
  /* NOT USED YET...
  ossimRefPtr<ossimBilinearProjection> bproj = new ossimBilinearProjection();

  std::vector<ossimDpt> sensorPoints;
  std::vector<ossimGpt>  geoPoints;
  ContinuousIndexType idFix, idMov;

  ossimDpt spoint;
  ossimGpt gpoint;

  for(unsigned int i = 0; i<m_IndexesList.size(); i++)
    {
      idFix =  m_IndexesList[i].first;
      idMov =  m_IndexesList[i].second;
      spoint = ossimDpt(idFix[0],idFix[1]);
      sensorPoints.push_back(spoint);
      gpoint = ossimGpt(idMov[0],idMov[1]);
      geoPoints.push_back(gpoint);
    }

  m_GroundError = bproj->setTiePoints(sensorPoints,geoPoints);

  m_Projection = bproj.get();

  // To avoid infinite loop (notify->notify->notify...)
  if( m_HasNewImage == false )
    {
      m_ProjectionUpdated = true;
      this->NotifyAll();
      m_ProjectionUpdated = false;
    }
  */
}


void
GCPToSensorModelModel
::ComputeRPCProjection()
{
  if( m_DEMsElevation.size() != m_GCPsElevation.size() || m_DEMsElevation.size() != m_IndexesList.size() )
    {
      itkExceptionMacro(<<"Invalid heights");
    }

  this->GenerateUsedElevation();

  std::vector<ossimDpt> sensorPoints;
  std::vector<ossimGpt>  geoPoints;
  ContinuousIndexType idFix, idMov;
  ossimDpt spoint;
  ossimGpt gpoint;
  for(unsigned int i = 0; i<m_IndexesList.size(); i++)
    {
      idFix =  m_IndexesList[i].first;
      idMov =  m_IndexesList[i].second;
      spoint = ossimDpt(idFix[0],idFix[1]);
      sensorPoints.push_back(spoint);
      gpoint = ossimGpt(idMov[1],idMov[0], m_UsedElevation[i]);
      geoPoints.push_back(gpoint);
    }

  // Use a solver to compute coef
  ossimRefPtr<ossimRpcSolver> solver = new ossimRpcSolver(false, false);
  solver->solveCoefficients( sensorPoints, geoPoints);
  m_GroundError = vcl_pow(solver->getRmsError(), 2);

  // infer the associate projection
  m_RpcProjection = new ossimRpcProjection();
  ossimRefPtr< ossimRpcProjection > objp = dynamic_cast<ossimRpcProjection*>(solver->createRpcProjection()->getProjection());
  ossimKeywordlist kwl;
  objp->saveState(kwl);
  m_RpcProjection->loadState(kwl);

  m_Projection = m_RpcProjection.get();

  // deduct the associate model
  m_RpcModel = new ossimRpcModel();
  ossimRefPtr< ossimRpcModel > objm = dynamic_cast<ossimRpcModel*>(solver->createRpcModel()->getProjection());
  ossimKeywordlist kwlm;
  objm->saveState(kwlm);
  m_RpcModel->loadState(kwlm);


  // To avoid infinite loop (notify->notify->notify...)
  if( m_HasNewImage == false )
    {
      m_ProjectionUpdated = true;
      this->NotifyAll();
      m_ProjectionUpdated = false;
    }
}


void
GCPToSensorModelModel
::GenerateUsedElevation()
{
  m_UsedElevation.clear();
  if(m_ElevMgt == GCP)
    m_UsedElevation = m_GCPsElevation;
  else if(m_ElevMgt == MEAN)
    m_UsedElevation = std::vector<double>(m_DEMsElevation.size(), m_MeanElevation);
  else if(m_ElevMgt == DEM)
    m_UsedElevation = m_DEMsElevation;
  else
    itkExceptionMacro(<<"Unknown elevation management.")
}


GCPToSensorModelModel::Continuous3DIndexType
GCPToSensorModelModel
::TransformPoint(ContinuousIndexType id, double height)
{
  Continuous3DIndexType out;

  ossimDpt spoint(id[0], id[1]);
  ossimGpt gpoint;
  if (m_ProjectionType == RPC)
    gpoint.hgt = height;

  if( m_Projection != NULL )
    {
      m_Projection->lineSampleToWorld(spoint, gpoint);

      out[0] = gpoint.lon;
      out[1] = gpoint.lat;
      out[2] = gpoint.hgt;
    }
  else
    out.Fill(0.);

  return out;
}


GCPToSensorModelModel::Continuous3DIndexListType
GCPToSensorModelModel
::TransformPoints()
{
  Continuous3DIndexListType outList;
  for(unsigned int i=0; i<m_IndexesList.size(); i++)
    {
      if (m_ProjectionType == RPC)
        outList.push_back( this->TransformPoint(m_IndexesList[i].first, m_UsedElevation[i]) );
      else if(m_ProjectionType == BILINEAR)
        outList.push_back( this->TransformPoint(m_IndexesList[i].first) );
    }
  return outList;
}



void
GCPToSensorModelModel
::OK()
{
  if(m_ProjectionType == RPC)
    {
      if( m_RpcModel == NULL )
        return;

      ossimKeywordlist geom_kwl;
      m_RpcModel->updateModel();
      m_RpcModel->saveState(geom_kwl);

      ImageKeywordlist otb_kwl;
      otb_kwl.SetKeywordlist( geom_kwl );

      itk::MetaDataDictionary& dict = m_InputImage->GetMetaDataDictionary();
      itk::EncapsulateMetaData< ImageKeywordlist >( dict, MetaDataKey::OSSIMKeywordlistKey, otb_kwl );

      m_Output = m_InputImage;
      m_Output->UpdateOutputInformation();

      m_OutputChanged = true;
      this->NotifyAll();
    }
}

#ifdef OTB_USE_CURL

void
GCPToSensorModelModel
::SetMap(long int sizeX, long int sizeY)
{
  m_SizeX = sizeX;
  m_SizeY = sizeY;

  // Configure Tile
  m_TileIO = TileMapType::New();
  m_TileIO->SetDepth( m_Depth );
  m_TileIO->SetCacheDirectory( m_CacheDirectory );
  
  // Configure reader
  m_MapReader = MapReaderType::New();
  m_MapReader->SetFileName( m_ServerName );
  m_MapReader->SetImageIO( m_TileIO );
  m_MapReader->UpdateOutputInformation();
  
  // Create Model To transform coordinates
  m_MapModel = MapModelType::New();
  
  // Configure m_Model
  m_MapModel->SetImageGeometry( m_MapReader->GetOutput()->GetImageKeywordlist() );
  dynamic_cast<ossimTileMapModel*>(m_MapModel->GetOssimModel())->setDepth( m_Depth );
  if (!m_MapModel)
  {
    itkExceptionMacro(<<"Unable to create projection.");
  }
  
  // Set lon/lat
  OutPointType lonLatPoint;
  lonLatPoint[0]  = m_Longitude;
  lonLatPoint[1]  = m_Latitude;
  
  // Transform lon/lat to pixel
  OutPointType tilePoint;
  tilePoint = m_MapModel->TransformPoint(lonLatPoint);
 
  VisualizationModelType::RegionType::IndexType index;
  VisualizationModelType::RegionType::SizeType  size;    
    
  // Set parameters
  long int startX=static_cast<long int>( tilePoint[0] );
  long int startY=static_cast<long int>( tilePoint[1] );
      
  size[0] = sizeX;
  size[1] = sizeY;
  index[0] = startX-sizeX/2;
  index[1] = startY-sizeY/2;
     
  m_Region.SetSize(size);
  m_Region.SetIndex(index);
  
  m_MapVisualizationModel->SetExtractRegion(m_Region);
  
  // Generate Image
  std::vector<unsigned int> channels;
  if(m_MapReader->GetOutput()->GetNumberOfComponentsPerPixel()==3)
    {
      channels.push_back(0);
      channels.push_back(1);
      channels.push_back(2);
    }
  else if(m_MapReader->GetOutput()->GetNumberOfComponentsPerPixel()>3)
    {
      channels.push_back(2);
      channels.push_back(1);
      channels.push_back(0);
    }
  if( channels.size() == 3 )
    m_MapImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);
    
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
::SearchPlaceName(double latitude, double longitude)
{
  // Get coordinates
  m_Longitude = longitude;
  m_Latitude  = latitude;
  
  if ((m_Longitude < -1000) || (m_Longitude > 1000) || (m_Latitude < -1000) || (m_Latitude > 1000))
  {
    itkDebugMacro(<<"Latitude or Longitude is wrong.");
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
      itkDebugMacro(<<"No resuslts for this Lat/Lon.");
    }
  }
}

void
GCPToSensorModelModel
::SearchLatLong(std::string placename)
{
  // Get placename from fluid
  m_PlaceName = placename;
  
  // Check placename
  if (m_PlaceName == "")
  {
    itkDebugMacro(<<"Placename is empty");
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
      itkDebugMacro(<<"Error when fetching lat/lon.");
    }
  }
}

void
GCPToSensorModelModel
::DisplayMap(std::string placename, double latitude, double longitude, unsigned int depth, long int sizeX, long int sizeY)
{
  m_SizeX = sizeX;
  m_SizeY = sizeY;

    // Update display
  if(placename != m_PlaceName)
  {
    this->SearchLatLong(placename);
  }
  else if (latitude != m_Latitude || longitude != m_Longitude)
  {
    this->SearchPlaceName(latitude, longitude);
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
    m_MapModel = MapModelType::New();
    
    // Configure m_Model
    m_MapModel->SetImageGeometry( m_MapReader->GetOutput()->GetImageKeywordlist() );
    dynamic_cast<ossimTileMapModel*>(m_MapModel->GetOssimModel())->setDepth( depth );
    
    if (!m_MapModel)
    {
      itkExceptionMacro(<<"Unable to create projection.");
    }
    
    // Set lon/lat
    OutPointType lonLatPoint;
    lonLatPoint[0]  = m_Longitude;
    lonLatPoint[1]  = m_Latitude;
    
    // Transform lon/lat to pixel
    OutPointType tilePoint;
    tilePoint = m_MapModel->TransformPoint(lonLatPoint);
    
    // Set parameters for extract ROI
    long int startX=static_cast<long int>( tilePoint[0] );
    long int startY=static_cast<long int>( tilePoint[1] );
    
    RegionType::IndexType index;
    RegionType::SizeType size;
    
    size[0] = sizeX;
    size[1] = sizeY;
    index[0] = startX-sizeX/2;
    index[1] = startY-sizeY/2;
    
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
  index[0] = x - m_SizeX/2;
  index[1] = y - m_SizeX/2;
  
  // Refresh region
  m_Region.SetIndex(index);
  
  // Create Model
  m_MapInverseModel = MapForwardModelType::New();
    
  // Configure m_Model
  m_MapInverseModel->SetImageGeometry( m_MapReader->GetOutput()->GetImageKeywordlist() );
  dynamic_cast<ossimTileMapModel*>(m_MapInverseModel->GetOssimModel())->setDepth( m_Depth );

  if (!m_MapInverseModel)
  {
    itkExceptionMacro(<<"Unable to create projection.");
  }

  
  OutPointType point, latlong;
  point[0] = index[0] + m_SizeX/2;
  point[1] = index[1] + m_SizeY/2;
  latlong = m_MapInverseModel->TransformPoint(point);
  
  // Refresh LatLong
  m_Longitude = static_cast<double>(latlong[0]);
  m_Latitude = static_cast<double>(latlong[1]);
  
  this->SearchPlaceName(m_Latitude, m_Longitude);
  
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
  m_MapInverseModel = MapForwardModelType::New();
    
  // Configure m_Model
  m_MapInverseModel->SetImageGeometry( m_MapReader->GetOutput()->GetImageKeywordlist() );
  dynamic_cast<ossimTileMapModel*>(m_MapInverseModel->GetOssimModel())->setDepth( m_Depth );

  if (!m_MapInverseModel)
  {
    itkExceptionMacro(<<"Unable to create projection.");
  }

  
  OutPointType point, latlong;
  point[0] = index[0] + m_SizeX/2;
  point[1] = index[1] + m_SizeY/2;
  latlong = m_MapInverseModel->TransformPoint(point);
  
  // Refresh LatLong
  m_Longitude = static_cast<double>(latlong[0]);
  m_Latitude = static_cast<double>(latlong[1]);
  
  this->SearchPlaceName(m_Latitude, m_Longitude);
  
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

  // Create Model
  m_MapInverseModel = MapForwardModelType::New();
    
  // Configure m_Model
  m_MapInverseModel->SetImageGeometry( m_MapReader->GetOutput()->GetImageKeywordlist() );
  dynamic_cast<ossimTileMapModel*>(m_MapInverseModel->GetOssimModel())->setDepth( m_Depth );
  
  if (!m_MapInverseModel)
  {
    itkExceptionMacro(<<"Unable to create projection.");
  }

  OutPointType point, latlong;
  point[0] = x;
  point[1] = y;
  latlong = m_MapInverseModel->TransformPoint(point);
  
  // Refresh LatLong
  m_SelectedLongitude = static_cast<double>(latlong[0]);
  m_SelectedLatitude = static_cast<double>(latlong[1]);
  
  m_SelectedPointChanged = true;
  this->NotifyAll();
  m_SelectedPointChanged = false;

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
::SearchPlaceName(double latitude, double longitude)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::SearchLatLong(std::string placename)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

void
GCPToSensorModelModel
::DisplayMap(std::string placename, double latitude, double longitude, unsigned int depth, long int sizeX, long int sizeY)
{
  itkExceptionMacro(<< "OTB_USE_CURL is undefine.");
}

#endif

}// namespace otb
