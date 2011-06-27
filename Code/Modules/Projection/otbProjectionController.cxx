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

#include "otbProjectionController.h"
#include "otbMsgReporter.h"

namespace otb
{

ProjectionController
::ProjectionController()
{
}

ProjectionController
::~ProjectionController()
{}

/**
 *
 */
void
ProjectionController
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
ProjectionController
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
ProjectionController
::UpdateUTMTransform(int zone, bool north)
{
  try
    {
    m_Model->UpdateUTMTransform(zone, north);
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
ProjectionController
::UpdateEPSGTransform(int rsidNumber)
{
  try
    {
    m_Model->UpdateEPSGTransform(rsidNumber);
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
ProjectionController
::InitializeLambertIITransform()
{
  try
    {
    m_Model->InitializeLambertIITransform();
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
ProjectionController
::UpdateTMTransform(double scale, double falseEasting, double falseNorthing)
{
  try
    {
    m_Model->UpdateTMTransform(scale, falseEasting, falseNorthing);
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
ProjectionController
::UpdateInputUTMTransform(int zone, bool north)
{
  try
    {
    m_Model->UpdateInputUTMTransform(zone, north);
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
ProjectionController
::UpdateInputEPSGTransform(int rsidNumber)
{
  try
    {
    m_Model->UpdateInputEPSGTransform(rsidNumber);
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
ProjectionController
::InitializeInputLambertIITransform()
{
  try
    {
    m_Model->InitializeInputLambertIITransform();
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
ProjectionController
::UpdateInputTMTransform(double scale, double falseEasting, double falseNorthing)
{
  try
    {
    m_Model->UpdateInputTMTransform(scale, falseEasting, falseNorthing);
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
ProjectionController
::InitializeWGS84Transform()
{
  try
    {
    m_Model->UpdateWGS84Transform();
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
ProjectionController
::SetDEMPath(std::string dem)
{

  typedef otb::DEMHandler DEMHandlerType;
  DEMHandlerType::Pointer DEMTest = DEMHandlerType::New();

  if (!DEMTest->IsValidDEMDirectory(dem.c_str()))
    {
    m_Model->SetUseDEM(false);
    MsgReporter::GetInstance()->SendError("invalid DEM directory, no DEM directory has been set.");
    }
  try
    {
    m_Model->SetDEMPath(dem);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}


} // end namespace otb
