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

#include "otbFeature.h"
#include "otbFeatureExtractionModel.h"
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
  typedef HaralickTexture::TextureType TextureType;
  typedef std::vector<TextureType>     TextureVectorType;

  /***************************/
  /** Filter type declaration*/
  /***************************/
  typedef ScalarImageToTexturesFilter<SingleImageType, SingleImageType> HarTexturesFilterType;

  HaralickTexturesGenerator()
  {
    m_TextToHarMap[HaralickTexture::ENERGY] = FeatureInfo::TEXT_HAR_ENERGY;
    m_TextToHarMap[HaralickTexture::ENTROPY] = FeatureInfo::TEXT_HAR_ENTROPY;
    m_TextToHarMap[HaralickTexture::CORRELATION] = FeatureInfo::TEXT_HAR_CORR;
    m_TextToHarMap[HaralickTexture::INERTIA] = FeatureInfo::TEXT_HAR_INERTIA;
    m_TextToHarMap[HaralickTexture::INVDIFMO] = FeatureInfo::TEXT_HAR_INVDIFMO;
    m_TextToHarMap[HaralickTexture::CLUSPRO] = FeatureInfo::TEXT_HAR_CLUSPRO;
    m_TextToHarMap[HaralickTexture::CLUSHA] = FeatureInfo::TEXT_HAR_CLUSHA;
    m_TextToHarMap[HaralickTexture::HARCORR] = FeatureInfo::TEXT_HAR_HARCORR;
    m_TextToHarMap[HaralickTexture::UNKNOWN] = FeatureInfo::TEXT_HAR_UNKNOWN;
  }

  virtual ~HaralickTexturesGenerator(){}

  /************/
  /** Methods */
  /************/

  // AddHarTexturesFilter
  void AddHarTexturesFilter(ModelPointerType pModel,
                            TextureVectorType pHarList,
                            SizeType pRadius,
                            OffsetType pOff,
                            unsigned int pBin)
  {
    for (unsigned int i = 0; i < pModel->GetInputImageList()->Size(); i++)
      {
      HarTexturesFilterType::Pointer filter = HarTexturesFilterType::New();
      filter->SetRadius(pRadius);
      filter->SetOffset(pOff);
      filter->SetNumberOfBinsPerAxis(pBin);

      filter->SetInputImageMinimum(static_cast<SinglePixelType>(pModel->GetSelectedMinValues()[i]));
      filter->SetInputImageMaximum(static_cast<SinglePixelType>(pModel->GetSelectedMaxValues()[i]));
      filter->SetInput(pModel->GetInputImageList()->GetNthElement(i));

      for (unsigned int textId = 0; textId < pHarList.size(); textId++)
        {
        itk::OStringStream oss;
        oss << "Haralick Text : ";

        switch (pHarList[textId])
          {
          case HaralickTexture::ENERGY:
            {
            oss << "Energy : ";
            break;
            }
          case HaralickTexture::ENTROPY:
            {
            oss << "Entropy : ";
            break;
            }
          case HaralickTexture::CORRELATION:
            {
            oss << "Correlation : ";
            break;
            }
          case HaralickTexture::INERTIA:
            {
            oss << "Inertia : ";
            break;
            }
          case HaralickTexture::INVDIFMO:
            {
            oss << "Inv. Diff. Moment : ";
            break;
            }
          case HaralickTexture::CLUSPRO:
            {
            oss << "Cluster Pro. : ";
            break;
            }
          case HaralickTexture::CLUSHA:
            {
            oss << "Cluster Shade : ";
            break;
            }
          case HaralickTexture::HARCORR:
            {
            oss << "Har. Corr : ";
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

  SingleImagePointerType GenerateHaralickTextureOutputImage(ModelPointerType pModel,
                                                            FeatureType pType,
                                                            unsigned int pInputListId)
  {
    SingleImagePointerType         image =  SingleImageType::New();
    HarTexturesFilterType::Pointer filter =
      dynamic_cast<HarTexturesFilterType*>(static_cast<FilterType *>(pModel->GetFilterList()->GetNthElement(
                                                                       pInputListId)));
    switch (pType)
      {
      case FeatureInfo::TEXT_HAR_ENERGY:
        {
        image = filter->GetEnergyOutput();
        break;
        }
      case FeatureInfo::TEXT_HAR_ENTROPY:
        {
        image = filter->GetEntropyOutput();
        break;
        }
      case FeatureInfo::TEXT_HAR_CORR:
        {
        image = filter->GetCorrelationOutput();
        break;
        }
      case FeatureInfo::TEXT_HAR_INVDIFMO:
        {
        image = filter->GetInverseDifferenceMomentOutput();
        break;
        }
      case FeatureInfo::TEXT_HAR_CLUSPRO:
        {
        image = filter->GetClusterProminenceOutput();
        break;
        }
      case FeatureInfo::TEXT_HAR_CLUSHA:
        {
        image = filter->GetClusterShadeOutput();
        break;
        }
      case FeatureInfo::TEXT_HAR_HARCORR:
        {
        image = filter->GetHaralickCorrelationOutput();
        break;
        }
      case FeatureInfo::TEXT_HAR_INERTIA:
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
