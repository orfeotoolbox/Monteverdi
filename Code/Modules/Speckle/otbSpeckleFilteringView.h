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


#ifndef __otbSpeckleFilteringView_h
#define __otbSpeckleFilteringView_h

#include "otbListenerBase.h"

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include "otbSpeckleFilteringViewGUI.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "otbSpeckleFilteringControllerInterface.h"
#include  "itkObjectFactory.h"
#include  "itkObject.h"

//#include "otbImageView.h"
#include <FL/Fl_Group.H>

namespace otb
{
/** \class SpeckleFilteringView
 *
 */
class ITK_EXPORT SpeckleFilteringView
  : public ListenerBase, public SpeckleFilteringViewGUI, public itk::Object
{
public:
  /** Standard typedefs */
  typedef SpeckleFilteringView                  Self;
  typedef itk::Object                              Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(SpeckleFilteringView,itk::Object);

  //typedef ImageView<VisualizationModelType>      ImageViewType;
  typedef SpeckleFilteringControllerInterface::Pointer ControllerPointerType;

  // Update the display
  virtual void Notify();
  // Show Method
  virtual void Show();
  // Quit method
  virtual void Quit();
  // Process
  virtual void Process();
  //
  virtual void FrostFilterUpdate();
  //
  virtual void LeeFilterUpdate();

  // Method to set the controller
  itkGetObjectMacro(Controller,SpeckleFilteringControllerInterface);
  itkSetObjectMacro(Controller,SpeckleFilteringControllerInterface);

  protected:
  /** Constructor */
  SpeckleFilteringView();
  /** Destructor */
  ~SpeckleFilteringView(){};
  /** PrintSelf*/
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

 private:
  SpeckleFilteringView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ControllerPointerType    m_Controller;

  };
}//end namespace otb

#endif

