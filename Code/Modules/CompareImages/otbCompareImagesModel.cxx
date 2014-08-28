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

#include "otbCompareImagesModel.h"
#include "otbFltkFilterWatcher.h"

namespace otb
{
/**
 * Constructor
 */
CompareImagesModel::CompareImagesModel()
{
  m_LeftVisuModel    = VisuModelType::New();
  m_RightVisuModel    = VisuModelType::New();
  
  m_LeftImageGenerator = LayerGeneratorType::New();
  
  m_LeftPixelDescriptionModel = PixelDescriptionModelType::New();
  m_RightPixelDescriptionModel = PixelDescriptionModelType::New();

  m_CompareImagessFilter = CompareImagessFilterType::New();

  m_PSNR = 0;
  m_RMSE = 0;
  m_MaxError = 0;
  
  m_OutputChanged = false;
}

/**
 * Destructor
 */
CompareImagesModel::
~CompareImagesModel(){}


/**
* SetInputLeftImage
*/
void
CompareImagesModel
::SetInputLeftImage(InputImagePointer leftImage)
{
  m_InputLeftImage = leftImage;

  m_InputLeftImage->UpdateOutputInformation();
  //m_CompareImagessFilter->SetInput1(m_InputLeftImage);

  // Generate the layer
  //LayerGeneratorType::Pointer leftImageGenerator = LayerGeneratorType::New();
  m_LeftImageGenerator->SetImage(m_InputLeftImage);
  m_LeftImageGenerator->GenerateQuicklookOn();
  //FltkFilterWatcher qlwatcher(m_LeftImageGenerator->GetResampler(), 0, 0, 200, 20, "Generating QuickLook...");
  FltkFilterWatcher qlwatcher(m_LeftImageGenerator->GetProgressSource(), 0, 0, 200, 20,
                                  "Generating DEM QuickLook ...");
  m_LeftImageGenerator->GenerateLayer();

  m_LeftImageGenerator->GetLayer()->SetName("LeftImage");
  m_LeftImageGenerator->GetLayer()->VisibleOn();

  // Clear previous layers
  m_LeftVisuModel->ClearLayers();

  // Add the layer to the models
  m_LeftVisuModel->AddLayer(m_LeftImageGenerator->GetLayer());
  m_LeftVisuModel->Update();
  
  //m_PixelDescriptionModel->AddLayer(m_LeftVisuModel->GetLayerByName("LeftImage"));
  m_LeftPixelDescriptionModel->SetLayers(m_LeftVisuModel->GetLayers());
  
  this->NotifyAll("SetInputLeftImage");
  


}

/**
* SetInputRightImage
*/
void
CompareImagesModel
::SetInputRightImage(InputImagePointer rightImage)
{
  m_InputRightImage = rightImage;

  m_InputRightImage->UpdateOutputInformation();
  //m_CompareImagessFilter->SetInput2(m_InputRightImage);


  //std::cout<<"m_LeftImageGenerator->GetQuicklook() : "<<m_LeftImageGenerator->GetQuicklook()->GetLargestPossibleRegion()<<std::endl;
  
  // Generate the layer
  LayerGeneratorType::Pointer rightImageGenerator = LayerGeneratorType::New();
  rightImageGenerator->SetImage(m_InputRightImage);
  rightImageGenerator->GenerateQuicklookOn();
  //rightImageGenerator->SetQuicklook(m_LeftImageGenerator->GetQuicklook());
  rightImageGenerator->GenerateLayer();


  rightImageGenerator->GetLayer()->SetName("RightImage");
  rightImageGenerator->GetLayer()->VisibleOn();

  // Clear previous layers
  m_RightVisuModel->ClearLayers();

  // Add the layer to the models
  m_RightVisuModel->AddLayer(rightImageGenerator->GetLayer());

  m_RightVisuModel->Update();
  
  m_RightPixelDescriptionModel->SetLayers(m_RightVisuModel->GetLayers());
  this->NotifyAll("SetInputRightImage");

}


/**
* ComputeCompareImages
*/
void
CompareImagesModel
::ComputeCompareImages()
{
  m_CompareImagessFilter->SetInput1(m_InputLeftImage);
  m_CompareImagessFilter->SetInput2(m_InputRightImage);
  m_CompareImagessFilter->SetPhysicalSpaceCheck(false);
  m_CompareImagessFilter->Update();
  
  this->SetRMSE(static_cast<double>(vcl_sqrt(m_CompareImagessFilter->GetMSE())));
  this->SetPSNR(static_cast<double>(m_CompareImagessFilter->GetPSNR()));
  this->SetMaxError(static_cast<double>(m_CompareImagessFilter->GetMAE()));
  
  m_OutputChanged=true;
  this->NotifyAll("StatsComputed");
}

/**
* SaveQuit
*/
void
CompareImagesModel
::SaveQuit()
{
  m_OutputChanged=false;
  this->NotifyAll("SaveQuit");
}

void
CompareImagesModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}

} // End namespace
