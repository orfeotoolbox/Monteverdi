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

#ifndef __otbCompareImagesController_h
#define __otbCompareImagesController_h

#include "otbCompareImagesControllerInterface.h"
#include "otbCompareImagesView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbMouseClickActionHandler.h"
#include "itkMultiThreader.h"


namespace otb
{
class ITK_EXPORT CompareImagesController
  : public CompareImagesControllerInterface
{
  
  /** Inner model class for mouse click action handler */
  template <class TModel, class TView>
  class MyChangeExtractRegionActionHandler : public ChangeExtractRegionActionHandler<TModel, TView>
  {
  public:
    //virtual bool HandleWidgetEvent(std::string widgetId, int event)
    /** Standard class typedefs */
    typedef MyChangeExtractRegionActionHandler              Self;
    typedef ChangeExtractRegionActionHandler<TModel, TView> Superclass;
    typedef itk::SmartPointer<Self>                         Pointer;
    typedef itk::SmartPointer<const Self>                   ConstPointer;

    /** Standard type macros */
    itkTypeMacro(MyChangeExtractRegionActionHandler, Superclass);
    itkNewMacro(Self);

    itkSetObjectMacro(Controller, CompareImagesControllerInterface);

    virtual bool HandleWidgetEvent(std::string widgetId, int event)
    {
      // Declare the view and model because IsNotNull() doesn't compile with the setters...
      typename TView::Pointer view = this->GetView();
      typename TModel::Pointer model = this->GetModel();

      if (view.IsNotNull() && model.IsNotNull() && this->GetIsActive())
        {
        if (widgetId == view->GetScrollWidget()->GetIdentifier()
            && event == FL_PUSH && Fl::event_button() == this->GetMouseButton())
          {
          // Get the clicked index
          typename TView::ImageWidgetType::PointType screenPoint, imagePoint;
          screenPoint = view->GetScrollWidget()->GetMousePosition();
          // Transform to image point
          imagePoint = view->GetScrollWidget()->GetScreenToImageTransform()->TransformPoint(screenPoint);
          // Transform to index
          typename TView::IndexType index;
          index[0] = static_cast<int>(imagePoint[0]);
          index[1] = static_cast<int>(imagePoint[1]);
          //Add Offset
          index += this->GetOffset();
          // Change scaled extract region center
          model->SetExtractRegionCenter(index);
          // Update model
          model->Update();
          /*if (m_Controller->GetModel()->GetRightVisuModel()->GetNumberOfLayers() > 0)
            {
            std::cout<<"juste avant update"<<std::endl;
            //m_Controller->ShowBarStatus();
            model->Update();
            std::cout<<"apres update"<<std::endl;
            //m_Controller->HideBarStatus();
            }*/
          return true;
          }
        }
      return false;
    }

  protected:
    /** Constructor */
    MyChangeExtractRegionActionHandler() : m_Controller(){}
    /** Destructor */
    virtual ~MyChangeExtractRegionActionHandler(){}
    CompareImagesControllerInterface::Pointer m_Controller;
  };
  
  
public:
  /** Standard class typedefs */
  typedef CompareImagesController          Self;
  typedef CompareImagesControllerInterface Superclass;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::SmartPointer<const Self>       ConstPointer;

  typedef CompareImagesView ViewType;
  typedef ViewType::Pointer    ViewPointerType;

  /** Widgets controller and action handlers */
  typedef CompareImagesView::VisuViewType           VisuViewType;
  typedef ModelType::VisuModelType      VisuModelType;
  typedef ModelType::PixelDescriptionModelType      PixelDescriptionModelType;


  typedef WidgetResizingActionHandler<VisuModelType, VisuViewType>            ResizingHandlerType;
  typedef ChangeExtractRegionActionHandler<VisuModelType, VisuViewType>       ChangeRegionHandlerType;
  typedef MyChangeExtractRegionActionHandler<VisuModelType, VisuViewType>     MyChangeExtractRegionHandlerType;
  typedef PixelDescriptionActionHandler<PixelDescriptionModelType, VisuViewType>   PixelDescriptionActionHandlerType;
  
  typedef ChangeScaledExtractRegionActionHandler<VisuModelType, VisuViewType>    ChangeScaledExtractRegionActionHandlerType;
  typedef ChangeScaleActionHandler<VisuModelType, VisuViewType>                  ChangeScaleActionHandlerType;


  /** Standard type macros */
  itkTypeMacro(CompareImagesController, Superclass);
  itkNewMacro(Self);


  void SetModel(ModelType* model);

  /** Set the pointer to the view */
  void SetView(CompareImagesView * view);

  virtual void ProcessCompareImagesFilter();
  virtual void SaveQuit();
  virtual ModelType* GetModel();
  
  /** Get the visu controller */
  VisuControllerType::Pointer GetLeftVisuController()
  {
    return m_LeftVisuController;
  }

  VisuControllerType::Pointer GetRightVisuController()
  {
    return m_RightVisuController;
  }

protected:
  /** Constructor */
  CompareImagesController();
  /** Destructor */
  virtual ~CompareImagesController();

private:
  CompareImagesController(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;


  /** The instance of the model */
  ModelType::Pointer m_Model;

  /** Widgets controller */
  VisuControllerType::Pointer m_LeftVisuController;
  VisuControllerType::Pointer m_RightVisuController;

  /** Action handlers */
  
  ResizingHandlerType::Pointer           m_LeftResizingHandler;
  ChangeRegionHandlerType::Pointer       m_LeftChangeRegionHandler;
  ResizingHandlerType::Pointer           m_RightResizingHandler;
  MyChangeExtractRegionHandlerType::Pointer       m_RightChangeRegionHandler;
  PixelDescriptionActionHandlerType::Pointer      m_LeftPixelActionHandler;
  PixelDescriptionActionHandlerType::Pointer      m_RightPixelActionHandler;

  
};
} //end namespace otb

#endif
