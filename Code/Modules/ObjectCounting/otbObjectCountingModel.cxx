/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#include "otbObjectCountingModel.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>
#include "itkExceptionObject.h"
#include "otbMacro.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "otbMath.h"
#include "otbImageToEdgePathFilter.h"
#include "itkPolyLineParametricPath.h"
#include "otbImageFileWriter.h"
#include "otbStreamingImageFileWriter.h"
#include "otbVectorData.h"
#include "otbVectorDataFileWriter.h"
#include "otbVectorDataProjectionFilter.h"
namespace otb
{
/** Initialize the singleton */
ObjectCountingModel::Pointer ObjectCountingModel::Instance = NULL;
/** Manage the singleton */
ObjectCountingModel::Pointer
ObjectCountingModel::GetInstance()
{
  if (!Instance)
    {
    Instance = ObjectCountingModel::New();
    }
  return Instance;
}

void ObjectCountingModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

ObjectCountingModel::ObjectCountingModel()
{
  this->InitInputFilters();

  m_Histogram.clear();
  m_QuicklookSize.Fill(256);
  m_State = NO_IMAGE;
  m_UseSVM                = true;
  m_UseSmoothing          = false;
  m_RefPixelSmoothedImage = false;
  m_ClampThreshold        = 0.01;
  m_InputPolygonListIndex = 0;
  m_ErasedPolygonIndex    = -1;
  m_ThresholdValue        = 0.95;
  m_SpatialRadius         = 5;
  m_RangeRadius           = 80;
  m_Scale                 = 1;
  m_MinRegionSize         = 10;
  m_NuParameter           = 0.01;

  m_Kernel = new KernelType();
  m_Kernel->Update();

  this->InitFilters();
}

ObjectCountingModel
::~ObjectCountingModel() {}

ObjectCountingModel::PixelType
ObjectCountingModel
::GetMin(unsigned int i) const
{

  PixelType min = m_Histogram[i]->Quantile(0, m_ClampThreshold);
  return min;
}

ObjectCountingModel::PixelType
ObjectCountingModel
::GetMax(unsigned int i) const
{
  PixelType max;
  max = m_Histogram[i]->Quantile(0, 1 - m_ClampThreshold);
  return max;
}

void
ObjectCountingModel
::InitFilters()
{
  m_SpectralAngleFilter   = SpectralAngleType::New();
  m_Rescaler              = RescaleFilterType::New();
  m_Classifier            = ClassifierType::New();
  m_Thresholder           = ThresholdFilterType::New();
  m_MeanShiftFilter       = MeanShiftFilterType::New();
  m_ClassifOutputImage    = LabeledImageType::New();
  m_ChangeLabelFilter     = ChangeLabelImageFilterType::New();
  m_ExtractOutputImage    = LabeledImageType::New();
  m_ConnectedFilter       = ConnectedFilterType::New();
  m_RelabelFilter         = RelabelFilterType::New();
  m_ClassifBoundaryFilter = ClassifBoundaryFilterType::New();
  m_PersistentVectorizationFilter = PersistentVectorizationFilterType::New();
  m_OutputPolyList        = m_PersistentVectorizationFilter->GetPathList();

  m_Rescaler->SetOutputMinimum(1.);
  m_Rescaler->SetOutputMaximum(0.);
  m_Rescaler->SetWindowMinimum(0);
  m_Rescaler->SetWindowMaximum(CONST_PI);

  m_Rescaler->SetInput(m_SpectralAngleFilter->GetOutput());
  //std::cout << "rescaling ok" << std::endl;
  m_Thresholder->SetInsideValue(1);
  m_Thresholder->SetOutsideValue(0);
  m_Thresholder->SetLowerThreshold(m_ThresholdValue);
  m_Thresholder->SetUpperThreshold(1);
  m_Thresholder->SetInput(m_Rescaler->GetOutput());

  //std::cout << "thresholding ok" << std::endl;
  m_ChangeLabelFilter->SetChange(-1, 0);
  m_ChangeLabelFilter->SetInput(m_Classifier->GetOutput());

  m_RelabelFilter->SetInput(m_ConnectedFilter->GetOutput());
  m_PersistentVectorizationFilter->SetInput(m_RelabelFilter->GetOutput());
  //std::cout << "init filter ok" << std::endl;
}

void
ObjectCountingModel
::InitInputFilters()
{
  m_InputPolyList   = PolygonListType::New();
  m_InputSampleList = SampleListType::New();
  m_ExtractedImage  = ImageType::New();
  m_Shrinker        = ShrinkFilterType::New();
  m_Quicklook       = m_Shrinker->GetOutput();
}

void
ObjectCountingModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}

