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
#ifndef __otbSpeckleFilteringEventsListener_h
#define __otbSpeckleFilteringEventsListener_h


#include "itkObject.h"

namespace otb
{
/**
 * \class SpeckleFilteringEventsListener
 *
 *  SpeckleFiltering 
 *
 */

class ITK_EXPORT SpeckleFilteringEventsListener
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef SpeckleFilteringEventsListener Self;
  typedef itk::Object                Superclass;
  typedef itk::SmartPointer<Self>    Pointer;
  typedef itk::SmartPointer<Self>    ConstPointer;

  itkTypeMacro(SpeckleFilteringEventsListener,Object);

  // Update the display
  virtual void SpeckleFilteringNotify() = 0;

protected:
  /** Constructor */
  SpeckleFilteringEventsListener() {};
  /** Destructor */
  virtual ~SpeckleFilteringEventsListener() {};

private:
  SpeckleFilteringEventsListener(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace otb
#endif
