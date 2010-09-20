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
#ifndef otbCurveLabel_h
#define otbCurveLabel_h

#include "otbSpectralSignatureCurve.h"
#include "otbCurveBarWidget.h"
#include "otbPixelSpectrumControllerInterface.h"

#include <FL/Fl_Color_Chooser.H>

#include <iostream>
#include <sstream>

namespace otb {

class CurveLabel : public CurveBarWidget {
public:
  typedef otb::PixelSpectrumControllerInterface  PixelSpectrumControllerType;
  typedef PixelSpectrumControllerType::Pointer   PixelSpectrumControllerPointerType;
  typedef PixelSpectrumControllerType::ColorType ColorType;
  typedef PixelSpectrumControllerType::IndexType IndexType;

  CurveLabel(int i, PixelSpectrumControllerPointerType pixSpectCtrlr, int X, int Y, int W, int H);

  virtual void CheckVisibleCallBack();
  virtual void ColorButtonCallBack();
  virtual void DeleteButtonCallBack();

private:
  std::string m_Name;
  int         m_Id;
  bool        opac;

  PixelSpectrumControllerPointerType m_PixelSpectrumController;
};

}

# endif
