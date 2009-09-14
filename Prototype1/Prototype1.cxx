#include <cstdlib>

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "itkBinaryThresholdImageFilter.h"

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





  return EXIT_SUCCESS;
}
