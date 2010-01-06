/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbOpticCalibrationEnum_h
#define __otbOpticCalibrationEnum_h


namespace otb
{
/**
 * this enum defines the different images available.
 */
typedef enum
{
  INPUT_IMAGE,
  LUMINANCE,
  REFLECTANCE_TOA,
  REFLECTANCE_TOC,
  DIFF_REFL
}
EnumImageType;


}// namespace otb
#endif
