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
#ifndef __otbAutomaticSegmentationMouseClickHandler_h
#define __otbAutomaticSegmentationMouseClickHandler_h

#include "otbImageWidgetActionHandler.h"

namespace otb
{
/** \class AutomaticSegmentationMouseClickHandler
*   \brief Implements clicking on widgets
*   Triggers the IndexClicked(); methods on the destination.
*
*   \sa ImageWidgetController
*   \sa ImageWidgetActionHandler
*/

template <class TModel, class TView>
class AutomaticSegmentationMouseClickHandler
  : public ImageWidgetActionHandler
{
public:
  /** Standard class typedefs */
  typedef AutomaticSegmentationMouseClickHandler   Self;
  typedef ImageWidgetActionHandler                 Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(AutomaticSegmentationMouseClickHandler,ImageWidgetActionHandler);

  /** Model typedefs */
  typedef TModel                      ModelType;
  typedef typename ModelType::Pointer ModelPointerType;

  /** View typedefs */
  typedef TView                      ViewType;
  typedef typename ViewType::Pointer ViewPointerType;

  /** Handle widget event
   * \param widgetId The id of the moved widget
   * \param event The event
   * \return The handling return code
   */
  virtual bool HandleWidgetEvent(std::string widgetId, int event)
  {
    if (m_View.IsNotNull() && m_Model.IsNotNull() && this->GetIsActive())
      {
      typename ViewType::ImageWidgetType::Pointer source;

      if (m_ActiveOnScrollWidget && widgetId == m_View->GetScrollWidget()->GetIdentifier())
        {
        source = m_View->GetScrollWidget();
        }
      else if (m_ActiveOnFullWidget && widgetId == m_View->GetFullWidget()->GetIdentifier())
        {
        source = m_View->GetFullWidget();
        }
      else if (m_ActiveOnZoomWidget && widgetId == m_View->GetZoomWidget()->GetIdentifier())
        {
        source = m_View->GetZoomWidget();
        }

      //Left Click
      if (source.IsNotNull() && event == FL_PUSH && Fl::event_button() == m_AddMouseButton && widgetId != m_View->GetScrollWidget()->GetIdentifier())
        {
        // Get the clicked index
        typename ViewType::ImageWidgetType::PointType screenPoint, imagePoint;
        screenPoint = source->GetMousePosition();
        
        // Transform to image point
        imagePoint = source->GetScreenToImageTransform()->TransformPoint(screenPoint);
        
        // Transform to index
        typename ViewType::IndexType index;
        index[0] = static_cast<int>(imagePoint[0]);
        index[1] = static_cast<int>(imagePoint[1]);
        
        // Change scaled extract region center
        m_Model->LeftIndexClicked(index, m_View->GetExtractRegionGlComponent()->GetRegion());
        
        return true;
        }
      //Right Click
      if (source.IsNotNull() && event == FL_PUSH && Fl::event_button() == m_EndMouseButton && widgetId != m_View->GetScrollWidget()->GetIdentifier())
        {
        // Get the clicked index
        typename ViewType::ImageWidgetType::PointType screenPoint, imagePoint;
        screenPoint = source->GetMousePosition();
        
        // Transform to image point
        imagePoint = source->GetScreenToImageTransform()->TransformPoint(screenPoint);
        
        // Transform to index
        typename ViewType::IndexType index;
        index[0] = static_cast<int>(imagePoint[0]);
        index[1] = static_cast<int>(imagePoint[1]);
        
        // Change scaled extract region center
        m_Model->RightIndexClicked(index, m_View->GetExtractRegionGlComponent()->GetRegion());
        return true;
        }
       /*if ((event == FL_KEYBOARD) || (event == FL_SHORTCUT))
        {
        switch (Fl::event_key())
          {
          case FL_Delete:
          {
          m_Model->DeleteGeometry();
          return true;
          break;
          }
          default:
            break;
          }
        }*/
      }
    return false;
  }

  /** Set/Get the pointer to the model */
  itkSetObjectMacro(Model, ModelType);
  itkGetObjectMacro(Model, ModelType);

  /** Set/Get the pointer to the view */
  itkSetObjectMacro(View, ViewType);
  itkGetObjectMacro(View, ViewType);
  
  /** Set/Get End polygon Mouse butto */
  itkSetMacro(EndMouseButton, int);
  itkGetMacro(EndMouseButton, int);

  /** Set/Get Add polygon Mouse butto */
  itkSetMacro(AddMouseButton, int);
  itkGetMacro(AddMouseButton, int);

  itkSetMacro(ActiveOnScrollWidget, bool);
  itkGetMacro(ActiveOnScrollWidget, bool);
  itkBooleanMacro(ActiveOnScrollWidget);

  itkSetMacro(ActiveOnFullWidget, bool);
  itkGetMacro(ActiveOnFullWidget, bool);
  itkBooleanMacro(ActiveOnFullWidget);

  itkSetMacro(ActiveOnZoomWidget, bool);
  itkGetMacro(ActiveOnZoomWidget, bool);
  itkBooleanMacro(ActiveOnZoomWidget);

protected:
  /** Constructor */
  AutomaticSegmentationMouseClickHandler() : m_View(), m_Model(), 
                                             m_ActiveOnScrollWidget(true), 
                                             m_ActiveOnFullWidget(true),
                                             m_ActiveOnZoomWidget(true),
                                             m_AddMouseButton(1),
                                             m_EndMouseButton(3)
  {}

  /** Destructor */
  virtual ~AutomaticSegmentationMouseClickHandler(){}
  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

private:
  AutomaticSegmentationMouseClickHandler(const Self&);    // purposely not implemented
  void operator =(const Self&); // purposely not implemented

  // Pointer to the view
  ViewPointerType  m_View;

  // Pointer to the model
  ModelPointerType m_Model;

  // ClickOnScroll
  bool             m_ActiveOnScrollWidget;
  bool             m_ActiveOnFullWidget;
  bool             m_ActiveOnZoomWidget;

  // Button selection 
  int              m_EndMouseButton;
  int              m_AddMouseButton;

}; // end class
} // end namespace otb
#endif
