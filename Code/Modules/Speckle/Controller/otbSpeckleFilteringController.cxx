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

#include "otbSpeckleFilteringController.h"
//#include "otbMsgReporter.h"

namespace otb
{


SpeckleFilteringController
::SpeckleFilteringController()
{

/** NewVisu */
  // Build a "visu"controller
}


SpeckleFilteringController
::~SpeckleFilteringController()
{

}

void
SpeckleFilteringController
::ProcessLeeFilter(unsigned int radius)
{

  std::cout <<"Controller : Process Lee Filter Begin ... " << std::endl;
  try
    {
      m_Model->LeeFiltering(radius );
    }
  catch(itk::ExceptionObject & err)
    {
      std::cout << err.GetDescription() << std::endl;
      //MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

  std::cout <<"Controller:  Process Lee Filter done ..." << std::endl;
}

void
SpeckleFilteringController
::ProcessFrostFilter(unsigned int radius, double DeRamp)
{
    try
    {
      m_Model->FrostFiltering(radius, DeRamp);
    }
  catch(itk::ExceptionObject & err)
    {
      std::cout << err.GetDescription() << std::endl;
      //MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}



} // end namespace otb
