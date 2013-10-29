/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbSmoothingModel_cxx
#define __otbSmoothingModel_cxx

#include "otbSmoothingModel.h"
#include "otbFeatureSmooth.h"

namespace otb
{

SmoothingModel::SmoothingModel()
{
  m_MSImageListList          = ImageListObjectListType::New();
  m_MSListToVectorFilterList = ImageListToVectorObjectListType::New();
  m_MeanShiftFilterList      = MeanShiftFilterListType::New();
}

SmoothingModel::~SmoothingModel()
{
}

void
SmoothingModel
::InitMeanShiftLists()
{
  m_MSImageListList->Clear();
  m_MSListToVectorFilterList->Clear();
  m_MeanShiftFilterList->Clear();
}

void
SmoothingModel
::AddOriginalData()
{
  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    ShiftScaleFilterType::Pointer copy = ShiftScaleFilterType::New();
    copy->SetShift(0);
    copy->SetScale(1);
    copy->SetInput(GetInputImageList()->GetNthElement(i));

    std::ostringstream oss;
    oss << "Original data";
    std::string mess = oss.str();

    this->AddFeatureFilter(copy, FeatureInfoSmooth::ORIGINAL, i, 0, mess);
    }
}

void
SmoothingModel
::AddMeanFilter(int radiusX, int radiusY)
{
  SingleSizeType rad;
  rad[0] = radiusX;
  rad[1] = radiusY;
  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    MeanFilterType::Pointer mean = MeanFilterType::New();
    mean->SetRadius(rad);
    mean->SetInput(GetInputImageList()->GetNthElement(i));

    std::ostringstream oss;
    oss << "Mean: " << radiusX << ", " << radiusY;
    std::string mess = oss.str();
    this->AddFeatureFilter(mean, FeatureInfoSmooth::MEAN, i, 0, mess);
    }
}

