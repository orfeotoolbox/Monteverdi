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

#ifndef __otbSpeckleFilteringViewGUI_h
#define __otbSpeckleFilteringViewGUI_h

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include "otbSpeckleFilteringViewGroup.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "otbSpeckleFilteringEventsListener.h"
#include <FL/Fl_Group.H>

#include "otbImageView.h"

namespace otb
{

/** \class SpeckleFilteringViewGUI
 *
 */

class ObjectLabelingApplicationView
      : public ObjectLabelingApplicationEventsListener, 
	public ObjectLabelingApplicationGUI, 
	public itk::Object
{

public:
  /** Standard typedefs */
  typedef SpeckleFilteringViewGUI                  Self;
  typedef SpeckleFilteringEventsListener           Superclass;
  typedef itk::SmartPointer<Self>                    Pointer;
  typedef itk::SmartPointer<const Self>              ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(SpeckleFilteringViewGUI,SpeckleFilteringEventListener);

  typedef ImageView<VisualizationModelType>      ImageViewType;


  // Update the display
  virtual void SpeckleFilteringNotify();
  
  protected:
  /** Constructor */
  ObjectLabelingApplicationView();
  /** Destructor */
  ~ObjectLabelingApplicationView();
  
  private:
  ObjectLabelingApplicationView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  };
}//end namespace otb

#endif
  
