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
#ifndef __otbFeatureSmooth_h
#define __otbFeatureSmooth_h

#include "otbFeatureBase.h"

namespace otb
{

class FeatureInfoSmooth : public FeatureInfoBase
{
public:
/**
 * this enum defines the different features available.
   */
  typedef enum
    {
    UNKNOWN,
    ORIGINAL,
    MEAN,
    MS_SMOOTH,
    MS_CLUSTERED
    } FeatureType;

  FeatureInfoSmooth()
  {
    m_MapInfo.insert(std::pair<FeatureType, std::string>(UNKNOWN,
                                                         "Tools for image smoothing and denoising.\nItems marked as experimental are not likely to work on huge images"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(ORIGINAL, "Copy input data"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MEAN, "Mean image filter"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MS_SMOOTH, "Mean shift output filter image"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MS_CLUSTERED, "Mean shift clustered image"));
  }

  virtual ~FeatureInfoSmooth(){}

};

} // end namespace OTB
#endif
