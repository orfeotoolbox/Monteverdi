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
#ifndef __otbFeatureRI_h
#define __otbFeatureRI_h

#include "otbFeatureBase.h"

namespace otb
{

class FeatureInfoRI : public FeatureInfoBase
{
public:
/**
 * this enum defines the different features available.
   */
  typedef enum
    {
    UNKNOWN,
    NDVI,
    RVI,
    PVI,
    SAVI,
    MSAVI,
    MSAVI2,
    TSAVI,
    GEMI,
    WDVI,
    IPVI,
    TNDVI,
    ARVI,
    EVI,
    TSARVI,
    AVI,
    IR,
    IC,
    IB,
    IB2,
    NDBI,
    ISU,
    SRWI,
    NDWI,
    NDWI2,
    MNDWI,
    NDPI,
    NDTI,
    SPECTRALANGLE
    } FeatureType;

  FeatureInfoRI()
  {
    m_MapInfo.insert(std::pair<FeatureType, std::string>(UNKNOWN,
                                                         "Tools for classification.\nItems marked as experimental are not likely to work on huge images"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(NDVI, "Normalized difference vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(RVI, "Ratio vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(PVI, "Perpendicular vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SAVI, "Soil adjusted vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MSAVI, "Modified soil adjusted vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MSAVI2, "Modified soil adjusted vegetation index 2"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(TSAVI, "Transformed soil adjusted vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(GEMI, "Global environment monitoring index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(WDVI, "Weigthed difference modified vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(IPVI, "Infrared percentage vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(TNDVI, "Transformed normalized difference vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(ARVI, "Atmospherically resistant vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(EVI, "Enhanced vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(TSARVI,
                                                         "Transformed soil atmospherically resistant vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(AVI, "Angular vegetation index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(IR, "Redness index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(IC, "Color index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(IB, "Brightness index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(IB2, "Brightness index 2"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(NDBI, "Normalized difference built up index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(ISU, "Indice des surfaces baties"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SRWI, "Simple ratio water index"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(NDWI, "Normalized difference water index (Gao 1996)"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(NDWI2, "Normalized difference water index (Mc Feeters 1996)"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MNDWI, "Modified normalized difference water index (Xu 2006)"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(NDPI, "Normalized difference pond index (Lacaux & al)"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(NDTI, "Normalized difference turbidity index (Lacaux & al)"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(SPECTRALANGLE,
                                                         "Spectral angle image filter \nSelect reference pixel clicking over middle image"));
  }

  virtual ~FeatureInfoRI(){}

};

} // end namespace OTB
#endif