void
ObjectCountingModel
::OpenImage()
{
  //std::cout << "Open image in the model" << std::endl;
  //this->InitFilters();
  //std::cout << "Init input filter" << std::endl;
  //this->InitInputFilters();

//   m_Reader->SetFileName( filename );
//std::cout << "Update output information" << std::endl;
  m_InputImage->UpdateOutputInformation();

  if (m_InputImage->GetNumberOfComponentsPerPixel() < 3) itkExceptionMacro(
      << "Invalid input Image, must have at least 3 channels (only " <<
      m_InputImage->GetNumberOfComponentsPerPixel() <<
      " in the selected image)");
//
//   m_InputImage = m_InputImage;

  //std::cout << "generate quicklook" << std::endl;
  this->GenerateQuicklook();
  //std::cout << "generate histogram" << std::endl;
  this->GenerateHistogram();

  m_InputPolygonListIndex = 0;
  m_InputPolyList->Clear();
  m_InputSampleList->Clear();
  //std::cout << "Open image in the model OVER" << std::endl;
}

void
ObjectCountingModel
::GenerateQuicklook()
{
  // Shrink factor
  SizeType largestRegionSize = m_InputImage->GetLargestPossibleRegion().GetSize();
  //std::cout << "size " << largestRegionSize << std::endl;
  unsigned int maxQuicklook = std::max(m_QuicklookSize[0], m_QuicklookSize[1]);
  unsigned int maxLargest   = std::max(largestRegionSize[0], largestRegionSize[1]);

  m_ShrinkFactor =  maxLargest / maxQuicklook;

  if (m_ShrinkFactor == 0)
    {
    m_ShrinkFactor = 1;
    }
  m_Shrinker->SetInput(m_InputImage);
  m_Shrinker->SetShrinkFactor(m_ShrinkFactor);
  m_Shrinker->Update();

  m_Quicklook = m_Shrinker->GetOutput();
}

void
ObjectCountingModel
::GenerateHistogram()
{
  m_Histogram.clear();
  for (unsigned int j = 0; j < m_Quicklook->GetNumberOfComponentsPerPixel(); j++)
    {
    ListSampleType::Pointer sl = ListSampleType::New();

    IteratorType it(m_Quicklook, m_Quicklook->GetLargestPossibleRegion());
    it.GoToBegin();
    //std::cout << "while iterator quicklook" << std::endl;
    while (!it.IsAtEnd())
      {
      PixelType pixel = it.Get()[j];
      sl->PushBack(pixel);
      ++it;
      }
    //std::cout << "histogramm generator" << std::endl;
    HistogramGeneratorType::Pointer generator = HistogramGeneratorType::New();
    generator->SetListSample(sl);
    HistogramGeneratorType::HistogramType::SizeType size;
    size.Fill(256);
    generator->SetNumberOfBins(size);
    //std::cout << "update generator" << std::endl;
    generator->Update();
    m_Histogram.push_back(const_cast<HistogramType *>(generator->GetOutput()));
    }
}

void
ObjectCountingModel
::RunImageExtraction()
{
  if (!this->ConstrainsCurrentRegion())
    {
    m_CurrentRegion.Crop(m_InputImage->GetLargestPossibleRegion());
    }

  ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();

  extract->SetInput(m_InputImage);
  extract->SetExtractionRegion(m_CurrentRegion);
  extract->Update();
  m_ExtractedImage = extract->GetOutput();

  m_InputPolyList->Clear();
  m_InputSampleList->Clear();
  m_InputPolygonListIndex = 0;
  m_ErasedPolygonIndex    = -1;
  //std::cout << "extract value " << EXTRACT << std::endl;
  ////std::cout << "extract value2 " << ImageToWorkType::EXTRACT << std::endl;
  m_WhichImage = EXTRACT;
  //std::cout << "has image value " << HAS_IMAGE << std::endl;
  m_State = HAS_IMAGE;
  //std::cout << "m_State " << m_State << std::endl;
  this->NotifyAll("SetInputImage");
  //std::cout << "RunImageExtraction over" << std::endl;
}

