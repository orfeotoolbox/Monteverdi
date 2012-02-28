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
#ifndef __otbFeatureMorpho_h
#define __otbFeatureMorpho_h

#include "otbFeatureBase.h"

namespace otb
{

class FeatureInfoMorpho : public FeatureInfoBase
{
public:
/**
 * this enum defines the different features available.
   */
  typedef enum
    {
    UNKNOWN,
    MORPHOLOGICALOPENING,
    MORPHOLOGICALCLOSING
    } FeatureType;

  FeatureInfoMorpho()
  {
    m_MapInfo.insert(std::pair<FeatureType, std::string>(UNKNOWN,
                                                         "Tools for classification.\nItems marked as experimental are not likely to work on huge images"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MORPHOLOGICALOPENING, "Morphological opening"));
    m_MapInfo.insert(std::pair<FeatureType, std::string>(MORPHOLOGICALCLOSING, "Morphological closing"));
  }

  virtual ~FeatureInfoMorpho(){}

};

} // end namespace OTB
#endif
