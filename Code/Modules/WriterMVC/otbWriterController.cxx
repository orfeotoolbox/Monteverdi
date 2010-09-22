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

#include "otbWriterController.h"
#include "otbMsgReporter.h"

namespace otb
{

WriterController
::WriterController()
{

/** NewVisu */
// Build a "visu"controller
  m_VisuController = VisuControllerType::New();
  m_ResultVisuController = VisuControllerType::New();

}

WriterController
::~WriterController()
{}

void
WriterController
::OpenInputImage(const char * filename)
{
  try
    {
    std::string strFilename = filename;
    this->ClearSelectedChannels();
    m_Model->SetInputImage(strFilename);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::SetOutputFileName(const char * name)
{
  try
    {
    m_Model->SetOutputFileName(name);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::SaveOutput(const std::string& fname, int pixelType,
             const bool useScale, const bool isWriteGeomFile)
{
  try
    {
//     m_Model->GenerateOutputImage(fname, pType, useScale);
    //Try to multi thread
    m_Model->ThreadedGenerateOutputImage(fname, pixelType,
                                         useScale, isWriteGeomFile);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::ClearFeatures()
{
  try
    {
    m_Model->ClearOutputListOrder();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

void
WriterController
::ClearSelectedChannels()
{
  try
    {
    this->ClearFeatures();
    m_Model->ClearOutputChannelsInformation();
    m_Model->ClearInputImageList();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::AddInputChannels(std::vector<unsigned int> chList)
{
  try
    {
    // channel index starts at 1
    m_Model->ClearInputImageList();
    m_Model->AddChannels(chList);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::InitInput()
{
  try
    {
    m_Model->InitInput();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::AddToOutputListOrder(int id)
{
  try
    {
    m_Model->AddToOutputListOrder(id);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::RemoveFromOutputListOrder(int id)
{
  try
    {
    m_Model->RemoveFromOutputListOrder(id);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::ExchangeOutputListOrder(int direction)
{
  int id = m_View->guiOutputFeatureList->value() - 1;
  // if switch with  next element
  if (direction > 0)
    {
    if (id != 0) m_Model->ExchangeOutputListOrder(id, id - 1);
    else m_Model->ExchangeOutputListOrder(id, m_View->guiOutputFeatureList->size() - 1);
    }
  // if switch with  previous element
  else
    {
    if (id != m_View->guiOutputFeatureList->size() - 1) m_Model->ExchangeOutputListOrder (id + 1, id);
    else m_Model->ExchangeOutputListOrder (id, 0);
    }
}

void WriterController::ViewedRegionChanged()
{
  try
    {
    m_View->UpdateFeaturePreview();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::CreateFeature()
{
  try
    {
    m_View->UpdateChannels();
    m_Model->AddFeature();
    m_View->InitFeatureOutputList();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
WriterController
::Quit()
{
  m_Model->Quit();
}
} // end namespace otb
