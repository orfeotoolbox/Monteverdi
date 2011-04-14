/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

====================================================================*/
#include "otbPolarimetricSynthesisController.h"
#include "otbMsgReporter.h"
#include <FL/fl_ask.H>

namespace otb
{

PolarimetricSynthesisController::PolarimetricSynthesisController()
{
  // Build the widgets controller
  m_WidgetsController         = WidgetsControllerType::New();

}

PolarimetricSynthesisController::~PolarimetricSynthesisController()
{}

void PolarimetricSynthesisController::SetView(ViewPointerType view)
{
  m_View = view;
}

bool PolarimetricSynthesisController::IsHEmissionMode()
{
  return m_Model->GetHEmissionMode();
}

bool PolarimetricSynthesisController::IsVEmissionMode()
{
  return m_Model->GetVEmissionMode();
}

void PolarimetricSynthesisController::Save()
{
  try
    {
    m_Model->Save();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void PolarimetricSynthesisController::LoadImages()
{
#if 0
  m_Model->SetUseVectorImage(m_View->bOpenVectorImage->value());
  m_Model->SetHEmissionMode(m_View->bHEmission->value());
  m_Model->SetVEmissionMode(m_View->bVEmission->value());

  // Clear the model to avoid still having 4 images on the second use (...)
  m_Model->SetVectorImageFilename("");
  m_Model->SetHHImageFilename("");
  m_Model->SetHVImageFilename("");
  m_Model->SetVVImageFilename("");
  m_Model->SetVHImageFilename("");

  if (m_Model->GetUseVectorImage())
  {
    m_Model->SetVectorImageFilename(m_View->fVectorInputImage->value());
  }
  else
  {
    if (m_Model->GetHEmissionMode())
    {
      m_Model->SetHHImageFilename(m_View->fHHInputImage->value());
      m_Model->SetHVImageFilename(m_View->fHVInputImage->value());
    }
    if (m_Model->GetVEmissionMode())
    {
      m_Model->SetVVImageFilename(m_View->fVVInputImage->value());
      m_Model->SetVHImageFilename(m_View->fVHInputImage->value());
    }
  }
#endif

  try
  {
    m_Model->LoadImages();
  }
  catch (itk::ExceptionObject & err)
  {
  fl_alert("%s", err.GetDescription());
  }
}

void PolarimetricSynthesisController::ChangeRGB(bool value)
{
  m_Model->SetRGB(value);
}
void PolarimetricSynthesisController::ChangeGain(double value)
{
  m_Model->SetGain(value);
}
//------------------------- GRAYSCALE ----------------------//
void PolarimetricSynthesisController::ChangeGrayPsiI(double value)
{
  m_Model->SetGrayPsiI(value);
}
void PolarimetricSynthesisController::ChangeGrayPsiR(double value)
{
  m_Model->SetGrayPsiR(value);
}
void PolarimetricSynthesisController::ChangeGrayKhiI(double value)
{
  m_Model->SetGrayKhiI(value);
}
void PolarimetricSynthesisController::ChangeGrayKhiR(double value)
{
  m_Model->SetGrayKhiR(value);
}
void PolarimetricSynthesisController::ChangeGrayPolarizationMode(PolarizationMode mode)
{
  m_Model->SetGrayPolarizationMode(mode);
}
//------------------------- RED ----------------------//
void PolarimetricSynthesisController::ChangeRedPsiI(double value)
{
  m_Model->SetRedPsiI(value);
}
void PolarimetricSynthesisController::ChangeRedPsiR(double value)
{
  m_Model->SetRedPsiR(value);
}
void PolarimetricSynthesisController::ChangeRedKhiI(double value)
{
  m_Model->SetRedKhiI(value);
}
void PolarimetricSynthesisController::ChangeRedKhiR(double value)
{
  m_Model->SetRedKhiR(value);
}
void PolarimetricSynthesisController::ChangeRedPolarizationMode(PolarizationMode mode)
{
  m_Model->SetRedPolarizationMode(mode);
}
//------------------------- GREEN ----------------------//
void PolarimetricSynthesisController::ChangeGreenPsiI(double value)
{
  m_Model->SetGreenPsiI(value);
}
void PolarimetricSynthesisController::ChangeGreenPsiR(double value)
{
  m_Model->SetGreenPsiR(value);
}
void PolarimetricSynthesisController::ChangeGreenKhiI(double value)
{
  m_Model->SetGreenKhiI(value);
}
void PolarimetricSynthesisController::ChangeGreenKhiR(double value)
{
  m_Model->SetGreenKhiR(value);
}
void PolarimetricSynthesisController::ChangeGreenPolarizationMode(PolarizationMode mode)
{
  m_Model->SetGreenPolarizationMode(mode);
}
//------------------------- BLUE ----------------------//
void PolarimetricSynthesisController::ChangeBluePsiI(double value)
{
  m_Model->SetBluePsiI(value);
}
void PolarimetricSynthesisController::ChangeBluePsiR(double value)
{
  m_Model->SetBluePsiR(value);
}
void PolarimetricSynthesisController::ChangeBlueKhiI(double value)
{
  m_Model->SetBlueKhiI(value);
}
void PolarimetricSynthesisController::ChangeBlueKhiR(double value)
{
  m_Model->SetBlueKhiR(value);
}
void PolarimetricSynthesisController::ChangeBluePolarizationMode(PolarizationMode mode)
{
  m_Model->SetBluePolarizationMode(mode);
}

void PolarimetricSynthesisController::IndexClicked(const IndexType& index)
{
  ViewType::RegionType region = m_View->GetFullWidget()->GetViewedRegion();
  ViewType::RegionType largest = m_Model->GetOutputImage()->GetLargestPossibleRegion();
  IndexType newIndex;
  newIndex[0]=index[0]*m_Model->GetShrinkFactor()-region.GetSize()[0]/2;
  newIndex[1]=index[1]*m_Model->GetShrinkFactor()-region.GetSize()[1]/2;
  region.SetIndex(newIndex);

  if (region.GetSize()[0]>largest.GetSize()[0]
      ||region.GetSize()[1]>largest.GetSize()[1])
  {
    itkExceptionMacro("Region not inside largest region !");
  }
  else
  {
    IndexType rindex = region.GetIndex();
    ViewType::RegionType::SizeType rsize = region.GetSize();
    if (region.GetIndex()[0]<largest.GetIndex()[0])
    {
      rindex[0]=largest.GetIndex()[0];
    }
    if (region.GetIndex()[1]<largest.GetIndex()[1])
    {
      rindex[1]=largest.GetIndex()[1];
    }
    if (rindex[0]+rsize[0]>=largest.GetIndex()[0]+largest.GetSize()[0])
    {
      rindex[0]=largest.GetIndex()[0]+largest.GetSize()[0]-rsize[0];
    }
    if (rindex[1]+rsize[1]>=largest.GetIndex()[1]+largest.GetSize()[1])
    {
      rindex[1]=largest.GetIndex()[1]+largest.GetSize()[1]-rsize[1];
    }
    m_View->GetFullWidget()->SetUpperLeftCorner(rindex);
    m_View->Notify("Update");
  }
}


void PolarimetricSynthesisController::MouseIn()
{}


void PolarimetricSynthesisController::MouseOut()
{
  m_View->vPixelValue->value("");
}


void PolarimetricSynthesisController::PixelHovered(const IndexType& index)
{
  itk::OStringStream oss;
  oss.str("");
  if (m_Model->GetRGB() && m_Model->GetOutputVectorImage()->GetBufferedRegion().IsInside(index))
  {
    oss<<index<<": "<<m_Model->GetOutputVectorImage()->GetPixel(index);
  }
  else if (m_Model->GetOutputImage()->GetBufferedRegion().IsInside(index))
  {
    oss<<index<<": "<<m_Model->GetOutputImage()->GetPixel(index);
  }
  m_View->vPixelValue->value(oss.str().c_str());
}

} // end namespace otb
