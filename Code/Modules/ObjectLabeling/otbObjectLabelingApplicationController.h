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
#ifndef __otbObjectLabelingApplicationController_h
#define __otbObjectLabelingApplicationController_h

#include "otbObjectLabelingApplicationControllerInterface.h"
#include "otbObjectLabelingApplicationView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbMouseClickActionHandler.h"

namespace otb
{
class ObjectLabelingApplicationController
      : public ObjectLabelingApplicationControllerInterface
{
public:
  /** Standard class typedefs */
  typedef ObjectLabelingApplicationController Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  /** Standard type macros */
  itkTypeMacro(ObjectLabelingApplicationController,Superclass);
  itkNewMacro(Self);

  /** Widgets controller and action handlers */
  typedef ObjectLabelingApplicationView::ImageViewType              ImageViewType;
  typedef ObjectLabelingApplicationModel::VisualizationModelType    VisualizationModelType;
  typedef ObjectLabelingApplicationModel::PixelDescriptionModelType PixelDescriptionModelType;
  typedef ImageWidgetController                                     WidgetsControllerType;
  typedef WidgetResizingActionHandler 
    <VisualizationModelType,ImageViewType>                          ResizingHandlerType;
  typedef ChangeScaledExtractRegionActionHandler
  <VisualizationModelType,ImageViewType>                            ChangeScaledRegionHandlerType;
  typedef ChangeExtractRegionActionHandler
      <VisualizationModelType,ImageViewType>                        ChangeRegionHandlerType;
  typedef ChangeScaleActionHandler
  <VisualizationModelType,ImageViewType>                            ChangeScaleHandlerType;
 typedef otb::PixelDescriptionActionHandler
    < PixelDescriptionModelType, ImageViewType>                     PixelDescriptionActionHandlerType;
  typedef otb::MouseClickActionHandler
  <ObjectLabelingApplicationModel,ImageViewType>                    MouseClickActionHandlerType;
  
  /** User action */
  virtual void ClassSelected(unsigned int classIndex);
  virtual void ClearSelectedClass();
  virtual void AddClass();
  virtual void RemoveClass(unsigned int classIndex);
  virtual void ClearClasses();
  virtual void ChangeClassColor(unsigned int classIndex, const ColorType & color);
  virtual void ChangeClassName(unsigned int classIndex, const char * name);
  virtual void ChangeClassLabel(unsigned int classIndex, const LabelType& label);
  virtual void RemoveObject(unsigned int objectIndex);
  virtual void SelectObject(unsigned int objectIndex);
  virtual void ClearObjects();
  //virtual void LoadImages(const char * image, const char * label);
  virtual void SaveSamplesToXMLFile(const char * fname);
  virtual void LoadSamplesFromXMLFile(const char * fname);
  virtual void SaveClassificationParametersToXMLFile(const char * fname);
  virtual void SaveColorsToAsciiFile(const char * fname);
  virtual void SaveClassification(const char * fname);
  virtual void SaveClassificationGraph(const char * fname);
  virtual void ExportClassificationToGIS(const GISExportInfo& exportInfo);
  virtual void SampleMargin();
  virtual void ChangeKernelType(int kernel);
  virtual void ChangeNumberOfCrossValidationFolders(unsigned int nb);
  virtual void ChangeParametersOptimisation(bool value);
  virtual void ChangeNumberOfCoarseSteps(unsigned int nb);
  virtual void ChangeNumberOfFineSteps(unsigned int nb);
  virtual void ChangeNumberOfMarginSamples(unsigned int nb);
  virtual void ChangeMarginColor(const ColorType & color);
  virtual void FocusOnMarginSample(unsigned int sample);
  virtual void ClearMarginSamples();
  virtual void Classify();
  virtual void ClearClassification();
  virtual void ChangeFeatureState(const std::string & fname, bool state);
  virtual void SetUseContext(bool context);
  virtual void ChangeClassificationOpacity(double value);
  virtual void UpdateViewerDisplay();

  /** Set the pointer to the view */
  void SetView(ObjectLabelingApplicationView * view);


  virtual void SetModel(ObjectLabelingApplicationModel * model)
  {
    ObjectLabelingApplicationControllerInterface::SetModel(model);
    
    // Register the model to the action handlers
    m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeScaleHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel());
    m_PixelDescriptionHandler->SetModel(m_Model->GetPixelDescriptionModel());
    m_MouseClickHandler->SetModel(m_Model);
  }
 
  
  /** Get the widgets controller */
  itkGetObjectMacro(WidgetsController,WidgetsControllerType);
  

protected:
  /** Constructor */
  ObjectLabelingApplicationController();
  /** Destructor */
  ~ObjectLabelingApplicationController();

private:
  ObjectLabelingApplicationController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ObjectLabelingApplicationView *            m_View;

  /** Widgets controller */
  WidgetsControllerType::Pointer             m_WidgetsController;

  /** Action handlers */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  ChangeScaleHandlerType::Pointer            m_ChangeScaleHandler;
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;
  ChangeScaledRegionHandlerType::Pointer     m_ChangeScaledRegionHandler;
  PixelDescriptionActionHandlerType::Pointer m_PixelDescriptionHandler;
  MouseClickActionHandlerType::Pointer       m_MouseClickHandler;
  
};
} //end namespace otb

#endif
