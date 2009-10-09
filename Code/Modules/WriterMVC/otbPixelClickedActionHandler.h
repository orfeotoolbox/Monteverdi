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
#ifndef __otbPixelClickedActionHandler_h
#define __otbPixelClickedActionHandler_h

#include "otbImageWidgetActionHandler.h"

namespace otb
{
/** \class PixelClickedActionHandler
*   \brief Implements basic Scroll, Full and Zoom  widgets resizing.
* 
*   \sa ImageWidgetController
*   \sa ImageWidgetActionHandler
*/

template <class TModel, class TView> 
class PixelClickedActionHandler
  : public ImageWidgetActionHandler
{
public:
  /** Standard class typedefs */
  typedef PixelClickedActionHandler Self;
  typedef ImageWidgetActionHandler               Superclass;
  typedef itk::SmartPointer<Self>                Pointer;
  typedef itk::SmartPointer<const Self>          ConstPointer;
  
  /** Method for creation through the object factory */
  itkNewMacro(Self);
  
  /** Runtime information */
  itkTypeMacro(PixelClickedActionHandler,ImageWidgetActionHandler);

  /** Model typedefs */
  typedef TModel                           ModelType;
  typedef typename ModelType::Pointer      ModelPointerType;
 
  /** View typedefs */
  typedef TView                            ViewType;
  typedef typename ViewType::VisuModelType VisuModelType;
  typedef typename ViewType::VisuViewType  VisuViewType;
  typedef typename ViewType::Pointer       ViewPointerType;

  /** Handle widget event
   * \param widgetId The id of the moved widget
   * \param event The event
   * \return The handling return code
   */
  virtual bool HandleWidgetEvent(std::string widgetId, int event)
  { 
    if( m_View.IsNotNull() && m_Model.IsNotNull() && m_View->GetVisuView().IsNotNull() &&  m_Model->GetVisuModel().IsNotNull() && m_Model->GetHasInput() )
      {
       if(widgetId == m_View->GetVisuView()->GetFullWidget()->GetIdentifier() && event == FL_PUSH)
         {
           otbMsgDevMacro(<<"PixelClickedActionHandler::HandleWidgetEvent(): handling ("<<widgetId<<", "<<event<<")");
           
           // Get the clicked index
           typename VisuViewType::ImageWidgetType::PointType screenPoint, imagePoint;
           screenPoint = m_View->GetVisuView()->GetScrollWidget()->GetMousePosition();
           
           // Transform to image point
           imagePoint = m_View->GetVisuView()->GetScrollWidget()->GetScreenToImageTransform()->TransformPoint(screenPoint);
           
           // Transform to index
           typename VisuViewType::IndexType index;
           index[0]=static_cast<int>(imagePoint[0]);
           index[1]=static_cast<int>(imagePoint[1]);
           
           // Change scaled extract region center
           m_View->UpdateSelectedPixel(index);
       
           return true;
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
  PixelClickedActionHandler() : m_View(), m_Model()
  {}

  /** Destructor */
  virtual ~PixelClickedActionHandler(){}
  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  }
 
private:
  PixelClickedActionHandler(const Self&);    // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  // Pointer to the view
  ViewPointerType m_View;
  
  // Pointer to the model
  ModelPointerType m_Model;
  
}; // end class 
} // end namespace otb
#endif


