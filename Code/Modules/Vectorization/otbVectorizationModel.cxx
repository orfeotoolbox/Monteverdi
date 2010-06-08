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
#include "otbVectorizationModel.h"

namespace otb
{
/** Initialize the singleton */
VectorizationModel::Pointer VectorizationModel::Instance = NULL;

/** Manage the singleton */
VectorizationModel::Pointer
VectorizationModel::
GetInstance()
{
  if (!Instance)
  {
    Instance = VectorizationModel::New();
  }
  return Instance;
}

void VectorizationModel::
Notify(ListenerBase * listener)
{
  listener->Notify();
}

VectorizationModel::
VectorizationModel() : m_VisualizationModel(),
                       m_ImageGenerator(),
                       m_BlendingFunction(),
                       m_InputImage(),
                       m_VectorDataModel()
{
  // Visualization
  m_VisualizationModel  = VisualizationModelType::New();
  m_BlendingFunction    = BlendingFunctionType::New();
  m_ImageGenerator      = LayerGeneratorType::New();
  
  m_BlendingFunction->SetAlpha(0.6);
  
  // Input & Output
  m_InputImage = VectorImageType::New();

  // VectorData model
  m_VectorDataModel = VectorDataModelType::New();
  m_VectorDataModel->RegisterListener(this);
}

VectorizationModel
::~VectorizationModel()
{}


void
VectorizationModel
::SetImage(VectorImagePointerType image)
{
  image->UpdateOutputInformation();
  m_InputImage = image;

  // Generate the layer
  m_ImageGenerator->SetImage(image);
  m_ImageGenerator->GenerateQuicklookOn();
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

  m_VectorDataModel->SetOrigin(m_InputImage->GetOrigin());
  m_VectorDataModel->SetSpacing(m_InputImage->GetSpacing());
}

void
VectorizationModel
::Notify()
{
  this->NotifyAll();
}

}// namespace otb
