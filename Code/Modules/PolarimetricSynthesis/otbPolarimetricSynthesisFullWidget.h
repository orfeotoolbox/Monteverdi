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
#ifndef __otbPolarimetricSynthesisFullWidget_h
#define __otbPolarimetricSynthesisFullWidget_h

#include "otbFullResolutionImageWidget.h"


namespace otb
{
/** \class PolarimetricSynthesisFullWidget
 *
 */
template <class TPixel, class TController, class TLabel = double>
class ITK_ABI_EXPORT PolarimetricSynthesisFullWidget
      : public FullResolutionImageWidget<TPixel>
{
public:
  /** Standard class typedefs */
  typedef PolarimetricSynthesisFullWidget       Self;
  typedef FullResolutionImageWidget<TPixel>  Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PolarimetricSynthesisFullWidget, FullResolutionImageWidget);

  typedef TController                      ControllerType;
  typedef typename ControllerType::Pointer ControllerPointerType;
  typedef typename Superclass::IndexType   IndexType;

  itkSetObjectMacro(Controller, ControllerType);
  itkGetObjectMacro(Controller, ControllerType);

  /** Default mode handling, without ROI selection */
  virtual int handle(int event)
  {
    switch (event)
    {
    case FL_ENTER:
    {
      if (m_Controller.IsNotNull())
      {
        m_Controller->MouseIn();
      }
      return 1;
    }
    case FL_LEAVE:
    {
      if (m_Controller.IsNotNull())
      {
        m_Controller->MouseOut();
      }
      return 1;
    }
    case FL_MOVE:
    {
      if (m_MouseMoveCount%m_MoveReportFrequency==0)
      {
        IndexType mousePos;
        mousePos[0]=Fl::event_x();
        mousePos[1]=Fl::event_y();
        IndexType index = this->WindowToImageCoordinates(mousePos);
        if (m_Controller.IsNotNull())
        {
          m_Controller->PixelHovered(index);
        }
        m_MouseMoveCount = 1;
      }
      else
      {
        ++m_MouseMoveCount;
      }
      return 1;
    }
    }
    return Superclass::handle(event);
  }

protected:
  /**
   * Constructor.
   */
  PolarimetricSynthesisFullWidget()
  {
    m_MouseMoveCount = 0;
    m_MoveReportFrequency = 5;
  }
  /**
   * Destructor.
   */
  ~PolarimetricSynthesisFullWidget()
  {}

private:
  // Pointer to the controller
  ControllerPointerType m_Controller;
  unsigned int m_MouseMoveCount;
  unsigned int m_MoveReportFrequency;
};

} // end namespace otb
#endif
