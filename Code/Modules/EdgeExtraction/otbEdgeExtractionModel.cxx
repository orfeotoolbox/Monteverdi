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
#ifndef __otbEdgeExtractionModel_cxx
#define __otbEdgeExtractionModel_cxx

#include "otbEdgeExtractionModel.h"

#include "otbEdgeDensityGenerator.h"

namespace otb
{

EdgeExtractionModel::EdgeExtractionModel()
{
  m_MSImageListList          = ImageListObjectListType::New();
  m_MSListToVectorFilterList = ImageListToVectorObjectListType::New();
  m_MeanShiftFilterList      = MeanShiftFilterListType::New();
}

EdgeExtractionModel::~EdgeExtractionModel()
{
}

void
EdgeExtractionModel
::AddHarrisFilter(double sigmaD, double sigmaI, double alpha)
{
  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    HarrisType::Pointer harris = HarrisType::New();

    harris->SetSigmaD(sigmaD);
    harris->SetSigmaI(sigmaI);
    harris->SetAlpha(alpha);
    harris->SetInput(GetInputImageList()->GetNthElement(i));

    itk::OStringStream oss;
    oss << "HARRIS: " << sigmaD << " , " << sigmaI << " , " << alpha;
    std::string mess = oss.str();
    this->AddFeatureFilter(harris, FeatureInfoEdge::HARRIS, i, 0, mess);
    }
}

void
EdgeExtractionModel
::AddVarianceFilter(int radiusX, int radiusY)
{
  SingleSizeType rad;
  rad[0] = radiusX;
  rad[1] = radiusY;
  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    VarFilterType::Pointer var = VarFilterType::New();
    var->SetRadius(rad);
    var->SetInput(GetInputImageList()->GetNthElement(i));

    itk::OStringStream oss;
    oss << "Variance: " << radiusX << ", " << radiusY;
    std::string mess = oss.str();
    this->AddFeatureFilter(var, FeatureInfoEdge::VARIANCE, i, 0, mess);
    }
}

void
EdgeExtractionModel
::AddGradientFilter(double sigma)
{
  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    GradientFilterType::Pointer grad =  GradientFilterType::New();
    grad->SetSigma(sigma);
    grad->SetInput(GetInputImageList()->GetNthElement(i));

    itk::OStringStream oss;
    oss << "Rec. Gradient: " << sigma;
    std::string mess = oss.str();
    this->AddFeatureFilter(grad, FeatureInfoEdge::GRADIENT, i, 0, mess);
    }
}

void
EdgeExtractionModel
::AddEdgeDensityFilter(FeatureType type, std::vector<double> params)
{
  EdgeDensityGenerator lEdgeDensityGenerator;
  lEdgeDensityGenerator.AddEdgeDensityFilter(this, type, params);
}

void
EdgeExtractionModel
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

//     if (type == FeatureInfoEdge::MS_SMOOTH ||  type == FeatureInfoEdge::MS_CLUSTERED)
//       {
//       for (unsigned int k = 0; k < GetInputImageList()->Size(); k++)
//         {
//         ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
//         extract->SetChannel(k + 1);
//         itk::OStringStream oss;
//         oss << "Mean Shift ";
//         if (type == FeatureInfoEdge::MS_SMOOTH)
//           {
//           extract->SetInput(meanShift->GetOutput());
//           oss << "Smooth";
//           }
//         else if (type == FeatureInfoEdge::MS_CLUSTERED)
//           {
//           extract->SetInput(meanShift->GetClusteredOutput());
//           oss << "Clustered";
//           }
//         oss << " (Ch." << k + 1 << "): ";
//         oss << spatial << ", " << range << ", " << minSize << ", " << scale;
//         std::string mess = oss.str();
//         this->AddFeatureFilter(extract, type, -1, 0, mess);
//         }
//       }
//     else
//       {
      itk::OStringStream oss;
      oss << "Mean Shift Clustered Boundries: ";
//       if (type == FeatureInfoEdge::MS_BOUND) oss << "Clustered Boundries: ";
//       else if (type == FeatureInfoEdge::MS_LABELED) oss << "Labeled: ";
      oss << spatial << ", " << range << ", " << minSize << ", " << scale;
      std::string mess = oss.str();

      this->AddFeatureFilter(meanShift, type, -1, 0, mess);
