/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "otbSpectrumModuleModel.h"

#include "otbStreamingImageFileWriter.h"
#include "otbStreamingMinMaxVectorImageFilter.h"
#include "otbFltkFilterWatcher.h"
#include "otbMath.h"

namespace otb {

SpectrumModuleModel
::SpectrumModuleModel()
{
  m_SpectralAngleLayerAvailable = false;
  m_UseColorMap = false;

  // Member initialization
  m_PixelSpectrumModel = PixelSpectrumModelType::New();
  m_ImageModel = ImageModelType::New();

  m_PixelSpectrumModel->SetLayers(m_ImageModel->GetLayers());
  m_SpectralLayerGenerator = LayerGeneratorType::New();
  m_SpectralLayerGenerator->SetGenerateQuicklook(false);

  m_SpectralAnglefilter = SpectralFilterType::New();
  m_SpectralAnglefilterQL = SpectralFilterType::New();

  m_OriginalDynamic = (int) (255 / CONST_PI);
  m_LastBWContrast = m_OriginalDynamic;
  m_rescaleFilterImage = rescaleFiltertype::New();
  m_rescaleFilterQL = rescaleFiltertype::New();
  m_rescaleFilterImage->SetScale(m_OriginalDynamic);
  m_rescaleFilterQL->SetScale(m_OriginalDynamic);

  m_CastImage = ImageToVectorImageFilterType::New();
  m_CastQL = ImageToVectorImageFilterType::New();

  m_SecondLayerToAdd = 1;
  m_BlendingFunction = BlendingFunctionType::New();
  m_BlendingFunction->SetAlpha(0.5);

  //color mapper
  m_Colormapper = ColorMapFilterType::New();
  m_ColormapperQL = ColorMapFilterType::New();

  // color map
  m_Colormap = ColorMapFunctorType::New();
  m_Colormap->SetMinimumInputValue(0);
  m_Colormap->SetMaximumInputValue(255 / CONST_PI);

  // filter to cast rgb image to vector image
  m_RgbToVectorFilter = RGBtoVectorImageCastFilterType::New();
  m_RgbToVectorFilterQL = RGBtoVectorImageCastFilterType::New();

}

SpectrumModuleModel
::~SpectrumModuleModel()
{
}

void
SpectrumModuleModel
::UpdateViewerDisplay(std::vector<unsigned int> ch)
{
  if (!m_IsImageReady)
    {
    return;
    }

  unsigned int layerNb = m_ImageModel->GetNumberOfLayers();
  if (layerNb == 0)
    {
    itkExceptionMacro("Invalid number of layers");
    }

  m_Channels = ch;
  m_LayerGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(m_Channels);
  m_ImageModel->Update();
}

void
SpectrumModuleModel
::DetermineMinMaxValues()
{
  bool             hasQuickLook;
  PixelVectorType  minValues, maxValues;
  ImagePointerType tempImage;

  m_LayerGenerator = LayerGeneratorType::New();
  m_LayerGenerator->SetImage(m_InputImage);

  //FltkFilterWatcher qlwatcher(m_LayerGenerator->GetProgressSource(), 0, 0, 200, 20, otbGetTextMacro("Generating QuickLook ..."));
  m_LayerGenerator->GenerateLayer();

  hasQuickLook = m_LayerGenerator->GetLayer()->GetHasQuicklook();

  // checking for any quicklook
  if (hasQuickLook)
    {
    tempImage = m_LayerGenerator->GetLayer()->GetQuicklook();
    }
  else
    {
    tempImage = m_InputImage;
    }

  typedef otb::StreamingMinMaxVectorImageFilter<ImageType> StatFilterType;
  StatFilterType::Pointer statFilter = StatFilterType::New();
  statFilter->SetInput(tempImage);
  statFilter->Update();

  ImageType::PixelType min = statFilter->GetMinimum();
  ImageType::PixelType max = statFilter->GetMaximum();

  // Limit the bounds arbitrarily : performance issue with files containing arbitrary values (like uninitialized memory)
  for (unsigned int i = 0; i < min.GetSize(); i++)
    {
    if (min [i] < 0)
      {
      min[i] = 0;
      }
    if (max [i] > 65535)
      {
      max[i] = 65535;
      }
    }

  m_LayerGenerator->GetLayer()->SetMinValues(min);
  m_LayerGenerator->GetLayer()->SetMaxValues(max);
}

void
SpectrumModuleModel
::SetInputImage(ImagePointerType image)
{

  m_InputImage = image;
  m_InputImage->UpdateOutputInformation();

  this->NotifyAll("SetInputImage");
  m_IsImageReady = true;

  this->DetermineMinMaxValues();
  m_ImageModel->AddLayer(m_LayerGenerator->GetLayer());

  std::vector<unsigned int> channels;
  unsigned int              nbComponent = m_InputImage->GetNumberOfComponentsPerPixel();

  if (nbComponent >= 3)
    {
    channels.push_back(0);
    channels.push_back(1);
    channels.push_back(2);
    }
  else
    {
    channels.push_back(0);
    channels.push_back(0);
    channels.push_back(0);
    }
  this->UpdateViewerDisplay(channels);

}

void
SpectrumModuleModel
::Notify(ListenerBase * listener)
{
  listener->Notify();
}

void
SpectrumModuleModel
::SaveAndQuit()
{
  if (m_SpectralAngleLayerAvailable)
    this->NotifyAll("OutputsUpdated");
  this->Quit();
}

void
SpectrumModuleModel
::Quit()
{
  this->NotifyAll("BusyOff");
}

void
SpectrumModuleModel
::GenerateSpectralAngle(unsigned int Id)
{

  m_ImageModel->DeleteLayerByName("Spectral");

  if (Id == PixelSpectrumModelType::NoneID)
    {
    m_ImageModel->Update();
    return;
    }

  // recupération du spectre
  PixelVectorType spectre = m_PixelSpectrumModel->GetSpectrumByID(Id);

  //  spectral angle computing
  m_SpectralAnglefilter->SetReferencePixel(spectre);
  m_SpectralAnglefilter->SetInput(m_InputImage);
  m_rescaleFilterImage->SetInput(m_SpectralAnglefilter->GetOutput());

  // traitement du quicklook
  m_SpectralAnglefilterQL->SetReferencePixel(spectre);
  m_SpectralAnglefilterQL->SetInput(m_LayerGenerator->GetLayer()->GetQuicklook());
  m_rescaleFilterQL->SetInput(m_SpectralAnglefilterQL->GetOutput());

  float val = 0;

  if (m_UseColorMap)
    {
    ForceSpectralContrast(m_OriginalDynamic);
    val = m_OriginalDynamic;
    // set a color-mapper
    m_Colormapper->UseInputImageExtremaForScalingOff();
    m_ColormapperQL->UseInputImageExtremaForScalingOff();
    m_Colormapper->SetColormap(m_Colormap);
    m_ColormapperQL->SetColormap(m_Colormap);
    m_Colormapper->SetInput(m_rescaleFilterImage->GetOutput());
    m_ColormapperQL->SetInput(m_rescaleFilterQL->GetOutput());

    // rgb to vector image (in)
    m_RgbToVectorFilter->SetInput(m_Colormapper->GetOutput());
    m_RgbToVectorFilterQL->SetInput(m_ColormapperQL->GetOutput());

    // rgb to vector image (out)
    m_spectralImage = m_RgbToVectorFilter->GetOutput();
    m_spectralQL = m_RgbToVectorFilterQL->GetOutput();

    //m_RgbToVectorFilter->Update();
    m_RgbToVectorFilterQL->Update();

    }
  else
    {
    ForceSpectralContrast(m_LastBWContrast);

    val = m_LastBWContrast;

    // image to vector image (in)
    m_CastImage->SetInput(m_rescaleFilterImage->GetOutput());
    m_CastQL->SetInput(m_rescaleFilterQL->GetOutput());

    // image to vector image (out)
    m_spectralImage = m_CastImage->GetOutput();
    m_spectralQL = m_CastQL->GetOutput();

    //m_CastImage->Update();
    m_CastQL->Update();
    }

  // 2 - association a l'image
  m_SpectralLayerGenerator = LayerGeneratorType::New();
  m_SpectralLayerGenerator->SetGenerateQuicklook(false);

  m_SpectralLayerGenerator->SetImage(m_spectralImage);
  m_SpectralLayerGenerator->SetQuicklook(m_spectralQL);

  // 3 - generation du layer
  m_SpectralLayerGenerator->GenerateLayer();
  m_SpectralLayerGenerator->GetLayer()->SetName("Spectral");
  m_SpectralLayerGenerator->GetLayer()->SetBlendingFunction(m_BlendingFunction);

  // 4 - ajout au ImageModel
  m_ImageModel->AddLayer(m_SpectralLayerGenerator->GetLayer());
  m_SecondLayerToAdd = 0;

  m_ImageModel->Update();

  m_SpectralAngleLayerAvailable = true;
}

void
SpectrumModuleModel
::ClearSpectralAngle()
{
  m_SpectralAngleLayerAvailable = false;
  if (m_ImageModel->GetLayerByName("Spectral") != NULL)
    {
    m_ImageModel->GetLayerByName("Spectral")->SetVisible(false);
    m_ImageModel->Update();
    }
}

void
SpectrumModuleModel
::SetSpectralLayerOpacity(float f)
{
  m_BlendingFunction->SetAlpha(f);
  m_ImageModel->Update();
}

void
SpectrumModuleModel
::WriteSpreadsheetFile(int id, const char *path)
{

  std::ostringstream address, content;
  address << path;

  string fileType = address.str().substr(address.str().length() - 4, address.str().length() - 1);
  if (fileType.compare(".csv"))
    {
    address << ".csv";
    }

  PixelSpectrumModelType::SpectrumVectorType data = m_PixelSpectrumModel->GetSpectrumByID(id);
  int                                        size = data.GetSize();

  content << "# The following data represents the whole spectrum at the index " << "( " <<
  m_PixelSpectrumModel->GetSpectrumIndexByID(id)[0] << " , " << m_PixelSpectrumModel->GetSpectrumIndexByID(id)[1] <<
  " )\n\n";
  content << "# Channel;Intensity\n";

  for (int i = 0; i < size; i++)
    {
    content << i << ";" << data.GetElement(i) << "\n";
    }

  /** text format (cvs) export */
  std::ofstream csvFile(address.str().c_str(), std::ios::out | std::ios::trunc);

  if (csvFile)
    {
    csvFile << content.str() << std::endl;
    csvFile.close();
    }
  else
    {
    otbMsgDevMacro(<< "The file cannot be opened" << std::endl);
    }
}

void
SpectrumModuleModel
::ForceSpectralContrast(float c)
{
  m_rescaleFilterImage->SetScale(c);
  m_rescaleFilterQL->SetScale(c);

  if (c != m_OriginalDynamic)
    {
    m_LastBWContrast = c;
    }

  m_ImageModel->Update();
}

}
