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
#include "otbCurveLabel.h"

namespace otb {

//CurveLabel::CurveLabel(int i, std::string name, int X, int Y, int W, int H)
CurveLabel::CurveLabel(int i, PixelSpectrumControllerPointerType pixSpectCtrlr, int X, int Y, int W, int H)
  : CurveBarWidget(X, Y,  W,  H)
{
  opac = false;
  ColorType curveColor;
  IndexType index;

  m_PixelSpectrumController = pixSpectCtrlr;

  m_Id = m_PixelSpectrumController->GetIdFromPosition(i);
  curveColor = m_PixelSpectrumController->GetCurveColorByID(m_Id);
  index = m_PixelSpectrumController->GetCurveIndexByID(m_Id);

  std::ostringstream oss1, oss2;

  oss1 << "Curve ID: " << m_Id;
  m_Label->copy_label(oss1.str().c_str());

  oss2 << "[x= " << index[0] << ", y= " << index[1] << "]";
  m_Index->copy_label(oss2.str().c_str());

  m_ColorButton->color(fl_rgb_color(curveColor[0] * 255, curveColor[1] * 255, curveColor[2] * 255));
  m_CheckVisible->value(m_PixelSpectrumController->IsVisibleById(m_Id));
}

void
CurveLabel::CheckVisibleCallBack()
{
  float val = (float) m_CheckVisible->value();
  m_PixelSpectrumController->SetCurveVisibleById(m_Id, val * 0.5);
  m_PixelSpectrumController->UpdateWidgetView();
  m_PixelSpectrumController->SetVisibleById(m_Id, (int) m_CheckVisible->value());
}

void
CurveLabel::ColorButtonCallBack()
{
  unsigned char r2, g2, b2;
  double        r = 1;
  double        g = 1;
  double        b = 1;
  int           ok = fl_color_chooser("Changed class color:", r, g, b);

  if (ok)
    {
    r2 = r * 255;
    g2 = g * 255;
    b2 = b * 255;
    m_ColorButton->color(fl_rgb_color(r2, g2, b2));

    m_PixelSpectrumController->ChangeCurveColor(m_Id, (float) r, (float) g, (float) b, (float) 0.5);
    m_PixelSpectrumController->UpdateWidgetView();
    }
}

void
CurveLabel::DeleteButtonCallBack()
{
  m_PixelSpectrumController->RemoveAPixelSpectrumById(m_Id);
}

}
