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


#include "otbSpeckleFilteringView.h"

namespace otb
{
/**
 * Constructor
 */
SpeckleFilteringView
::SpeckleFilteringView()
{
  // Create the component of the GUI
  this->CreateGUI();
}


/**
 * Handle the notification of the model
 */
void
SpeckleFilteringView
::Notify()
{

}

/**
 * Show the GUI
 */
void
SpeckleFilteringView
::Show()
{
  guiMainWindow->show();
}

/**
 * Quit method
 */
void
SpeckleFilteringView
::Quit()
{
  guiMainWindow->hide();
}

/**
 * Method process
 */
void
SpeckleFilteringView
::Process()
{

  if( guiFilterType->value() == 0)
    m_Controller->ProcessFrostFilter(static_cast<unsigned int>(guiFrostRadius->value()),guiLeeDeRamp->value());
  else
    if(guiFilterType->value() == 1)
      m_Controller->ProcessLeeFilter(static_cast<unsigned int>(guiLeeRadius->value()));
  guiMainWindow->hide();
}

/**
 * Frost Filter Update
 */
void
SpeckleFilteringView
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
SpeckleFilteringView
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
SpeckleFilteringView
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // End namespac