bool ObjectCountingModel
::ConstrainsCurrentRegion()
{
  RegionType largest = m_InputImage->GetLargestPossibleRegion();

  if (m_CurrentRegion.GetSize()[0] > largest.GetSize()[0]
      || m_CurrentRegion.GetSize()[1] > largest.GetSize()[1])
    {
    return false;
    }
  RegionType::SizeType  size = m_CurrentRegion.GetSize();
  RegionType::IndexType index = m_CurrentRegion.GetIndex();

  if (index[0] < largest.GetIndex()[0])
    {
    index[0] = largest.GetIndex()[0];
    }

  if (index[1] < largest.GetIndex()[1])
    {
    index[1] = largest.GetIndex()[1];
    }

  if (index[0] + size[0] > largest.GetIndex()[0] + largest.GetSize()[0])
    {
    index[0] = largest.GetIndex()[0] + largest.GetSize()[0] - size[0];
    }

  if (index[1] + size[1] > largest.GetIndex()[1] + largest.GetSize()[1])
    {
    index[1] = largest.GetIndex()[1] + largest.GetSize()[1] - size[1];
    }

  m_CurrentRegion.SetIndex(index);
  m_CurrentRegion.SetSize(size);

  return true;
}

void
ObjectCountingModel
::RunChain(const char * cfname)
{
  m_OutputImageName = cfname;
  m_WhichImage = FULL_IMAGE;
  this->ComputeMeanShift();

  if (m_UseSVM == true)
    {
    this->Classification();
    }
  else
    {
    this->ComputeSpectralAngle();
    }

  this->FuseData();

  this->PrepareOutput();
}

void
ObjectCountingModel
::RunOverExtract()
{
  this->CheckInputPolygonsValidity();

  if (m_WhichImage == FULL_IMAGE) this->InitFilters();

  m_WhichImage = EXTRACT;
  this->ComputeMeanShift();
  if (m_UseSVM == true)
    {
    this->Learning();
    this->Classification();
    }
  else
    {
    if (m_UseSmoothing == true)
      {
      m_RefPixelSmoothedImage = true;
      this->ComputeReferencePixel();
      m_RefPixelSmoothedImage = false;
      }
    this->ComputeSpectralAngle();
    }

  this->FuseData();

  this->PrepareOutput();

  this->GetNumberOfObjects();
}

