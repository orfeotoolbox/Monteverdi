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
#ifndef __MouseMapActionHandler_h
#define __MouseMapActionHandler_h

#include "otbImageWidgetActionHandler.h"

#include <FL/Fl_Value_Input.H>

/** \class MouseMapActionHandler
 *   \brief Implements zoom on map.
 *
 *   \sa ImageWidgetController
 *   \sa ImageWidgetActionHandler
 *  \ingroup Visualization
 */
namespace otb
{
template <class TView, class TModel>
class MouseMapActionHandler : public ImageWidgetActionHandler
{
public:
  /** Standard class typedefs */
  typedef MouseMapActionHandler         Self;
  typedef ImageWidgetActionHandler      Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(MouseMapActionHandler, ImageWidgetActionHandler);

  /** View typedefs */
  typedef TView                              ViewType;
  typedef typename ViewType::Pointer         ViewPointerType;
  typedef typename ViewType::ImageWidgetType WidgetType;
  typedef typename WidgetType::Pointer       WidgetPointerType;
  typedef typename WidgetType::IndexType     IndexType;
  typedef typename WidgetType::PointType     PointType;

  typedef TModel                      ModelType;
  typedef typename ModelType::Pointer ModelPointerType;

  /** Handle widget event
   * \param widgetId The id of the moved widget
   * \param event The event
   * \return The handling return code
   */
  virtual bool HandleWidgetEvent(const std::string & widgetId, int event)
  {
    // if left mouse button pressed
    if (Fl::event_button() == FL_RIGHT_MOUSE)
      {
      // Reset double click var
      m_DoubleClickStart = false;
      m_SecondClick = false;

      if (this->GetIsActive())
        {
        if (m_View.IsNotNull() && m_Model.IsNotNull())
          {
          m_Widget = m_View->GetFullWidget();

          if (widgetId == m_Widget->GetIdentifier())
            {
            if (event == FL_PUSH)
              {
              // Get the clicked index
              PointType screenPoint, imagePoint;
              screenPoint = m_Widget->GetMousePosition();

              // Transform to image point
              imagePoint = m_Widget->GetScreenToImageTransform()->TransformPoint(screenPoint);
              m_Model->SetSelectedPoint(static_cast<long int>(imagePoint[0]), static_cast<long int>(imagePoint[1]));

              return true;
              }
            }
          }
        }
      return false;
      }
    else if (Fl::event_button() == FL_LEFT_MOUSE)
      {
      if (this->GetIsActive())
        {
        if (m_View.IsNotNull() && m_Model.IsNotNull())
          {
          m_Widget = m_View->GetFullWidget();

          if (widgetId == m_Widget->GetIdentifier())
            {
            // Get the clicked index
            PointType screenPoint, imagePoint;
            screenPoint = m_Widget->GetMousePosition();

            // Transform to image point
            imagePoint = m_Widget->GetScreenToImageTransform()->TransformPoint(screenPoint);

            if (event == FL_PUSH)
              {
              m_StartIndex[0] = static_cast<long int>(imagePoint[0]);
              m_StartIndex[1] = static_cast<long int>(imagePoint[1]);

              // If its first click
              if (m_DoubleClickStart == false)
                {
                // Set first click and index
                m_DoubleClickStart = true;
                m_DoubleClickIndex[0] = m_StartIndex[0];
                m_DoubleClickIndex[1] = m_StartIndex[1];
                }
              else
                {
                // Set second click
                m_SecondClick = true;
                }

              return true;
              }
            if (event == FL_RELEASE)
              {
              // Get end index
              IndexType endIndex;
              endIndex[0] = static_cast<long int>(imagePoint[0]);
              endIndex[1] = static_cast<long int>(imagePoint[1]);

              // Get moveX and moveY for drag and drop
              long int moveX = m_StartIndex[0] - endIndex[0];
              long int moveY = m_StartIndex[1] - endIndex[1];

              if (moveX != 0 && moveY != 0)
                {
                // run DragMap Function
                m_Model->DragMap(moveX, moveY);
                return true;
                }

              // Get moveX and moveY for doubleClick
              moveX = m_DoubleClickIndex[0] - endIndex[0];
              moveY = m_DoubleClickIndex[1] - endIndex[1];

              // Double click
              if (m_DoubleClickStart && m_SecondClick && moveX == 0 && moveY == 0)
                {
                m_Model->IncreaseDepth(1, m_StartIndex[0], m_StartIndex[1]);
                m_DoubleClickStart = false;
                m_SecondClick = false;
                return true;
                }
              }
            }
          }
        }
      return false;
      }
    return false;
  }

  /** Set/Get the pointer to the view */
  itkSetObjectMacro(View, ViewType);
  itkGetObjectMacro(View, ViewType);

  /** Set/Get the pointer to the model */
  itkSetObjectMacro(Model, ModelType);
  itkGetObjectMacro(Model, ModelType);

protected:
  /** Constructor */
  MouseMapActionHandler() : m_View()
  {
    m_DoubleClickStart = false;
    m_SecondClick = false;
  }

  /** Destructor */
  virtual ~MouseMapActionHandler(){}
  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

private:
  MouseMapActionHandler(const Self&);        // purposely not implemented
  void operator =(const Self&);    // purposely not implemented

  // Double click
  bool m_DoubleClickStart;
  bool m_SecondClick;

  // Pointer to the view
  ViewPointerType m_View;

  // Pointer to the model
  ModelPointerType m_Model;

  // Pointer to the widget
  WidgetPointerType m_Widget;

  // Start point
  IndexType m_StartIndex;
  IndexType m_DoubleClickIndex;

};     // end class
} // otb namespace

#endif
