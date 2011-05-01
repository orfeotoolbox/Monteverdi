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
#ifndef __otbFeatureExtractionController_h
#define __otbFeatureExtractionController_h

#include "otbFeatureExtractionControllerInterface.h"
#include "otbFeatureExtractionViewGUI.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbMouseClickActionHandler.h"

namespace otb
{
class ITK_EXPORT FeatureExtractionController
  : public FeatureExtractionControllerInterface
{
  /** Inner model class for mouse click action handler */
  class MouseClickedController : public itk::Object
  {
public:
    /** Standard class typedefs */
    typedef MouseClickedController        Self;
    typedef itk::Object                   Superclass;
    typedef itk::SmartPointer<Self>       Pointer;
    typedef itk::SmartPointer<const Self> ConstPointer;

    /** Standard type macros */
    itkTypeMacro(MouseClickedController, Superclass);
    itkNewMacro(Self);

    typedef itk::Index<> IndexType;

    void IndexClicked(ContinuousIndexType index)
    {
      if (!m_Controller.IsNotNull()) itkExceptionMacro(<< "Not Controller set.");
      if (m_MouseButton == 1) m_Controller->LeftMouseButtonClicked(index);
      else itkExceptionMacro(<< "Mouse event not supproted yet.");
    }
    itkSetClampMacro(MouseButton, int, 1, 3);
    itkGetMacro(MouseButton, int);
    itkSetClampMacro(ImageViewId, int, 0, 1);
    itkGetMacro(ImageViewId, int);
    itkSetObjectMacro(Controller, FeatureExtractionControllerInterface);
protected:
    /** Constructor */
    MouseClickedController() : m_MouseButton(1), m_ImageViewId(1), m_Controller(){}
    /** Destructor */
    virtual ~MouseClickedController(){}
    /** Set the mouse button used (1 = left, 2 = center, 3 = right) */
    int m_MouseButton;
    /** Set the associated view (0: left, 1: right) */
    int m_ImageViewId;
    /** Inner model class for mouse click action handler */
    FeatureExtractionControllerInterface::Pointer m_Controller;
  };

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

    itkSetObjectMacro(Controller, FeatureExtractionControllerInterface);

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
          if (m_Controller->GetModel()->GetResultVisuModel()->GetNumberOfLayers() > 0)
            {
            m_Controller->ShowBarStatus();
            model->Update();
            m_Controller->HideBarStatus();
            }
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
    FeatureExtractionControllerInterface::Pointer m_Controller;
  };

public:
  /** Standard class typedefs */
  typedef FeatureExtractionController          Self;
  typedef FeatureExtractionControllerInterface Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;

  typedef FeatureExtractionViewGUI          ViewType;
  typedef ViewType::Pointer                 ViewPointerType;
  typedef Superclass::InputImageType        InputImageType;
  typedef Superclass::InputImagePointerType InputImagePointerType;
  typedef ModelType::IndexType              IndexType;
  typedef ModelType::SizeType               SizeType;
  typedef ModelType::OffsetType             OffsetType;

  typedef ModelType::VisuModelType                                        VisuModelType;
  typedef ViewType::VisuViewType                                          VisuViewType;
  typedef WidgetResizingActionHandler<VisuModelType, VisuViewType>        ResizingHandlerType;
  typedef ResizingHandlerType::Pointer                                    ResizingHandlerPointerType;
  typedef ChangeExtractRegionActionHandler<VisuModelType, VisuViewType>   ChangeExtractRegionHandlerType;
  typedef ChangeExtractRegionHandlerType::Pointer                         ChangeExtractRegionHandlerPointerType;
  typedef MyChangeExtractRegionActionHandler<VisuModelType, VisuViewType> MyChangeExtractRegionHandlerType;
  typedef MyChangeExtractRegionHandlerType::Pointer                       MyChangeExtractRegionHandlerPointerType;
  typedef MouseClickActionHandler<MouseClickedController, VisuViewType>   MouseClickedHandlertype;

  typedef FeatureInfo::FeatureType         FeatureType;
  typedef HaralickTexture::TextureType     HaralickTextureType;
  typedef AdvancedTexture::TextureType     AdvancedTextureType;
  typedef std::vector<HaralickTextureType> HaralickTextureVectorType;
  typedef std::vector<AdvancedTextureType> AdvancedTextureVectorType;

/** Standard type macros */
  itkTypeMacro(FeatureExtractionController, Superclass);
  itkNewMacro(Self);

  /** Set the model */
  void SetModel(ModelType::Pointer model);

  VisuControllerPointerType GetVisuController()
  {
    return m_VisuController;
  }

  VisuControllerPointerType GetResultVisuController()
  {
    return m_ResultVisuController;
  }

  //itkSetObjectMacro(View, ViewType);
  void SetView(ViewPointerType pView);

  virtual void SaveOutput();
  virtual void InitInput();
  virtual void ClearFeatures();
  virtual void CreateFeature(FeatureType featureType);
  virtual void ClearSelectedChannels();
  virtual void SetOutputFileName(const char * name);
  virtual void AddInputChannels(std::vector<unsigned int> list);
  virtual void ChangeFilterStatus(int id);
  virtual void ViewedRegionChanged();
  virtual void AddToOutputListOrder(int id);
  virtual void RemoveFromOutputListOrder(int id);
  virtual void ExchangeOutputListOrder(int direction);
  virtual ModelType* GetModel();
  virtual void UpdateFeaturePreview(unsigned int id);

  virtual void Quit();
  virtual void Cancel();

  HaralickTextureVectorType GetHaralickTextList();
  AdvancedTextureVectorType GetAdvancedTextList();

  /** Pixel Clicked method */
  virtual void LeftMouseButtonClicked(ContinuousIndexType index);

  virtual void ShowBarStatus();
  virtual void HideBarStatus();

protected:
  /** Constructor */
  FeatureExtractionController();
  /** Destructor */
  virtual ~FeatureExtractionController();

private:
  FeatureExtractionController(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;

/** NewVisu */
  MouseClickedController::Pointer         m_MouseClickedController;
  MouseClickedHandlertype::Pointer        m_LeftMouseClickedHandler;
  VisuControllerPointerType               m_VisuController;
  VisuControllerPointerType               m_ResultVisuController;
  ResizingHandlerType::Pointer            m_ResizingHandler;
  ChangeExtractRegionHandlerPointerType   m_ChangeExtractRegionHandler;
  ResizingHandlerType::Pointer            m_ResultResizingHandler;
  MyChangeExtractRegionHandlerPointerType m_ResultChangeExtractRegionHandler;
};
} //end namespace otb

#endif
