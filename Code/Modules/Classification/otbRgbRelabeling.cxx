#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkRGBPixel.h"
#include "itkScalarToRGBPixelFunctor.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "otbCommandLineArgumentParser.h"

int main(int argc, char * argv[])
{
  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();

  parser->SetProgramDescription("Relabel a contiguous integer labeled image into a colored labeled image");
  parser->AddInputImage();
  parser->AddOutputImage();

  typedef otb::CommandLineArgumentParseResult ParserResultType;
  ParserResultType::Pointer  parseResult = ParserResultType::New();

  try
  {
    parser->ParseCommandLine(argc,argv,parseResult);
  }
  catch ( itk::ExceptionObject & err )
  {
    std::string descriptionException = err.GetDescription();
    if (descriptionException.find("ParseCommandLine(): Help Parser") != std::string::npos)
    {
      return EXIT_SUCCESS;
    }
    if (descriptionException.find("ParseCommandLine(): Version Parser") != std::string::npos)
    {
      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
  }

  std::string infname = parseResult->GetInputImage();
  std::string outfname = parseResult->GetOutputImage();

  typedef unsigned short LabeledPixelType;
  typedef itk::RGBPixel<unsigned char> ColorPixelType;

  typedef otb::Image<LabeledPixelType,2> LabeledImageType;
  typedef otb::Image<ColorPixelType,2>   ColorImageType;

  typedef otb::ImageFileReader<LabeledImageType> ReaderType;
  typedef otb::ImageFileWriter<ColorImageType> WriterType;
  typedef itk::RescaleIntensityImageFilter<LabeledImageType,LabeledImageType> RescaleFilterType;
  typedef itk::Functor::ScalarToRGBPixelFunctor<LabeledPixelType> FunctorType;
  typedef itk::UnaryFunctorImageFilter<LabeledImageType,ColorImageType,FunctorType> ColorLabelFilterType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(infname);

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();
  rescaler->SetInput(reader->GetOutput());
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);

  ColorLabelFilterType::Pointer colorFilter = ColorLabelFilterType::New();
  colorFilter->SetInput(rescaler->GetOutput());

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(colorFilter->GetOutput());
  writer->SetFileName(outfname);
  writer->Update();

  return EXIT_SUCCESS;
}
