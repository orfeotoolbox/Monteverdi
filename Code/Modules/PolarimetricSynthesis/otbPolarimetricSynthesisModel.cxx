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
#include "otbPolarimetricSynthesisModel.h"
#include "otbImageFileWriter.h"

namespace otb
{
/** Initialize the singleton */
PolarimetricSynthesisModel::Pointer PolarimetricSynthesisModel::Instance = NULL;
/** Manage the singleton */
PolarimetricSynthesisModel::Pointer
PolarimetricSynthesisModel::GetInstance()
{
  if (!Instance)
    {
    Instance = PolarimetricSynthesisModel::New();
    }
  return Instance;
}


void PolarimetricSynthesisModel::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}

PolarimetricSynthesisModel::PolarimetricSynthesisModel()
{
  // init the shrink filter pointer (not in the InitPointers() method to support progress bar
  m_ShrinkFilter = ShrinkFilterType::New();

  m_ImageHH = ComplexImageType::New();
  m_ImageHV = ComplexImageType::New();
  m_ImageVH = ComplexImageType::New();
  m_ImageVV = ComplexImageType::New();

  m_HEmissionMode = false;
  m_VEmissionMode = false;
  m_GrayPolarizationMode = ANY_POLAR;
  m_RedPolarizationMode = ANY_POLAR;
  m_GreenPolarizationMode = ANY_POLAR;
  m_BluePolarizationMode = ANY_POLAR;
  m_RGB = true;
  m_GrayPsiI = 0;
  m_GrayPsiR = 0;
  m_GrayKhiI = 0;
  m_GrayKhiR = 0;
  m_RedPsiI = 0;
  m_RedPsiR = 0;
  m_RedKhiI = 0;
  m_RedKhiR = 0;
  m_GreenPsiI = 0;
  m_GreenPsiR = 0;
  m_GreenKhiI = 0;
  m_GreenKhiR = 0;
  m_BluePsiI = 0;
  m_BluePsiR = 0;
  m_BlueKhiI = 0;
  m_BlueKhiR = 0;

  InitPointers();

  m_QuicklookSize.Fill(220);

  m_Min = 0;
  m_Max = itk::NumericTraits<double>::max();
  m_Valid = false;
}

PolarimetricSynthesisModel::~PolarimetricSynthesisModel()
{}

void PolarimetricSynthesisModel::InitPointers()
{

  // Filters initialization
  m_ImageList = ComplexImageListType::New();
  m_ScrollImageList = DoubleImageListType::New();
  m_FullImageList = DoubleImageListType::New();
  // Transfom the channels into a vectorImage
  m_ImageListToVectorImageFilter = ComplexImageListToVectorImageFilterType::New();

  // Turn three channels into a RGBvectorImage
  m_ImageListToVectorImageFilterForFull = DoubleImageListToVectorImageFilterType::New();
  m_ImageListToVectorImageFilterForScroll = DoubleImageListToVectorImageFilterType::New();

  m_GrayPolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();
  m_RedPolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();
  m_GreenPolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();
  m_BluePolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();

  m_GrayQuicklookPolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();
  m_RedQuicklookPolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();
  m_GreenQuicklookPolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();
  m_BlueQuicklookPolarimetricSynthesisFilter = PolarimetricSynthesisFilterType::New();

  m_HistogramGenerator = HistogramGeneratorType::New();

  m_Quicklook = ComplexVectorImageType::New();
}

void PolarimetricSynthesisModel::Save()
{
    this->NotifyAll("OutputsUpdated");
}

