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
#ifndef __GuessActionHandler_h
#define __GuessActionHandler_h

#include "otbImageWidgetActionHandler.h"

#include "otbMsgReporter.h"

namespace otb
{
/** \class GuessActionHandler
 *   \brief Implements lat/lon evaluator, a right click on image point center the map on this point.
 *
 *   \sa ImageWidgetController
 *   \sa ImageWidgetActionHandler
 *  \ingroup Visualization
 */
  template <class TView, class TModel>
  class GuessActionHandler : public ImageWidgetActionHandler
    {
    public:
      /** Standard class typedefs */
      typedef GuessActionHandler                      Self;
      typedef ImageWidgetActionHandler               Superclass;
      typedef itk::SmartPointer<Self>                Pointer;
      typedef itk::SmartPointer<const Self>          ConstPointer;
      
      /** Method for creation through the object factory */
      itkNewMacro(Self);
      
      /** Runtime information */
      itkTypeMacro(GuessActionHandler,ImageWidgetActionHandler);

      
      /** View typedefs */
      typedef TView                                 ViewType;
      typedef typename ViewType::Pointer            ViewPointerType;
      typedef typename ViewType::ImageWidgetType    WidgetType;
      typedef typename WidgetType::Pointer          WidgetPointerType;
      typedef typename WidgetType::IndexType        IndexType;
      typedef typename WidgetType::PointType        PointType;
      
      typedef TModel                                ModelType;
      typedef typename ModelType::Pointer           ModelPointerType;
            
      /** Handle widget event
       * \param widgetId The id of the moved widget
       * \param event The event
       * \return The handling return code
       */
      virtual bool HandleWidgetEvent(std::string widgetId, int event)
      {
        // if left mouse button pressed
        if ( Fl::event_button() == FL_RIGHT_MOUSE)
        {
          if( this->GetIsActive() )
          {
            if(m_View.IsNotNull() && m_Model.IsNotNull())
            {
              m_Widget = m_View->GetFullWidget();
              
              if(widgetId == m_Widget->GetIdentifier())
              {                 
                if (event == FL_RELEASE)
                {
                  // Get the clicked index
                  PointType screenPoint, imagePoint;
                  screenPoint = m_Widget->GetMousePosition();
                  
                  // Transform to image point
                  imagePoint = m_Widget->GetScreenToImageTransform()->TransformPoint(screenPoint);
                  try
                  {
                    m_Model->CenterMapOnSelectedPoint(static_cast<long int>(imagePoint[0]), static_cast<long int>(imagePoint[1]), 14);
                  }
                  catch (itk::ExceptionObject & err)
                  {
                    MsgReporter::GetInstance()->SendError(err.GetDescription());
                    return false;
                  }
                  return true;
                }
              }
              
              m_Widget = m_View->GetScrollWidget();
              
              if(widgetId == m_Widget->GetIdentifier())
              {                 
                if (event == FL_RELEASE)
                {
                  // Get the clicked index
                  PointType screenPoint, imagePoint;
                  screenPoint = m_Widget->GetMousePosition();
                  
                  // Transform to image point
                  imagePoint = m_Widget->GetScreenToImageTransform()->TransformPoint(screenPoint);
                  try
                  {
                    m_Model->CenterMapOnSelectedPoint(static_cast<long int>(imagePoint[0]), static_cast<long int>(imagePoint[1]), 12);
                  }
                  catch (itk::ExceptionObject & err)
                  {
                    MsgReporter::GetInstance()->SendError(err.GetDescription());
                    return false;
                  }
                  return true;
                }
              }
              m_Widget = m_View->GetZoomWidget();
              
              if(widgetId == m_Widget->GetIdentifier())
              {                 
                if (event == FL_RELEASE)
                {
                  // Get the clicked index
                  PointType screenPoint, imagePoint;
                  screenPoint = m_Widget->GetMousePosition();
                  
                  // Transform to image point
                  imagePoint = m_Widget->GetScreenToImageTransform()->TransformPoint(screenPoint);
                  try
                  {
                    m_Model->CenterMapOnSelectedPoint(static_cast<long int>(imagePoint[0]), static_cast<long int>(imagePoint[1]), 16);
                  }
                  catch (itk::ExceptionObject & err)
                  {
                    MsgReporter::GetInstance()->SendError(err.GetDescription());
                    return false;
                  }
                  return true;
                }
              }
              
            }
          }
        }
        return false;
      }
      
      /** Set/Get the pointer to the view */
      itkSetObjectMacro(View,ViewType);
      itkGetObjectMacro(View,ViewType);
      
      /** Set/Get the pointer to the model */
      itkSetObjectMacro(Model,ModelType);
      itkGetObjectMacro(Model,ModelType);
      
    protected:
      /** Constructor */
      GuessActionHandler() : m_View()
      {
      }
      
      /** Destructor */
      virtual ~GuessActionHandler(){}
      /** Printself method */
      void PrintSelf(std::ostream& os, itk::Indent indent) const
      {
        Superclass::PrintSelf(os,indent);
      }
      
    private:
      GuessActionHandler(const Self&);    // purposely not implemented
      void operator=(const Self&); // purposely not implemented
      
      // Pointer to the view
      ViewPointerType  m_View;
      
      // Pointer to the model
      ModelPointerType m_Model;
      
      // Pointer to the widget
      WidgetPointerType m_Widget;
      
    }; // end class
} // otb namespace 

#endif


