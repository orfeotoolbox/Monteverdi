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
#include "otbFeatureExtractionBaseModel.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>
#include "itkExceptionObject.h"
#include "otbMacro.h"

#include "otbImageFileWriter.h"
#include "itkExtractImageFilter.h"

namespace otb
{

FeatureExtractionBaseModel::FeatureExtractionBaseModel()
{
  /** Initial parameters */
  m_NumberOfInputImages = 0;
  m_NumberOfChannels = 0;
  m_InputImageList = SingleImageListType::New();

  m_FilterList = FilterListType::New();
  m_FilterTypeList.clear();
  m_OutputFilterInformation.clear();
  m_OutputListOrder.clear();
  m_MinValues.clear();
  m_MaxValues.clear();
  m_SelectedMinValues.clear();
  m_SelectedMaxValues.clear();

  m_ChannelExtractorList     = ExtractROIFilterListType::New();
  m_IntensityFilter          = IntensityFilterType::New();

  // init input image + m_HasInput
  this->InitInput();

  // Instantiate the model
  m_VisuModel = VisuModelType::New();
  m_ResultVisuModel = VisuModelType::New();

  //Instantiate output image attributes
  m_image = SingleImageType::New();
  m_imageList = ImageListType::New();
  m_iL2VI = ImageListToVectorImageFilterType::New();

  m_HasChanged = false;
}

FeatureExtractionBaseModel
::~FeatureExtractionBaseModel() {}

void
FeatureExtractionBaseModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}

void
FeatureExtractionBaseModel
::SetInputImage(InputImagePointerType image)
{
  // Set the input image
  m_InputImage = image;
  m_InputImage->UpdateOutputInformation();

  m_NumberOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();

  // Togle the valid flag
  m_HasInput = true;
  // Notify the observers
  this->NotifyAll("SetInputImage");
}

void
FeatureExtractionBaseModel
::GenerateLayers()
{
  // Generate image layers
  LayerGeneratorPointerType lVisuGenerator = LayerGeneratorType::New();

  lVisuGenerator->SetImage(m_InputImage);
  lVisuGenerator->GenerateLayer();
  lVisuGenerator->GetLayer()->SetName("Image");
  // Add the layer to the model
  m_VisuModel->ClearLayers();
  m_VisuModel->AddLayer(lVisuGenerator->GetLayer());

  // Render
  m_VisuModel->Update();

  // Compute Min and max of the input image through the generated QL
  VectorImageMinMaxFilterType::Pointer stater = VectorImageMinMaxFilterType::New();
  stater->SetInput(lVisuGenerator->GetLayer()->GetQuicklook());

  stater->Update();

  for (unsigned int i = 0; i < m_NumberOfChannels; i++)
    {
    m_MinValues.push_back(stater->GetMinimum()[i]);
    m_MaxValues.push_back(stater->GetMaximum()[i]);
    }

  // Notify the observers
  this->NotifyAll("GenerateLayers");
}

void
FeatureExtractionBaseModel
::InitInput()
{
  m_HasInput = false;
  m_NumberOfChannels = 0;
  m_InputImageList->Clear();
  m_InputImage = InputImageType::New();
}

/***************************************
 ****** SINGLE INPUT GENERATION ********
 ***************************************/
void
FeatureExtractionBaseModel
::AddChannels(std::vector<unsigned int> chList)
{
  m_OutputChannelsInformation.clear();
  if (m_NumberOfChannels != 0)
    {
    unsigned int size = chList.size();
    m_SelectedMinValues.clear();
    m_SelectedMaxValues.clear();

    for (unsigned int i = 0; i < size; i++)
      {
      if (chList[i] <=  m_NumberOfChannels)
        {
        this->AddChannel(chList[i]);
        }
      else if (chList[i] ==  m_NumberOfChannels + 1)
        {
        this->AddIntensityChannel();
        }
      else
        {
        return;
        }

      // Add min, max values
      m_SelectedMinValues.push_back(static_cast<double>(m_MinValues[chList[i] - 1]));
      m_SelectedMaxValues.push_back(static_cast<double>(m_MaxValues[chList[i] - 1]));
      }
    }
}

void
FeatureExtractionBaseModel
::AddChannel(int id)
{
  ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
  extract->SetInput(m_InputImage);
  extract->SetChannel(id);
  extract->GetOutput()->UpdateOutputInformation();

  this->AddInputImage(extract->GetOutput());

  m_ChannelExtractorList->PushBack(extract);

  itk::OStringStream oss;
  oss << "Ch" << id;
  m_OutputChannelsInformation.push_back(oss.str());
}

