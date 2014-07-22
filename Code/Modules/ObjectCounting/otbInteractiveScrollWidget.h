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
#ifndef __otbInteractiveScrollWidget_h
#define __otbInteractiveScrollWidget_h

#include "otbFixedSizeFullImageWidget.h"

namespace otb
{
/** \class InteractiveScrollWidget
 *
 */
template <class TPixel, class TController, class TLabel = double>
class ITK_ABI_EXPORT InteractiveScrollWidget
  : public FixedSizeFullImageWidget<TPixel>
{
public:
  /** Standard class typedefs */
  typedef InteractiveScrollWidget          Self;
  typedef FixedSizeFullImageWidget<TPixel> Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(InteractiveScrollWidget, FixedSizeFullImageWidget);

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
      case FL_PUSH:
        {
        // in case of mouse click, change the point of view
        int       x = Fl::event_x();
        int       y = Fl::event_y();
        IndexType clickedIndex;
        clickedIndex[0] = x;
        clickedIndex[1] = y;
        clickedIndex = this->WindowToImageCoordinates(clickedIndex);
        if (m_Controller.IsNotNull())
          {
          m_Controller->IndexClicked(clickedIndex);
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
  InteractiveScrollWidget()
  {}
  /**
   * Destructor.
   */
  ~InteractiveScrollWidget()
  {}

private:
  // Pointer to the controller
  ControllerPointerType m_Controller;
};

} // end namespace otb
#endif
