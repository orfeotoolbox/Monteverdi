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
#include "otbRadioButtonId.h"

namespace otb {

RadioButtonId
::RadioButtonId(PixelSpectrumControllerPointerType pixSpectCtrlr, int x, int y, int w, int h, int id)
  : Superclass(x, y, w, h)
{
  std::ostringstream oss;

  m_Id = id;
  m_PixelSpectrumController = pixSpectCtrlr;

  if (m_Id == -1)
    {
    oss << "None";
    value(1);
    }
  else
    {
    oss << "Curve ID: " << m_Id;
    }

  this->copy_label(oss.str().c_str());

  //callback:
  this->callback((Fl_Callback *) RadioButtonId::RadioButtonIdCallbackFirst, (void *) this);
  this->color((Fl_Color) 55);
}

RadioButtonId
::~RadioButtonId()
{
}

void
RadioButtonId
::SetId(int id)
{
  m_Id = id;
}

int
RadioButtonId
::GetId()
{
  return m_Id;
}

void
RadioButtonId
::RadioButtonIdCallbackSecond()
{
}

void
RadioButtonId
::RadioButtonIdCallbackFirst(Fl_Widget* o, void* v)
{
  Self * pthis = static_cast<Self *>(v);
  pthis->RadioButtonIdCallbackSecond();
}

}
