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
#ifndef __otbFeatureBase_h
#define __otbFeatureBase_h

#include <string>
#include <map>

namespace otb
{

class FeatureInfoBase
{
public:

  typedef unsigned int FeatureType;

  FeatureInfoBase()
  {
  }

  virtual ~FeatureInfoBase(){}

  typedef std::map<FeatureType, std::string> MapType;

  MapType GetMapInfo()
  {
    return m_MapInfo;
  }

protected:
  MapType m_MapInfo;
};

} // end namespace OTB
#endif
