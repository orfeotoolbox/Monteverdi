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
#ifndef __otbMSBoundaryFunctor_h
#define __otbMSBoundaryFunctor_h

namespace otb
{
namespace Functor
{

/** 
 * \class MSBoundaryFunctor
 * 
 * \brief Functor to extract segmentation boundaries
 * 
 * Functor intended to work with 3x3 neighborhood and scalar label image
 * 
 */
template < class TInput, class TOutput >
class MSBoundaryFunctor
{
public:
  MSBoundaryFunctor() { }
  virtual ~MSBoundaryFunctor() { }

  TOutput operator() ( const TInput & input )
  {
    unsigned char output = 1;
    if (input.GetCenterPixel() == input.GetPixel(5) &&
        input.GetCenterPixel() == input.GetPixel(7) &&
        input.GetCenterPixel() == input.GetPixel(8))
      {
      output = 0;
      }
    return static_cast<TOutput>(output);
  }
}; // end of class

} // end of Functor namespace
} // end of otb namespace

#endif
