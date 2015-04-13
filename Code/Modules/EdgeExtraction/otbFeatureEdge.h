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
#ifndef __otbFeatureEdge_h
#define __otbFeatureEdge_h

#include "otbFeatureBase.h"

namespace otb
{

class FeatureInfoEdge : public FeatureInfoBase
{
public:
/**
 * this enum defines the different features available.
   */
  typedef enum
    {
    UNKNOWN,
    VARIANCE,
    GRADIENT,
    HARRIS,
    EDGE_CANNY,
    EDGE_SOBEL,
    TOUZI
    } FeatureType;

  FeatureInfoEdge()
  {
    m_MapInfo.insert(std::pair<FeatureType, std::string>(UNKNOWN,
                                                         "Tools for edge detection.\nItems marked as experimental are not likely to work on huge images"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(HARRIS, "Harris image filter"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(VARIANCE, "Variance image filter"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(GRADIENT, "Recursive gradient magnitude image filter"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(EDGE_SOBEL, "Edge density filter (with Sobel edge detector)"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(EDGE_CANNY, "Edge density filter (with Canny edge detector)"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(TOUZI, "Touzi image filter"));
  }

  virtual ~FeatureInfoEdge(){}

};

} // end namespace OTB
#endif