void PolarimetricSynthesisModel::LoadImages()
{

  //Init or re-init pointers
  InitPointers();

  SetupEmission(GRAYSCALE);
  SetupEmission(RED);
  SetupEmission(GREEN);
  SetupEmission(BLUE);

  m_Valid = true;

  if (m_HEmissionMode && m_VEmissionMode)
    {
    }
    else if (!m_HEmissionMode && m_VEmissionMode)
    {
      m_GrayPsiI = 90;
      m_GrayKhiI = 0;
      m_RedPsiI = 90;
      m_RedKhiI = 0;
      m_GreenPsiI = 90;
      m_GreenKhiI = 0;
      m_BluePsiI = 90;
      m_BlueKhiI = 0;
      m_Valid = true;
    }
    else if (m_HEmissionMode && !m_VEmissionMode)
    {
      m_GrayPsiI = 0;
      m_GrayKhiI = 0;
      m_RedPsiI = 0;
      m_RedKhiI = 0;
      m_GreenPsiI = 0;
      m_GreenKhiI = 0;
      m_BluePsiI = 0;
      m_BlueKhiI = 0;
    }
    else
    {
      m_Valid = false;
      itkExceptionMacro(<<"At least vertical or horizontal emission is required!");
    }
  this->WirePipeline();
  this->ComputeNormalizationFactors();
  this->GenerateOutputHistogram(GRAYSCALE);
  this->GenerateOutputHistogram(RED);
  this->GenerateOutputHistogram(GREEN);
  this->GenerateOutputHistogram(BLUE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::WirePipeline()
{
  m_ImageList->Clear();
  if (m_HEmissionMode)
  {
    m_ImageHH->UpdateOutputInformation();
    m_ImageHV->UpdateOutputInformation();
    m_ImageList->PushBack(m_ImageHH);
    m_ImageList->PushBack(m_ImageHV);
  }
  if (m_VEmissionMode)
  {
    m_ImageVH->UpdateOutputInformation();
    m_ImageVV->UpdateOutputInformation();
    m_ImageList->PushBack(m_ImageVH);
    m_ImageList->PushBack(m_ImageVV);
  }

  m_ImageListToVectorImageFilter->SetInput(m_ImageList);
  m_GrayPolarimetricSynthesisFilter->SetInput(m_ImageListToVectorImageFilter->GetOutput());
  m_GrayPolarimetricSynthesisFilter->GetOutput()->UpdateOutputInformation();
  m_RedPolarimetricSynthesisFilter->SetInput(m_ImageListToVectorImageFilter->GetOutput());
  m_RedPolarimetricSynthesisFilter->GetOutput()->UpdateOutputInformation();
  m_GreenPolarimetricSynthesisFilter->SetInput(m_ImageListToVectorImageFilter->GetOutput());
  m_GreenPolarimetricSynthesisFilter->GetOutput()->UpdateOutputInformation();
  m_BluePolarimetricSynthesisFilter->SetInput(m_ImageListToVectorImageFilter->GetOutput());
  m_BluePolarimetricSynthesisFilter->GetOutput()->UpdateOutputInformation();

  // Connexions for the Full Image
  m_FullImageList->Clear();
  m_FullImageList->PushBack(m_RedPolarimetricSynthesisFilter->GetOutput());
  m_FullImageList->PushBack(m_GreenPolarimetricSynthesisFilter->GetOutput());
  m_FullImageList->PushBack(m_BluePolarimetricSynthesisFilter->GetOutput());
  m_ImageListToVectorImageFilterForFull->SetInput(m_FullImageList);
  m_ImageListToVectorImageFilterForFull->UpdateOutputInformation();


  this->GenerateQuicklook();

  m_GrayQuicklookPolarimetricSynthesisFilter->SetInput(m_Quicklook);
  m_RedQuicklookPolarimetricSynthesisFilter->SetInput(m_Quicklook);
  m_GreenQuicklookPolarimetricSynthesisFilter->SetInput(m_Quicklook);
  m_BlueQuicklookPolarimetricSynthesisFilter->SetInput(m_Quicklook);
  // Connexions for the Scroll Image
  m_ScrollImageList->PushBack(m_RedQuicklookPolarimetricSynthesisFilter->GetOutput());
  m_ScrollImageList->PushBack(m_GreenQuicklookPolarimetricSynthesisFilter->GetOutput());
  m_ScrollImageList->PushBack(m_BlueQuicklookPolarimetricSynthesisFilter->GetOutput());
  m_ImageListToVectorImageFilterForScroll->SetInput(m_ScrollImageList);
  m_ImageListToVectorImageFilterForScroll->Update();

  this->SetupPolarization(GRAYSCALE);
  this->SetupPolarization(RED);
  this->SetupPolarization(GREEN);
  this->SetupPolarization(BLUE);

  m_GrayPolarimetricSynthesisFilter->GetOutput()->UpdateOutputInformation();
}

void PolarimetricSynthesisModel::ComputeNormalizationFactors()
{
  ComplexIteratorType it;
  it = ComplexIteratorType(m_Quicklook, m_Quicklook->GetLargestPossibleRegion());
  it.GoToBegin();

  ListSampleType::Pointer listSample = ListSampleType::New();
  while ( !it.IsAtEnd() )
  {
    ComplexVectorPixelType pixel = it.Get();
    for (unsigned int i = 0; i<m_Quicklook->GetNumberOfComponentsPerPixel(); ++i)
    {
      listSample->PushBack(static_cast<double>(pixel[i].real()*pixel[i].real() + pixel[i].imag()*pixel[i].imag()));
    }
    ++it;
  }


  HistogramGeneratorType::Pointer generator = HistogramGeneratorType::New();
  generator->SetListSample(listSample);
  HistogramGeneratorType::HistogramType::SizeType size;
  size.Fill(256);
  generator->SetNumberOfBins(size);
  generator->Update();
  m_Min = generator->GetOutput()->Quantile(0, 0.02);
  m_Max = generator->GetOutput()->Quantile(0, 0.98);
  m_AbsoluteMin = generator->GetOutput()->Quantile(0, 0);
  m_AbsoluteMax =  generator->GetOutput()->Quantile(0, 1);
}

void PolarimetricSynthesisModel::SetupEmission(Color value)
{

  switch (value)
  {
  case GRAYSCALE:
    m_GrayPolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_GrayPolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    m_GrayQuicklookPolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_GrayQuicklookPolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    break;
  case RED:
    m_RedPolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_RedPolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    m_RedQuicklookPolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_RedQuicklookPolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    break;
  case GREEN:
    m_GreenPolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_GreenPolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    m_GreenQuicklookPolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_GreenQuicklookPolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    break;
  case BLUE:
    m_BluePolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_BluePolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    m_BlueQuicklookPolarimetricSynthesisFilter->SetEmissionH(m_HEmissionMode);
    m_BlueQuicklookPolarimetricSynthesisFilter->SetEmissionV(m_VEmissionMode);
    break;
  default:
    break;
  }

}
void PolarimetricSynthesisModel::SetupPolarization(Color value)
{
  switch (value)
  {
  case GRAYSCALE:
    m_GrayPolarimetricSynthesisFilter->SetPsiI(m_GrayPsiI);
    m_GrayPolarimetricSynthesisFilter->SetPsiR(m_GrayPsiR);
    m_GrayPolarimetricSynthesisFilter->SetKhiI(m_GrayKhiI);
    m_GrayPolarimetricSynthesisFilter->SetKhiR(m_GrayKhiR);
    m_GrayQuicklookPolarimetricSynthesisFilter->SetPsiI(m_GrayPsiI);
    m_GrayQuicklookPolarimetricSynthesisFilter->SetPsiR(m_GrayPsiR);
    m_GrayQuicklookPolarimetricSynthesisFilter->SetKhiI(m_GrayKhiI);
    m_GrayQuicklookPolarimetricSynthesisFilter->SetKhiR(m_GrayKhiR);
    break;
  case RED:
    m_RedPolarimetricSynthesisFilter->SetPsiI(m_RedPsiI);
    m_RedPolarimetricSynthesisFilter->SetPsiR(m_RedPsiR);
    m_RedPolarimetricSynthesisFilter->SetKhiI(m_RedKhiI);
    m_RedPolarimetricSynthesisFilter->SetKhiR(m_RedKhiR);
    m_RedQuicklookPolarimetricSynthesisFilter->SetPsiI(m_RedPsiI);
    m_RedQuicklookPolarimetricSynthesisFilter->SetPsiR(m_RedPsiR);
    m_RedQuicklookPolarimetricSynthesisFilter->SetKhiI(m_RedKhiI);
    m_RedQuicklookPolarimetricSynthesisFilter->SetKhiR(m_RedKhiR);
    m_ImageListToVectorImageFilterForFull->Modified();
    m_ImageListToVectorImageFilterForScroll->Modified();
    break;
  case GREEN:
    m_GreenPolarimetricSynthesisFilter->SetPsiI(m_GreenPsiI);
    m_GreenPolarimetricSynthesisFilter->SetPsiR(m_GreenPsiR);
    m_GreenPolarimetricSynthesisFilter->SetKhiI(m_GreenKhiI);
    m_GreenPolarimetricSynthesisFilter->SetKhiR(m_GreenKhiR);
    m_GreenQuicklookPolarimetricSynthesisFilter->SetPsiI(m_GreenPsiI);
    m_GreenQuicklookPolarimetricSynthesisFilter->SetPsiR(m_GreenPsiR);
    m_GreenQuicklookPolarimetricSynthesisFilter->SetKhiI(m_GreenKhiI);
    m_GreenQuicklookPolarimetricSynthesisFilter->SetKhiR(m_GreenKhiR);
    m_ImageListToVectorImageFilterForFull->Modified();
    m_ImageListToVectorImageFilterForScroll->Modified();
    break;
  case BLUE:
    m_BluePolarimetricSynthesisFilter->SetPsiI(m_BluePsiI);
    m_BluePolarimetricSynthesisFilter->SetPsiR(m_BluePsiR);
    m_BluePolarimetricSynthesisFilter->SetKhiI(m_BlueKhiI);
    m_BluePolarimetricSynthesisFilter->SetKhiR(m_BlueKhiR);
    m_BlueQuicklookPolarimetricSynthesisFilter->SetPsiI(m_BluePsiI);
    m_BlueQuicklookPolarimetricSynthesisFilter->SetPsiR(m_BluePsiR);
    m_BlueQuicklookPolarimetricSynthesisFilter->SetKhiI(m_BlueKhiI);
    m_BlueQuicklookPolarimetricSynthesisFilter->SetKhiR(m_BlueKhiR);
    m_ImageListToVectorImageFilterForFull->Modified();
    m_ImageListToVectorImageFilterForScroll->Modified();
    break;
  default:
    break;
  }
}

void PolarimetricSynthesisModel::GenerateQuicklook()
{
  SizeType largestRegionSize;

  m_ShrinkFilter->SetInput(m_ImageListToVectorImageFilter->GetOutput());
  largestRegionSize = m_ImageListToVectorImageFilter->GetOutput()->GetLargestPossibleRegion().GetSize();

  unsigned int maxQuicklook = std::max(m_QuicklookSize[0], m_QuicklookSize[1]);
  unsigned int maxLargest   = std::max(largestRegionSize[0], largestRegionSize[1]);
  m_ShrinkFactor = maxLargest/maxQuicklook;
  if (m_ShrinkFactor == 0)
  {
    m_ShrinkFactor = 1;
  }


  m_ShrinkFilter->SetShrinkFactor(m_ShrinkFactor);
  m_ShrinkFilter->Update();
  m_Quicklook = m_ShrinkFilter->GetOutput();
  m_ShrinkFilter = ShrinkFilterType::New();
}

void PolarimetricSynthesisModel::GenerateOutputHistogram(Color value)
{
  if (m_Valid)
  {

    OutputIteratorType it;
    switch (value)
    {
    case GRAYSCALE :
      m_GrayQuicklookPolarimetricSynthesisFilter->Update();
      it = OutputIteratorType(m_GrayQuicklookPolarimetricSynthesisFilter->GetOutput(), m_GrayQuicklookPolarimetricSynthesisFilter->GetOutput()->GetLargestPossibleRegion());
      break;
    case RED :
      m_RedQuicklookPolarimetricSynthesisFilter->Update();
      it = OutputIteratorType(m_RedQuicklookPolarimetricSynthesisFilter->GetOutput(), m_RedQuicklookPolarimetricSynthesisFilter->GetOutput()->GetLargestPossibleRegion());
      break;
    case GREEN :
      m_GreenQuicklookPolarimetricSynthesisFilter->Update();
      it = OutputIteratorType(m_GreenQuicklookPolarimetricSynthesisFilter->GetOutput(), m_GreenQuicklookPolarimetricSynthesisFilter->GetOutput()->GetLargestPossibleRegion());
      break;
    case BLUE :
      m_BlueQuicklookPolarimetricSynthesisFilter->Update();
      it = OutputIteratorType(m_BlueQuicklookPolarimetricSynthesisFilter->GetOutput(), m_BlueQuicklookPolarimetricSynthesisFilter->GetOutput()->GetLargestPossibleRegion());
      break;
    }

    it.GoToBegin();
    ListSampleType::Pointer listSample = ListSampleType::New();

    while ( !it.IsAtEnd() )
    {
      listSample->PushBack(it.Get());
      ++it;
    }

    HistogramGeneratorType::Pointer generator = HistogramGeneratorType::New();
    m_HistogramGenerator->SetListSample(listSample);
    HistogramGeneratorType::HistogramType::SizeType size;
    size.Fill(256);
    m_HistogramGenerator->SetNumberOfBins(size);
    MeasurementVectorType min, max;
    min[0]= m_AbsoluteMin;
    max[0]= m_AbsoluteMax;
    m_HistogramGenerator->SetHistogramMin(min);
    m_HistogramGenerator->SetHistogramMax(max);
    m_HistogramGenerator->Update();
  }
}

PolarimetricSynthesisModel::DoubleVectorImageType * PolarimetricSynthesisModel::GetOutputVectorImage()
{
  return m_ImageListToVectorImageFilterForFull->GetOutput();
}
PolarimetricSynthesisModel::OutputImageType * PolarimetricSynthesisModel::GetOutputImage()
{
  return m_GrayPolarimetricSynthesisFilter->GetOutput();
}


PolarimetricSynthesisModel::DoubleVectorImageType * PolarimetricSynthesisModel::GetQuicklookOutputVectorImage()
{
  return m_ImageListToVectorImageFilterForScroll->GetOutput();
}
PolarimetricSynthesisModel::OutputImageType * PolarimetricSynthesisModel::GetQuicklookOutputImage()
{
  return m_GrayQuicklookPolarimetricSynthesisFilter->GetOutput();
}


const PolarimetricSynthesisModel::HistogramType * PolarimetricSynthesisModel::GetOutputHistogram()
{
  return m_HistogramGenerator->GetOutput();
}

void PolarimetricSynthesisModel::SetGain(double value)
{
  m_GrayPolarimetricSynthesisFilter->SetGain(value);
  m_GrayQuicklookPolarimetricSynthesisFilter->SetGain(value);
  this->GenerateOutputHistogram(GRAYSCALE);
  m_RedPolarimetricSynthesisFilter->SetGain(value);
  m_RedQuicklookPolarimetricSynthesisFilter->SetGain(value);
  this->GenerateOutputHistogram(RED);
  m_GreenPolarimetricSynthesisFilter->SetGain(value);
  m_GreenQuicklookPolarimetricSynthesisFilter->SetGain(value);
  this->GenerateOutputHistogram(GREEN);
  m_BluePolarimetricSynthesisFilter->SetGain(value);
  m_BlueQuicklookPolarimetricSynthesisFilter->SetGain(value);
  this->GenerateOutputHistogram(BLUE);
  m_ImageListToVectorImageFilterForFull->Modified();
  m_ImageListToVectorImageFilterForScroll->Modified();
  this->NotifyAll("Update");
}

//---------------------- GRAYSCALE PART ----------------------//
void PolarimetricSynthesisModel::SetGrayPolarizationMode(PolarizationMode mode)
{
  m_GrayPolarizationMode = mode;
  switch (mode)
  {
  case CO_POLAR:
    m_GrayKhiR = m_GrayKhiI;
    m_GrayPsiR = m_GrayPsiI;
    break;
  case CROSS_POLAR:
    m_GrayPsiR = m_GrayPsiI+90;
    m_GrayKhiR = -m_GrayKhiI;
    break;
  default:
    break;
  }
  this->SetupPolarization(GRAYSCALE);
  this->GenerateOutputHistogram(GRAYSCALE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetGrayPsiI(double value)
{
  if (!m_VEmissionMode)
  {
    m_GrayPsiI = 90;
  }
  else if (!m_HEmissionMode)
  {
    m_GrayPsiI = 0;
  }
  else
  {
    m_GrayPsiI = value;
    switch (m_GrayPolarizationMode)
    {
    case CO_POLAR:
      m_GrayPsiR = m_GrayPsiI;
      break;
    case CROSS_POLAR:
      m_GrayPsiR = m_GrayPsiI+90;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(GRAYSCALE);
  this->GenerateOutputHistogram(GRAYSCALE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetGrayPsiR(double value)
{
  m_GrayPsiR = value;
  switch (m_GrayPolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GrayPsiR = m_GrayPsiI;
    }
    else
    {
      m_GrayPsiI = m_GrayPsiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GrayPsiR = m_GrayPsiI+90;
    }
    else
    {
      m_GrayPsiI = m_GrayPsiR-90;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(GRAYSCALE);
  this->GenerateOutputHistogram(GRAYSCALE);
  this->NotifyAll("Update");
}
void PolarimetricSynthesisModel::SetGrayKhiI(double value)
{
  if (!m_VEmissionMode || !m_HEmissionMode)
  {
    m_GrayKhiI = 0;
  }
  else
  {
    m_GrayKhiI = value;
    switch (m_GrayPolarizationMode)
    {
    case CO_POLAR:
      m_GrayKhiR = m_GrayKhiI;
      break;
    case CROSS_POLAR:
      m_GrayKhiR = -m_GrayKhiI;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(GRAYSCALE);
  this->GenerateOutputHistogram(GRAYSCALE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetGrayKhiR(double value)
{
  m_GrayKhiR = value;
  switch (m_GrayPolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GrayKhiR = m_GrayKhiI;
    }
    else
    {
      m_GrayKhiI = m_GrayKhiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GrayKhiR = -m_GrayKhiI;
    }
    else
    {
      m_GrayKhiI = -m_GrayKhiR;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(GRAYSCALE);
  this->GenerateOutputHistogram(GRAYSCALE);
  this->NotifyAll("Update");
}


//---------------------- RED PART ----------------------//
void PolarimetricSynthesisModel::SetRedPolarizationMode(PolarizationMode mode)
{
  m_RedPolarizationMode = mode;
  switch (mode)
  {
  case CO_POLAR:
    m_RedKhiR = m_RedKhiI;
    m_RedPsiR = m_RedPsiI;
    break;
  case CROSS_POLAR:
    m_RedPsiR = m_RedPsiI+90;
    m_RedKhiR = -m_RedKhiI;
    break;
  default:
    break;
  }
  this->SetupPolarization(RED);
  this->GenerateOutputHistogram(RED);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetRedPsiI(double value)
{
  if (!m_VEmissionMode)
  {
    m_RedPsiI = 90;
  }
  else if (!m_HEmissionMode)
  {
    m_RedPsiI = 0;
  }
  else
  {
    m_RedPsiI = value;
    switch (m_RedPolarizationMode)
    {
    case CO_POLAR:
      m_RedPsiR = m_RedPsiI;
      break;
    case CROSS_POLAR:
      m_RedPsiR = m_RedPsiI+90;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(RED);
  this->GenerateOutputHistogram(RED);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetRedPsiR(double value)
{
  m_RedPsiR = value;
  switch (m_RedPolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_RedPsiR = m_RedPsiI;
    }
    else
    {
      m_RedPsiI = m_RedPsiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_RedPsiR = m_RedPsiI+90;
    }
    else
    {
      m_RedPsiI = m_RedPsiR-90;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(RED);
  this->GenerateOutputHistogram(RED);
  this->NotifyAll("Update");
}
void PolarimetricSynthesisModel::SetRedKhiI(double value)
{
  if (!m_VEmissionMode || !m_HEmissionMode)
  {
    m_RedKhiI = 0;
  }
  else
  {
    m_RedKhiI = value;
    switch (m_RedPolarizationMode)
    {
    case CO_POLAR:
      m_RedKhiR = m_RedKhiI;
      break;
    case CROSS_POLAR:
      m_RedKhiR = -m_RedKhiI;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(RED);
  this->GenerateOutputHistogram(RED);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetRedKhiR(double value)
{
  m_RedKhiR = value;
  switch (m_RedPolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_RedKhiR = m_RedKhiI;
    }
    else
    {
      m_RedKhiI = m_RedKhiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_RedKhiR = -m_RedKhiI;
    }
    else
    {
      m_RedKhiI = -m_RedKhiR;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(RED);
  this->GenerateOutputHistogram(RED);
  this->NotifyAll("Update");
}

//---------------------- GREEN PART ----------------------//
void PolarimetricSynthesisModel::SetGreenPolarizationMode(PolarizationMode mode)
{
  m_GreenPolarizationMode = mode;
  switch (mode)
  {
  case CO_POLAR:
    m_GreenKhiR = m_GreenKhiI;
    m_GreenPsiR = m_GreenPsiI;
    break;
  case CROSS_POLAR:
    m_GreenPsiR = m_GreenPsiI+90;
    m_GreenKhiR = -m_GreenKhiI;
    break;
  default:
    break;
  }
  this->SetupPolarization(GREEN);
  this->GenerateOutputHistogram(GREEN);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetGreenPsiI(double value)
{
  if (!m_VEmissionMode)
  {
    m_GreenPsiI = 90;
  }
  else if (!m_HEmissionMode)
  {
    m_GreenPsiI = 0;
  }
  else
  {
    m_GreenPsiI = value;
    switch (m_GreenPolarizationMode)
    {
    case CO_POLAR:
      m_GreenPsiR = m_GreenPsiI;
      break;
    case CROSS_POLAR:
      m_GreenPsiR = m_GreenPsiI+90;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(GREEN);
  this->GenerateOutputHistogram(GREEN);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetGreenPsiR(double value)
{
  m_GreenPsiR = value;
  switch (m_GreenPolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GreenPsiR = m_GreenPsiI;
    }
    else
    {
      m_GreenPsiI = m_GreenPsiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GreenPsiR = m_GreenPsiI+90;
    }
    else
    {
      m_GreenPsiI = m_GreenPsiR-90;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(GREEN);
  this->GenerateOutputHistogram(GREEN);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetGreenKhiI(double value)
{
  if (!m_VEmissionMode || !m_HEmissionMode)
  {
    m_GreenKhiI = 0;
  }
  else
  {
    m_GreenKhiI = value;
    switch (m_GreenPolarizationMode)
    {
    case CO_POLAR:
      m_GreenKhiR = m_GreenKhiI;
      break;
    case CROSS_POLAR:
      m_GreenKhiR = -m_GreenKhiI;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(GREEN);
  this->GenerateOutputHistogram(GREEN);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetGreenKhiR(double value)
{
  m_GreenKhiR = value;
  switch (m_GreenPolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GreenKhiR = m_GreenKhiI;
    }
    else
    {
      m_GreenKhiI = m_GreenKhiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_GreenKhiR = -m_GreenKhiI;
    }
    else
    {
      m_GreenKhiI = -m_GreenKhiR;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(GREEN);
  this->GenerateOutputHistogram(GREEN);
  this->NotifyAll("Update");
}

//---------------------- BLUE PART ----------------------//
void PolarimetricSynthesisModel::SetBluePolarizationMode(PolarizationMode mode)
{
  m_BluePolarizationMode = mode;
  switch (mode)
  {
  case CO_POLAR:
    m_BlueKhiR = m_BlueKhiI;
    m_BluePsiR = m_BluePsiI;
    break;
  case CROSS_POLAR:
    m_BluePsiR = m_BluePsiI+90;
    m_BlueKhiR = -m_BlueKhiI;
    break;
  default:
    break;
  }
  this->SetupPolarization(BLUE);
  this->GenerateOutputHistogram(BLUE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetBluePsiI(double value)
{
  if (!m_VEmissionMode)
  {
    m_BluePsiI = 90;
  }
  else if (!m_HEmissionMode)
  {
    m_BluePsiI = 0;
  }
  else
  {
    m_BluePsiI = value;
    switch (m_BluePolarizationMode)
    {
    case CO_POLAR:
      m_BluePsiR = m_BluePsiI;
      break;
    case CROSS_POLAR:
      m_BluePsiR = m_BluePsiI+90;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(BLUE);
  this->GenerateOutputHistogram(BLUE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetBluePsiR(double value)
{
  m_BluePsiR = value;
  switch (m_BluePolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_BluePsiR = m_BluePsiI;
    }
    else
    {
      m_BluePsiI = m_BluePsiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_BluePsiR = m_BluePsiI+90;
    }
    else
    {
      m_BluePsiI = m_BluePsiR-90;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(BLUE);
  this->GenerateOutputHistogram(BLUE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetBlueKhiI(double value)
{
  if (!m_VEmissionMode || !m_HEmissionMode)
  {
    m_BlueKhiI = 0;
  }
  else
  {
    m_BlueKhiI = value;
    switch (m_BluePolarizationMode)
    {
    case CO_POLAR:
      m_BlueKhiR = m_BlueKhiI;
      break;
    case CROSS_POLAR:
      m_BlueKhiR = -m_BlueKhiI;
      break;
    default:
      break;
    }
  }
  this->SetupPolarization(BLUE);
  this->GenerateOutputHistogram(BLUE);
  this->NotifyAll("Update");
}

void PolarimetricSynthesisModel::SetBlueKhiR(double value)
{
  m_BlueKhiR = value;
  switch (m_BluePolarizationMode)
  {
  case CO_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_BlueKhiR = m_BlueKhiI;
    }
    else
    {
      m_BlueKhiI = m_BlueKhiR;
    }
    break;
  case CROSS_POLAR:
    if (!m_HEmissionMode || !m_VEmissionMode)
    {
      m_BlueKhiR = -m_BlueKhiI;
    }
    else
    {
      m_BlueKhiI = -m_BlueKhiR;
    }
    break;
  default:
    break;
  }
  this->SetupPolarization(BLUE);
  this->GenerateOutputHistogram(BLUE);
  this->NotifyAll("Update");
}

}
