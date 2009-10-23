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

  GCPToSensorModelModel::GCPToSensorModelModel() : m_VisualizationModel(), m_BlendingFunction(), m_Output(), m_Transform(), m_DEMPath(), m_UseDEM(), m_MeanElevation()
{
  m_VisualizationModel = VisualizationModelType::New();

  m_BlendingFunction = BlendingFunctionType::New();
  m_BlendingFunction->SetAlpha(0.6);
  m_ImageGenerator = LayerGeneratorType::New();
  m_InputImage = VectorImageType::New();
  m_Output = VectorImageType::New();
  m_IndexesList.clear();
  m_Transform = new ossimBilinearProjection();
  m_DEMPath = "";
  m_UseDEM = false;
  m_MeanElevation = 0;
  m_OutputChanged = false;  
}

GCPToSensorModelModel
::~GCPToSensorModelModel()
{
  delete m_Transform;
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

  this->NotifyAll();
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
  m_UseDEM = true;
  this->Modified();
}

void
GCPToSensorModelModel
::SetMeanElevation( double meanElev )
{
  m_MeanElevation = meanElev;
  m_UseDEM = false;
  this->Modified();
}


void
GCPToSensorModelModel
::AddIndexesToList( ContinuousIndexType id1, ContinuousIndexType id2 )
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
}
  
void
GCPToSensorModelModel
::RemovePointFromList( unsigned int id )
{
  if( id>=m_IndexesList.size() )
    itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size ("<<m_IndexesList.size()<<").");
  
  m_IndexesList.erase(m_IndexesList.begin()+id);
}


void 
GCPToSensorModelModel
::ConvertList( PointSetPointerType fix, PointSetPointerType mov )
{
  if(m_IndexesList.size()==0)
    itkExceptionMacro(<<"No point selected...");

  PointType fixedPoint;
  PointType movingPoint;
  
  for(unsigned int i=0; i<m_IndexesList.size(); i++)
    {
      ContinuousIndexType idFix, idMov;
      idFix = m_IndexesList[i].first;
      idMov = m_IndexesList[i].second;
    }
}  
 

void 
GCPToSensorModelModel
::ComputeTransform()
{
  std::cout<<"Using ossim bilinear projection"<<std::endl;

  //ossimBilinearProjection bproj;
  delete m_Transform;
  m_Transform = new ossimBilinearProjection();

  std::vector<ossimDpt> sensorPoints;
  std::vector<ossimGpt>  geoPoints;
  ContinuousIndexType idFix, idMov;
  

  for(unsigned int i = 0; i<m_IndexesList.size(); i++)
    {
    idFix =  m_IndexesList[i].first;
    idMov =  m_IndexesList[i].second;
    ossimDpt spoint(idFix[0],idFix[1]);
    sensorPoints.push_back(spoint);
    ossimGpt gpoint(idMov[0],idMov[1]);
    geoPoints.push_back(gpoint);
    }

  m_Transform->setTiePoints(sensorPoints,geoPoints);
}



GCPToSensorModelModel::ContinuousIndexType
GCPToSensorModelModel
::TransformPoint(ContinuousIndexType id)
{
  ContinuousIndexType out;

  //typename AffineTransformType::InputPointType  inPoint; 
  //typename AffineTransformType::OutputPointType outPoint;
  
  //m_InputImage->TransformIndexToPhysicalPoint(index,inPoint);
  ossimDpt spoint(id[0], id[1]);
  ossimGpt gpoint;
  m_Transform->lineSampleToWorld(spoint, gpoint);
  out[0] = gpoint.lat;
  out[1] = gpoint.lon;
  
  return out;
}


GCPToSensorModelModel::ContinuousIndexListType 
GCPToSensorModelModel
::TransformPoints()
{
  //IndexListType inList;
  ContinuousIndexListType outList;
  for(unsigned int i=0; i<m_IndexesList.size(); i++)
    {
      //inList.push_back(m_IndexesList[i].first);
      outList.push_back( this->TransformPoint(m_IndexesList[i].first) );
    }

  //typename AffineTransformType::InputPointType  inPoint; 
//   typename AffineTransformType::OutputPointType outPoint;
//   ContinuousIndexType         idOut;

//   for(unsigned int i=0; i<inList.size(); i++)
//     {
//       //m_InputImage->TransformIndexToPhysicalPoint(inList[i],inPoint);
//       outPoint = m_Transform->TransformPoint(inList[i]);
//       outList.push_back(idOut);
//     }

    return outList;
}


void 
GCPToSensorModelModel
::OK()
{
  ossimKeywordlist kwl;
  m_Transform->saveState(kwl);
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
