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
#ifndef __otbPixelType_h
#define __otbPixelType_h

#include <string>
#include <map>

namespace otb
{
/**
 * this enum defines the different Pixel type available.
   */
typedef enum
{
  UNSIGNEDCHAR, //0
  SHORTINT,
  INT,
  FLOAT,
  DOUBLE,
  UNSIGNEDSHORTINT,
  UNSIGNEDINT
} PixelType;

}
#endif
