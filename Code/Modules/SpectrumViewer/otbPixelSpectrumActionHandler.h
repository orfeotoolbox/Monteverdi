/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbPixelSpectrumActionHandler_h
#define __otbPixelSpectrumActionHandler_h

#include "otbImageWidgetActionHandler.h"
#include <sstream>

namespace otb
{
/** \class PixelSpectrumActionHandler
 *  \brief Implements pixel reporting capabilities.
 *
 *  \sa ImageWidgetController
 *  \sa ImageWidgetActionHandler
 *  \ingroup Visualization
 */

template <class TController, class TView>
class PixelSpectrumActionHandler
: public ImageWidgetActionHandler
{
public:
  /** Standard class typedefs */
  typedef PixelSpectrumActionHandler    Self;
  typedef ImageWidgetActionHandler      Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(PixelSpectrumActionHandler, WidgetActionHandler);

  /** Controller typedefs */
  typedef TController                        ControllerType;
  typedef typename ControllerType::Pointer   ControllerPointerType;
  typedef typename ControllerType::IndexType IndexType;

  /** View typedef */
  typedef TView                                     ViewType;
  typedef typename ViewType::Pointer                ViewPointerType;
  typedef typename ViewType::OffsetType             OffsetType;
  typedef typename ViewType::ImageWidgetPointerType WidgetPointerType;

  /** Handle widget event
   */
  virtual bool HandleWidgetEvent(std::string widgetId, int event)
  {
    if (m_Controller.IsNotNull() &&  m_View.IsNotNull() && this->GetIsActive())
      {
      // Find the source widget
      WidgetPointerType sourceWidget;
      bool              handle = false;
      if (widgetId == m_View->GetScrollWidget()->GetIdentifier())
        {
        sourceWidget = m_View->GetScrollWidget();
        handle = true;
        }
      else if (widgetId == m_View->GetFullWidget()->GetIdentifier())
        {
        sourceWidget = m_View->GetFullWidget();
        handle = true;
        }
      else if (widgetId == m_View->GetZoomWidget()->GetIdentifier())
        {
        sourceWidget = m_View->GetZoomWidget();
        handle = true;
        }
      if (handle)
        {

        switch (event)
          {
          case FL_ENTER:
            {
            return true;
            break;
            }
          case FL_LEAVE:
            {
            return true;
            break;
            }
          case FL_MOVE:
            {
            // Get the clicked index
            typename ViewType::ImageWidgetType::PointType screenPoint, imagePoint;
            screenPoint = sourceWidget->GetMousePosition();

            // Transform to image point
            imagePoint = sourceWidget->GetScreenToImageTransform()->TransformPoint(screenPoint);

            // Transform to index
            typename ViewType::IndexType index;
            index[0] = static_cast<int>(imagePoint[0]);
            index[1] = static_cast<int>(imagePoint[1]);

            //Add the offsetAddAPixelSpectrum
            index += m_Offset;

            // Communicate new index to controller
            m_Controller->UpdateCurrentPixelSpectrum(index);

            std::ostringstream oss;
            oss << "( x: " << index[0] << " ; y: " << index[1] << " )";
            m_Controller->SetViewIndex(oss.str());

            return true;
            break;
            }

          case FL_RELEASE:
            {
            // Get the clicked index
            typename ViewType::ImageWidgetType::PointType screenPoint, imagePoint;
            screenPoint = sourceWidget->GetMousePosition();

            // Transform to image point
            imagePoint = sourceWidget->GetScreenToImageTransform()->TransformPoint(screenPoint);

            // Transform to index
            typename ViewType::IndexType index;
            index[0] = static_cast<int>(imagePoint[0]);
            index[1] = static_cast<int>(imagePoint[1]);

            //Add the offset
            index += m_Offset;

            // Communicate new index to controller

            switch (Fl::event_button())
              {
              case FL_RIGHT_MOUSE:
                m_Controller->AddAPixelSpectrum(index);
                break;

              case FL_MIDDLE_MOUSE:
                m_Controller->RemoveAllSpectrums();
                break;
              default:;
              }
            return true;
            break;
            }

          // Hit some key

          case 12:
            {
            switch (Fl::event_key())
              {
              // the "-" key
              case 65453:
                m_Controller->SetYAxisLenghtControl(1.2, false);
                break;

              // the "+" key
              case 65451:
                m_Controller->SetYAxisLenghtControl(1 / (1.2), false);

                break;

              // the "h" key (for "hide")
              case 104:
                m_Controller->SetCurentCurveAlpha(0);
                break;
              // the "s" key (for "show")
              case 115:
                m_Controller->SetCurentCurveAlpha(1);
                break;

              // 1
              case 38:
                m_Controller->SetAutomaticGridStep(false);
                m_Controller->SetGridStepY(0.1);
                break;

              // 2
              case 233:
                m_Controller->SetAutomaticGridStep(false);
                m_Controller->SetGridStepY(10);
                break;

              // 3
              case 34:
                m_Controller->SetAutomaticGridStep(false);
                m_Controller->SetGridStepY(50);
                break;

              // 4
              case 39:
                m_Controller->SetAutomaticGridStep(false);
                m_Controller->SetGridStepY(100);
                break;

              // 5
              case 40:
                m_Controller->SetAutomaticGridStep(false);
                m_Controller->SetGridStepY(500);
                break;

              // a
              case 97:
                m_Controller->SetAutomaticGridStep(true);
                break;

              // UP
              case 65362:
                m_Controller->SetGridOriginYTranslate(m_Controller->GetGridOriginYTranslate() - 5);
                break;

              // DOWN
              case 65364:
                m_Controller->SetGridOriginYTranslate(m_Controller->GetGridOriginYTranslate() + 5);
                break;

              // LEFT
              case 65361:
                m_Controller->SetGridOriginXTranslate(m_Controller->GetGridOriginXTranslate() + 5);
                break;

              // RIGHT
              case 65363:
                m_Controller->SetGridOriginXTranslate(m_Controller->GetGridOriginXTranslate() - 5);
                break;

              default:
                break;

              }

            return true;
            break;
            }

          default:
            {
            break;
            }
          }
        }
      }
    return false;
  }

  /** Set/Get the pointer to the controller */
  itkSetObjectMacro(Controller, ControllerType);
  itkGetObjectMacro(Controller, ControllerType);

  /** Set/Get the pointer to the view */
  itkSetObjectMacro(View, ViewType);
  itkGetObjectMacro(View, ViewType);

  /** Set/Get Offset */
  itkSetMacro(Offset, OffsetType);
  itkGetMacro(Offset, OffsetType);

protected:
  /** Constructor */
  PixelSpectrumActionHandler() : m_View(), m_Controller()
    {
    m_Offset.Fill(0);
    }

  /** Destructor */
  virtual ~PixelSpectrumActionHandler(){}
  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
    {
      Superclass::PrintSelf(os, indent);
    }

private:
  PixelSpectrumActionHandler(const Self &);    // purposely not implemented
  void                                   operator = (const Self &); // purposely not implemented

  //Offset
  OffsetType m_Offset;

  // Pointer to the view
  ViewPointerType m_View;

  // Pointer to the Controller
  ControllerPointerType m_Controller;
}; // end class
} // end namespace otb
#endif
