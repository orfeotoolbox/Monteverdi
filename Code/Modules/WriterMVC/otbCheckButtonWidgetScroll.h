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

=========================================================================*/
#ifndef __otbCheckButtonWidgetScroll_h
#define __otbCheckButtonWidgetScroll_h


#include "otbWidgetScroll.h"
#include "FL/Fl_Check_Button.h"

namespace otb
{
class AdaptedCheckButton : public Fl_Check_Button
{
public:
  typedef Fl_Check_Button Superclass;
  AdaptedCheckButton(int X, int Y, int W, int H, const char*L=0) : Fl_Check_Button(X,Y,W,H,L) {};
  virtual ~AdaptedCheckButton() {};
  virtual int handle(int event)
  {
    switch (event)
    {
    case FL_ENTER:
    {
      return 1;
    }
    case FL_LEAVE:
    {
      return 1;
    }
    case FL_RELEASE:
    {
      if (value()==1)
        value(0);
      else
        value(1);
      return 1;
    }
    }
  }
};




class CheckButtonWidgetScroll : public WidgetScroll<Fl_Check_Button>
{

public:
  typedef AdaptedCheckButton             InternalWidgetType;
  typedef CheckButtonWidgetScroll        Self;
  typedef WidgetScroll <Fl_Check_Button> Superclass;
  typedef Superclass::StringVectorType   StringVectorType;
  typedef std::vector<int>               CheckedBoxListType;

  CheckButtonWidgetScroll(int X, int Y, int W, int H, const char*L=0) : WidgetScroll<Fl_Check_Button>(X,Y,W,H,L)
  {
  };
  virtual ~CheckButtonWidgetScroll() {};

  CheckedBoxListType GetCheckedBoxList()
  {
    this->ComputeCheckedBoxList();
    return m_CheckedBoxList;
  };

  void ComputeCheckedBoxList()
  {
    // number of tile childre

    for (unsigned int i = 0; i<static_cast<Fl_Tile *>(static_cast<Fl_Scroll *>(child(0))->child(0))->children(); i++)
    {
      if (static_cast<Fl_Check_Button *>(static_cast<Fl_Tile *>(static_cast<Fl_Scroll *>(child(0))->child(0))->child(i))->value()==1)
      {
        m_CheckedBoxList.push_back(i);
      }
    }

  }

protected:
  CheckedBoxListType m_CheckedBoxList;

private:
};

}// End namespace otb

#endif
