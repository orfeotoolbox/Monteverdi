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
::SpeckleFilteringViewGUI()
{
  // Create the component of the GUI
  this->CreateGUI();
}


/**
 * Handle the notification of the model 
 */ 
void
SpeckleFilteringViewGUI
::Notify()
{

}

/**
 * Show the GUI
 */ 
void
SpeckleFilteringViewGUI
::Show()
{
  guiMainWindow->show();
}

/**
 * Quit method 
 */ 
void
SpeckleFilteringViewGUI
::Quit()
{
  guiMainWindow->hide();
}

/**
 * Method process
 */
void 
SpeckleFilteringViewGUI
::Process()
{
  
  if( guiFilterType->value() == 0) 
    m_Controller->ProcessFrostFilter(guiFrostRadius->value(),guiLeeDeRamp->value());
  else
    if(guiFilterType->value() == 1)
      m_Controller->ProcessLeeFilter(guiLeeRadius->value());
}

/**
 * Frost Filter Update
 */
void 
SpeckleFilteringViewGUI
::FrostFilterUpdate()
{
  guiLeeGroup->hide();
  guiFrostGroup->set_visible();
  guiFrostGroup->activate();
}

/**
 * Lee Filter Update
 */
void 
SpeckleFilteringViewGUI
::LeeFilterUpdate()
{
  guiFrostGroup->hide();
  guiLeeGroup->set_visible();
  guiLeeGroup->activate();
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

} // End namespac
