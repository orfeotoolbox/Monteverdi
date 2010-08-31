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
#include "otbPixelSpectrumController.h"

namespace otb {
void
PixelSpectrumController
::SetCurentCurveAlpha(float a)
{
  m_Model->SetCurentCurveAlpha(a);
}

void
PixelSpectrumController
::DetermineMinMaxPixelValues()
{
  m_Model->DetermineMinMaxPixelValues();
}

void
PixelSpectrumController
::SetAxisLengthAtInit(double x, double y)
{
  m_Model->SetAxisLengthX(x);
  m_Model->SetAxisLengthY(y);
}

void
PixelSpectrumController
::RemoveAPixelSpectrumById(int i)
{
  m_Model->RemoveAPixelSpectrumById(i);
  m_View->UpdateCurvesList(i);
}

int
PixelSpectrumController
::GetIdFromPosition(int i)
{
  return m_Model->GetIdFromPosition(i);
}

PixelSpectrumController::ColorType
PixelSpectrumController
::GetCurveColorByID(int i)
{

  return m_View->GetCurveById(i)->GetSpectralCurveColor();
}

PixelSpectrumController::IndexType
PixelSpectrumController
::GetCurveIndexByID(int i)
{
  IndexType index;
  index[0] = m_View->GetCurveById(i)->GetCoord()[0];
  index[1] = m_View->GetCurveById(i)->GetCoord()[1];

  return index;
}

void
PixelSpectrumController
::SetXGridStep(float x)
{
  m_Model->SetGridStepX(x);

}

void
PixelSpectrumController
::SetYGridStep(float y)
{
  m_Model->SetGridStepY(y);
  UpdateWidgetView();
}

float
PixelSpectrumController
::GetYGridStep()
{
  return m_Model->GetGridStepY();
}

void
PixelSpectrumController
::SetGridOriginX(double x)
{
  m_Model->SetGridOriginX(x);
}

void
PixelSpectrumController
::SetGridOriginY(double y)
{
  m_Model->SetGridOriginY(y);
}

void
PixelSpectrumController
::SetGridOriginXTranslate(double x)
{
  m_Model->SetGridOriginXTranslate(x);
}

void
PixelSpectrumController
::SetGridOriginYTranslate(double y)
{
  m_Model->SetGridOriginXTranslate(y);
}

void
PixelSpectrumController
::SetCurveVisibleById(int id, float alpha)
{
  m_View->SetCurveVisibleById(id, alpha);
}

void
PixelSpectrumController
::UpdateWidgetView()
{
  m_View->Update();
}

void
PixelSpectrumController
::UpdateHCIView()
{
  m_View->UpdateHCIDisplay();
}

void
PixelSpectrumController
::ChangeCurveColor(int id, float r, float g, float b, float a)
{
  m_View->ChangeCurveColor(id, r, g, b, a);
}

int
PixelSpectrumController
::IsVisibleById(int id)
{
  return m_Model->IsVisibleById(id);
}

void
PixelSpectrumController
::SetVisibleById(int id, int v)
{
  m_Model->SetVisibleById(id, v);
}

void
PixelSpectrumController
::UpdateCurrentPixelSpectrum(IndexType index)
{
  m_Model->UpdateCurrentPixelSpectrum(index);
}

void PixelSpectrumController
::RemoveAllSpectrums()
{
  m_Model->RemoveAllSpectrums();
}

void
PixelSpectrumController
::AddAPixelSpectrum(IndexType index)
{
  if (!m_Model->CheckIfAlreadyExistingSprectrum(index))
    m_Model->AddAPixelSpectrum(index);
}

void PixelSpectrumController
::SetYAxisLenghtControl(float f, bool direct)
{
  m_Model->SetYAxisLenghtControl(f, direct);
  UpdateWidgetView();
}

float PixelSpectrumController
::GetYAxisLenghtControl()
{
  return m_Model->GetAxisLengthY();
}

void PixelSpectrumController
::SetAutomaticGridStep(bool b)
{
  m_Model->SetAutomaticGridStep(b);
  UpdateWidgetView();
}

void PixelSpectrumController
::SetGridStepY(double f)
{
  m_Model->SetGridStepY(f);
}

void PixelSpectrumController
::SetGridStepX(double f)
{
  m_Model->SetGridStepX(f);
}

double PixelSpectrumController
::GetGridOriginYTranslate()
{
  return m_Model->GetGridOriginYTranslate();
}

double PixelSpectrumController
::GetGridOriginXTranslate()
{
  return m_Model->GetGridOriginXTranslate();
}

void PixelSpectrumController
::SetViewIndex(std::string s)
{
  m_View->GetIndexBox()->value(s.c_str());
}

}
