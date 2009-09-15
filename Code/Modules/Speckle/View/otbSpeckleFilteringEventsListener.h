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

namespace otb
{
/**
 * \class SpeckleFilteringEventsListener
 *
 *  SpeckleFiltering 
 *
 */

class ITK_EXPORT SpeckleFilteringEventsListener

{
public:
  /** Standard class typedefs */
  typedef SpeckleFilteringEventsListener Self;

  itkTypeMacro(SpeckleFilteringEventsListener,itk::Object);

  // Update the display
  virtual void Notify() = 0;

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
