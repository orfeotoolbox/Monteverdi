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
#ifndef __otbEdgeDensityGenerator_h
#define __otbEdgeDensityGenerator_h

#include "otbFeatureEdge.h"
#include "otbEdgeExtractionModel.h"

#include "otbEdgeDensityImageFilter.h"
#include "otbBinaryImageDensityFunction.h"
#include "itkCannyEdgeDetectionImageFilter.h"
#include "itkSobelEdgeDetectionImageFilter.h"
#include "otbEdgeDetectorImageFilter.h"

namespace otb
{
/** \class EdgeDensityGenerator
 *
 *
 *
 */
class EdgeDensityGenerator
{
public:
  typedef EdgeExtractionModel               ModelType;
  typedef ModelType::Pointer                ModelPointerType;
  typedef ModelType::FilterType             FilterType;
  typedef ModelType::PixelType              PixelType;
  typedef ModelType::SingleImageType        SingleImageType;
  typedef ModelType::SingleImagePointerType SingleImagePointerType;
  typedef SingleImageType::SizeType         SizeType;
  typedef FeatureInfoEdge::FeatureType      FeatureType;
  typedef FeatureInfoBase::FeatureType      FeatureBaseType;

  /***************************/
  /** Filter type declaration*/
  /***************************/

  typedef otb::BinaryImageDensityFunction<SingleImageType>                          CountFunctionType;
  typedef itk::CannyEdgeDetectionImageFilter<SingleImageType, SingleImageType>      CannyDetectorType;
  typedef itk::SobelEdgeDetectionImageFilter<SingleImageType, SingleImageType>      SobelType;
  typedef otb::EdgeDetectorImageFilter<SingleImageType, SingleImageType, SobelType> SobelDetectorType;

  typedef EdgeDensityImageFilter<SingleImageType, SingleImageType, CannyDetectorType,
      CountFunctionType> CannyEdgeDensityFilterType;
  typedef EdgeDensityImageFilter<SingleImageType, SingleImageType, SobelDetectorType,
      CountFunctionType> SobelEdgeDensityFilterType;

  /* TODO */
  //typedef otb::EdgeDensityImageFilter<ImageType, ImageType, CannyDetectorType  , CountFunctionType> GradientEdgeDensityFilterType;

  // AddEdgeDensityFilter
  void AddEdgeDensityFilter(ModelPointerType pModel, FeatureBaseType pType, std::vector<double> params)
  {

    for (unsigned int i = 0; i < pModel->GetInputImageList()->Size(); i++)
      {
      std::ostringstream oss;
      switch (pType)
        {
        /** CASE NOT USED: ITK PROBLEM WITH CANNY FILTER*/
        case FeatureInfoEdge::EDGE_CANNY:
          {
          oss << "Edge Canny: ";

          CannyEdgeDensityFilterType::Pointer edgeDetectorFilter = CannyEdgeDensityFilterType::New();
          CannyDetectorType::Pointer          cannyFilter = CannyDetectorType::New();

          edgeDetectorFilter->SetInput(pModel->GetInputImageList()->GetNthElement(i));
          edgeDetectorFilter->SetDetector(cannyFilter);
          edgeDetectorFilter->SetNeighborhoodRadius(static_cast<unsigned int>(params[0]));

          cannyFilter->SetUpperThreshold(static_cast<PixelType>(params[1]));
          cannyFilter->SetLowerThreshold(static_cast<PixelType>(params[2]));
          cannyFilter->SetVariance(static_cast<double>(params[3]));
          cannyFilter->SetMaximumError(static_cast<double>(params[4]));

          oss << params[0] << "," << params[1] << "," << params[2] << "," << params[3] << "," << params[4];
          std::string mess = oss.str();
          pModel->AddFeatureFilter(edgeDetectorFilter, pType, i, 0, mess);
          break;
          }
        case FeatureInfoEdge::EDGE_SOBEL:
          {
          oss << "Edge Sobel: ";

          SobelEdgeDensityFilterType::Pointer edgeDetectorFilter = SobelEdgeDensityFilterType::New();
          SobelDetectorType::Pointer          sobelFilter = SobelDetectorType::New();

          sobelFilter->SetLowerThreshold(params[2]);
          sobelFilter->SetUpperThreshold(params[3]);
          SizeType size;
          size[0] = static_cast<unsigned int>(params[0]);
          size[1] = static_cast<unsigned int>(params[1]);
          edgeDetectorFilter->SetInput(pModel->GetInputImageList()->GetNthElement(i));
          edgeDetectorFilter->SetDetector(sobelFilter);
          edgeDetectorFilter->SetNeighborhoodRadius(size);

          oss << "[" << params[0] << "," << params[1] << "]," << params[2] << "," << params[3];
          std::string mess = oss.str();
          pModel->AddFeatureFilter(edgeDetectorFilter, pType, i, 0, mess);
          break;
          }
        default:
          {
          }
        }
      }
  }

  SingleImagePointerType GenerateEdgeDensityOutputImage(ModelPointerType pModel,
                                                        FeatureBaseType pType,
                                                        unsigned int pInputListId)
  {
    SingleImagePointerType image =  SingleImageType::New();
    switch (pType)
      {
      case FeatureInfoEdge::EDGE_CANNY:
        {
        CannyEdgeDensityFilterType::Pointer filter =
          dynamic_cast<CannyEdgeDensityFilterType*>(static_cast<FilterType *>(pModel->GetFilterList()->GetNthElement(
                                                                                pInputListId)));
        filter->GetOutput()->UpdateOutputInformation();
        filter->GetOutput()->SetRequestedRegionToLargestPossibleRegion();
        image = filter->GetOutput();
        break;
        }
      case FeatureInfoEdge::EDGE_SOBEL:
        {
        SobelEdgeDensityFilterType::Pointer filter =
          dynamic_cast<SobelEdgeDensityFilterType*>(static_cast<FilterType *>(pModel->GetFilterList()->GetNthElement(
                                                                                pInputListId)));
        filter->GetOutput()->UpdateOutputInformation();
        filter->GetOutput()->SetRequestedRegionToLargestPossibleRegion();
        image = filter->GetOutput();
        break;
        }
      default:
        {
        }
      }
    return image;
  }

protected:

private:

};

}
#endif
