#include "itkRGBPixel.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "otbCommandLineArgumentParser.h"
#include <map>

// Functor to compare RGB values
template <class TValue>
class RGBLexicographicCompare
{
public:
  bool operator()(const itk::RGBPixel<TValue>& l,const itk::RGBPixel<TValue>& r) const
  {
    for (unsigned int i=0; i < 3; ++i)
    {
      if (l[i] < r[i])
      {
        return true;
      }
      else if (l[i] > r[i])
      {
        return false;
      }
    }
    return false;
  }
};

int main(int argc, char * argv[])
{
  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();

  parser->SetProgramDescription("Relabel a colored labeled image into a contiguous integer labeled image");
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
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  std::string infname = parseResult->GetInputImage();
  std::string outfname = parseResult->GetOutputImage();

  typedef unsigned short LabeledPixelType;
  typedef unsigned char  InputValueType;
  typedef itk::RGBPixel<InputValueType> RGBPixelType;
  typedef RGBLexicographicCompare<InputValueType> RGBCompare;

  typedef otb::Image<RGBPixelType,2>                                  InputImageType;
  typedef otb::Image<LabeledPixelType,2>                              OutputImageType;
  typedef otb::ImageFileReader<InputImageType>                        ReaderType;
  typedef otb::ImageFileWriter<OutputImageType>                       WriterType;
  typedef itk::ImageRegionConstIterator<InputImageType>               InputIteratorType;
  typedef itk::ImageRegionIterator<OutputImageType>                   OutputIteratorType;
  typedef std::map<RGBPixelType,LabeledPixelType,RGBCompare>          ColorMapType;

  // Objects instantiation
  ReaderType::Pointer            reader = ReaderType::New();
  WriterType::Pointer            writer = WriterType::New();

  // Reading the input image (must be small enough to resid in memory)
  reader->SetFileName(infname);
  reader->Update();

  // Walk the input image
  InputIteratorType inIt(reader->GetOutput(),reader->GetOutput()->GetLargestPossibleRegion());
  inIt.GoToBegin();

  // Instantiating the change map
  ColorMapType colorMap;

  /*******************************************/
  /*           Building color map            */
  /*******************************************/
  std::cout<<std::endl;
  std::cout<<"-- BUILDING THE COLOR MAP --"<<std::endl;
  std::cout<<std::endl;
  // Black should remain black
  RGBPixelType black;
  black.Fill(0);
  LabeledPixelType currentLabel;
  currentLabel = 0;
  colorMap[black]=currentLabel;
  ++currentLabel;

  while (!inIt.IsAtEnd())
  {
    // If the current color is not already in the change map
    if (colorMap.find(inIt.Get())==colorMap.end())
    {
      colorMap[inIt.Get()]=currentLabel;
      std::cout<<"Color ["<<(int)inIt.Get()[0]<<"\t"<<(int)inIt.Get()[1]<<"\t"<<(int)inIt.Get()[2]<<"]\t will be replaced by label "<<currentLabel<<std::endl;
      ++currentLabel;
    }
    ++inIt;
  }
  std::cout<<std::endl;
  std::cout<<"Color map has been built."<<std::endl;
  std::cout<<std::endl;

  /*******************************************/
  /*           Generating the new map        */
  /*******************************************/

  std::cout<<"-- GENERATING THE UNSIGNED SHORT MAP --"<<std::endl;
  std::cout<<std::endl;
  OutputImageType::Pointer output = OutputImageType::New();
  output->SetRegions(reader->GetOutput()->GetLargestPossibleRegion());
  output->Allocate();
  output->FillBuffer(0);

  OutputIteratorType outIt(output,output->GetLargestPossibleRegion());
  outIt.GoToBegin();
  inIt.GoToBegin();

  // Changin labels
  while (!outIt.IsAtEnd()&&!inIt.IsAtEnd())
  {
    outIt.Set(colorMap[inIt.Get()]);
    ++outIt;
    ++inIt;
  }

  // Writing the image
  writer->SetInput(output);
  writer->SetFileName(outfname);
  writer->Update();

  std::cout<<"New label image has been written to disk."<<std::endl;
  std::cout<<std::endl;

  return EXIT_SUCCESS;
}
