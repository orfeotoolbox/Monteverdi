/*=========================================================================

Program:   ORFEO Toolbox
Language:  C++
Date:      $Date$
Version:   $Revision$


Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULA
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "otbSpeckleFilteringViewGUI.h"

namespace otb
{
/**
 * Constructor
 */ 
SpeckleFilteringViewGUI
::ImageViewerManagerViewGUI()
{

}

/**
 * Handle the notification of the model 
 */ 
void
SpeckleFilteringViewGUI
::SpeckleFilteringNotify()
{

}


  /**
 * PrintSelf Method
 */
void
SpeckleFilteringViewGUI
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // End namespace otb
