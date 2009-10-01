#include "otbMeanShiftApplicationModel.h"
#include "otbFltkFilterWatcher.h"



namespace otb
{
/** Initialize the singleton */
MeanShiftApplicationModel::Pointer MeanShiftApplicationModel::Instance = NULL;
/** Manage the singleton */
MeanShiftApplicationModel::Pointer
MeanShiftApplicationModel::GetInstance()
{
  if (!Instance)
  {
    Instance = MeanShiftApplicationModel::New();
  }
  return Instance;
}

void MeanShiftApplicationModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

MeanShiftApplicationModel::MeanShiftApplicationModel() : m_VisualizationModel(), m_Reader(), m_MeanShift(), m_BlendingFunction()
{
  m_VisualizationModel    = VisualizationModelType::New();
  m_Reader = VectorReaderType::New();
  m_MeanShift = MSFilterType::New();
  m_BlendingFunction = BlendingFunctionType::New();

  m_BlendingFunction->SetAlpha(0.6);

  m_ImageGenerator = LayerGeneratorType::New();
  m_ClustersGenerator = LayerGeneratorType::New();
  m_BoundariesGenerator = LabelLayerGeneratorType::New();

  m_MeanShift->SetInput( m_Reader->GetOutput() );
  m_IsUpdating = false;
  m_IsImageReady = false;

}

MeanShiftApplicationModel
::~MeanShiftApplicationModel() {}


void
MeanShiftApplicationModel
::OpenImage( const char * filename)
{
  m_Reader->SetFileName(filename);
  m_Reader->UpdateOutputInformation();
  m_MeanShift->SetInput(m_Reader->GetOutput());

  // Generate the layer
  m_ImageGenerator->SetImage(m_Reader->GetOutput());
  m_ImageGenerator->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(m_ImageGenerator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  m_ImageGenerator->GenerateLayer();

    std::vector<unsigned int> channels;
  if(m_Reader->GetOutput()->GetNumberOfComponentsPerPixel()==3)
    {
    channels.push_back(0);
    channels.push_back(1);
    channels.push_back(2);
    }
  else
    {
    channels.push_back(2);
    channels.push_back(1);
    channels.push_back(0);
    }

  m_ImageGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

  
  
  m_ImageGenerator->GetLayer()->SetName("Image");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(m_ImageGenerator->GetLayer());

  m_VisualizationModel->Update();

  this->NotifyAll();
  m_IsImageReady = true;
}

void
MeanShiftApplicationModel
::SaveLabelImage( const char * filename)
{
  m_IsUpdating = true;
  LabeledWriterType::Pointer writer = LabeledWriterType::New();
  FltkFilterWatcher qlwatcher(writer,0,0,200,20,"Save Image ...");
  writer->SetFileName(filename);
  writer->SetInput(m_MeanShift->GetLabeledClusteredOutput());
  writer->Update();
  m_IsUpdating = false;

}

void
MeanShiftApplicationModel
::SaveClusterImage( const char * filename)
{
  m_IsUpdating = true;
  VectorWriterType::Pointer writer = VectorWriterType::New();
  FltkFilterWatcher qlwatcher(writer,0,0,200,20,"Save Image ...");
  writer->SetFileName(filename);
  writer->SetInput(m_MeanShift->GetClusteredOutput());
  writer->Update();
  m_IsUpdating = false;

}


void
MeanShiftApplicationModel
::SetSpatialRadius(unsigned int sr){
  m_SpatialRadius = sr;
  m_MeanShift->SetSpatialRadius(m_SpatialRadius);
}
void
MeanShiftApplicationModel
::SetSpectralRadius(unsigned int sr){
  m_SpectralRadius = sr;
  m_MeanShift->SetRangeRadius(m_SpectralRadius);
}

void
MeanShiftApplicationModel
::SetMinRegionSize(unsigned int mr){
  m_MinRegionSize = mr;
  m_MeanShift->SetMinimumRegionSize(m_MinRegionSize);
}


void
MeanShiftApplicationModel
::RunSegmentation()
{

  if(  m_IsImageReady )
    {
    m_IsUpdating = true;
    // Generate the layer

    m_MeanShift->SetSpatialRadius(m_SpatialRadius);
    m_MeanShift->SetRangeRadius(m_SpectralRadius);
    m_MeanShift->SetMinimumRegionSize(m_MinRegionSize);
    
    m_ClustersGenerator->SetImage(m_MeanShift->GetClusteredOutput());
    m_ClustersGenerator->GenerateQuicklookOff();
    m_ClustersGenerator->GenerateLayer();

    std::vector<unsigned int> channels;
  if(m_Reader->GetOutput()->GetNumberOfComponentsPerPixel()==3)
    {
    channels.push_back(0);
    channels.push_back(1);
    channels.push_back(2);
    }
  else
    {
    channels.push_back(2);
    channels.push_back(1);
    channels.push_back(0);
    }

  m_ClustersGenerator->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

    
    m_ClustersGenerator->GetLayer()->SetName("Segmentation");
    m_VisualizationModel->AddLayer(m_ClustersGenerator->GetLayer());

    m_VisualizationModel->Update();

    this->NotifyAll();
  
    }
  
}

void
MeanShiftApplicationModel
::SwitchClusters(bool sc)
{
  if(m_IsImageReady)
    {
    m_ClustersGenerator->GetLayer()->SetVisible(sc);
    }
  
  m_VisualizationModel->Update();
  
  this->NotifyAll();

}

void
MeanShiftApplicationModel
::SwitchBoundaries(bool sb)
{
  if(m_IsImageReady)
    {
    m_BoundariesGenerator->GetLayer()->SetVisible(sb);
    }
  
  m_VisualizationModel->Update();
  
  this->NotifyAll();

}


}
