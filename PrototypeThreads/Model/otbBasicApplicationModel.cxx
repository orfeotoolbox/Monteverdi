#include "otbBasicApplicationModel.h"
#include "otbFltkFilterWatcher.h"


namespace otb
{
/** Initialize the singleton */
BasicApplicationModel::Pointer BasicApplicationModel::Instance = NULL;
/** Manage the singleton */
BasicApplicationModel::Pointer
BasicApplicationModel::GetInstance()
{
  if (!Instance)
  {
    Instance = BasicApplicationModel::New();
  }
  return Instance;
}

void BasicApplicationModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

BasicApplicationModel::BasicApplicationModel() : m_VisualizationModel(), m_Reader(), m_MeanShift()
{
  m_VisualizationModel    = VisualizationModelType::New();
  m_Reader = VectorReaderType::New();
  m_MeanShift = MSFilterType::New();

}

BasicApplicationModel
::~BasicApplicationModel() {}


void
BasicApplicationModel
::OpenImage( const char * filename)
{
  m_Reader->SetFileName(filename);
  m_Reader->UpdateOutputInformation();
  m_MeanShift->SetInput(m_Reader->GetOutput());

  // Generate the layer
  LayerGeneratorType::Pointer generator = LayerGeneratorType::New();
  //generator->SetImage(m_MeanShift->GetOutput());
  generator->SetImage(m_Reader->GetOutput());
  generator->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(generator->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  generator->GenerateLayer();

  generator->GetLayer()->SetName("Image");

  // Clear previous layers
  m_VisualizationModel->ClearLayers();

  // Add the layer to the models
  m_VisualizationModel->AddLayer(generator->GetLayer());

  m_VisualizationModel->Update();

  this->NotifyAll();
}

void
BasicApplicationModel
::RunLoop()
{
  /*
  unsigned long i = 0;
  while (true)
  {
    std::cerr << i << std::endl;
    ++i;
    sleep(1);
  }
  */
  
  VectorWriterType::Pointer writer = VectorWriterType::New();
  FltkFilterWatcher qlwatcher(writer,0,0,200,20,"Save Image ...");
  writer->SetFileName("msimage.tif");
  writer->SetInput(m_MeanShift->GetClusteredOutput());
  writer->Update();
  
}


}
