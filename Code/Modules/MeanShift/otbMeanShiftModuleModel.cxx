#include "otbMeanShiftModuleModel.h"
#include "otbFltkFilterWatcher.h"

namespace otb
{

MeanShiftModuleModel::MeanShiftModuleModel()
{
  m_VisualizationModel    = VisualizationModelType::New();
  m_MeanShift = MSFilterType::New();
  m_MeanShiftSmooth = MSSmoothFilterType::New();
  
  m_MeanShift->SetThreshold(0.1);
  m_MeanShift->SetMaxIterationNumber(100);
  
  m_MeanShiftSmooth->SetThreshold(0.1);
  m_MeanShiftSmooth->SetMaxIterationNumber(100);
  
  m_BoundaryExtractor = BoundaryExtractorType::New();
  m_BoundaryExtractor->SetRadius(1);
  
  m_BlendingFunction = BlendingFunctionType::New();

  m_BlendingFunction->SetAlpha(0.6);

  m_ImageGenerator = LayerGeneratorType::New();
  m_ClustersGenerator = LayerGeneratorType::New();
  m_BoundariesGenerator = LabelLayerGeneratorType::New();
  m_InputImage = VectorImageType::New();

  m_IsUpdating = false;
  m_IsImageReady = false;
  m_ShowingResult = false;

  m_Channels.clear();
}

MeanShiftModuleModel
::~MeanShiftModuleModel()
{
}

void MeanShiftModuleModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

void
MeanShiftModuleModel
::SetSpatialRadius(unsigned int sr){
  m_SpatialRadius = sr;
  m_MeanShift->SetSpatialBandwidth(m_SpatialRadius);
  m_MeanShiftSmooth->SetSpatialBandwidth(m_SpatialRadius);
}
void
MeanShiftModuleModel
::SetSpectralRadius(double sr){
  m_SpectralRadius = sr;
  m_MeanShift->SetRangeBandwidth(m_SpectralRadius);
  m_MeanShiftSmooth->SetRangeBandwidth(m_SpectralRadius);
}

void
MeanShiftModuleModel
::SetMinRegionSize(unsigned int mr){
  m_MinRegionSize = mr;
  m_MeanShift->SetMinRegionSize(m_MinRegionSize);
}

void
MeanShiftModuleModel
::SetInputImage(VectorImagePointerType image)
{
  m_InputImage = image;

  m_InputImage->UpdateOutputInformation();
  m_MeanShift->SetInput(m_InputImage);
  m_MeanShiftSmooth->SetInput(m_InputImage);

  // Generate the layer
  m_ImageGenerator->SetImage(m_InputImage);
  m_ImageGenerator->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(m_ImageGenerator->GetProgressSource(), 0, 0, 200, 20, "Generating QuickLook...");
  m_ImageGenerator->GenerateLayer();

  m_ImageGenerator->GetLayer()->SetName("Image");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());

  m_VisualizationModel->Update();

  m_Channels = m_ImageGenerator->GetLayer()->GetRenderingFunction()->GetChannelList();

  this->NotifyAll("SetInputImage");
  m_IsImageReady = true;
}

