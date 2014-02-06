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
#include "otbPixelSpectrumModel.h"
#include "otbMacro.h"

#include "itkTimeProbe.h"

namespace otb
{

unsigned int PixelSpectrumModel::NoneID = itk::NumericTraits<unsigned int>::max();

PixelSpectrumModel
::PixelSpectrumModel()
 : m_PixelSpectrum(""),
  m_AxisLengthX(0),
  m_AxisLengthY(0),
  m_CurentCurveAlpha(0),
  m_MinValue(0),
  m_MaxValue(0),
  m_GridStepX(0),
  m_GridStepY(0),
  m_GridOriginX(0),
  m_GridOriginY(0),
  m_GridOriginXTranslate(0),
  m_GridOriginYTranslate(0),
  m_AutomaticGridStep(false),
  m_Id(0)
{
}

PixelSpectrumModel
::~PixelSpectrumModel()
{
  m_SeveralSpectrumData.clear();
}

void
PixelSpectrumModel
::ClearPixelSpectrum()
{
  m_PixelSpectrum = "";
  m_AxisLengthY = 255;
}

//---------------------------------

void
PixelSpectrumModel
::DetermineMinMaxPixelValues()
{

  // Report pixel info for each visible layer
  for (Superclass::LayerIteratorType it = this->GetLayers()->Begin(); it != this->GetLayers()->End(); ++it)
    {

    for (unsigned int j = 0; j < it.Get()->GetMinValues().Size(); j++)
      {

      if (it.Get()->GetMinValues().GetElement(j) < m_MinValue)
        {
        m_MinValue = it.Get()->GetMinValues().GetElement(j);
        }

      if (it.Get()->GetMaxValues().GetElement(j) > m_MaxValue)
        {
        m_MaxValue = it.Get()->GetMaxValues().GetElement(j);
        }
      }
    }
}

PixelSpectrumModel::SeveralSpectrumsDataType
PixelSpectrumModel
::GetSeveralSpectrumData()
{
  return m_SeveralSpectrumData;
}

void
PixelSpectrumModel
::UpdateCurrentPixelSpectrum(const IndexType& index)
{

  // Report pixel info for each visible layer
  for (Superclass::LayerIteratorType it = this->GetLayers()->Begin(); it != this->GetLayers()->End(); ++it)
    {
    // If the layer is visible
    if (it.Get()->GetVisible()) m_CurrentPixelSpectrum = it.Get()->GetValueAtIndex(index);
    break;
    }

  // Notify all listeners
  this->NotifyAll();
}

bool
PixelSpectrumModel
::CheckIfAlreadyExistingSprectrum(const IndexType& index)
{
  bool exists = false;

  for (SeveralSpectrumsDataType::iterator it =  m_SeveralSpectrumData.begin(); it != m_SeveralSpectrumData.end(); ++it)
    {
    if (it->coord == index)
      {
      exists = true;
      break;
      }
    }

  return exists;
}

// add a pixel spectrum to the vector (in order to display the whole vector into the widget)
unsigned int
PixelSpectrumModel
::AddAPixelSpectrum(const IndexType& index)
{
  // Report pixel info for each visible layer
  for (Superclass::LayerIteratorType it = this->GetLayers()->Begin(); it != this->GetLayers()->End(); ++it)
    {
    // If the layer is visible
    if (it.Get()->GetVisible())
      {
      SpectrumAndIdVectorType svi;
      svi.sprectrum = it.Get()->GetValueAtIndex(index);
      svi.id = m_Id;
      svi.coord = index;
      svi.visible = true;
      m_Id++;
      //m_SeveralSpectrumData.push_back(it.Get()->GetPixelSpectrum(index));
      m_SeveralSpectrumData.push_back(svi);
      break;
      }
    }

  // Notify all listeners
  this->NotifyAll();
  return m_Id;
}

int
PixelSpectrumModel
::RemoveAPixelSpectrumById(int i)
{

  int size = m_SeveralSpectrumData.size();

  for (int j = 0; j < size; j++)
    {
    if (m_SeveralSpectrumData.at(j).id == i)
      {
      m_SeveralSpectrumData.erase(m_SeveralSpectrumData.begin() + j);
      break;
      }
    }

  // Notify all listeners
  // this->NotifyAll();

  return i;
}

int
PixelSpectrumModel
::IsVisibleById(int id)
{

  int size = m_SeveralSpectrumData.size();
  int visible = false;

  for (int j = 0; j < size; j++)
    {
    if (m_SeveralSpectrumData.at(j).id == id)
      {
      visible = m_SeveralSpectrumData.at(j).visible;
      break;
      }
    }

  // Notify all listeners
  // this->NotifyAll();

  return visible;
}

void
PixelSpectrumModel
::SetVisibleById(int id, int v)
{
  int size = m_SeveralSpectrumData.size();

  for (int j = 0; j < size; j++)
    {
    if (m_SeveralSpectrumData.at(j).id == id)
      {
      m_SeveralSpectrumData.at(j).visible = v;
      break;
      }
    }
}

void
PixelSpectrumModel
::RemoveAllSpectrums()
{
  // If the layer is visible
  if (m_SeveralSpectrumData.size() > 0) m_SeveralSpectrumData.clear();

  // Notify all listeners
  this->NotifyAll();
}

void
PixelSpectrumModel
::SetYAxisLenghtControl(float value, bool direct)
{
  if (!direct)
    {
    if (m_AxisLengthY <= 1)
      {
      m_AxisLengthY = 1.2;
      }
    else
      {
      m_AxisLengthY *= value;
      }
    }
  else
    {
    m_AxisLengthY = value;
    }
}

int
PixelSpectrumModel
::GetIdFromPosition(int i)
{
  int n = m_SeveralSpectrumData.at(i).id;
  m_SeveralSpectrumData.at(i).id;
  return n;
}

PixelSpectrumModel::IndexType
PixelSpectrumModel
::GetSpectrumIndexByID(int id)
{
  IndexType ret;
  ret.Fill(0);
  int       size = m_SeveralSpectrumData.size();

  for (int i = 0; i < size; i++)
    {
    if (m_SeveralSpectrumData.at(i).id == id)
      {
      ret = m_SeveralSpectrumData.at(i).coord;
      }
    }
  return ret;
}

PixelSpectrumModel::SpectrumVectorType
PixelSpectrumModel
::GetSpectrumByID(int id)
{
  SpectrumVectorType ret;
  unsigned int       size = m_SeveralSpectrumData.size();

  for (unsigned int i = 0; i < size; i++)
    {
    if (m_SeveralSpectrumData.at(i).id == id)
      {
      ret = m_SeveralSpectrumData.at(i).sprectrum;
      }
    }
  return ret;
}

void
PixelSpectrumModel
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

void
PixelSpectrumModel
::NotifyListener(ListenerType * listener)
{
  listener->Notify();
}

} // end namespace otb
