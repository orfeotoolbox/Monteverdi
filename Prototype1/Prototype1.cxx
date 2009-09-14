#include <cstdlib>

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "itkBinaryThresholdImageFilter.h"

#include "otbModule.h"

#include "otbImageFileWriter.h"

//Execution
// ./Prototype1 ~/OTB/trunk/OTB-Data/Examples/qb_RoadExtract.tif


int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing Parameters " << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << "image" << std::endl;
    return EXIT_FAILURE;
  }

  typedef otb::Image<double, 2> ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> ThresholdType;
  typedef itk::ProcessObject ProcessType;

  //First idea, checking that ProcessObject manipulation works

  //Either declare the filter with its type first
  //then manipulate a a ProcessObject
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
  ProcessType::Pointer process1 = reader.GetPointer();

  //Or directly declare it as a ProcessObject
  ProcessType::Pointer process2 = (ThresholdType::New()).GetPointer();

  //Syntax at the ProcessObject level is a bit convoluted
  process2->GetInputs()[0] = process1->GetOutputs()[0];
  process2->Update();


  //Now, we try to get a more generic interface, the problem being passing the
  //parameters uniformly

  typedef otb::ModuleBase ModuleBase;
  ModuleBase::Pointer moduleReader = (otb::ModuleReader::New()).GetPointer();
  otb::Parameter<std::string>* filename = new otb::Parameter<std::string>(argv[1]);
  moduleReader->SetParameters("FileName", filename);

  /// Describe first module IO
  ModuleBase::InputDataDescriptorMapType inmap1 =  moduleReader->GetInputDataDescriptorsMap();
  ModuleBase::OutputDataDescriptorMapType outmap1 =  moduleReader->GetOutputDataDescriptorsMap();

  std::cout<<std::endl;
  std::cout<<"Module 1 (I/O): "<<std::endl;
  std::cout<<std::endl;
  
  for(ModuleBase::InputDataDescriptorMapType::const_iterator inIt1 = inmap1.begin(); inIt1 != inmap1.end();++inIt1)
    {
    std::cout<<"Name: "<<inIt1->second.m_DataName<<", type: "<<inIt1->second.m_DataKey<<", description: "<<inIt1->second.m_DataDescription;
    if(inIt1->second.m_Optional)
      std::cout<<" (optional)";
    if(inIt1->second.m_Multiple)
      std::cout<<" (multiple)";
    std::cout<<std::endl;
    }

  for(ModuleBase::OutputDataDescriptorMapType::const_iterator outIt1 = outmap1.begin(); outIt1 != outmap1.end();++outIt1)
    {
    std::cout<<"Name: "<<outIt1->second.m_DataName<<", type: "<<outIt1->second.m_DataKey<<", description: "<<outIt1->second.m_DataDescription<<", cardinal: "<<outIt1->second.m_NumberOfData<<std::endl;
    }


  ModuleBase::Pointer moduleThreshold = (otb::ModuleThreshold::New()).GetPointer();
  otb::Parameter<double>* thres = new otb::Parameter<double>(150.0);
  moduleThreshold->SetParameters("UpperThreshold", thres);

  /// Describe second module IO
  ModuleBase::InputDataDescriptorMapType inmap2 =  moduleThreshold->GetInputDataDescriptorsMap();
  ModuleBase::OutputDataDescriptorMapType outmap2 =  moduleThreshold->GetOutputDataDescriptorsMap();

  std::cout<<std::endl;
  std::cout<<"Module 2 (Threshold): "<<std::endl;
  std::cout<<std::endl;

  for(ModuleBase::InputDataDescriptorMapType::const_iterator inIt2 = inmap2.begin(); inIt2 != inmap2.end();++inIt2)
    {
    std::cout<<"Name: "<<inIt2->second.m_DataName<<", type: "<<inIt2->second.m_DataKey<<", description: "<<inIt2->second.m_DataDescription;
    if(inIt2->second.m_Optional)
      std::cout<<" (optional)";
    if(inIt2->second.m_Multiple)
      std::cout<<" (multiple)";
    std::cout<<std::endl;
    }

  for(ModuleBase::OutputDataDescriptorMapType::const_iterator outIt2 = outmap2.begin(); outIt2 != outmap2.end();++outIt2)
    {
    std::cout<<"Name: "<<outIt2->second.m_DataName<<", type: "<<outIt2->second.m_DataKey<<", description: "<<outIt2->second.m_DataDescription<<", cardinal: "<<outIt2->second.m_NumberOfData<<std::endl;
    }

  //Convenience accessor can be defined at the module level
  //to make the syntax better.
  moduleThreshold->AddInputData("InputImage",moduleReader->GetOutputData("OutputImage",0));

  // Update the module
  moduleThreshold->Update();

  // the moduleTreshold should not call filter update in its
  // implementation. This is left to writing or visualisation module.

  typedef otb::ImageFileWriter<ImageType> WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName("output.tif");
  ProcessType::Pointer process3 = writer.GetPointer();
  //this does not work because we are out of the particular case of the
  //BinaryThresholdImageFilter: no SetNthInput is performed in the constructor
  //ie, the space for m_Inputs[0] does not exist yet...
  process3->GetInputs()[0]  = moduleThreshold->GetOutputData("OutputImage",0);
  process3->Update();

  return EXIT_SUCCESS;
}