void
ObjectCountingModel
::SavePolygon(const char * cfname)
{
  std::string outName = cfname;
  int         length = outName.size() - (outName.find_last_of('.') + 1);
  //std::cout << "lenght of the file " <<   outName.size() << " Name: " << outName << " et length " << length << std::endl;
  std::string ext = outName.substr(outName.find_last_of('.') + 1, length);

  if (ext.compare("shp") == 0 && ext.compare("kml") == 0)
    {
    itkExceptionMacro(<< ext << ": Not recognized file vector format. Must be shp or kml");
    }

  typedef otb::VectorData<double, 2>                VectorDataType;
  typedef VectorDataType::DataNodeType              DataNodeType;
  typedef DataNodeType::PolygonType                 PolygonType;
  typedef PolygonType::VertexType                   VertexType;
  typedef otb::VectorDataFileWriter<VectorDataType> VectorDataWriterType;

  VectorDataType::Pointer vectorData      = VectorDataType::New();

  DataNodeType::Pointer document = DataNodeType::New();
  DataNodeType::Pointer root     = vectorData->GetDataTree()->GetRoot()->Get();
  document->SetNodeType(otb::DOCUMENT);
  document->SetNodeId("DOCUMENT");
  vectorData->GetDataTree()->Add(document, root);

  for (unsigned int i = 0; i < m_OutputPolyList->Size(); i++)
    {
    PolygonType::Pointer p = PolygonType::New();

    DataNodeType::Pointer poly   = DataNodeType::New();
    DataNodeType::Pointer folder = DataNodeType::New();
    poly->SetNodeType(otb::FEATURE_POLYGON);
    folder->SetNodeType(otb::FOLDER);
    itk::OStringStream oss;
    oss.str("");
    oss << i + 1;
    poly->SetNodeId(oss.str().c_str());
    poly->SetPolygonExteriorRing(m_OutputPolyList->GetNthElement(i));
    vectorData->GetDataTree()->Add(folder, document);
    vectorData->GetDataTree()->Add(poly, folder);
    }

  typedef otb::VectorDataProjectionFilter<VectorDataType, VectorDataType> ProjectionFilterType;
  ProjectionFilterType::Pointer vectorDataProjection = ProjectionFilterType::New();
  vectorDataProjection->SetInput(vectorData);
  vectorDataProjection->SetInputOrigin(m_InputImage->GetOrigin());
  vectorDataProjection->SetInputSpacing(m_InputImage->GetSpacing());

  std::string projectionRef;
  itk::ExposeMetaData<std::string>(m_InputImage->GetMetaDataDictionary(),
                                   MetaDataKey::ProjectionRefKey, projectionRef);
  vectorDataProjection->SetInputProjectionRef(projectionRef);

  vectorDataProjection->SetInputKeywordList(m_InputImage->GetImageKeywordlist());

  VectorDataWriterType::Pointer writer = VectorDataWriterType::New();
  writer->SetInput(vectorDataProjection->GetOutput());
  writer->SetFileName(outName);
  writer->Update();

}

void
ObjectCountingModel
::ErasePolygon(const IndexType& index)
{
  unsigned int i = 0;
  bool         done = false;
  while (i < m_InputPolyList->Size() && done == false)
    {
    if (m_InputPolyList->GetNthElement(i)->IsInside(index))
      {
      m_InputPolyList->Erase(i);
      m_ErasedPolygonIndex = i;
      m_InputPolygonListIndex--;
      done = true;
      }
    i++;
    }
  if (done == true)
    {
    m_State = EXAMPLES_SELECTED;

    this->NotifyAll("");
    m_ErasedPolygonIndex = -1;
    }
  else itkExceptionMacro(<< "No polygon found.");
}

void
ObjectCountingModel
::AddPointToCurrentPolygon(const IndexType& index)
{
  PolygonIndexType vertex;
  vertex[0] = index[0];
  vertex[1] = index[1];
  m_InputPolyList->GetNthElement(m_InputPolygonListIndex - 1)->AddVertex(vertex);
  m_State = EXAMPLES_SELECTED;
  this->NotifyAll("");
}

void
ObjectCountingModel
::NewPolygon(const IndexType& index)
{
  PolygonPointerType poly = PolygonType::New();
  PolygonIndexType   vertex;
  vertex[0] = index[0];
  vertex[1] = index[1];
  poly->AddVertex(vertex);
  m_InputPolyList->PushBack(poly);
  m_State = EXAMPLES_SELECTED;
  this->NotifyAll("");
  m_InputPolygonListIndex++;
}

void
ObjectCountingModel
::CheckInputPolygonsValidity()
{
  if (m_InputPolyList->Size() == 0)
    {
    itkExceptionMacro("No object of reference selected.");
    }
  if (m_InputPolyList->Size() == 1 && m_InputPolyList->GetNthElement(0)->GetVertexList()->Size() < 3)
    {
    itkExceptionMacro("You have to select a polygon with more than 2 points...");
    }
  for (unsigned int i = 1; i < m_InputPolyList->Size(); i++)
    {
    if (m_InputPolyList->GetNthElement(i)->GetVertexList()->Size() < 3)
      {
      m_InputPolyList->Erase(i);
      m_InputPolygonListIndex--;
      m_ErasedPolygonIndex = i;
      m_State = EXAMPLES_SELECTED;
      this->NotifyAll("");
      m_ErasedPolygonIndex = -1;
      }
    }

}

