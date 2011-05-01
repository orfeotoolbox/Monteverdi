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
#ifndef __otbObjectLabelingView_h
#define __otbObjectLabelingView_h

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbObjectLabelingGUI.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "otbEventsListener.h"
#include "otbObjectLabelingModel.h"
#include "otbObjectLabelingControllerInterface.h"

#include "otbImageView.h"
#include "otbPixelDescriptionView.h"
#include "otbVectorDataGlComponent.h"

#include <FL/Fl_Color_Chooser.H>

namespace otb
{
/** \class ObjectLabelingView
 *
 */
class ObjectLabelingView
      : public EventsListener<std::string>,
       public ObjectLabelingGUI,
       public itk::Object
{
public:
  /** Standard class typedefs */
  typedef ObjectLabelingView  Self;
  typedef itk::Object                    Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Standard macros */
  itkNewMacro(Self);
  itkTypeMacro(ObjectLabelingView, Object);

  /** Visualization typedefs */
  typedef ObjectLabelingModel::RGBPixelType              RGBPixelType;
  typedef ObjectLabelingModel::VisualizationModelType    VisualizationModelType;
  typedef ObjectLabelingModel::PixelDescriptionModelType PixelDescriptionModelType;
  typedef ImageView<VisualizationModelType>                         ImageViewType;
  typedef PixelDescriptionView<PixelDescriptionModelType>           PixelViewType;
  typedef ObjectLabelingModel::VectorDataType            VectorDataType;
  typedef VectorDataGlComponent<VectorDataType>                     VectorDataGlComponentType;
  typedef std::vector<VectorDataGlComponentType * >                 VectorDataGlComponentVectorType;

  /** Event from the model */
  virtual void Notify(const std::string & event);

  /** Callbacks */
  virtual void Classes();
  virtual void SamplesOpacity();
  virtual void AddClass();
  virtual void RemoveClass();
  virtual void ClearClasses();
  virtual void ClassColor();
  virtual void ClassLabel();
  virtual void ClassName();
  virtual void Objects();
  virtual void RemoveObject();
  virtual void ClearObjects();
  virtual void ObjectFocus();
  virtual void SaveSamplesToXMLFile();
  virtual void SaveClassificationParametersToXMLFile();
  virtual void LoadSamplesFromXMLFile();
  virtual void SaveClassification();
  virtual void SampleMargin();
  virtual void ChangeKernelType();
  virtual void ChangeNumberOfCrossValidationFolders();
  virtual void ChangeParametersOptimisation();
  virtual void ChangeNumberOfCoarseSteps();
  virtual void ChangeNumberOfFineSteps();
  virtual void ChangeNumberOfMarginSamples();
  virtual void ChangeMarginColor();
  virtual void FocusOnMarginSample();
  virtual void ClearMarginSamples();
  virtual void Classify();
  virtual void ClearClassification();
  virtual void Exit();
  virtual void Features();
  virtual void ClassificationOpacity();
  virtual void ViewerSetupOkCallback();

  /** Set the application controller */
  void SetController(ObjectLabelingControllerInterface * controller);

  /** Set the widgets controller */
  void SetWidgetsController(ImageWidgetController * controller);

  /** Set the model */
  void SetModel(ObjectLabelingModel * model)
  {
    m_Model = model;
    m_Model->RegisterListener(this);
  }

  /** Build the interface */
  virtual void Build();
  
  /** Initialize display channel manager */
  void UpdateViewerSetup();

  /** Get a pointer to the view parts for the controller */
  itkGetObjectMacro(ImageView, ImageViewType);
  itkGetObjectMacro(PixelView, PixelViewType);

protected:
  /** Constructor */
  ObjectLabelingView();
  /** Destructor */
  ~ObjectLabelingView();
  
  /** Refresh functions */
  void RefreshInterface();
  void RefreshVisualization();
  void UpdateClassInformation();

private:
  ObjectLabelingView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the controller */
  ObjectLabelingControllerInterface * m_Controller;
  
  /** Pointer to the widgets controller */
  ImageWidgetController *                        m_WidgetsController;

  /** Pointer to the model */
  ObjectLabelingModel *               m_Model;

  /** Image view */
  ImageViewType::Pointer                         m_ImageView;
  
  /** Pixel view */
  PixelViewType::Pointer                         m_PixelView;

  /** List of VectorDataGlComponent */
  VectorDataGlComponentVectorType                m_VectorDataGlComponents;


};
}//end namespace otb

#endif
