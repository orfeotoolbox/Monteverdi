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
  TEXT_CSH,
  TEXT_CPR,
  TEXT_CON,
  TEXT_COR, //40
  TEXT_DEN,
  TEXT_DVA,
  TEXT_ENJ,
  TEXT_ENT,
  TEXT_IC1,
  TEXT_IC2,
  TEXT_IDM,
  TEXT_MEA,
  TEXT_PANTEX,
  TEXT_SAV, //50
  TEXT_SEN,
  TEXT_SVA,
  TEXT_VAR,
  SFS_LEN,
  SFS_WID,
  SFS_PSI,
  SFS_WME,
  SFS_RAT,
  SFS_SD, 
  EDGE_CANNY, //60
  EDGE_SOBEL,
  MS_SMOOTH,
  MS_CLUSTERED,
  MS_LABELED,
  MS_BOUND,
  UNKNOWN 
} FeatureType;


class FeatureInfo
  {
public:
 FeatureInfo()
   {
     m_MapInfo.insert( std::pair<FeatureType, std::string>(UNKNOWN, "Tools for classification.\nItems marked as experimental are not likely to work on huge images.") );  // 0
     m_MapInfo.insert( std::pair<FeatureType, std::string>(ORIGINAL, "Copy Input Data.") ); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TOUZI, "Touzi Image Filter.")); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(HARRIS, "Harris Image Filter."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(VARIANCE, "Variance Image Filter."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SPECTRALANGLE, "Spectral Angle Image Filter \nSelect reference pixel clicking over middle image"));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MEAN, "Mean Image Filter."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(GRADIENT, "Recursive Gradient Magnitude Image Filter."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_ASM, "Haralick's Texture: Angular 2nd Moment."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_CSH, "Haralick's Texture: Cluster Shade."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_CPR, "Haralick's Texture: Cluster Prominence.")); //10
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_CON, "Haralick's Texture: Contrast."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_COR, "Haralick's Texture: Correlation."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_DEN, "Haralick's Texture: Difference Entropy."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_DVA, "Haralick's Texture: Difference Variance."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_ENJ, "Haralick's Texture: Energy."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_ENT, "Haralick's Texture: Entropy."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_IC1, "Haralick's Texture: Information Correlation 1."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_IC2, "Haralick's Texture: Information Correlation 2."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_IDM, "Haralick's Texture: Inverse Difference Moment."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_MEA, "Haralick's Texture: Mean.")); //20
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_PANTEX, "Haralick's Texture: Panchromatic Texture."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_SAV, "Haralick's Texture: Sum Average."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_SEN, "Haralick's Texture: Sum Entropy."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_SVA, "Haralick's Texture: Sum Variance."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TEXT_VAR, "Haralick's Texture: Variance."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_LEN, "Structural Feature Set : Length."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_WID, "Structural Feature Set : Width."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_PSI, "Structural Feature Set : Pixel Shape Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_WME, "Structural Feature Set : Weighted mean."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_RAT, "Structural Feature Set : Ratio.")); //30
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SFS_SD, "Structural Feature Set : Standard Deviation.")); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MORPHOLOGICALOPENING, "Morphological Opening."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MORPHOLOGICALCLOSING, "Morphological Closing."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDVI, "Normalized Difference Vegetation Index.")); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(RVI, "Ratio Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(PVI, "Perpendicular Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SAVI, "Soil Adjusted Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MSAVI, "Modified Soil Adjusted Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MSAVI2, "Modified Soil Adjusted Vegetation Index 2."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TSAVI, "Transformed Soil Adjusted Vegetation Index.")); //40
     m_MapInfo.insert( std::pair<FeatureType, std::string>(GEMI, "Global Environment Monitoring Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(WDVI, "Weigthed Difference Modified Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IPVI, "Infrared Percentage Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TNDVI, "Transformed Normalized Difference Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(ARVI, "Atmospherically Resistant Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(EVI, "Enhanced Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(TSARVI, "Transformed Soil Atmospherically Resistant Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(AVI, "Angular Vegetation Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IR, "Redness Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IC, "Color Index.")); // 50
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IB, "Brightness Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(IB2, "Brightness Index 2."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDBI, "Normalized Difference Built up Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(ISU, "Indice des Surfaces Baties."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(SRWI, "Simple Ratio Water Index."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDWI, "Normalized Difference Water Index (Gao 1996)."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDWI2, "Normalized Difference Water Index (Mc Feeters 1996)."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MNDWI, "Modified Normalized Difference Water Index (Xu 2006)."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDPI, "Normalized Difference Pond Index (Lacaux & al)."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(NDTI, "Normalized Difference Turbidity Index (Lacaux & al).")); //60
     m_MapInfo.insert( std::pair<FeatureType, std::string>(EDGE_SOBEL, "Edge Density Filter (with Sobel Edge Detector)."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(EDGE_CANNY, "Edge Density Filter (with Canny Edge Detector).")); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_SMOOTH, "Mean Shift Output Filter Image."));
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_CLUSTERED, "Mean Shift Clustered Image.")); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_LABELED, "Mean Shift Labeled Image.")); 
     m_MapInfo.insert( std::pair<FeatureType, std::string>(MS_BOUND, "Mean Shift Cluster Boundaries Image.")); 
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

}
#endif
