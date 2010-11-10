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

#include "otbDEMToImageGeneratorController.h"
#include "otbMsgReporter.h"

namespace otb
{

DEMToImageGeneratorController
::DEMToImageGeneratorController()
{
}

DEMToImageGeneratorController
::~DEMToImageGeneratorController()
{}

/**
 *
 */
void
DEMToImageGeneratorController
::ReprojectImage()
{
  try
    {
    m_Model->ReprojectImage();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

/**
 *
 */
void
DEMToImageGeneratorController
::ProjectRegion(unsigned int sizeX,
                unsigned int sizeY,
                double spacingX,
                double spacingY,
                double originX,
                double originY)
{
  try
    {
    m_Model->ProjectRegion(sizeX, sizeY, spacingX, spacingY, originX, originY);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

/**
 *
 */
void
DEMToImageGeneratorController
::SetDEMDirectoryPath(const char* DEMDirectory)
{
  try
    {
    m_Model->SetDEMDirectoryPath(DEMDirectory);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

/**
 *
 */
void
DEMToImageGeneratorController
::ProcessHillShading(double azimutAngle,double elevationAngle,unsigned int radius)
{
  try
    {
    m_Model->ProcessHillShading( azimutAngle, elevationAngle, radius);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

/**
 *
 */
void
DEMToImageGeneratorController
::ProcessColorRelief()
{
  try
    {
    m_Model->ProcessColorRelief( );
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}



} // end namespace otb
