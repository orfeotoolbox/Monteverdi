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
#ifndef __otbObjectLabelingEventsListener_h
#define __otbObjectLabelingEventsListener_h

namespace otb
{
/**
 * \class ObjectLabelingEventsListener
 *
 */

class ObjectLabelingEventsListener
{
public:
  /** Standard class typedefs */
  typedef ObjectLabelingEventsListener Self;

  // Update the display
  virtual void Notify() = 0;

protected:
  /** Constructor */
  ObjectLabelingEventsListener() {};
  /** Destructor */
  virtual ~ObjectLabelingEventsListener() {};

private:
  ObjectLabelingEventsListener(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace otb
#endif
