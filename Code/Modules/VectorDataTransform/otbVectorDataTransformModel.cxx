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

#include "otbVectorDataTransformModel.h"
#include "otbMsgReporter.h"
#include "otbListenerBase.h"
#include "otbMath.h"

namespace otb
{

VectorDataTransformModel::VectorDataTransformModel() : m_VisualizationModel(), 
                                                       m_PixelDescriptionModel(),
                                                       m_VectorImage(),
                                                       m_TransformedVectorData()
{
  m_VisualizationModel    = VisualizationModelType::New();
  m_PixelDescriptionModel = PixelDescriptionModelType::New();
}

void VectorDataTransformModel::OpenInputs(const VectorImageType * image, const VectorDataType* inputVd)
{
  m_VectorImage  = image;
  
  // Generate the layer
  m_ImageGenerator = LayerGeneratorType::New();
  m_ImageGenerator->SetImage(const_cast<VectorImageType *>(m_VectorImage.GetPointer()));
  m_ImageGenerator->GenerateLayer();
  
  if(m_VectorImage->GetNumberOfComponentsPerPixel() > 3)
    {
    m_Channels.push_back(2);
    m_Channels.push_back(1);
    m_Channels.push_back(0);
    }
  else if(m_VectorImage->GetNumberOfComponentsPerPixel() == 3)
    {
    m_Channels.push_back(0);
    m_Channels.push_back(1);
    m_Channels.push_back(2);
    }
  else if(m_VectorImage->GetNumberOfComponentsPerPixel() == 1)
    {
    m_Channels.push_back(0);
    m_Channels.push_back(0);
    m_Channels.push_back(0);
    }

  m_ImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(m_Channels);
  m_ImageGenerator->GetLayer()->SetName("Image");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();
  m_PixelDescriptionModel->ClearLayers();
  
  // Add the layer to the models
  m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());
  m_PixelDescriptionModel->AddLayer(m_ImageGenerator->GetLayer());

  m_VisualizationModel->Update();

  // Strore the input vector data projectionRef 
  m_InputVectorDataProjectionRef = inputVd->GetProjectionRef();

  // Project it to be able to visualize it in the GUI
  VectorDataProjectionFilterType::Pointer vproj = VectorDataProjectionFilterType::New();
  vproj->SetInput(inputVd);
  vproj->SetInputProjectionRef( inputVd->GetProjectionRef());
  vproj->SetOutputKeywordList(m_VectorImage->GetImageKeywordlist());
  vproj->SetOutputProjectionRef(m_VectorImage->GetProjectionRef());
  vproj->SetOutputOrigin(m_VectorImage->GetOrigin());
  vproj->SetOutputSpacing(m_VectorImage->GetSpacing());
  vproj->Update();
  m_InputVectorData = vproj->GetOutput();
  
  // Apply an identity transform to be showned in the GUI
  this->ApplyTransformToVectorData(1., 0., 0., 0., 0.,0.);

  // inputs opened notification
  this->NotifyAll("InputsOpened");
}

void 
VectorDataTransformModel::ApplyTransformToVectorData(double scale, double angle, 
                                                     double translationX, 
                                                     double translationY,
                                                     double centerX, double centerY)
{
  TransformType::Pointer transform = TransformType::New();     
  ParametersType parameters(6);
  parameters[0] = scale;                                                 // Scale
  parameters[1] = CONST_PI*angle/180.;                                   // Rotation Angle in radian
  parameters[2] = centerX;                                               // Center of the rotation (X)
  parameters[3] = centerY;                                               // Center of the rotation (Y) 
  parameters[4] = translationX * m_VectorImage->GetSpacing()[0];         // Translation (X) 
  parameters[5] = translationY * vcl_abs(m_VectorImage->GetSpacing()[1]);// Translation (Y)

  transform->SetParameters(parameters);
  
  // Apply the transform
  VectorDataTransformType::Pointer shift = VectorDataTransformType::New();
  shift->SetInput(m_InputVectorData);
  shift->SetTransform(transform);
  shift->Update();
  
  m_TransformedVectorData = shift->GetOutput();

  // Notiy the view that the VectorData is ready
  this->NotifyAll("VectorDataUpdated");
}

void 
VectorDataTransformModel::GenerateOuputVectorData()
{
  VectorDataProjectionFilterType::Pointer vproj = VectorDataProjectionFilterType::New();
  vproj->SetInput(m_TransformedVectorData);
  vproj->SetOutputProjectionRef(m_InputVectorDataProjectionRef);
  vproj->SetInputKeywordList(m_VectorImage->GetImageKeywordlist());
  vproj->SetInputProjectionRef(m_VectorImage->GetProjectionRef());
  vproj->SetInputOrigin(m_VectorImage->GetOrigin());
  vproj->SetInputSpacing(m_VectorImage->GetSpacing());
  vproj->Update();
  m_OutputVectorData = vproj->GetOutput();

  // Notiy the module that the oputput VectorData was built
  this->NotifyAll("OutputsUpdated");
}

void 
VectorDataTransformModel::Exit()
{
  // Generate the output VectorData
  this->GenerateOuputVectorData();
  
  // Exit notification
  this->NotifyAll("Quit");
}

}// end namespace
