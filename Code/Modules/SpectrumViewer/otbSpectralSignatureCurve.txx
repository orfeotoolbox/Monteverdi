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
#ifndef __otbSpectralSignatureCurve_txx
#define __otbSpectralSignatureCurve_txx

#include "otbSpectralSignatureCurve.h"

namespace otb
{

template <class TSpectralSignature>
SpectralSignatureCurve<TSpectralSignature>
::SpectralSignatureCurve() : m_SpectralCurve(), m_SpectralCurveColor()
{
  // Default spectralCurve color
  m_SpectralCurveColor.Fill(0.5);
}

template <class TSpectralSignature>
SpectralSignatureCurve<TSpectralSignature>
::~SpectralSignatureCurve()
{}

template <class TSpectralSignature>
void
SpectralSignatureCurve<TSpectralSignature>
::SetTransparency(float t)
{
  m_SpectralCurveColor[3] = t;
}

template <class TSpectralSignature>
void
SpectralSignatureCurve<TSpectralSignature>
::Render(const RegionType& extent, const AffineTransformType * space2ScreenTransform)
{
  // Temporary variables
  PointType           spacePoint, screenPoint;
  ContinuousIndexType cindex;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4d(m_SpectralCurveColor[0], m_SpectralCurveColor[1], m_SpectralCurveColor[2], m_SpectralCurveColor[3]);

  glLineWidth(1.5);
  glEnable(GL_LINE_SMOOTH);
  glBegin(GL_LINE_STRIP);

  unsigned int i = 0;
  while (i < m_SpectralCurve.Size())
    {

    // Render each bin
    spacePoint[0] = i;
    spacePoint[1] = m_SpectralCurve[i];

    // Transform to screen
    screenPoint = space2ScreenTransform->TransformPoint(spacePoint);

    glVertex2d(screenPoint[0], screenPoint[1]);

    i++;

    }
  glEnd();
  glDisable(GL_BLEND);
  glLineWidth(1);
  glDisable(GL_LINE_SMOOTH);
}

template <class TSpectralSignature>
void
SpectralSignatureCurve<TSpectralSignature>
::BeforeRendering()
{
  InternPxlType min, max;

  int numberOfChannel = m_SpectralCurve.GetSize();

  max = 0;
  min = 0;
  if (m_SpectralCurve.Size() > 0)
    {
    max = m_SpectralCurve[0];
    min = m_SpectralCurve[0];
    }

  for (int i = 1; i < numberOfChannel; i++)
    {
    if (m_SpectralCurve[i] < min)
      {
      min = m_SpectralCurve[i];
      }
    if (m_SpectralCurve[i] > max)
      {
      max = m_SpectralCurve[i];
      }
    }

  m_Minimum[0] = 0;
  m_Minimum[1] = min;

  m_Maximum[0] = numberOfChannel;
  m_Maximum[1] = max;
}

template <class TSpectralSignature>
typename SpectralSignatureCurve<TSpectralSignature>
::PointType
SpectralSignatureCurve<TSpectralSignature>
::GetMinimum()
{
  return m_Minimum;
}

template <class TSpectralSignature>
typename SpectralSignatureCurve<TSpectralSignature>
::PointType
SpectralSignatureCurve<TSpectralSignature>
::GetMaximum()
{
  return m_Maximum;
}

}
#endif
