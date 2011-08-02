/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>

#include "otbFl_DND_Box.h"

namespace otb
{
int Fl_DND_Box::handle(int e)
{
  switch(e)
    {
    case FL_DND_ENTER:
      break;

    case FL_DND_RELEASE:
    {
    return 1;
    }
    break;

    case FL_DND_LEAVE:
      break;

    case FL_DND_DRAG:
    {
    m_DragDetected = true;
    return 1;
    }
    break;

    case FL_PASTE:
    {
    // Clear previous files added to m_FileNameList
    m_FileNameList.clear();

    // make a copy of the DND payload
    std::string payload(Fl::event_text());

    // Parse the payload stream to extract all the files
    std::stringstream s (payload);
    std::string token;

    while ( s>> token )
      {
#ifdef WIN32
      // Under Win32 the files name are stored with no file://
      m_FileNameList.push_back(token);
#else
      // Remove the "file://" form the strings : subst(7,end)
      m_FileNameList.push_back(token.substr(7,token.size()));
#endif
      }

    this->NotifyAll(MonteverdiEvent("FileDropped", "Fl_DND_Box"));
    m_DragDetected = false;
    return 1;
    }
    break;

    // Cause segfault when nothing is dragged and the mouse
    // is released over the DND area
    // Problem : it deactivate the move of the scroll bar
    // using the mouse pointer. it still work via the wheel
    case FL_DRAG:
      break;

    default:
    {
    if(!m_DragDetected)
      {
      m_TransferedEvent = e;
      this->NotifyAll(MonteverdiEvent("EventTransfered", "Fl_DND_Box"));
      }
    }
    break;

    }

  return 1;
}
}
