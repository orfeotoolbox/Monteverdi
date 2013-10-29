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
#ifndef __otbHaralickTexturesGenerator_h
#define __otbHaralickTexturesGenerator_h

#include "otbFeatureTex.h"
#include "otbTextureExtractionModel.h"
#include "otbScalarImageToTexturesFilter.h"

namespace otb
{
/** \class HaralickTexturesGenerator
 *
 *
 *
 */
class HaralickTexturesGenerator
{
public:
  typedef TextureExtractionModel            ModelType;
  typedef ModelType::Pointer                ModelPointerType;
  typedef ModelType::FilterType             FilterType;
  typedef ModelType::PixelType              PixelType;
  typedef ModelType::SizeType               SizeType;
  typedef ModelType::OffsetType             OffsetType;
  typedef ModelType::SingleImageType        SingleImageType;
  typedef ModelType::SingleImagePointerType SingleImagePointerType;
  typedef ModelType::SinglePixelType        SinglePixelType;

  typedef FeatureInfoTex::FeatureType  FeatureType;
  typedef HaralickTexture::TextureType TextureType;
  typedef std::vector<TextureType>     TextureVectorType;
  typedef FeatureInfoBase::FeatureType FeatureBaseType;

  /***************************/
  /** Filter type declaration*/
  /***************************/
  typedef ScalarImageToTexturesFilter<SingleImageType, SingleImageType> HarTexturesFilterType;

  HaralickTexturesGenerator()
  {
    m_TextToHarMap[HaralickTexture::ENERGY] = FeatureInfoTex::TEXT_HAR_ENERGY;
    m_TextToHarMap[HaralickTexture::ENTROPY] = FeatureInfoTex::TEXT_HAR_ENTROPY;
    m_TextToHarMap[HaralickTexture::CORRELATION] = FeatureInfoTex::TEXT_HAR_CORR;
    m_TextToHarMap[HaralickTexture::INERTIA] = FeatureInfoTex::TEXT_HAR_INERTIA;
    m_TextToHarMap[HaralickTexture::INVDIFMO] = FeatureInfoTex::TEXT_HAR_INVDIFMO;
    m_TextToHarMap[HaralickTexture::CLUSPRO] = FeatureInfoTex::TEXT_HAR_CLUSPRO;
    m_TextToHarMap[HaralickTexture::CLUSHA] = FeatureInfoTex::TEXT_HAR_CLUSHA;
    m_TextToHarMap[HaralickTexture::HARCORR] = FeatureInfoTex::TEXT_HAR_HARCORR;
    m_TextToHarMap[HaralickTexture::UNKNOWN] = FeatureInfoTex::TEXT_HAR_UNKNOWN;
  }

  virtual ~HaralickTexturesGenerator(){}

  /************/
  /** Methods */
  /************/

// AddHarTexturesFilter using computed image min/max
void AddHarTexturesFilter(ModelPointerType pModel,
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

  this->AddHarTexturesFilter( pModel, pHarList, pRadius, pOff, minHarVect, maxHarVect, pBin);
}

// AddHarTexturesFilter using user's image min/max
void AddHarTexturesFilter(ModelPointerType pModel,
                          TextureVectorType pHarList,
                          SizeType pRadius,
                          OffsetType pOff,
                          double minHar,
                          double maxHar,
                          unsigned int pBin)
{
  std::vector<double> minHarVect(pModel->GetInputImageList()->Size(), minHar);
  std::vector<double> maxHarVect(pModel->GetInputImageList()->Size(), maxHar);
  
  this->AddHarTexturesFilter( pModel, pHarList, pRadius, pOff, minHarVect, maxHarVect, pBin);
}

// AddHarTexturesFilter
void AddHarTexturesFilter(ModelPointerType pModel,
                          TextureVectorType pHarList,
                          SizeType pRadius,
                          OffsetType pOff,
                          std::vector<double> minHarVect,
                          std::vector<double> maxHarVect,
                          unsigned int pBin)
{
  for (unsigned int i = 0; i < pModel->GetInputImageList()->Size(); i++)
    {
      HarTexturesFilterType::Pointer filter = HarTexturesFilterType::New();
      filter->SetRadius(pRadius);
      filter->SetOffset(pOff);
      filter->SetNumberOfBinsPerAxis(pBin);
      
      filter->SetInputImageMinimum(static_cast<SinglePixelType>(minHarVect[i]));
      filter->SetInputImageMaximum(static_cast<SinglePixelType>(maxHarVect[i]));
      filter->SetInput(pModel->GetInputImageList()->GetNthElement(i));
      
      for (unsigned int textId = 0; textId < pHarList.size(); textId++)
        {
          std::ostringstream oss;
          //oss << "Haralick Text: ";
          
          switch (pHarList[textId])
            {
            case HaralickTexture::ENERGY:
              {
                oss << "Energy: ";
                break;
              }
            case HaralickTexture::ENTROPY:
              {
                oss << "Entropy: ";
                break;
              }
            case HaralickTexture::CORRELATION:
              {
                oss << "Correlation: ";
                break;
              }
            case HaralickTexture::INERTIA:
              {
                oss << "Inertia: ";
                break;
              }
            case HaralickTexture::INVDIFMO:
              {
                oss << "Inv. Diff. Moment: ";
                break;
              }
            case HaralickTexture::CLUSPRO:
              {
                oss << "Cluster Pro.: ";
                break;
              }
            case HaralickTexture::CLUSHA:
              {
                oss << "Cluster Shade: ";
                break;
              }
            case HaralickTexture::HARCORR:
              {
                oss << "Har. Corr: ";
                break;
              }
            default:
              {
                break;
              }
            }
          
          oss << pOff << ", " << pRadius << ", " << pBin << ", min: " << minHarVect[i] <<
            ", max: " << maxHarVect[i];
          std::string mess = oss.str();
          pModel->AddFeatureFilter(filter, m_TextToHarMap[pHarList.at(textId)], i, 0, mess);
        }
    }
}
 
SingleImagePointerType GenerateHaralickTextureOutputImage(ModelPointerType pModel,
                                                          FeatureBaseType pType,
                                                          unsigned int pInputListId)
{
  SingleImagePointerType image =  SingleImageType::New();
  HarTexturesFilterType::Pointer filter =
    dynamic_cast<HarTexturesFilterType*>(static_cast<FilterType *>(
                                                                   pModel->GetFilterList()->GetNthElement(pInputListId)));
  switch (pType)
    {
    case FeatureInfoTex::TEXT_HAR_ENERGY:
      {
        image = filter->GetEnergyOutput();
        break;
      }
    case FeatureInfoTex::TEXT_HAR_ENTROPY:
      {
        image = filter->GetEntropyOutput();
        break;
      }
    case FeatureInfoTex::TEXT_HAR_CORR:
        {
          image = filter->GetCorrelationOutput();
          break;
        }
    case FeatureInfoTex::TEXT_HAR_INVDIFMO:
      {
        image = filter->GetInverseDifferenceMomentOutput();
        break;
      }
    case FeatureInfoTex::TEXT_HAR_CLUSPRO:
      {
        image = filter->GetClusterProminenceOutput();
        break;
      }
    case FeatureInfoTex::TEXT_HAR_CLUSHA:
      {
        image = filter->GetClusterShadeOutput();
        break;
      }
    case FeatureInfoTex::TEXT_HAR_HARCORR:
      {
        image = filter->GetHaralickCorrelationOutput();
        break;
      }
    case FeatureInfoTex::TEXT_HAR_INERTIA:
      {
        image = filter->GetInertiaOutput();
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