void
ObjectCountingModel
::GenerateInputExampleSampleList()
{
  RegionType inputRegion = m_ExtractedImage->GetLargestPossibleRegion();
  for (unsigned int i = 0; i < m_InputPolyList->Size(); i++)
    {

    RegionType region = m_InputPolyList->GetNthElement(i)->GetBoundingRegion().GetImageRegion();
    // Check if every the upper left corner and the downer right corner of the region are contained into the extract
    if ((region.GetIndex()[0] >= inputRegion.GetIndex()[0]) &&
        (region.GetIndex()[1] >= inputRegion.GetIndex()[1]) &&
        (region.GetIndex()[0] + region.GetSize()[0] <= inputRegion.GetIndex()[0] + inputRegion.GetSize()[0]) &&
        (region.GetIndex()[1] + region.GetSize()[1] <= inputRegion.GetIndex()[1] + inputRegion.GetSize()[1])
        )
      {
      itk::ImageRegionIteratorWithIndex<ImageType> it(m_ExtractedImage, region);
      it.GoToBegin();

      while (!it.IsAtEnd())
        {
        if (m_InputPolyList->GetNthElement(i)->IsInside(it.GetIndex()))
          {
          SampleType sample(it.Get().Size());
          for (unsigned int k = 0; k < it.Get().Size(); k++)
            {
            sample[k] = it.Get()[k];
            }
          m_InputSampleList->PushBack(sample);
          }
        ++it;
        }
      }
    else
      {
      itkExceptionMacro(<< "The considered polygon is not contained into the extract image");
      }
    }
}

void
ObjectCountingModel
::Classification()
{
  m_Classifier->SetModel(m_Model);

  if (m_UseSmoothing == true) m_Classifier->SetInput(m_MeanShiftFilter->GetOutput());
  else if (m_WhichImage == EXTRACT) m_Classifier->SetInput(m_ExtractedImage);
  else m_Classifier->SetInput(m_InputImage);

  m_ClassifOutputImage = m_ChangeLabelFilter->GetOutput();

}

void
ObjectCountingModel
::Learning()
{
// Extract image to sample list:
  EstimatorType::Pointer          estimator = EstimatorType::New();
  SampleListType::Pointer         extractSampleList = SampleListType::New();
  TrainingListSampleType::Pointer trainingSampleList = TrainingListSampleType::New();
  TrainingSampleType              trainSample;
  trainSample[0] = 1;
  itk::ImageRegionIteratorWithIndex<ImageType> it;
  if (m_UseSmoothing == true)
    {
    m_MeanShiftFilter->Update();
    it = itk::ImageRegionIteratorWithIndex<ImageType>(m_MeanShiftFilter->GetOutput(),
                                                      m_MeanShiftFilter->GetOutput()->GetLargestPossibleRegion());
    }
  else it = itk::ImageRegionIteratorWithIndex<ImageType>(m_ExtractedImage, m_ExtractedImage->GetLargestPossibleRegion());

  it.GoToBegin();
  while (!it.IsAtEnd())
    {
    for (unsigned int i = 0; i < m_InputPolyList->Size(); i++)
      {
      if (m_InputPolyList->GetNthElement(i)->IsInside(it.GetIndex()))
        {
        SampleType sample(it.Get().Size());
        sample.Fill(0);

        for (unsigned int k = 0; k < it.Get().Size(); k++)
          {
          sample[k] = it.Get()[k];
          }
        extractSampleList->PushBack(sample);
        trainingSampleList->PushBack(trainSample);
        }
      }
    ++it;
    }

  estimator->SetInputSampleList(extractSampleList);
  estimator->SetTrainingSampleList(trainingSampleList);
  estimator->SetNumberOfClasses(2);
  estimator->SetSVMType(ONE_CLASS);
  estimator->SetNu(m_NuParameter);

  estimator->SetKernelFunctor(m_Kernel);
  estimator->SetKernelType(GENERIC);

  estimator->Update();

  m_Model = estimator->GetModel();
}

