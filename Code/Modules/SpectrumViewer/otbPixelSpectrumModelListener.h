/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbPixelSpectrumModelListener_h
#define __otbPixelSpectrumModelListener_h

namespace otb
{
/**
 * \class PixelSpectrumModelListener
 * \brief todo
 *
 *  \ingroup Visualization
 */

class PixelSpectrumModelListener
{
public:
  /** Standard class typedefs */
  typedef PixelSpectrumModelListener Self;

  // Update the display
  virtual void Notify() = 0;

protected:
  /** Constructor */
  PixelSpectrumModelListener()
    {
    }
  /** Destructor */
  virtual ~PixelSpectrumModelListener() {}

private:
  PixelSpectrumModelListener(const Self &); //purposely not implemented
  void operator = (const Self &); //purposely not implemented
};

} // end namespace otb
#endif
