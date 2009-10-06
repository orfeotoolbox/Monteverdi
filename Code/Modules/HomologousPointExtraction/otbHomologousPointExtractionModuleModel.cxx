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
  m_VisualizationModel    = VisualizationModelType::New();
  m_BlendingFunction = BlendingFunctionType::New();

  m_BlendingFunction->SetAlpha(0.6);

  m_ImageGenerator = LayerGeneratorType::New();

  m_FirstInputImage = VectorImageType::New();
  m_SecondInputImage = VectorImageType::New();

  m_OutputChanged = false;  

}

HomologousPointExtractionModuleModel
::~HomologousPointExtractionModuleModel() {}


// void
// HomologousPointExtractionModuleModel
// ::OpenImage( const char * filename)
// {
//   m_Reader->SetFileName(filename);
//   m_Reader->UpdateOutputInformation();
//   m_HomologousPointExtraction->SetInput(m_Reader->GetOutput());

//   m_OutputFilteredImage = m_HomologousPointExtraction->GetOutput();
//   m_OutputClusteredImage = m_HomologousPointExtraction->GetClusteredOutput();
//   m_OutputLabeledImage = m_HomologousPointExtraction->GetLabeledClusteredOutput();

//   // Generate the layer
//   m_ImageGenerator->SetImage(m_Reader->GetOutput());
//   m_ImageGenerator->GenerateQuicklookOn();
//   FltkFilterWatcher qlwatcher(m_ImageGenerator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
//   m_ImageGenerator->GenerateLayer();

//     std::vector<unsigned int> channels;
//   if(m_Reader->GetOutput()->GetNumberOfComponentsPerPixel()==3)
//     {
//     channels.push_back(0);
//     channels.push_back(1);
//     channels.push_back(2);
//     }
//   else
//     {
//     channels.push_back(2);
//     channels.push_back(1);
//     channels.push_back(0);
//     }

//   m_ImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

  
  
//   m_ImageGenerator->GetLayer()->SetName("Image");

//   // Clear previous layers
//   m_VisualizationModel->ClearLayers();

//   // Add the layer to the models
//   m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());

//   m_VisualizationModel->Update();

//   this->NotifyAll();
//   m_IsImageReady = true;
// }


// void
// HomologousPointExtractionModuleModel
// ::SetInputImage(VectorImagePointerType image)
// {
//   m_InputImage = image;
  
//   m_InputImage->UpdateOutputInformation();
//   m_HomologousPointExtraction->SetInput(m_InputImage);

//   // Generate the layer
//   m_ImageGenerator->SetImage(m_InputImage);
//   m_ImageGenerator->GenerateQuicklookOn();
//   FltkFilterWatcher qlwatcher(m_ImageGenerator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
//   m_ImageGenerator->GenerateLayer();

//   std::vector<unsigned int> channels;
//   if(m_InputImage->GetNumberOfComponentsPerPixel()==3)
//     {
//       channels.push_back(0);
//       channels.push_back(1);
//       channels.push_back(2);
//     }
//   else
//     {
//       channels.push_back(2);
//       channels.push_back(1);
//       channels.push_back(0);
//     }

//   m_ImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

  
  
//   m_ImageGenerator->GetLayer()->SetName("Image");

//   // Clear previous layers
//   m_VisualizationModel->ClearLayers();

//   // Add the layer to the models
//   m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());

//   m_VisualizationModel->Update();

//   this->NotifyAll();
//   m_IsImageReady = true;
// }




}
