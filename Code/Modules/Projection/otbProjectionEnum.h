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

#ifndef __OTBPROJECTIONENUM_H__
#define __OTBPROJECTIONENUM_H__

typedef enum { MAP_UTM, MAP_LAMBERT, MAP_LAMBERT2, MAP_SINUS, MAP_ECKERT4, MAP_TRANSMERCATOR, MAP_MOLLWEID,
               MAP_WGS84,MAP_UNKOWN }  ProjectionMapType;
typedef enum { MAP_LINEAR_, MAP_NEAREST, MAP_SINC,
               MAP_SPLINES,MAP_BCO} ProjectionInterpolatorType;

#endif
