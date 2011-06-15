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
#ifndef __otbAdvancedTexturesGenerator_h
#define __otbAdvancedTexturesGenerator_h

#include "otbFeature.h"
#include "otbFeatureExtractionModel.h"
#include "otbScalarImageToAdvancedTexturesFilter.h"

namespace otb
{
/** \class AdvancedTexturesGenerator
 *
 *
 *
 */
class AdvancedTexturesGenerator
{
public:
  typedef FeatureExtractionModel            ModelType;
  typedef ModelType::Pointer                ModelPointerType;
  typedef ModelType::FilterType             FilterType;
  typedef ModelType::PixelType              PixelType;
  typedef ModelType::SizeType               SizeType;
  typedef ModelType::OffsetType             OffsetType;
  typedef ModelType::SingleImageType        SingleImageType;
  typedef ModelType::SingleImagePointerType SingleImagePointerType;
  typedef ModelType::SinglePixelType        SinglePixelType;

  typedef FeatureInfo::FeatureType     FeatureType;
  typedef AdvancedTexture::TextureType TextureType;
  typedef std::vector<TextureType>     TextureVectorType;

  /***************************/
  /** Filter type declaration*/
  /***************************/
  typedef ScalarImageToAdvancedTexturesFilter<SingleImageType, SingleImageType> AdvTexturesFilterType;

  AdvancedTexturesGenerator()
  {
    m_TextToHarMap[AdvancedTexture::VARIANCE] = FeatureInfo::TEXT_ADV_VARIANCE;
    m_TextToHarMap[AdvancedTexture::MEAN] = FeatureInfo::TEXT_ADV_MEAN;
    m_TextToHarMap[AdvancedTexture::SUMAV] = FeatureInfo::TEXT_ADV_SUMAV;
    m_TextToHarMap[AdvancedTexture::SUMVAR] = FeatureInfo::TEXT_ADV_SUMVAR;
    m_TextToHarMap[AdvancedTexture::SUMENT] = FeatureInfo::TEXT_ADV_SUMENT;
    m_TextToHarMap[AdvancedTexture::DIFFENT] = FeatureInfo::TEXT_ADV_DIFFENT;
    m_TextToHarMap[AdvancedTexture::DIFFVAR] = FeatureInfo::TEXT_ADV_DIFFVAR;
    m_TextToHarMap[AdvancedTexture::IC1] = FeatureInfo::TEXT_ADV_IC1;
    m_TextToHarMap[AdvancedTexture::IC2] = FeatureInfo::TEXT_ADV_IC2;
    m_TextToHarMap[AdvancedTexture::UNKNOWN] = FeatureInfo::TEXT_ADV_UNKNOWN;
  }

  virtual ~AdvancedTexturesGenerator(){}

  /************/
  /** Methods */
  /************/

// AddHarTexturesFilter using computed image min/max
  void AddAdvTexturesFilter(ModelPointerType pModel,
                            TextureVectorType pHarList,
                            SizeType pRadius,
                            OffsetType pOff,
                            unsigned int pBin)
  {
    std::vector<double> minHarVect, maxHarVect;
    
    for (unsigned int i = 0; i < pModel->GetInputImageList()->Size(); i++)
      {
        minHarVect.push_back(pModel->GetSelectedMinValues()[i]);
        maxHarVect.push_back(pModel->GetSelectedMaxValues()[i]);
      }
    
    this->AddAdvTexturesFilter( pModel, pHarList, pRadius, pOff, minHarVect, maxHarVect, pBin);
  }
  
  // AddHarTexturesFilter using user's image min/max
  void AddAdvTexturesFilter(ModelPointerType pModel,
                            TextureVectorType pHarList,
                            SizeType pRadius,
                            OffsetType pOff,
                            double minHar,
                            double maxHar,
                            unsigned int pBin)
  {
    std::vector<double> minHarVect(pModel->GetInputImageList()->Size(), minHar);
    std::vector<double> maxHarVect(pModel->GetInputImageList()->Size(), maxHar);
  
    this->AddAdvTexturesFilter( pModel, pHarList, pRadius, pOff, minHarVect, maxHarVect, pBin);
  }
  
