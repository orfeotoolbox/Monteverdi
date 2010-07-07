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
#ifndef __otbTextureFilterGenerator_h
#define __otbTextureFilterGenerator_h

#include "otbFeature.h"
#include "otbFeatureExtractionModel.h"
#include "otbScalarImageToPanTexTextureFilter.h"

namespace otb
{
class TextureFilterGenerator
{
public:
  typedef FeatureExtractionModel            ModelType;
  typedef ModelType::Pointer                ModelPointerType;
  typedef ModelType::FilterType             FilterType;
  typedef ModelType::SizeType               SizeType;
  typedef ModelType::OffsetType             OffsetType;
  typedef ModelType::IteratorType           IteratorType;
  typedef ModelType::InputImagePixelType    InputImagePixelType;
  typedef ModelType::SingleImageType        SingleImageType;
  typedef ModelType::SinglePixelType        SinglePixelType;
  typedef ModelType::SingleImagePointerType SingleImagePointerType;
  typedef FeatureInfo::FeatureType          FeatureType;

  // Pan Tax
  typedef ScalarImageToPanTexTextureFilter<SingleImageType, SingleImageType> PanTexFilterType;

  template <class TFilterTypeMethod> void GenericAddTextureFilter(ModelPointerType pModel,
                                                                  FeatureType pType,
                                                                  unsigned int inputListId,
                                                                  SizeType pRadius,
                                                                  OffsetType pOff,
                                                                  unsigned int pBin,
                                                                  double pMin,
                                                                  double pMax,
                                                                  std::string pMsg);

  void GenerateTextureFilter(ModelPointerType pModel,
                             FeatureType pType,
                             SizeType pRadius,
                             OffsetType pOff,
                             unsigned int pBin)
  {
    for (unsigned int i = 0; i < pModel->GetInputImageList()->Size(); i++)
      {
      std::string mess;
      switch (pType)
        {
        case FeatureInfo::TEXT_PANTEX:
          {
          // Don't call the generic method, no offset set in this filter
          SinglePixelType min = static_cast<SinglePixelType>(pModel->GetSelectedMinValues()[i]);
          SinglePixelType max = static_cast<SinglePixelType>(pModel->GetSelectedMaxValues()[i]);

          itk::OStringStream oss;
          oss << "PanTex: " << pRadius << ", " << pBin << ", min: " << min << ", max: " << max;

          PanTexFilterType::Pointer filter = PanTexFilterType::New();
          filter->SetRadius(pRadius);
          filter->SetNumberOfBinsPerAxis(pBin);
          filter->SetInputImageMinimum(static_cast<SinglePixelType>(min));
          filter->SetInputImageMaximum(static_cast<SinglePixelType>(max));
          filter->SetInput(pModel->GetInputImageList()->GetNthElement(i));

          std::string mess = oss.str();
          pModel->AddFeatureFilter(filter, pType, i, 0, mess);

          break;
          }
        default:
          {
          return;
          }
        } // if switch (type)
      } //  for
  }

  SingleImagePointerType GenerateTextureOutputImage(ModelPointerType model,  FeatureType type, unsigned int inputListId)
  {
    SingleImagePointerType image =  SingleImageType::New();
    switch (type)
      {
      case FeatureInfo::TEXT_PANTEX:
        {
        PanTexFilterType::Pointer panTexFilter =
          dynamic_cast<PanTexFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = panTexFilter->GetOutput();

        break;
        }
      default:
        {
        break;
        }
      } // if switch (type)
    return image;
  }

};

template <class TFilterTypeMethod>
void
TextureFilterGenerator::GenericAddTextureFilter(ModelPointerType pModel,
                                                FeatureType pType,
                                                unsigned int inputListId,
                                                SizeType pRadius,
                                                OffsetType pOff,
                                                unsigned int pBin,
                                                double pMin,
                                                double pMax,
                                                std::string pMsg)
{
  itk::OStringStream oss;
  oss << pMsg;
  oss << pOff << ", " << pRadius << ", " << pBin << ", min: " << pMin << ", max: " << pMax;
  typename TFilterTypeMethod::Pointer filter = TFilterTypeMethod::New();

  filter->SetRadius(pRadius);
  filter->SetOffset(pOff);
  filter->SetNumberOfBinsPerAxis(pBin);
  filter->SetInputImageMinimum(static_cast<SinglePixelType>(pMin));
  filter->SetInputImageMaximum(static_cast<SinglePixelType>(pMax));
  filter->SetInput(pModel->GetInputImageList()->GetNthElement(inputListId));

  std::string mess = oss.str();
  pModel->AddFeatureFilter(filter, pType, inputListId, 0, mess);
}

}
#endif
