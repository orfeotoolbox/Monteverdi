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

#ifndef __otbHomologousPointExtractionView_h
#define __otbHomologousPointExtractionView_h




#include "otbHomologousPointExtractionViewGroup.h"

#include "otbListenerBase.h"
#include "otbHomologousPointExtractionModuleModel.h"
#include "otbHomologousPointExtractionModuleControllerInterface.h"

#include "otbCrossGlComponent.h"
#include "otbCircleGlComponent.h"
#include "otbImageView.h"

namespace otb
{
/** \class HomologousPointExtractionModuleView
 *
 */
class ITK_EXPORT HomologousPointExtractionModuleView
      : public ListenerBase, public HomologousPointExtractionViewGroup, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef HomologousPointExtractionModuleView  Self;
  typedef itk::Object                          Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(HomologousPointExtractionModuleView,Object);


  typedef HomologousPointExtractionModuleModel::VisualizationModelType VisualizationModelType;
  typedef HomologousPointExtractionModuleModel::RGBPixelType           PixelType;
  typedef HomologousPointExtractionModuleModel::IndexType              IndexType;
  typedef ImageView<VisualizationModelType>                            ImageViewType;
  typedef CrossGlComponent::ColorType                                  ColorType;
  typedef std::vector<ColorType>                                       ColorListType;

  /** Event from the model */
  virtual void Notify();

  /** Fluid call backs*/
  virtual void Quit();

  /** Set the controller */
  itkSetObjectMacro(Controller,HomologousPointExtractionModuleControllerInterface);
  itkGetObjectMacro(Controller,HomologousPointExtractionModuleControllerInterface);

  /** Set the widgets controller */
  itkSetObjectMacro(FirstWidgetsController,ImageWidgetController);
  itkSetObjectMacro(SecondWidgetsController,ImageWidgetController);

  /** Build the interface */
  virtual void BuildInterface();
  virtual void AddPoints();
  virtual void ClearPointList();
  virtual void DeletePoint();
  virtual void ComputeTransform();
  virtual void Focus(unsigned int i);
  virtual void ClearTransformationInfo();
  virtual void Evaluate();
  virtual void HideAll();

  void UpdateListSelectionColor(bool whichOne);
  void ChangePointValue(IndexType index, unsigned int viewId );
  void AddPointsToList( IndexType id1, IndexType id2);
  void RedrawWidgets();
  
  /** Get a pointer to the view parts for the controller */
  itkGetObjectMacro(FirstImageView,ImageViewType);
  itkGetObjectMacro(SecondImageView,ImageViewType);

  void SetModel(HomologousPointExtractionModuleModel* model);
  
protected:
  /** Constructor */
  HomologousPointExtractionModuleView();

  /** Destructor */
  virtual ~HomologousPointExtractionModuleView();


private:
  HomologousPointExtractionModuleView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the controller */
  HomologousPointExtractionModuleControllerInterface::Pointer m_Controller;
  /** Pointer to the widgets controller */
  ImageWidgetController::Pointer                m_FirstWidgetsController;
  ImageWidgetController::Pointer                m_SecondWidgetsController;
  /** Pointer to the model */
  HomologousPointExtractionModuleModel::Pointer m_Model;
  /** Image view */
  ImageViewType::Pointer                        m_FirstImageView;
  ImageViewType::Pointer                        m_SecondImageView;
  /** Cross Gl Component */
  CrossGlComponent::Pointer                     m_FirstCrossGlComponent;
  CrossGlComponent::Pointer                     m_SecondCrossGlComponent;
  /** Circle Gl Component */
  CircleGlComponent::Pointer                    m_FirstCircleGlComponent;
  CircleGlComponent::Pointer                    m_SecondCircleGlComponent;
  /** Point color */
  ColorListType                                 m_ColorList;
  /** Green color */
  ColorType                                     m_Green;
};
}//end namespace otb

#endif