void
MeanShiftModuleModel
::RunSegmentationModel()
{
  if (m_IsImageReady && !m_ShowingResult)
    {
    m_IsUpdating = true;
    // Generate the layer

    m_MeanShift->SetSpatialBandwidth(m_SpatialRadius);
    m_MeanShift->SetRangeBandwidth(m_SpectralRadius);
    m_MeanShift->SetMinRegionSize(m_MinRegionSize);
    
    m_MeanShiftSmooth->SetSpatialBandwidth(m_SpatialRadius);
    m_MeanShiftSmooth->SetRangeBandwidth(m_SpectralRadius);

    m_ClustersGenerator->SetImage(m_MeanShift->GetClusteredOutput());
    m_ClustersGenerator->GenerateQuicklookOff();
    m_ClustersGenerator->GenerateLayer();

    m_BoundaryExtractor->SetInput(m_MeanShift->GetLabelOutput());
    m_BoundariesGenerator->SetImage(m_BoundaryExtractor->GetOutput());
    m_BoundariesGenerator->GenerateQuicklookOff();
    m_BoundariesGenerator->GenerateLayer();

    m_ClustersGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(m_Channels);

    m_ClustersGenerator->GetLayer()->SetName("Segmentation");
    m_ClustersGenerator->GetLayer()->SetVisible(false);

    m_BoundariesGenerator->GetLayer()->SetName("Boundaries");
    m_BoundariesGenerator->GetLayer()->SetVisible(false);

    m_VisualizationModel->AddLayer(m_ClustersGenerator->GetLayer());
    m_VisualizationModel->AddLayer(m_BoundariesGenerator->GetLayer());

    m_VisualizationModel->Update();

    //m_MeanShift->UpdateOutputInformation();
    //m_MeanShift->GetClusteredOutput()->CopyInformation(m_InputImage);
    //m_MeanShift->GetLabelOutput()->CopyInformation(m_InputImage);
    //m_MeanShift->GetLabelOutput()->SetNumberOfComponentsPerPixel(1);
    
    m_MeanShiftSmooth->Update();

    m_IsUpdating = false;
    }
}

void
MeanShiftModuleModel
::SwitchClusters(bool sc)
{
  if (m_ShowingResult)
    {
    return;
    }
  if (m_IsImageReady)
    {
      m_ClustersGenerator->GetLayer()->SetVisible(sc);
      m_ImageGenerator->GetLayer()->SetVisible(!sc);
    }

  m_ShowingResult = true;
  m_VisualizationModel->Update();
  m_ShowingResult = false;
}

void
MeanShiftModuleModel
::SwitchBoundaries(bool sb)
{
  if (m_ShowingResult)
    {
    return;
    }
  if (m_IsImageReady)
    {
    m_BoundariesGenerator->GetLayer()->SetVisible(sb);
    }

  m_ShowingResult = true;
  m_VisualizationModel->Update();
  m_ShowingResult = false;
}

void
MeanShiftModuleModel
::SetOpacity(double op)
{
  if (m_ShowingResult)
    {
    return;
    }
  dynamic_cast<BlendingFunctionType *>(m_BoundariesGenerator->GetLayer()->GetBlendingFunction())->SetAlpha(op);
  m_ShowingResult = true;
  m_VisualizationModel->Update();
  m_ShowingResult = false;
}

void
MeanShiftModuleModel
::UpdateViewerDisplay(std::vector<unsigned int> ch)
{
  if (m_ShowingResult)
    {
    return;
    }
  if (!m_IsImageReady)
    {
    return;
    }
  unsigned int layerNb = m_VisualizationModel->GetNumberOfLayers();
  if (layerNb == 0)
    {
    itkExceptionMacro("Invalid number of layers");
    }

  m_Channels = ch;

  m_ImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(m_Channels);

  if (layerNb > 1)
    {
    m_ClustersGenerator->GetRenderingFunction()->SetChannelList(m_Channels);
    }

  m_ShowingResult = true;
  m_VisualizationModel->Update();
  m_ShowingResult = false;
}

void
MeanShiftModuleModel
::Quit()
{
  if (m_IsImageReady && !m_IsUpdating && !m_ShowingResult)
    {
    m_OutputFilteredImage = m_MeanShiftSmooth->GetOutput();
    m_OutputClusteredImage = m_MeanShift->GetClusteredOutput();
    m_OutputLabeledImage = m_MeanShift->GetLabelOutput();
    m_OutputBoundariesImage = m_BoundaryExtractor->GetOutput();
    this->NotifyAll("OutputsUpdated");
    }
  else
    {
    m_OutputFilteredImage = 0;
    m_OutputClusteredImage = 0;
    m_OutputLabeledImage = 0;
    m_OutputBoundariesImage = 0;
    }

  this->NotifyAll("BusyOff");
}

}