//       }
    m_MSImageListList->PushBack(imList);
    m_MSListToVectorFilterList->PushBack(list2Vec);
    m_MeanShiftFilterList->PushBack(meanShift);
    }
  else
    {
//     if (type == FeatureInfoEdge::MS_SMOOTH || type == FeatureInfoEdge::MS_CLUSTERED)
//       {
//       for (unsigned int k = 0; k < GetInputImageList()->Size(); k++)
//         {
//         ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
//         extract->SetChannel(k + 1);
//         itk::OStringStream oss;
//         oss << "Mean Shift ";
//         if (type == FeatureInfoEdge::MS_SMOOTH)
//           {
//           extract->SetInput(m_MeanShiftFilterList->GetNthElement(i - 1)->GetOutput());
//           oss << "Smooth";
//           }
//         else if (type == FeatureInfoEdge::MS_CLUSTERED)
//           {
//           extract->SetInput(m_MeanShiftFilterList->GetNthElement(i - 1)->GetClusteredOutput());
//           oss << "Clustered";
//           }
//         oss << " (Ch." << k + 1 << "): ";
//         oss << spatial << ", " << range << ", " << minSize << ", " << scale;
//         std::string mess = oss.str();
//         this->AddFeatureFilter(extract, type, -1, 0, mess);
//         }
//       }
//     else
//       {
      itk::OStringStream oss;
      oss << "Mean Shift Clustered Boundries: ";
//       if (type == FeatureInfoEdge::MS_LABELED) oss << "Labeled: ";
//       else if (type == FeatureInfoEdge::MS_BOUND) oss << "Clustered Boundries: ";
      oss << spatial << ", " << range << ", " << minSize << ", " << scale;
      std::string mess = oss.str();

      this->AddFeatureFilter(m_MeanShiftFilterList->GetNthElement(i - 1), type, -1, 0, mess);
//       }
    }
}

void
EdgeExtractionModel
::AddTouziFilter(unsigned int radiusX)
{
  TouziFilterType::SizeType radius;
  radius[0] = radiusX;
  radius[1] = radiusX;

  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    TouziFilterType::Pointer filterTouzi = TouziFilterType::New();
    filterTouzi->SetRadius(radius);
    filterTouzi->SetInput(GetInputImageList()->GetNthElement(i));

    itk::OStringStream oss;
    oss << "TOUZI: " << radiusX;
    std::string mess = oss.str();
    this->AddFeatureFilter(filterTouzi, FeatureInfoEdge::TOUZI, i, 0, mess);
    }
}

void
EdgeExtractionModel
::InitMeanShiftLists()
{
  m_MSImageListList->Clear();
  m_MSListToVectorFilterList->Clear();
  m_MeanShiftFilterList->Clear();
}

EdgeExtractionModel
::SingleImagePointerType
EdgeExtractionModel
::GetSingleImage(int i)
{
  EdgeDensityGenerator        lEdgeDensityGenerator;
  SingleImagePointerType      image = SingleImageType::New();

  switch ((GetFilterTypeList())[i])
    {
    case FeatureInfoEdge::HARRIS:
      {
      HarrisType::Pointer harris = dynamic_cast<HarrisType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = harris->GetOutput();
      break;
      }
    case FeatureInfoEdge::VARIANCE:
      {
      VarFilterType::Pointer var = dynamic_cast<VarFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = var->GetOutput();
      break;
      }
    case FeatureInfoEdge::GRADIENT:
      {
      GradientFilterType::Pointer grad =
        dynamic_cast<GradientFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = grad->GetOutput();
      break;
      }
    case FeatureInfoEdge::EDGE_CANNY:
    case FeatureInfoEdge::EDGE_SOBEL:
      {
      image = lEdgeDensityGenerator.GenerateEdgeDensityOutputImage(this, (GetFilterTypeList())[i], i);
      break;
      }
    case FeatureInfoEdge::MS_BOUND:
      {
      MeanShiftFilterType::Pointer meanShift =
        dynamic_cast<MeanShiftFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = meanShift->GetClusterBoundariesOutput();
      break;
      }
    case FeatureInfoEdge::TOUZI:
      {
      TouziFilterType::Pointer touzi =
        dynamic_cast<TouziFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = touzi->GetOutput();
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
