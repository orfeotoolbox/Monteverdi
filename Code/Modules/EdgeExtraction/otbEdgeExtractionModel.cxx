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

    std::ostringstream oss;
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

    std::ostringstream oss;
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

    std::ostringstream oss;
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

    std::ostringstream oss;
    oss << "TOUZI: " << radiusX;
    std::string mess = oss.str();
    this->AddFeatureFilter(filterTouzi, FeatureInfoEdge::TOUZI, i, 0, mess);
    }
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
