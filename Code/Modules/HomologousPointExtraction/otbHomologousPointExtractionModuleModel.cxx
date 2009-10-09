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

#include "otbHomologousPointExtractionModuleModel.h"
#include "otbFltkFilterWatcher.h"



namespace otb
{
/** Initialize the singleton */
HomologousPointExtractionModuleModel::Pointer HomologousPointExtractionModuleModel::Instance = NULL;
/** Manage the singleton */
HomologousPointExtractionModuleModel::Pointer
HomologousPointExtractionModuleModel::GetInstance()
{
  if (!Instance)
  {
    Instance = HomologousPointExtractionModuleModel::New();
  }
  return Instance;
}

void HomologousPointExtractionModuleModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

HomologousPointExtractionModuleModel::HomologousPointExtractionModuleModel() : m_VisualizationModel(), m_BlendingFunction()
{
  srand((unsigned)time(0));

  VisualizationModelType::Pointer visualizationModel1 = VisualizationModelType::New();
  VisualizationModelType::Pointer visualizationModel2 = VisualizationModelType::New();
  m_VisualizationModel.push_back(visualizationModel1);
  m_VisualizationModel.push_back(visualizationModel2);

  BlendingFunctionType::Pointer blendingFunction1 = BlendingFunctionType::New();
  BlendingFunctionType::Pointer blendingFunction2 = BlendingFunctionType::New();
  blendingFunction1->SetAlpha(0.6);
  blendingFunction2->SetAlpha(0.6);
  m_BlendingFunction.push_back(blendingFunction1);
  m_BlendingFunction.push_back(blendingFunction2);


  LayerGeneratorType::Pointer imageGenerator1 = LayerGeneratorType::New();
  LayerGeneratorType::Pointer imageGenerator2 = LayerGeneratorType::New();
  m_ImageGenerator.push_back(imageGenerator1);
  m_ImageGenerator.push_back(imageGenerator2);

  m_FirstInputImage = VectorImageType::New();
  m_SecondInputImage = VectorImageType::New();
  m_Images.push_back(m_FirstInputImage);
  m_Images.push_back(m_SecondInputImage);

  m_IndexesList.clear();

  m_OutputChanged = false;  

}

HomologousPointExtractionModuleModel
::~HomologousPointExtractionModuleModel() {}


void
HomologousPointExtractionModuleModel
::SetFirstInputImage(VectorImagePointerType img)
{
  this->SetImage(0, img);
}
  

void
HomologousPointExtractionModuleModel
::SetSecondInputImage(VectorImagePointerType img)
{
  this->SetImage(1, img);
}

void
HomologousPointExtractionModuleModel
::SetImage(unsigned int id, VectorImagePointerType image)
{
  if( id != 0 && id != 1 )
    itkExceptionMacro(<<"Invalid image id "<<id<<".");
 
  image->UpdateOutputInformation();
  if( id == 0 )
    m_FirstInputImage = image;
 else
    m_SecondInputImage = image;

  m_Images[id] = image;
  
  // Generate the layer
  m_ImageGenerator[id]->SetImage(image);
  m_ImageGenerator[id]->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(m_ImageGenerator[id]->GetResampler(),0,0,200,20,"Generating QuickLook...");
  m_ImageGenerator[id]->GenerateLayer();
  
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
    m_ImageGenerator[id]->GetLayer()->GetRenderingFunction()->SetChannelList(channels);
  
  if( id == 0 )
    m_ImageGenerator[id]->GetLayer()->SetName("FirstImage");
  else
    m_ImageGenerator[id]->GetLayer()->SetName("SecondImage");

  // Clear previous layers
  m_VisualizationModel[id]->ClearLayers();
  
  // Add the layer to the models
  m_VisualizationModel[id]->AddLayer(m_ImageGenerator[id]->GetLayer());
  m_VisualizationModel[id]->Update();
  
  this->NotifyAll();
  //m_IsImageReady = true;
}

void
HomologousPointExtractionModuleModel
::AddIndexesToList( IndexType id1, IndexType id2 )
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
HomologousPointExtractionModuleModel
::RemovePointFromList( unsigned int id )
{
  if( id>=m_IndexesList.size() )
    itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size ("<<m_IndexesList.size()<<").");
  
  m_IndexesList.erase(m_IndexesList.begin()+id);
}
}

