// $Id: Flu_Tree_Browser.h,v 1.91 2004/11/05 17:03:20 jbryan Exp $

/***************************************************************
 *                FLU - FLTK Utility Widgets
 *  Copyright (C) 2002 Ohio Supercomputer Center, Ohio State University
 *
 * This file and its content is protected by a software license.
 * You should have received a copy of this license with this file.
 * If not, please contact the Ohio Supercomputer Center immediately:
 * Attn: Jason Bryan Re: FLU 1224 Kinnear Rd, Columbus, Ohio 43212
 *
 ***************************************************************/



#ifndef _otbFLUTREEBROWSER_H
#define _otbFLUTREEBROWSER_H

#include <FLU/Flu_Tree_Browser.h>
#include "otbMonteverdiEnum.h"
#include "otbMonteverdiMenu.h"

namespace otb
{
class FLU_EXPORT FluTreeBrowser : public Flu_Tree_Browser
{
 public:

  //! Normal FLTK widget constructor
  FluTreeBrowser( int x, int y, int w, int h, const char *label = 0 );

  //! Default destructor
  virtual ~FluTreeBrowser();

  typedef Flu_Tree_Browser::Node NodeType;

  //! Override of Fl_Widget::handle
  int handle( int event );
  
  MonteverdiMenu * GetModuleMenu()
    {
      return m_ModuleMenu;
    }

 protected:

 private:
  /** Module menu */
  MonteverdiMenu * m_ModuleMenu;
};
} // namespace otb

#endif
