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
#ifndef __otbRadioButtonId_h
#define __otbRadioButtonId_h

#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl.H>
#include "otbSpectralSignatureCurve.h"
#include "otbPixelSpectrumControllerInterface.h"

#include <iostream>
#include <sstream>

namespace otb {

class RadioButtonId
: public Fl_Round_Button
{
public:
  typedef RadioButtonId                          Self;
  typedef Fl_Round_Button                        Superclass;
  typedef otb::PixelSpectrumControllerInterface  PixelSpectrumControllerType;
  typedef PixelSpectrumControllerType::Pointer   PixelSpectrumControllerPointerType;
  typedef PixelSpectrumControllerType::ColorType ColorType;
  typedef PixelSpectrumControllerType::IndexType IndexType;

  RadioButtonId(PixelSpectrumControllerPointerType pixSpectCtrlr, int x, int y, int w, int h, int id);
  ~RadioButtonId();

  void SetId(int id);
  int GetId();

private:
  void RadioButtonIdCallbackSecond();
  static void RadioButtonIdCallbackFirst(Fl_Widget* o, void* v);

  int                                m_Id;
  PixelSpectrumControllerPointerType m_PixelSpectrumController;
};
}

#endif
