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
#include "otbPixelClickedActionHandler.h"


namespace otb
{
class ITK_EXPORT FeatureExtractionController
      : public FeatureExtractionControllerInterface
{
public:
  /** Standard class typedefs */
  typedef FeatureExtractionController          Self;
  typedef FeatureExtractionControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef FeatureExtractionViewGUI          ViewType;
  typedef ViewType::Pointer                 ViewPointerType;
  typedef Superclass::InputImageType        InputImageType;
  typedef Superclass::InputImagePointerType InputImagePointerType;
  typedef ModelType::IndexType              IndexType;
  typedef ModelType::SizeType               SizeType;
  typedef ModelType::OffsetType             OffsetType;

  typedef ModelType::VisuModelType         VisuModelType;
  typedef ViewType::VisuViewType           VisuViewType;
  typedef WidgetResizingActionHandler<VisuModelType,VisuViewType>      ResizingHandlerType;
  typedef ResizingHandlerType::Pointer                                 ResizingHandlerPointerType;
  typedef ChangeExtractRegionActionHandler<VisuModelType,VisuViewType> ChangeExtractRegionHandlerType;
  typedef ChangeExtractRegionHandlerType::Pointer                      ChangeExtractRegionHandlerPointerType;
  typedef PixelClickedActionHandler<ModelType,ViewType>                PixelClickedHandlerType;
  typedef PixelClickedHandlerType::Pointer                             PixelClickedHandlerPointerType;

/** Standard type macros */
  itkTypeMacro(FeatureExtractionController,Superclass);
  itkNewMacro(Self);

/** Set the model */
  itkSetObjectMacro(Model,ModelType);

/** NewVisu */
  VisuControllerPointerType GetVisuController()
  {
    return m_VisuController;
  }
  
  VisuControllerPointerType GetResultVisuController()
  {
    return m_ResultVisuController;
  }

  //itkSetObjectMacro(View,ViewType);
  void SetView(ViewPointerType pView)
  {
    m_View = pView;

/* Full View Actions */
    // Add the resizing handler
    ResizingHandlerType::Pointer lResizingHandler = ResizingHandlerType::New();
    lResizingHandler->SetModel(m_Model->GetVisuModel());
    lResizingHandler->SetView(m_View->GetVisuView());

    ChangeExtractRegionHandlerPointerType lChangeExtractRegionHandler = ChangeExtractRegionHandlerType::New();
    lChangeExtractRegionHandler->SetModel(m_Model->GetVisuModel());
    lChangeExtractRegionHandler->SetView(m_View->GetVisuView());

    PixelClickedHandlerPointerType lPixelClickedHandler = PixelClickedHandlerType::New();
    lPixelClickedHandler->SetModel(m_Model);
    lPixelClickedHandler->SetView(m_View);

    // Connect the handlers
    m_VisuController->AddActionHandler(lResizingHandler);
    m_VisuController->AddActionHandler(lChangeExtractRegionHandler);
    m_VisuController->AddActionHandler(lPixelClickedHandler);

/* Feature View Actions */
    ResizingHandlerType::Pointer lResultResizingHandler = ResizingHandlerType::New();
    lResultResizingHandler->SetModel(m_Model->GetResultVisuModel());
    lResultResizingHandler->SetView(m_View->GetResultVisuView());

    ChangeExtractRegionHandlerPointerType lResultChangeExtractRegionHandler = ChangeExtractRegionHandlerType::New();
    lResultChangeExtractRegionHandler->SetModel(m_Model->GetResultVisuModel());
    lResultChangeExtractRegionHandler->SetView(m_View->GetVisuView());

    // Connect the handlers
    m_ResultVisuController->AddActionHandler(lResultResizingHandler);
    m_VisuController->AddActionHandler(lResultChangeExtractRegionHandler);

  }


  virtual void OpenInputImage(const char * filename);
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
  virtual void NotifyMonteverdi();

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
  VisuControllerPointerType m_VisuController;
  VisuControllerPointerType m_ResultVisuController;




};
} //end namespace otb

#endif