  void AddAdvTexturesFilter(ModelPointerType pModel,
                            TextureVectorType pHarList,
                            SizeType pRadius,
                            OffsetType pOff,
                            std::vector<double> minHarVect,
                            std::vector<double> maxHarVect,
                            unsigned int pBin)
  {
    for (unsigned int i = 0; i < pModel->GetInputImageList()->Size(); i++)
      {
      AdvTexturesFilterType::Pointer filter = AdvTexturesFilterType::New();
      filter->SetRadius(pRadius);
      filter->SetOffset(pOff);
      filter->SetNumberOfBinsPerAxis(pBin);

      filter->SetInputImageMinimum(static_cast<SinglePixelType>(minHarVect[i]));
      filter->SetInputImageMaximum(static_cast<SinglePixelType>(maxHarVect[i]));
      filter->SetInput(pModel->GetInputImageList()->GetNthElement(i));

      for (unsigned int textId = 0; textId < pHarList.size(); textId++)
        {
        itk::OStringStream oss;
        //oss<<"Advanced Text: ";

        switch (pHarList[textId])
          {
          case AdvancedTexture::VARIANCE:
            {
            oss << "Variance: ";
            break;
            }
          case AdvancedTexture::MEAN:
            {
            oss << "Mean: ";
            break;
            }
          case AdvancedTexture::SUMAV:
            {
            oss << "Sum average: ";
            break;
            }
          case AdvancedTexture::SUMVAR:
            {
            oss << "Sum variance: ";
            break;
            }
          case AdvancedTexture::SUMENT:
            {
            oss << "Sum entropy: ";
            break;
            }
          case AdvancedTexture::DIFFENT:
            {
            oss << "Diff. entropy: ";
            break;
            }
          case AdvancedTexture::DIFFVAR:
            {
            oss << "Diff. variance: ";
            break;
            }
          case AdvancedTexture::IC1:
            {
            oss << "Inf. correlation 1: ";
            break;
            }
          case AdvancedTexture::IC2:
            {
            oss << "Inf. correlation 2: ";
            break;
            }
          default:
            {
            break;
            }
          }

        oss << pOff << ", " << pRadius << ", " << pBin << ", min: " << pModel->GetSelectedMinValues()[i] <<
        ", max: " << pModel->GetSelectedMaxValues()[i];
        std::string mess = oss.str();
        pModel->AddFeatureFilter(filter, m_TextToHarMap[pHarList.at(textId)], i, 0, mess);
        }
      }
  }

  SingleImagePointerType GenerateAdvancedTextureOutputImage(ModelPointerType pModel,
                                                            FeatureType pType,
                                                            unsigned int pInputListId)
  {
    SingleImagePointerType         image =  SingleImageType::New();
    AdvTexturesFilterType::Pointer filter =
      dynamic_cast<AdvTexturesFilterType*>(static_cast<FilterType *>(pModel->GetFilterList()->GetNthElement(
                                                                       pInputListId)));
    switch (pType)
      {
      case FeatureInfo::TEXT_ADV_VARIANCE:
        {
        image = filter->GetVarianceOutput();
        break;
        }
      case FeatureInfo::TEXT_ADV_MEAN:
        {
        image = filter->GetMeanOutput();
        break;
        }
      case FeatureInfo::TEXT_ADV_SUMAV:
        {
        image = filter->GetSumAverageOutput();
        break;
        }
      case FeatureInfo::TEXT_ADV_SUMVAR:
        {
        image = filter->GetSumVarianceOutput();
        break;
        }
      case FeatureInfo::TEXT_ADV_SUMENT:
        {
        image = filter->GetSumEntropyOutput();
        break;
        }
      case FeatureInfo::TEXT_ADV_DIFFENT:
        {
        image = filter->GetDifferenceEntropyOutput();
        break;
        }
      case FeatureInfo::TEXT_ADV_DIFFVAR:
        {
        image = filter->GetDifferenceVarianceOutput();
        break;
        }
      case FeatureInfo::TEXT_ADV_IC1:
        {
        image = filter->GetIC1Output();
        break;
        }
      case FeatureInfo::TEXT_ADV_IC2:
        {
        image = filter->GetIC2Output();
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
  std::map<TextureType, FeatureType> m_TextToHarMap;
};

}
#endif
