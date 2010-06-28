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
#ifndef __otbFeature_h
#define __otbFeature_h

#include <string>
#include <map>

namespace otb
{

class FeatureInfo
  {
public:
/**
 * this enum defines the different features available.
   */
typedef enum
{
  TOUZI, //0
  HARRIS,
  NDVI,
  RVI,
  ARVI,
  EVI,
  PVI,
  SAVI,
  MSAVI,
  MSAVI2,
  TSAVI,//10
  TSARVI,
  GEMI,
  AVI,
  WDVI,
  IPVI,
  TNDVI,
  IR,
  IC,
  IB,
  IB2, //20
  NDBI,
  ISU,
  SRWI,
  NDWI,
  NDWI2,
  MNDWI,
  NDPI,
  NDTI,
  SPECTRALANGLE,
  MORPHOLOGICALOPENING,//30
  MORPHOLOGICALCLOSING,
  VARIANCE,
  MEAN,
  GRADIENT,
  ORIGINAL,
  TEXT_ASM,
  //TEXT_CSH,
  //TEXT_CPR,
  //TEXT_CON,
  //TEXT_COR, //40
  //TEXT_DEN,
  //TEXT_DVA,
  //TEXT_ENJ,
  //TEXT_ENT,
  //TEXT_IC1,
  //TEXT_IC2,
  //TEXT_IDM,
  //TEXT_MEA,
  TEXT_PANTEX,
  //TEXT_SAV, //50
  //TEXT_SEN,
  //TEXT_SVA,
  //TEXT_VAR,
  SFS_LEN,
  SFS_WID,
  SFS_PSI, 
  SFS_WME, // 40
  SFS_RAT,
  SFS_SD,
  EDGE_CANNY,
  EDGE_SOBEL,
  MS_SMOOTH,
  MS_CLUSTERED,
  MS_LABELED,
  MS_BOUND,
  TEXT_HAR, 
  TEXT_ADV, // 50
  TEXT_HAR_ENERGY,
  TEXT_HAR_ENTROPY,
  TEXT_HAR_CORR,
  TEXT_HAR_INVDIFMO,
  TEXT_HAR_CLUSPRO,
  TEXT_HAR_CLUSHA,
  TEXT_HAR_INERTIA,
  TEXT_HAR_HARCORR,
  TEXT_HAR_UNKNOWN, 
  TEXT_ADV_VARIANCE,  // 60
  TEXT_ADV_MEAN,
  TEXT_ADV_SUMAV,
  TEXT_ADV_SUMVAR,
  TEXT_ADV_SUMENT,
  TEXT_ADV_DIFFENT,
  TEXT_ADV_DIFFVAR,
  TEXT_ADV_IC1,
  TEXT_ADV_IC2,
  TEXT_ADV_UNKNOWN,
  UNKNOWN  // 70
} FeatureType;