void
FeatureExtractionBaseModel
::AddIntensityChannel()
{
  m_IntensityFilter->SetInput(m_InputImage);
  m_IntensityFilter->GetOutput()->UpdateOutputInformation();
  this->AddInputImage(m_IntensityFilter->GetOutput());
  m_OutputChannelsInformation.push_back("Int.");

  // Compute Min and max of the intensity image, onmy once
  if (m_MinValues.size() != m_NumberOfChannels + 1 && m_MaxValues.size() != m_NumberOfChannels + 1)
    {
    ImageMinMaxFilterType::Pointer stater = ImageMinMaxFilterType::New();
    stater->SetInput(m_IntensityFilter->GetOutput());
    stater->Update();
    m_MinValues.push_back(static_cast<double>(stater->GetMinimum()));
    m_MaxValues.push_back(static_cast<double>(stater->GetMaximum()));
    }
}

/************************************************
 ******************* FILTERS FUNCTIONS **********
 ************************************************/
void
FeatureExtractionBaseModel
::AddFeatureFilter(FilterType * filter, FeatureType type, int inputId, unsigned int indexMapval, std::string mess)
{
  this->AddFilter(filter);
  this->AddFilterType(type);

  itk::OStringStream oss;
  if (inputId != -1) oss << m_OutputChannelsInformation[inputId] << ": " << mess;
  else oss << mess;
  m_OutputFilterInformation.push_back(oss.str());
  m_OutputIndexMap.push_back(indexMapval);
  m_SelectedFilters.push_back(true);
  m_OutputListOrder.push_back(std::max(0, static_cast<int>(m_OutputListOrder.size())));
}

/****************************************************
 ******************* FIN FILTERS FUNCTIONS **********
 ****************************************************/

void
FeatureExtractionBaseModel
::GenerateOutputImage()
{
//   SingleImagePointerType image = SingleImageType::New();
//   ImageListType::Pointer imageList = ImageListType::New();

  bool todo = false;
  int  outputNb = 0;
  int  i = 0;

  if (!m_HasInput) itkExceptionMacro("Impossible to create output image: no image image selected.");
  if (m_OutputListOrder.size() == 0) itkExceptionMacro("Impossible to create output image: no feature selected.");

  m_imageList->Clear();

  for (unsigned int ii = 0; ii < m_OutputListOrder.size(); ii++)
    {
    i = m_OutputListOrder[ii];
    if (m_SelectedFilters[i] == true)
      {
      todo = true;
      m_image = GetSingleImage(i);
      m_imageList->PushBack(m_image);
      outputNb++;
      } // if(m_SelectedFilters[i] == true)
    } //  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)

  if (todo == true)
    {
    m_iL2VI->SetInput(m_imageList);

    m_OutputImage = m_iL2VI->GetOutput();

    m_OutputImage->UpdateOutputInformation();

    m_HasChanged = true;
    this->NotifyAll("OutputsUpdated");
    }
}

FeatureExtractionBaseModel
::SingleImagePointerType
FeatureExtractionBaseModel
::GetSingleImage(int i)
{
  itkExceptionMacro(<<"GetSingleImage(int) method must be re-implemented by sub-classes.");
}

void
FeatureExtractionBaseModel
::GetSingleOutput(int id)
{
  // Generate image layers
  SingleLayerGeneratorPointerType lResultVisuGenerator = SingleLayerGeneratorType::New();
  // To avoid drawing a quicklook( ScrollView) for nothing
  lResultVisuGenerator->SetGenerateQuicklook(false);

  lResultVisuGenerator->SetImage(this->GetSingleImage(id));
  lResultVisuGenerator->GenerateLayer();
  lResultVisuGenerator->GetLayer()->SetName("FeatureImage");

  // Add the layer to the model
  m_ResultVisuModel->ClearLayers();
  m_ResultVisuModel->AddLayer(lResultVisuGenerator->GetLayer());

  // Render
  m_ResultVisuModel->Update();
}

void
FeatureExtractionBaseModel
::Quit()
{

  this->NotifyAll("BusyOff");
}

void
FeatureExtractionBaseModel
::Cancel()
{
  this->NotifyAll("Cancel");
}
}
