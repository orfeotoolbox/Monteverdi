#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbStreamingImageFileWriter.h"
#include "otbSVMSampleListModelEstimator.h"
#include "otbSVMImageClassificationFilter.h"
#include "itkImageRegionSplitter.h"
#include "otbStreamingTraits.h"
#include "itkImageRegionConstIterator.h"
#include "itkVariableSizeMatrix.h"
#include "otbCommandLineArgumentParser.h"
#include "otbSVMClassifier.h"
#include "itkListSample.h"

int main(int argc, char * argv[])
{

  // Parse command line parameters
  typedef otb::CommandLineArgumentParser ParserType;
  ParserType::Pointer parser = ParserType::New();

  parser->SetProgramDescription("Supervised SVM image classification with random training and validation set");
  parser->AddInputImage();
  parser->AddOutputImage();
  parser->AddOption("--TrainingMap","Labeled training map","-tm",1,true);
  parser->AddOption("--ValidityMask","Validity mask","-vm",1,true);
  parser->AddOption("--MaxTrainingSetSize","Size of the training set","-ts",1,true);
  parser->AddOption("--TrainingSetProbability","Probabilty for a sample to be selected in the training set","-tp",1,true);
  parser->AddOption("--MaxValidationSetSize","Size of the validation set","-vs",1,true);
  parser->AddOption("--ValidationSetProbability","Probabilty for a sample not selected in the training set to be selected in the validation set","-vp",1,true);
  parser->AddOption("--StreamingNumberOfLines","Number of lined for each streaming block","-sl",1,true);

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

  // initiating random number generation
  srand(time(NULL));

  std::string infname = parseResult->GetInputImage();
  std::string labelfname = parseResult->GetParameterString("--TrainingMap",0);
  std::string maskfname = parseResult->GetParameterString("--ValidityMask",0);
  std::string outfname = parseResult->GetOutputImage();
  const unsigned int nbsamples = parseResult->GetParameterUInt("--MaxTrainingSetSize");
  const double trainingProb =parseResult->GetParameterDouble("--TrainingSetProbability");
  const unsigned int nbvalsamples =parseResult->GetParameterUInt("--MaxValidationSetSize");
  const double valProb = parseResult->GetParameterDouble("--ValidationSetProbability");
  const unsigned int nbLinesForStreaming = parseResult->GetParameterUInt("--StreamingNumberOfLines");

  typedef unsigned short PixelType;
  typedef unsigned short LabeledPixelType;

  typedef otb::VectorImage<PixelType,2> ImageType;
  typedef otb::Image<LabeledPixelType,2> LabeledImageType;
  typedef otb::ImageFileReader<ImageType> ImageReaderType;
  typedef otb::ImageFileReader<LabeledImageType> LabeledImageReaderType;
  typedef otb::StreamingImageFileWriter<LabeledImageType> WriterType;

  typedef otb::StreamingTraits<ImageType> StreamingTraitsType;
  typedef itk::ImageRegionSplitter<2>  SplitterType;
  typedef ImageType::RegionType RegionType;

  typedef itk::ImageRegionConstIterator<ImageType> IteratorType;
  typedef itk::ImageRegionConstIterator<LabeledImageType> LabeledIteratorType;

  typedef otb::SVMImageClassificationFilter<ImageType,LabeledImageType>           ClassificationFilterType;
  typedef ClassificationFilterType::ModelType                                     ModelType;
  typedef itk::VariableLengthVector<PixelType>                                    SampleType;
  typedef itk::Statistics::ListSample<SampleType>                                 ListSampleType;
  typedef itk::FixedArray<LabeledPixelType,1>                                     TrainingSampleType;
  typedef itk::Statistics::ListSample<TrainingSampleType>                         TrainingListSampleType;
  typedef otb::SVMSampleListModelEstimator<ListSampleType,TrainingListSampleType> EstimatorType;
  typedef otb::SVMClassifier<ListSampleType,LabeledPixelType>                     ClassifierType;

  typedef std::map<LabeledPixelType,unsigned int>                                 ClassesMapType;

  typedef itk::VariableSizeMatrix<double>                                         ConfusionMatrixType;

  ImageReaderType::Pointer reader = ImageReaderType::New();
  LabeledImageReaderType::Pointer labeledReader = LabeledImageReaderType::New();
  LabeledImageReaderType::Pointer maskReader = LabeledImageReaderType::New();

  reader->SetFileName(infname);
  labeledReader->SetFileName(labelfname);
  maskReader->SetFileName(maskfname);

  /*******************************************/
  /*           Sampling data                 */
  /*******************************************/

  std::cout<<"-- SAMPLING DATA --"<<std::endl;
  std::cout<<std::endl;

  // Update input images information
  reader->GenerateOutputInformation();
  labeledReader->GenerateOutputInformation();
  maskReader->GenerateOutputInformation();

  if (reader->GetOutput()->GetLargestPossibleRegion()
      != labeledReader->GetOutput()->GetLargestPossibleRegion()
      ||reader->GetOutput()->GetLargestPossibleRegion()
      != maskReader->GetOutput()->GetLargestPossibleRegion()
     )
  {
    std::cerr<<"Label image, mask image and input image have different sizes."<<std::endl;
    return EXIT_FAILURE;
  }

  RegionType largestRegion = reader->GetOutput()->GetLargestPossibleRegion();

  // Setting up local streaming capabilities
  SplitterType::Pointer splitter = SplitterType::New();
  unsigned int numberOfStreamDivisions = StreamingTraitsType::CalculateNumberOfStreamDivisions(reader->GetOutput(),
                                         largestRegion,
                                         splitter,
                                         otb::SET_BUFFER_NUMBER_OF_LINES,
                                         0,0,nbLinesForStreaming);

  std::cout<<"The images will be streamed into "<<numberOfStreamDivisions<<" parts."<<std::endl;

  // Training sample lists
  ListSampleType::Pointer sampleList = ListSampleType::New();
  TrainingListSampleType::Pointer trainingSampleList = TrainingListSampleType::New();
  ListSampleType::Pointer valSampleList = ListSampleType::New();
  TrainingListSampleType::Pointer validationLabelSampleList = TrainingListSampleType::New();

  // Sample dimension
  unsigned int sampleSize = reader->GetOutput()->GetNumberOfComponentsPerPixel();

  std::cout<<"The following sample size will be used: "<<sampleSize<<std::endl;
  std::cout<<std::endl;
  // local streaming variables
  unsigned int piece = 0;
  RegionType streamingRegion;

  // Information on the different classes

  ClassesMapType classesMap;
  ClassesMapType valClassesMap;
  ClassesMapType totalMap;
  ClassesMapType indexMap;

  unsigned short currentIndex = 0;

  // For each streamed part
  for (piece = 0;
       piece < numberOfStreamDivisions;
       piece++)
  {
    streamingRegion = splitter->GetSplit(piece,numberOfStreamDivisions,largestRegion);

    std::cout<<"Processing region: "<<streamingRegion<<std::endl;

    reader->GetOutput()->SetRequestedRegion(streamingRegion);
    reader->GetOutput()->PropagateRequestedRegion();
    reader->GetOutput()->UpdateOutputData();

    labeledReader->GetOutput()->SetRequestedRegion(streamingRegion);
    labeledReader->GetOutput()->PropagateRequestedRegion();
    labeledReader->GetOutput()->UpdateOutputData();

    maskReader->GetOutput()->SetRequestedRegion(streamingRegion);
    maskReader->GetOutput()->PropagateRequestedRegion();
    maskReader->GetOutput()->UpdateOutputData();

    IteratorType it(reader->GetOutput(),streamingRegion);
    LabeledIteratorType labeledIt(labeledReader->GetOutput(),streamingRegion);
    LabeledIteratorType maskIt(maskReader->GetOutput(),streamingRegion);

    it.GoToBegin();
    labeledIt.GoToBegin();
    maskIt.GoToBegin();

    unsigned int totalSamples = 0;
    unsigned int totalValidationSamples = 0;

    // Loop on the image
    while (!it.IsAtEnd()&&!labeledIt.IsAtEnd()&&!maskIt.IsAtEnd())
    {
      // If the current pixel is labeled
      if (maskIt.Get()>0 && labeledIt.Get() >0)
      {
        // Updating the map
        if (classesMap.find(labeledIt.Get())==classesMap.end())
        {
          indexMap[labeledIt.Get()]=currentIndex;
          ++currentIndex;
          classesMap[labeledIt.Get()]=0;
          totalMap[labeledIt.Get()]=0;
          valClassesMap[labeledIt.Get()]=0;
        }
        // If this class is not already populated
        if (classesMap[labeledIt.Get()]<nbsamples && (rand()<trainingProb*RAND_MAX))
        {
          SampleType newSample(sampleSize);
          TrainingSampleType newTrainingSample;

          // build the sample
          newSample.Fill(0);
          for (unsigned int i = 0;i<sampleSize;++i)
          {
            newSample[i]=it.Get()[i];
          }

          // build the training sample
          newTrainingSample[0]=labeledIt.Get();

          // Update the the sample lists
          sampleList->PushBack(newSample);
          trainingSampleList->PushBack(newTrainingSample);
          classesMap[labeledIt.Get()]+=1;
          ++totalSamples;
        }
        else if (valClassesMap[labeledIt.Get()]<nbvalsamples && (rand()<valProb*RAND_MAX))
        {
          SampleType newSample(sampleSize);
          TrainingSampleType newTrainingSample;

          // build the sample
          newSample.Fill(0);
          for (unsigned int i = 0;i<sampleSize;++i)
          {
            newSample[i]=it.Get()[i];
          }

          // build the training sample
          newTrainingSample[0]=labeledIt.Get();

          // Update the the sample lists
          valSampleList->PushBack(newSample);
          validationLabelSampleList->PushBack(newTrainingSample);
          ++totalValidationSamples;
          valClassesMap[labeledIt.Get()]+=1;
        }
        totalMap[labeledIt.Get()]+=1;
      }
      ++it;
      ++labeledIt;
      ++maskIt;
    }
    std::cout<<totalSamples<<" samples added to the training set."<<std::endl;
    std::cout<<totalValidationSamples<<" samples added to the validation set."<<std::endl;
    std::cout<<std::endl;
  }

  std::cout<<classesMap.size()<<" classes found."<<std::endl;
  std::cout<<std::endl;

  for (ClassesMapType::iterator cIt = classesMap.begin();cIt!=classesMap.end();++cIt)
  {
    std::cout<<"Class "<<cIt->first<<"\tcontains "<<cIt->second<<"\ttraining samples over "<<totalMap[cIt->first]<<"\tpossible samples."<<std::endl;
  }

  std::cout<<std::endl;
  std::cout<<"Data sampling completed."<<std::endl;
  std::cout<<std::endl;

  /*******************************************/
  /*           Learning                      */
  /*******************************************/

  std::cout<<"-- LEARNING --"<<std::endl;
  std::cout<<std::endl;

  EstimatorType::Pointer estimator = EstimatorType::New();

  estimator->SetInputSampleList(sampleList);
  estimator->SetTrainingSampleList(trainingSampleList);
  estimator->SetNumberOfClasses(classesMap.size());
  estimator->Update();
  std::cout<<std::endl;
  std::cout<<"Learning completed."<<std::endl;
  std::cout<<std::endl;

  /*******************************************/
  /*           Validation                    */
  /*******************************************/
  std::cout<<"-- VALIDATION --"<<std::endl;
  std::cout<<std::endl;

  ClassifierType::Pointer validationClassifier = ClassifierType::New();
  validationClassifier->SetSample(valSampleList);
  validationClassifier->SetNumberOfClasses(classesMap.size());
  validationClassifier->SetModel(estimator->GetModel());
  validationClassifier->Update();

  ClassifierType::OutputType::Pointer membershipSample = validationClassifier->GetOutput();
  ClassifierType::OutputType::ConstIterator sampleIter = membershipSample->Begin();
  ClassifierType::OutputType::ConstIterator sampleLast = membershipSample->End();

  TrainingListSampleType::ConstIterator sampleValIter = validationLabelSampleList->Begin();
  TrainingListSampleType::ConstIterator sampleValEnd  = validationLabelSampleList->End();

  ConfusionMatrixType confusion(classesMap.size(),classesMap.size());
  confusion.Fill(0);


  while (sampleIter != sampleLast
         && sampleValIter != sampleValEnd)
  {
    confusion(indexMap[sampleValIter.GetMeasurementVector()[0]],indexMap[sampleIter.GetClassLabel()])+=1/static_cast<double>(valClassesMap[sampleIter.GetClassLabel()]);
    ++sampleIter;
    ++sampleValIter;
  }

  std::cout<<"Confusion matrix: "<<std::endl;
  std::cout<<std::endl;
  std::cout<<std::fixed;
  std::cout<<std::setprecision(2);
  for (unsigned int i = 0;i<classesMap.size();++i)
  {
    for (unsigned int j = 0;j<classesMap.size();++j)
    {
      std::cout<<100*confusion(i,j)<<"%\t";
    }
    std::cout<<std::endl;
  }
  std::cout<<std::endl;


  std::cout<<"End of validation."<<std::endl;
  std::cout<<std::endl;
  /*******************************************/
  /*           Classification                */
  /*******************************************/

  std::cout<<"-- CLASSIFICATION --"<<std::endl;
  std::cout<<std::endl;
  ClassificationFilterType::Pointer classifier = ClassificationFilterType::New();
  classifier->SetInput(reader->GetOutput());
  classifier->SetInputMask(maskReader->GetOutput());
  classifier->SetModel(estimator->GetModel());

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outfname);
  writer->SetInput(classifier->GetOutput());
  writer->SetNumberOfStreamDivisions(numberOfStreamDivisions);
  writer->Update();

  std::cout<<"Classification completed."<<std::endl;
  std::cout<<std::endl;

  return EXIT_SUCCESS;
}