void
SmoothingModel
::AddMeanShiftFilter(FeatureType type, unsigned int spatial, double range, unsigned int minSize, double scale)
{
  /*
     Mean shift clustered output is a vector image. We only deals with filter that have image as output.
     To avoid huge modification, in the  case clustered image, we extract each channel of the output
     those channels (ie extractor) are added the the filterlist.
  */
  bool         alreadyLinked = false;
  unsigned int i = 0;

  // if not already exist
  while (i < m_MeanShiftFilterList->Size() && alreadyLinked == false)
    {
    // check same parameters
    MeanShiftFilterType::Pointer meanShiftTemp = m_MeanShiftFilterList->GetNthElement(i);
    unsigned int                 spatialCur = meanShiftTemp->GetSpatialRadius();
    double                       rangeCur = meanShiftTemp->GetRangeRadius();
    unsigned int                 minSizeCur = meanShiftTemp->GetMinimumRegionSize();
    double                       scaleCur = meanShiftTemp->GetScale();

    if (spatialCur == spatial && rangeCur == range && minSizeCur == minSize && scaleCur == scale) alreadyLinked = true;
    i++;
    }

  if (alreadyLinked == false)
    {
    ImageListType::Pointer                    imList = ImageListType::New();
    ImageListToVectorImageFilterType::Pointer list2Vec = ImageListToVectorImageFilterType::New();

    // Create input vector image from selectd channels
    for (unsigned int j = 0; j < GetInputImageList()->Size(); j++)
      {
      imList->PushBack(GetInputImageList()->GetNthElement(j));
      }
    list2Vec->SetInput(imList);

    MeanShiftFilterType::Pointer meanShift = MeanShiftFilterType::New();
    meanShift->SetInput(list2Vec->GetOutput());
    meanShift->SetSpatialRadius(spatial);
    meanShift->SetRangeRadius(range);
    meanShift->SetMinimumRegionSize(minSize);
    meanShift->SetScale(scale);

//     if (type == FeatureInfoSmooth::MS_SMOOTH ||  type == FeatureInfoSmooth::MS_CLUSTERED)
//       {
      for (unsigned int k = 0; k < GetInputImageList()->Size(); k++)
        {
        ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
        extract->SetChannel(k + 1);
        std::ostringstream oss;
        oss << "Mean Shift ";
        if (type == FeatureInfoSmooth::MS_SMOOTH)
          {
          extract->SetInput(meanShift->GetOutput());
          oss << "Smooth";
          }
        else if (type == FeatureInfoSmooth::MS_CLUSTERED)
          {
          extract->SetInput(meanShift->GetClusteredOutput());
          oss << "Clustered";
          }
        oss << " (Ch." << k + 1 << "): ";
        oss << spatial << ", " << range << ", " << minSize << ", " << scale;
        std::string mess = oss.str();
        this->AddFeatureFilter(extract, type, -1, 0, mess);
        }
//       }
//     else
//       {
//       std::ostringstream oss;
//       oss << "Mean Shift ";
//       if (type == FeatureInfoSmooth::MS_BOUND) oss << "Clustered Boundries: ";
//       else if (type == FeatureInfoSmooth::MS_LABELED) oss << "Labeled: ";
//       oss << spatial << ", " << range << ", " << minSize << ", " << scale;
//       std::string mess = oss.str();
//
//       this->AddFeatureFilter(meanShift, type, -1, 0, mess);
//       }
    m_MSImageListList->PushBack(imList);
    m_MSListToVectorFilterList->PushBack(list2Vec);
    m_MeanShiftFilterList->PushBack(meanShift);
    }
  else
    {
//     if (type == FeatureInfoSmooth::MS_SMOOTH || type == FeatureInfoSmooth::MS_CLUSTERED)
//       {
      for (unsigned int k = 0; k < GetInputImageList()->Size(); k++)
        {
        ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
        extract->SetChannel(k + 1);
        std::ostringstream oss;
        oss << "Mean Shift ";
        if (type == FeatureInfoSmooth::MS_SMOOTH)
          {
          extract->SetInput(m_MeanShiftFilterList->GetNthElement(i - 1)->GetOutput());
          oss << "Smooth";
          }
        else if (type == FeatureInfoSmooth::MS_CLUSTERED)
          {
          extract->SetInput(m_MeanShiftFilterList->GetNthElement(i - 1)->GetClusteredOutput());
          oss << "Clustered";
          }
        oss << " (Ch." << k + 1 << "): ";
        oss << spatial << ", " << range << ", " << minSize << ", " << scale;
        std::string mess = oss.str();
        this->AddFeatureFilter(extract, type, -1, 0, mess);
        }
//       }
//     else
//       {
//       std::ostringstream oss;
//       oss << "Mean Shift ";
//       if (type == FeatureInfoSmooth::MS_LABELED) oss << "Labeled: ";
//       else if (type == FeatureInfoSmooth::MS_BOUND) oss << "Clustered Boundries: ";
//       oss << spatial << ", " << range << ", " << minSize << ", " << scale;
//       std::string mess = oss.str();
//
//       this->AddFeatureFilter(m_MeanShiftFilterList->GetNthElement(i - 1), type, -1, 0, mess);
//       }
    }
}

SmoothingModel
::SingleImagePointerType
SmoothingModel
::GetSingleImage(int i)
{
  SingleImagePointerType      image = SingleImageType::New();

  switch ((GetFilterTypeList())[i])
    {
    case FeatureInfoSmooth::MEAN:
      {
      MeanFilterType::Pointer mean =
        dynamic_cast<MeanFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = mean->GetOutput();
      break;
      }
    case FeatureInfoSmooth::MS_SMOOTH:
    case FeatureInfoSmooth::MS_CLUSTERED:
      {
      ExtractROIFilterType::Pointer extract =
        dynamic_cast<ExtractROIFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = extract->GetOutput();
      break;
      }
    case FeatureInfoSmooth::ORIGINAL:
      {
      ShiftScaleFilterType::Pointer copy =
        dynamic_cast<ShiftScaleFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = copy->GetOutput();
      break;
      }
    default:
      {
      }
    }

  return image;
}

}

#endif