void
ObjectCountingModel
::ComputeReferencePixel()
{
  int                                          count = 0;
  itk::ImageRegionIteratorWithIndex<ImageType> itIm;
  if (m_RefPixelSmoothedImage == true)
    {
    m_MeanShiftFilter->Update();
    itIm = itk::ImageRegionIteratorWithIndex<ImageType>(m_MeanShiftFilter->GetOutput(),
                                                        m_MeanShiftFilter->GetOutput()->GetLargestPossibleRegion());
    }
  else itIm = itk::ImageRegionIteratorWithIndex<ImageType>(m_ExtractedImage, m_ExtractedImage->GetLargestPossibleRegion());

  unsigned int nbComp = m_ExtractedImage->GetNumberOfComponentsPerPixel();
  m_ReferencePixel.SetSize(nbComp);
  m_ReferencePixel.Fill(0);

  for (unsigned int i = 0; i < m_InputPolyList->Size(); i++)
    {
    itIm.GoToBegin();
    while (!itIm.IsAtEnd())
      {
      if (m_InputPolyList->GetNthElement(i)->IsInside(itIm.GetIndex()))
        {
        for (unsigned int j = 0; j < nbComp; j++)
          {
          m_ReferencePixel[j] += itIm.Get()[j];
          }
        count++;
        }
      ++itIm;
      }
    }

  for (unsigned int j = 0; j < nbComp; j++)
    {
    m_ReferencePixel[j] /= count;
    }

  m_State = HAS_REFERENCEPIXEL;
  this->NotifyAll("ReferencePixelComputed");
}

void
ObjectCountingModel
::ComputeSpectralAngle()
{
  m_Thresholder->SetLowerThreshold(m_ThresholdValue);
  m_SpectralAngleFilter->SetReferencePixel(m_ReferencePixel);

  if (m_UseSmoothing == true)
    {
    // Update reference pixel with cluster image values
    m_SpectralAngleFilter->SetInput(m_MeanShiftFilter->GetOutput());
    }
  if (m_WhichImage == FULL_IMAGE) m_SpectralAngleFilter->SetInput(m_InputImage);
  else m_SpectralAngleFilter->SetInput(m_ExtractedImage);

  m_ClassifOutputImage = m_Thresholder->GetOutput();

}

void
ObjectCountingModel
::ComputeMeanShift()
{
  if (m_WhichImage == FULL_IMAGE) m_MeanShiftFilter->SetInput(m_InputImage);
  else m_MeanShiftFilter->SetInput(m_ExtractedImage);

  m_MeanShiftFilter->SetSpatialRadius(m_SpatialRadius);
  m_MeanShiftFilter->SetRangeRadius(m_RangeRadius);
  m_MeanShiftFilter->SetScale(m_Scale);
  m_MeanShiftFilter->SetMinimumRegionSize(m_MinRegionSize);

  //m_MeanShiftFilter->Update();
}

void
ObjectCountingModel
::FuseData()
{
  m_ClassifBoundaryFilter->SetInput1(m_ClassifOutputImage);
  m_ClassifBoundaryFilter->SetInput2(m_MeanShiftFilter->GetClusterBoundariesOutput());

}

void
ObjectCountingModel
::PrepareOutput()
{
  m_PersistentVectorizationFilter->Reset();
  m_ConnectedFilter->SetInput(m_ClassifBoundaryFilter->GetOutput());
  m_RelabelFilter->SetMinimumObjectSize(m_MinRegionSize);

  if (m_WhichImage == EXTRACT) m_PersistentVectorizationFilter->Update();
  else
    {
    typedef StreamingImageFileWriter<LabeledImageType> WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(m_OutputImageName);
    writer->SetInput(m_PersistentVectorizationFilter->GetOutput());
    writer->Update();
    }

  m_State = POLYGONS_DETECTED;
  this->NotifyAll("OutputPrepared");
}

unsigned int
ObjectCountingModel
::GetNumberOfObjects()
{
  return m_RelabelFilter->GetNumberOfObjects();

  m_State = POLYGONS_DETECTED;
  this->NotifyAll("NumberObjectAvailable");
}

void
ObjectCountingModel
::SetInputImage(ImagePointerType image)
{
  m_InputImage = image;
}

void
ObjectCountingModel
::Quit()
{
  this->GetOutputLabeledImage();
  this->NotifyAll("OutputsUpdated");
  this->NotifyAll("BusyOff");
}

}
