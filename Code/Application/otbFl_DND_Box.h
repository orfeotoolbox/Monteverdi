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

#include "otbListenerBase.h"
#include "otbModule.h"
#include <FL/Fl_Box.H>
#include "itkMacro.h"
#include "otbFluTreeBrowser.h"

namespace otb
{
/** \class Fl_DND_Box
 *  \brief Handle Drag and Drop events
 */

class Fl_DND_Box : public Fl_Box, public EventsSender<MonteverdiEvent>
{
public:

  // Constructor
  Fl_DND_Box(int X, int Y, int W, int H, const char *L = 0)
    : Fl_Box(X, Y, W, H, L), m_DragDetected(false){}

  // Desctructor
  virtual ~Fl_DND_Box(){}

  // Override of Fl_Box::handle
  int handle(int event);

  // Get the filename list
  std::vector<std::string> GetFileNameList()
  {
    return m_FileNameList;
  }

  // Get the event
  int GetTransferedEvent()
  {
    return m_TransferedEvent;
  }

protected :
  std::vector<std::string>   m_FileNameList;
  int                        m_TransferedEvent;
  bool                       m_DragDetected;
};

}