 FeatureInfo()
   {
     m_MapInfo.insert( std::pair<FeatureType, std::string>(UNKNOWN, "Tools for classification.\nItems marked as experimental are not likely to work on huge images") );  // 0
     m_MapInfo.insert( std::pair<FeatureType, std::string>(ORIGINAL, "Copy input data") );
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TOUZI, "Touzi image filter"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(HARRIS, "Harris image filter"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(VARIANCE, "Variance image filter"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SPECTRALANGLE, "Spectral angle image filter \nSelect reference pixel clicking over middle image"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MEAN, "Mean image filter"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(GRADIENT, "Recursive gradient magnitude image filter"));
     /*
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_ASM, "Haralick's texture: Angular 2nd moment"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_CSH, "Haralick's texture: Cluster shade"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_CPR, "Haralick's texture: Cluster prominence")); //10
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_CON, "Haralick's texture: Contrast"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_COR, "Haralick's texture: Correlation"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_DEN, "Haralick's texture: Difference entropy"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_DVA, "Haralick's texture: Difference variance"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_ENJ, "Haralick's texture: Energy"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_ENT, "Haralick's texture: Entropy"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_IC1, "Haralick's texture: Information correlation 1"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_IC2, "Haralick's texture: Information correlation 2"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_IDM, "Haralick's texture: Inverse difference moment"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_MEA, "Haralick's texture: Mean")); //20
     */
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_PANTEX, "Panchromatic texture"));
     /*
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_SAV, "Haralick's texture: Sum average"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_SEN, "Haralick's texture: Sum entropy"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_SVA, "Haralick's texture: Sum variance"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_VAR, "Haralick's texture: Variance"));
     */
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_LEN, "Structural Feature Set: Length"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_WID, "Structural Feature Set: Width"));  // 10
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_PSI, "Structural Feature Set: Pixel shape index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_WME, "Structural Feature Set: Weighted mean"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_RAT, "Structural Feature Set: Ratio")); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_SD, "Structural Feature Set: Standard deviation"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MORPHOLOGICALOPENING, "Morphological opening"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MORPHOLOGICALCLOSING, "Morphological closing"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDVI, "Normalized difference vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(RVI, "Ratio vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(PVI, "Perpendicular vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SAVI, "Soil adjusted vegetation index"));  // 20
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MSAVI, "Modified soil adjusted vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MSAVI2, "Modified soil adjusted vegetation index 2"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TSAVI, "Transformed soil adjusted vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(GEMI, "Global environment monitoring index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(WDVI, "Weigthed difference modified vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IPVI, "Infrared percentage vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TNDVI, "Transformed normalized difference vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(ARVI, "Atmospherically resistant vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(EVI, "Enhanced vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TSARVI, "Transformed soil atmospherically resistant vegetation index")); // 30
     m_MapInfo.insert( std::pair<FeatureType, std::string>(AVI, "Angular vegetation index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IR, "Redness index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IC, "Color index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IB, "Brightness index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IB2, "Brightness index 2"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDBI, "Normalized difference built up index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(ISU, "Indice des surfaces baties"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SRWI, "Simple ratio water index"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDWI, "Normalized difference water index (Gao 1996)"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDWI2, "Normalized difference water index (Mc Feeters 1996)")); // 40
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MNDWI, "Modified normalized difference water index (Xu 2006)"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDPI, "Normalized difference pond index (Lacaux & al)"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDTI, "Normalized difference turbidity index (Lacaux & al)"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(EDGE_SOBEL, "Edge density filter (with Sobel edge detector)"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(EDGE_CANNY, "Edge density filter (with Canny edge detector)"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_SMOOTH, "Mean shift output filter image"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_CLUSTERED, "Mean shift clustered image"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_LABELED, "Mean shift labeled image"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_BOUND, "Mean shift cluster boundaries image"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_HAR, "Haralick's textures. Select whose wanted ones.")); // 50
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_ADV, "Advanced textures. Select whose wanted ones."));
   };


 virtual ~FeatureInfo(){};

 typedef std::map<FeatureType, std::string> MapType;

 MapType GetMapInfo()
   {
     return m_MapInfo;
   };


 private:
 MapType m_MapInfo;
};



class HaralickTexture
{
 public:
  typedef enum
    {
      ENERGY, //0
      ENTROPY,
      CORRELATION,
      INVDIFMO,
      INERTIA,
      CLUSPRO,
      CLUSHA, // 6
      HARCORR,
      UNKNOWN
    } TextureType;
  
static TextureType FindTextureType(unsigned int i)
{
  TextureType res;
  switch(i)
    {
    case 0: {
      res = ENERGY;
      break; }
    case 1: {
      res = ENTROPY;
      break; }
    case 2: {
      res = CORRELATION;
      break; }
    case 3: {
      res = INVDIFMO;
      break; }
    case 4: {
      res = INERTIA;
      break; }
    case 5: {
      res = CLUSPRO;
      break; }
    case 6: {
      res = CLUSHA;
      break; }
    case 7: {
      res = HARCORR;
      break; }
    default: {
      res = UNKNOWN;
      break; }
    }
  return res;
}

  HaralickTexture(){};
  virtual ~HaralickTexture(){};
};

class AdvancedTexture
{
 public:
typedef enum
{
  VARIANCE, //0
  MEAN,
  SUMAV,
  SUMVAR,
  SUMENT,
  DIFFENT, //6
  DIFFVAR,
  IC1,    
  IC2,
  UNKNOWN
} TextureType;

static TextureType FindTextureType(unsigned int i)
{
  TextureType res;
  switch(i)
    {
    case 0: {
      res = VARIANCE;
      break; }
    case 1: {
      res = MEAN;
      break; }
    case 2: {
      res = SUMAV;
      break; }
    case 3: {
      res = SUMVAR;
      break; }
    case 4: {
      res = SUMENT;
      break; }
    case 5: {
      res = DIFFENT;
      break; }
    case 6: {
      res = DIFFVAR;
      break; }
    case 7: {
      res = IC1;
      break; }
    case 8: {
      res = IC2;
      break; }
    default: {
      res = UNKNOWN;
      break; }
    }
  return res;
}

 AdvancedTexture(){};
 virtual ~AdvancedTexture(){};
};


}// end namespace OTB
#endif
