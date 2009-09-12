#include <cstdlib>

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "itkBinaryThresholdImageFilter.h"

int main(int argc, char* argv[])
{

  typedef otb::Image<double, 2> ImageType;
  typedef otb::ImageFileReader<ImageType> ReaderType;
  typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> ThresholdType;
  typedef itk::ProcessObject ProcessType;

  ReaderType::Pointer reader = ReaderType::New();
  ProcessType::Pointer process1 = reader.GetPointer();

  ProcessType::Pointer process2 = (ThresholdType::New()).GetPointer();

  process2->GetInputs()[0] = process1->GetOutputs()[0];

  return EXIT_SUCCESS;
}
