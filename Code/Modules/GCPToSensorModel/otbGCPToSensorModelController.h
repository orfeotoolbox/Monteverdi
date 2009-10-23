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

#ifndef __otbGCPToSensorModelController_h
#define __otbGCPToSensorModelController_h

#include "otbGCPToSensorModelControllerInterface.h"
#include "otbGCPToSensorModelView.h"

#include "otbImageWidgetController.h"
#include "otbTransformEnumType.h"

// Handlers list
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbMouseClickActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbPixelDescriptionModel.h"
#include "otbPixelDescriptionView.h"
#include "otbMouseClickActionHandler.h"

namespace otb
{
class ITK_EXPORT GCPToSensorModelController
      : public GCPToSensorModelControllerInterface
{
public:

 /** Inner model class for mouse click action handler */
  class MouseClickedController : public itk::Object
  {
    //friend class GCPToSensorModelModel;
  public:
 /** Standard class typedefs */
  typedef MouseClickedController        Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macros */
  itkTypeMacro(MouseClickedController,Superclass);
  itkNewMacro(Self);

  typedef itk::Index<> IndexType;

  void IndexClicked(ContinuousIndexType index)
    {
      if( !m_Controller.IsNotNull() )
       itkExceptionMacro(<<"Not Controller set.");
      if( m_MouseButton==1 )
       m_Controller->LeftMouseButtonClicked(index);
      else
       itkExceptionMacro(<<"Mouse event not supproted yet.");
    }
    itkSetClampMacro(MouseButton,int,1,3);
    itkGetMacro(MouseButton,int);
    itkSetClampMacro(ImageViewId,int,0,1);
    itkGetMacro(ImageViewId,int);
    itkSetObjectMacro(Controller, GCPToSensorModelControllerInterface);
  protected:
 /** Constructor */
  MouseClickedController() : m_MouseButton(1), m_ImageViewId(1), m_Controller(){};
    /** Destructor */
    virtual ~MouseClickedController(){};
    /** Set the mouse button used (1 = left, 2 = center, 3 = right) */
    int m_MouseButton;
    /** Set the associated view (0: left, 1: right) */
    int m_ImageViewId;
    /** Inner model class for mouse click action handler */
    GCPToSensorModelControllerInterface::Pointer m_Controller;
  };

  /** Standard class typedefs */
  typedef GCPToSensorModelController          Self;
  typedef GCPToSensorModelControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Standard type macros */
  itkTypeMacro(GCPToSensorModelController,Superclass);
  itkNewMacro(Self);


  /** Widgets controller and action handlers */
  typedef GCPToSensorModelView::ImageViewType           ImageViewType;
  typedef GCPToSensorModelModel::VisualizationModelType VisualizationModelType;
  typedef GCPToSensorModelModel::VectorImageType        VectorImageType;
  typedef VectorImageType::IndexType                                   IndexType;
  typedef VectorImageType::PixelType                                   PixelType;
  typedef GCPToSensorModelModel::RGBImageType           RGBImageType;
 
  /** Handlers */
  typedef ImageWidgetController                                                        WidgetControllerType;
  typedef WidgetResizingActionHandler<VisualizationModelType,ImageViewType>            ResizingHandlerType;
  typedef ChangeExtractRegionActionHandler<VisualizationModelType,ImageViewType>       ChangeRegionHandlerType;
  typedef ChangeScaledExtractRegionActionHandler<VisualizationModelType,ImageViewType> ChangeScaledRegionHandlerType;
  typedef ChangeScaleActionHandler<VisualizationModelType,ImageViewType>               ChangeScaleHandlerType;
  typedef MouseClickActionHandler<MouseClickedController, ImageViewType>               MouseClickedHandlertype;

  typedef PixelDescriptionModel<RGBImageType>                                          PixelDescriptionModelType;
  typedef PixelDescriptionActionHandler<PixelDescriptionModelType, ImageViewType>      PixelDescriptionActionHandlerType;
  typedef PixelDescriptionView<PixelDescriptionModelType>                              PixelDescriptionViewType;
  typedef PixelDescriptionViewType::Pointer                                            PixelDescriptionViewPointerType;
  
  typedef ModelType::IndexesListType     IndexesListType;
  //typedef ModelType::TransformType       TransformType;
  typedef ModelType::OutPointListType    OutPointListType;
  typedef ModelType::OutPointType        OutPointType;
  typedef ModelType::ContinuousIndexType ContinuousIndexType;

  //typedef TransformType::OutputPointType OutputTransformPointType;
  //typedef TransformType::InputPointType InputTransformPointType;

  void SetModel(ModelType* model);

  /** Set the pointer to the view */
  void SetView(GCPToSensorModelView * view);

  /** Get the widgets controller */
  itkGetObjectMacro(WidgetController,WidgetControllerType);

  virtual void SetInputImage(GCPToSensorModelModel::VectorImageType* image)
  {
    m_Model->SetImage(image);
  }

  /** Link module and pixel descriptor action handlers. */
  virtual void LinkPixelDescriptors();
  virtual void AddPoints( float x, float y, float lat, float lon );
  virtual void ClearPointList();
  virtual void DeletePointFromList( unsigned int id );
  virtual void FocusOn(ContinuousIndexType id);
  virtual void SetDEMPath( const std::string & filePath );
  virtual void ChangeDEM();
  virtual void OK();

  /** Pixel Clicked method */
  virtual void LeftMouseButtonClicked( ContinuousIndexType index );

  void UpdateStats();

 protected:
  /** Constructor */
  GCPToSensorModelController();
  /** Destructor */
  virtual ~GCPToSensorModelController();


private:
  GCPToSensorModelController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  GCPToSensorModelView * m_View;

  /** The instance of the model */
  ModelType * m_Model;

  /** Widgets controller */
  WidgetControllerType::Pointer             m_WidgetController;
  MouseClickedController::Pointer            m_MouseClickedController;

  /** Action handlers */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;
  ChangeScaledRegionHandlerType::Pointer     m_ChangeScaledRegionHandler;
  ChangeScaleHandlerType::Pointer            m_ChangeScaleHandler;
  MouseClickedHandlertype::Pointer           m_LeftMouseClickedHandler;
  PixelDescriptionActionHandlerType::Pointer m_PixelActionHandler;
  PixelDescriptionModelType::Pointer         m_PixelModel;
  PixelDescriptionViewType::Pointer          m_PixelView;
  
};
} //end namespace otb

#endif