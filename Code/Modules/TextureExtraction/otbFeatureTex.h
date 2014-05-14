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
#ifndef __otbFeatureTex_h
#define __otbFeatureTex_h

#include "otbFeatureBase.h"

namespace otb
{

class FeatureInfoTex : public FeatureInfoBase
{
public:
/**
 * this enum defines the different features available.
   */
  typedef enum
    {
    UNKNOWN,
    TEXT_ASM,
    TEXT_PANTEX,
    SFS_LEN,
    SFS_WID,
    SFS_PSI,
    SFS_WME,
    SFS_RAT,
    SFS_SD,
    TEXT_HAR,
    TEXT_ADV,
    TEXT_HAR_ENERGY,
    TEXT_HAR_ENTROPY,
    TEXT_HAR_CORR,
    TEXT_HAR_INVDIFMO,
    TEXT_HAR_CLUSPRO,
    TEXT_HAR_CLUSHA,
    TEXT_HAR_INERTIA,
    TEXT_HAR_HARCORR,
    TEXT_HAR_UNKNOWN,
    TEXT_ADV_VARIANCE,
    TEXT_ADV_MEAN,
    TEXT_ADV_DISSIM,
    TEXT_ADV_SUMAV,
    TEXT_ADV_SUMVAR,
    TEXT_ADV_SUMENT,
    TEXT_ADV_DIFFENT,
    TEXT_ADV_DIFFVAR,
    TEXT_ADV_IC1,
    TEXT_ADV_IC2,
    TEXT_ADV_UNKNOWN
    } FeatureType;

  FeatureInfoTex()
  {
    m_MapInfo.insert(std::pair<FeatureType, std::string>(UNKNOWN,
                                                         "Tools for classification.\nItems marked as experimental are not likely to work on huge images"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(TEXT_PANTEX, "Panchromatic texture"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SFS_LEN, "Structural Feature Set: Length"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SFS_WID, "Structural Feature Set: Width"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SFS_PSI, "Structural Feature Set: Pixel shape index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SFS_WME, "Structural Feature Set: Weighted mean"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SFS_RAT, "Structural Feature Set: Ratio"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SFS_SD, "Structural Feature Set: Standard deviation"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(TEXT_HAR, "Haralick's textures. Select whose wanted ones."));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(TEXT_ADV, "Advanced textures. Select whose wanted ones."));

  }

  virtual ~FeatureInfoTex(){}

};

class HaralickTexture
{
public:
  typedef enum
    {
    ENERGY,   //0
    ENTROPY,
    CORRELATION,
    INVDIFMO,
    INERTIA,
    CLUSPRO,
    CLUSHA,   // 6
    HARCORR,
    UNKNOWN
    } TextureType;

  static TextureType FindTextureType(unsigned int i)
  {
    TextureType res;
    switch (i)
      {
      case 0: {
        res = ENERGY;
        break;
        }
      case 1: {
        res = ENTROPY;
        break;
        }
      case 2: {
        res = CORRELATION;
        break;
        }
      case 3: {
        res = INVDIFMO;
        break;
        }
      case 4: {
        res = INERTIA;
        break;
        }
      case 5: {
        res = CLUSPRO;
        break;
        }
      case 6: {
        res = CLUSHA;
        break;
        }
      case 7: {
        res = HARCORR;
        break;
        }
      default: {
        res = UNKNOWN;
        break;
        }
      }
    return res;
  }

  HaralickTexture(){}
  virtual ~HaralickTexture(){}
};

class AdvancedTexture
{
public:
  typedef enum
    {
    VARIANCE, //0
    MEAN,
    DISSIM,
    SUMAV,
    SUMVAR,
    SUMENT,
    DIFFENT, //7
    DIFFVAR,
    IC1,
    IC2,
    UNKNOWN
    } TextureType;

  static TextureType FindTextureType(unsigned int i)
  {
    TextureType res;
    switch (i)
      {
      case 0: {
        res = VARIANCE;
        break;
        }
      case 1: {
        res = MEAN;
        break;
        }
      case 2: {
        res = DISSIM;
        break;
       }
      case 3: {
        res = SUMAV;
        break;
        }
      case 4: {
        res = SUMVAR;
        break;
        }
      case 5: {
        res = SUMENT;
        break;
        }
      case 6: {
        res = DIFFENT;
        break;
        }
      case 7: {
        res = DIFFVAR;
        break;
        }
      case 8: {
        res = IC1;
        break;
        }
      case 9: {
        res = IC2;
        break;
        }
      default: {
        res = UNKNOWN;
        break;
        }
      }
    return res;
  }

  AdvancedTexture(){}
  virtual ~AdvancedTexture(){}
};

} // end namespace OTB
#endif
