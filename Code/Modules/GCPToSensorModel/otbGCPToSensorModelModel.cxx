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

//#include "otbPointSetRegister.h"
#include "elevation/ossimElevManager.h"
#include "base/ossimDirectory.h"
#include "base/ossimTieGpt.h"
#include "base/ossimTieGptSet.h"

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
                                             m_HasNewImage(), m_ProjectionUpdated(), m_ProjectionType(), m_GroundError()
{
  m_VisualizationModel = VisualizationModelType::New();
  m_BlendingFunction = BlendingFunctionType::New();
  m_BlendingFunction->SetAlpha(0.6);
  m_ImageGenerator = LayerGeneratorType::New();
  m_InputImage = VectorImageType::New();
  m_Output = VectorImageType::New();
  m_IndexesList.clear();
  //m_Projection = new ossimProjection();
  m_DEMPath = "";
  
  m_MeanElevation = 0;
  m_OutputChanged = false;
  m_ElevMgt = MEAN;
  m_DEMHandler = DEMHandler::New();
  m_GCPsElevation.clear();
  m_DEMsElevation.clear();
  m_UsedElevation.clear();
  m_HasNewImage = false;
  m_ProjectionUpdated = false;
  m_ProjectionType = BILINEAR;
  m_GroundError = 0.;
}

GCPToSensorModelModel
::~GCPToSensorModelModel()
{
  delete m_Projection;
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

  if( m_InputImage->GetGCPCount() == 0 )
    itkExceptionMacro(<<"Invalid Input Image, no GCPs detected.");

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
  std::cout<<"Using ossim bilinear projection"<<std::endl;

  ossimBilinearProjection * bproj = new ossimBilinearProjection();

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

  delete m_Projection;
  m_Projection = bproj;

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
::ComputeRPCProjection()
{
  std::cout<<"Using ossim rpc projection"<<std::endl;

  if( m_DEMsElevation.size() != m_GCPsElevation.size() || m_DEMsElevation.size() != m_IndexesList.size() )
    {
      itkExceptionMacro(<<"Invalid heights");
    }

  this->GenerateUsedElevation();

  ossimRpcProjection * bproj = new ossimRpcProjection();

  std::vector<ossimDpt> sensorPoints;
  std::vector<ossimGpt>  geoPoints;
  ContinuousIndexType idFix, idMov;
  ossimDpt spoint;
  ossimGpt gpoint;
  std::vector< ossimRefPtr<ossimTieGpt> > tieGptVect;
  ossimTieGptSet tieGptSet;
  ossim_float64 score = 0.0;
  for(unsigned int i = 0; i<m_IndexesList.size(); i++)
    {
      idFix =  m_IndexesList[i].first;
      idMov =  m_IndexesList[i].second;
      spoint = ossimDpt(idFix[0],idFix[1]);
      sensorPoints.push_back(spoint);
      gpoint = ossimGpt(idMov[0],idMov[1], m_UsedElevation[i]);
      std::cout<<gpoint<<std::endl;
      ossimTieGpt * tieGpt = new ossimTieGpt(gpoint, spoint, score);
      tieGptVect.push_back(tieGpt);    
    }
  tieGptSet.setTiePoints(tieGptVect);
  m_GroundError = bproj->optimizeFit(tieGptSet);

  delete m_Projection;
  m_Projection = bproj;

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
  m_Projection->lineSampleToWorld(spoint, gpoint);
  out[0] = gpoint.lat;
  out[1] = gpoint.lon;
  out[2] = gpoint.hgt;
 
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
  ossimKeywordlist kwl;
  m_Projection->saveState(kwl);
  kwl.print(std::cout);
  otb::ImageKeywordlist otb_kwl;
  otb_kwl.SetKeywordlist( kwl );
  itk::MetaDataDictionary& dict = m_InputImage->GetMetaDataDictionary();
  itk::EncapsulateMetaData< otb::ImageKeywordlist >(dict,otb::MetaDataKey::OSSIMKeywordlistKey,otb_kwl);

  m_Output = m_InputImage;

  m_OutputChanged = true;
  this->NotifyAll();
}

}// namespace otb
