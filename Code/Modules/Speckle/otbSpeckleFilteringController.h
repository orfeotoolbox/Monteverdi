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

#ifndef __otbSpeckleFilteringController_h
#define __otbSpeckleFilteringController_h

#include "otbSpeckleFilteringControllerInterface.h"
#include "otbSpeckleFilteringView.h"

namespace otb
{
class ITK_EXPORT SpeckleFilteringController
  : public SpeckleFilteringControllerInterface
{
public:
  /** Standard class typedefs */
  typedef SpeckleFilteringController          Self;
  typedef SpeckleFilteringControllerInterface Superclass;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::SmartPointer<const Self>       ConstPointer;

  typedef SpeckleFilteringView ViewType;
  typedef ViewType::Pointer    ViewPointerType;

  /** Standard type macros */
  itkTypeMacro(SpeckleFilteringController, Superclass);
  itkNewMacro(Self);

  void SetView(ViewPointerType pView)
  {
    m_View = pView;
  }

  /** */
  virtual void ProcessLeeFilter(unsigned int radius);
  virtual void ProcessFrostFilter(unsigned int radius, double DeRamp);

protected:
  /** Constructor */
  SpeckleFilteringController();
  /** Destructor */
  virtual ~SpeckleFilteringController();

private:
  SpeckleFilteringController(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;

};
} //end namespace otb

#endif
