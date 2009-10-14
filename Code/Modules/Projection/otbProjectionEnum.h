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

typedef enum { UTM, LAMBERT, LAMBERT2, SINUS, ECKERT4, TRANSMERCATOR, MOLLWEID}  MapType;
typedef enum { LINEAR_, NEAREST, SINC, SPLINES}  InterpolatorType;

#endif
