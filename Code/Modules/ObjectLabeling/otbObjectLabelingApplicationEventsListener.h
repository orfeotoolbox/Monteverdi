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
#ifndef __otbObjectLabelingApplicationEventsListener_h
#define __otbObjectLabelingApplicationEventsListener_h

namespace otb
{
/**
 * \class ObjectLabelingApplicationEventsListener
 *
 */

class ObjectLabelingApplicationEventsListener
{
public:
  /** Standard class typedefs */
  typedef ObjectLabelingApplicationEventsListener Self;

  // Update the display
  virtual void Notify() = 0;

protected:
  /** Constructor */
  ObjectLabelingApplicationEventsListener() {};
  /** Destructor */
  virtual ~ObjectLabelingApplicationEventsListener() {};

private:
  ObjectLabelingApplicationEventsListener(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace otb
#endif
