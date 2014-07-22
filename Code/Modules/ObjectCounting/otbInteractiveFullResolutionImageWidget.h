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
#ifndef __otbInteractiveFullResolutionImageWidget_h
#define __otbInteractiveFullResolutionImageWidget_h

#include "otbFixedSizeFullImageWidget.h"
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>
#include <FL/x.H>
#include <FL/Enumerations.H>

#include "otbImageWidgetCircleForm.h"
#include "otbImageWidgetPolygonForm.h"

namespace otb
{
/** \class InteractiveFullResolutionWidget
 *
 */
template <class TPixel, class TController, class TLabel = double>
class ITK_ABI_EXPORT InteractiveFullResolutionImageWidget
  : public FixedSizeFullImageWidget<TPixel>
{
public:
  /** Standard class typedefs */
  typedef InteractiveFullResolutionImageWidget Self;
  typedef FixedSizeFullImageWidget<TPixel>     Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(InteractiveFullResolutionImageWidget, FixedSizeFullImageWidget);

  typedef TController                      ControllerType;
  typedef typename ControllerType::Pointer ControllerPointerType;
  typedef typename Superclass::IndexType   IndexType;
  typedef std::vector<int>                 IntVectorType;
  typedef ImageWidgetCircleForm            CircleFormType;
  typedef CircleFormType::Pointer          CircleFormPointerType;
  typedef ImageWidgetPolygonForm<>         PolygonFormType;
  typedef PolygonFormType::Pointer         PolygonFormPointerType;

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
        IndexType clickedOldIndex;
        clickedOldIndex[0] = x;
        clickedOldIndex[1] = y;
        IndexType clickedIndex = this->WindowToImageCoordinates(clickedOldIndex);

        if (Fl::event_button1() != 0 && Fl::event_button3() == 0)
          {
          //  in case of mouse click, change the point of view
          if (m_Controller.IsNotNull())
            {
            // double click = end polygon
            if (m_LastIndexForDoubleLeft[0] == x && m_LastIndexForDoubleLeft[1] == y)
              {
              m_Controller->DoubleLeftClick(clickedIndex);
              }
            else
              {
              // If new polygon
              m_Controller->SimpleLeftClick(clickedIndex);
              m_LastIndexForDoubleLeft = clickedOldIndex;

              }
            }

          }
        else
          {
          if (Fl::event_button3())
            {
            if (m_LastIndexForDoubleRight[0] == x && m_LastIndexForDoubleRight[1] == y) m_Controller->DoubleRightClick(
                clickedIndex);
            else
              {
              m_LastIndexForDoubleRight = clickedOldIndex;
              }
            }
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
  InteractiveFullResolutionImageWidget()
  {
    m_ObjectNumber = 0;
    m_LastIndexForDoubleLeft[0] = -1;
    m_LastIndexForDoubleLeft[1] = -1;
    m_LastIndexForDoubleRight[0] = -1;
    m_LastIndexForDoubleRight[1] = -1;
    //m_LastCircle = CircleFormType::New();
    m_PolyIndex.clear();
  }
  /**
   * Destructor.
   */
  ~InteractiveFullResolutionImageWidget() {}

private:
  // Pointer to the controller
  ControllerPointerType m_Controller;
  // Displayed Polygon
  IntVectorType m_PolyIndex;
  unsigned int  m_ObjectNumber;
  IndexType     m_LastIndexForDoubleLeft;
  IndexType     m_LastIndexForDoubleRight;
};

} // end namespace otb
#endif
