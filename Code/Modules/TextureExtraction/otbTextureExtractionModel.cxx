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
#ifndef __otbTextureExtractionModel_cxx
#define __otbTextureExtractionModel_cxx

#include "otbTextureExtractionModel.h"

#include "otbTextureFilterGenerator.h"
#include "otbSFSTexturesGenerator.h"
#include "otbHaralickTexturesGenerator.h"
#include "otbAdvancedTexturesGenerator.h"

namespace otb
{

TextureExtractionModel::TextureExtractionModel()
{
}

TextureExtractionModel::~TextureExtractionModel()
{
}

void
TextureExtractionModel
::AddTextureFilter(FeatureBaseType featureType, SizeType radius, OffsetType offset, unsigned int bin)
{
  //for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  //{
  TextureFilterGenerator generator;
  generator.GenerateTextureFilter(this, featureType, radius, offset, bin);
  //}
}

void
TextureExtractionModel
::AddSFSTexturesFilter(FeatureBaseType type,
                       double spectralThr,
                       unsigned int spatialThr,
                       unsigned int nbDirection,
                       double alpha,
                       unsigned int ratioMaxConsNb)
{
  SFSTexturesGenerator lSFSTexturesGenerator;
  lSFSTexturesGenerator.AddSFSTexturesFilter(this, type, spectralThr, spatialThr, nbDirection, alpha, ratioMaxConsNb);
}

void
TextureExtractionModel
::AddHaralickTextureFilter(HaralickTextureVectorType harList, SizeType radius, OffsetType offset, double minHar, double maxHar, unsigned int bin)
{
  HaralickTexturesGenerator lHarTexturesGenerator;
  lHarTexturesGenerator.AddHarTexturesFilter(this, harList, radius, offset, minHar, maxHar, bin);
}

void
TextureExtractionModel
::AddHaralickTextureFilter(HaralickTextureVectorType harList, SizeType radius, OffsetType offset, unsigned int bin)
{
  HaralickTexturesGenerator lHarTexturesGenerator;
  lHarTexturesGenerator.AddHarTexturesFilter(this, harList, radius, offset, bin);
}

void
TextureExtractionModel
::AddAdvancedTextureFilter(AdvancedTextureVectorType advList, SizeType radius, OffsetType offset, double minHar, double maxHar, unsigned int bin)
{
  AdvancedTexturesGenerator lAdvTexturesGenerator;
  lAdvTexturesGenerator.AddAdvTexturesFilter(this, advList, radius, offset, minHar, maxHar, bin);
}

void
TextureExtractionModel
::AddAdvancedTextureFilter(AdvancedTextureVectorType advList, SizeType radius, OffsetType offset, unsigned int bin)
{
  AdvancedTexturesGenerator lAdvTexturesGenerator;
  lAdvTexturesGenerator.AddAdvTexturesFilter(this, advList, radius, offset, bin);
}


TextureExtractionModel
::SingleImagePointerType
TextureExtractionModel
::GetSingleImage(int i)
{
  TextureFilterGenerator      lTextureGenerator;
  SFSTexturesGenerator        lSFSTexturesGenerator;
  HaralickTexturesGenerator   lHarTexturesGenerator;
  AdvancedTexturesGenerator   lAdvTexturesGenerator;
  SingleImagePointerType      image = SingleImageType::New();

  switch ((GetFilterTypeList())[i])
    {
    case FeatureInfoTex::TEXT_PANTEX:
      {
      image = lTextureGenerator.GenerateTextureOutputImage(this, (GetFilterTypeList())[i], i);
      break;
      }
    case FeatureInfoTex::TEXT_HAR_ENERGY:
    case FeatureInfoTex::TEXT_HAR_ENTROPY:
    case FeatureInfoTex::TEXT_HAR_CORR:
    case FeatureInfoTex::TEXT_HAR_INVDIFMO:
    case FeatureInfoTex::TEXT_HAR_CLUSPRO:
    case FeatureInfoTex::TEXT_HAR_CLUSHA:
    case FeatureInfoTex::TEXT_HAR_HARCORR:
    case FeatureInfoTex::TEXT_HAR_INERTIA:
      {
      image = lHarTexturesGenerator.GenerateHaralickTextureOutputImage(this, (GetFilterTypeList())[i], i);
      break;
      }
    case FeatureInfoTex::TEXT_ADV_VARIANCE:
    case FeatureInfoTex::TEXT_ADV_MEAN:
    case FeatureInfoTex::TEXT_ADV_SUMAV:
    case FeatureInfoTex::TEXT_ADV_SUMVAR:
    case FeatureInfoTex::TEXT_ADV_SUMENT:
    case FeatureInfoTex::TEXT_ADV_DIFFENT:
    case FeatureInfoTex::TEXT_ADV_DIFFVAR:
    case FeatureInfoTex::TEXT_ADV_IC1:
    case FeatureInfoTex::TEXT_ADV_IC2:
      {
      image = lAdvTexturesGenerator.GenerateAdvancedTextureOutputImage(this, (GetFilterTypeList())[i], i);
      break;
      }
    case FeatureInfoTex::SFS_LEN:
    case FeatureInfoTex::SFS_WID:
    case FeatureInfoTex::SFS_PSI:
    case FeatureInfoTex::SFS_WME:
    case FeatureInfoTex::SFS_RAT:
    case FeatureInfoTex::SFS_SD:
      {
      image = lSFSTexturesGenerator.GenerateSFSTextureOutputImage(this, (GetFilterTypeList())[i], i);
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
