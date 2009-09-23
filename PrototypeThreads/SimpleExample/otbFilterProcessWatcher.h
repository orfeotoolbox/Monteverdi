/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Some parts of this code are derived from ITK. See ITKCopyright.txt
  for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbFilterProcessWatcher_h
#define __otbFilterProcessWatcher_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Progress.H>

#include "otbFilterWatcherBase.h"

namespace otb
{

/** \class FilterProcessWatcher
 *  \brief This class implements the progress mechanism
 *         on pipeline filtering execution
 *
 */
class ITK_EXPORT FilterProcessWatcher : public FilterWatcherBase
{
public:
  /** Classes that need access to filter's private data */
  // friend class XMLFilterWatcher;

  /** Constructor. Takes a ProcessObject to monitor and an optional
   * comment string that is prepended to each event message. */
  FilterProcessWatcher(itk::ProcessObject* process,
                    int x, int y, int w,int h,
                    const char *comment="");

  /** Destructor. */
  virtual ~FilterProcessWatcher();


  /** Callback method to show the EndEvent */
  virtual void EndFilter()
  {
    m_Window->hide();
  }

protected:

  /** Callback method to show the ProgressEvent */
  virtual void ShowProgress()
  {
    if (m_Process)
    {
      m_Progress->value(m_Process->GetProgress());
      Fl::check();
    }
  }

  /** Callback method to show the StartEvent */
  virtual void StartFilter()
  {
    m_Window->show();
    m_Progress->show();
  }



private:

  Fl_Window * m_Window;
  Fl_Progress * m_Progress;
};

} // end namespace otb

#endif