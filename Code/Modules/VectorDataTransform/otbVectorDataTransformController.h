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
#ifndef __otbVectorDataTransformController_h
#define __otbVectorDataTransformController_h

#include "otbVectorDataTransformControllerInterface.h"
#include "otbVectorDataTransformView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"

namespace otb
{
class VectorDataTransformController
      : public VectorDataTransformControllerInterface
{
public:
  /** Standard class typedefs */
  typedef VectorDataTransformController     Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  /** Standard type macros */
  itkTypeMacro(VectorDataTransformController,Superclass);
  itkNewMacro(Self);

  typedef VectorDataTransformModel ModelType;

  /** Widgets controller and action handlers */
  typedef VectorDataTransformView::ImageViewType              ImageViewType;
  typedef VectorDataTransformModel::VisualizationModelType    VisualizationModelType;
  typedef VectorDataTransformModel::PixelDescriptionModelType PixelDescriptionModelType;
  
  typedef ImageWidgetController                                 WidgetsControllerType;
  typedef WidgetResizingActionHandler 
    <VisualizationModelType,ImageViewType>                      ResizingHandlerType;
  typedef ChangeScaledExtractRegionActionHandler
  <VisualizationModelType,ImageViewType>                        ChangeScaledRegionHandlerType;
  typedef ChangeExtractRegionActionHandler
  <VisualizationModelType,ImageViewType>                        ChangeRegionHandlerType;
  typedef ChangeScaleActionHandler
  <VisualizationModelType,ImageViewType>                        ChangeScaleHandlerType;
  typedef otb::PixelDescriptionActionHandler
  < PixelDescriptionModelType, ImageViewType>                   PixelDescriptionActionHandlerType;
  
  /** User action */
  virtual void Exit();
  virtual void ApplyTransformToVectorData(double scale, double angle, 
                                          double translationX, 
                                          double translationY,
                                          double centerX, double centerY);
  
  /** Set the pointer to the view */
  void SetView(VectorDataTransformView * view);
 
   virtual void SetModel(ModelType * model)
  {
    VectorDataTransformControllerInterface::SetModel(model);
    
    // Register the model to the action handlers
    m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeScaleHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel());
    m_PixelDescriptionHandler->SetModel(m_Model->GetPixelDescriptionModel());
  }

  /** Get the widgets controller */
  itkGetObjectMacro(WidgetsController,WidgetsControllerType);

protected:
  /** Constructor */
  VectorDataTransformController();
  /** Destructor */
  ~VectorDataTransformController();

private:
  VectorDataTransformController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  VectorDataTransformView *                  m_View;

  /** Widgets controller */
  WidgetsControllerType::Pointer             m_WidgetsController;

  /** Action handlers */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  ChangeScaleHandlerType::Pointer            m_ChangeScaleHandler;
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;
  ChangeScaledRegionHandlerType::Pointer     m_ChangeScaledRegionHandler;
  PixelDescriptionActionHandlerType::Pointer m_PixelDescriptionHandler;
};
} //end namespace otb

#endif
