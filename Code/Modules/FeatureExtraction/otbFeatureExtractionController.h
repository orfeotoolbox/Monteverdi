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
    itkSetObjectMacro(Controller, FeatureExtractionControllerInterface);
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
    FeatureExtractionControllerInterface::Pointer m_Controller;
  };


public:
  /** Standard class typedefs */
  typedef FeatureExtractionController                                  Self;
  typedef FeatureExtractionControllerInterface                         Superclass;
  typedef itk::SmartPointer<Self>                                      Pointer;
  typedef itk::SmartPointer<const Self>                                ConstPointer;

  typedef FeatureExtractionViewGUI                                     ViewType;
  typedef ViewType::Pointer                                            ViewPointerType;
  typedef Superclass::InputImageType                                   InputImageType;
  typedef Superclass::InputImagePointerType                            InputImagePointerType;
  typedef ModelType::IndexType                                         IndexType;
  typedef ModelType::SizeType                                          SizeType;
  typedef ModelType::OffsetType                                        OffsetType;

  typedef ModelType::VisuModelType                                     VisuModelType;
  typedef ViewType::VisuViewType                                       VisuViewType;
  typedef WidgetResizingActionHandler<VisuModelType,VisuViewType>      ResizingHandlerType;
  typedef ResizingHandlerType::Pointer                                 ResizingHandlerPointerType;
  typedef ChangeExtractRegionActionHandler<VisuModelType,VisuViewType> ChangeExtractRegionHandlerType;
  typedef ChangeExtractRegionHandlerType::Pointer                      ChangeExtractRegionHandlerPointerType;
  typedef MouseClickActionHandler<MouseClickedController,VisuViewType> MouseClickedHandlertype;

  typedef FeatureInfo::FeatureType                                     FeatureType;

/** Standard type macros */
  itkTypeMacro(FeatureExtractionController,Superclass);
  itkNewMacro(Self);

  /** Set the model */
  void SetModel( ModelType::Pointer model);
  
  VisuControllerPointerType GetVisuController()
  {
    return m_VisuController;
  }
  
  VisuControllerPointerType GetResultVisuController()
  {
    return m_ResultVisuController;
  }

  //itkSetObjectMacro(View,ViewType);
  void SetView(ViewPointerType pView);

  virtual void SaveOutput();
  virtual void InitInput();
  virtual void ClearFeatures();
  virtual void CreateFeature(FeatureType featureType);
  virtual void ClearSelectedChannels();
  virtual void SetOutputFileName( const char * name );
  virtual void AddInputChannels(std::vector<unsigned int> list);
  virtual void ChangeFilterStatus(int id);
  virtual void ViewedRegionChanged();
  virtual void AddToOutputListOrder(int id);
  virtual void RemoveFromOutputListOrder(int id);
  virtual void ExchangeOutputListOrder( int direction );

  virtual void Quit();
  virtual void Cancel();


  /** Pixel Clicked method */
  virtual void LeftMouseButtonClicked( ContinuousIndexType index );

protected:
  /** Constructor */
  FeatureExtractionController();
  /** Destructor */
  virtual ~FeatureExtractionController();

private:
  FeatureExtractionController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;

/** NewVisu */
  MouseClickedController::Pointer       m_MouseClickedController;
  MouseClickedHandlertype::Pointer      m_LeftMouseClickedHandler;
  VisuControllerPointerType             m_VisuController;
  VisuControllerPointerType             m_ResultVisuController;
  ResizingHandlerType::Pointer          m_ResizingHandler;
  ChangeExtractRegionHandlerPointerType m_ChangeExtractRegionHandler;
  ResizingHandlerType::Pointer          m_ResultResizingHandler;
  ChangeExtractRegionHandlerPointerType m_ResultChangeExtractRegionHandler;
}; 
} //end namespace otb

#endif
