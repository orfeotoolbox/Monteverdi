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
#ifndef __otbVectorizationView_h
#define __otbVectorizationView_h

#include "otbVectorizationViewGroup.h"

#include "otbListenerBase.h"
#include "otbVectorizationModel.h"
#include "otbVectorizationControllerInterface.h"

#include "otbImageView.h"
#include "otbVectorDataGlComponent.h"
#include "otbVectorDataTreeBrowser.h"

namespace otb
{
/** \class VectorizationView
 *
 */
class ITK_EXPORT VectorizationView
  : public ListenerBase,
  public VectorizationViewGroup,
  public itk::Object
{
public:
  /** Standard class typedefs */
  typedef VectorizationView             Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(VectorizationView, Object);

  typedef VectorizationModel
  ::VisualizationModelType VisualizationModelType;
  typedef ImageView<VisualizationModelType>  ImageViewType;
  typedef VectorizationModel::VectorDataType VectorDataType;
  typedef VectorDataGlComponent
  <VectorDataType>                           VectorDataGlComponentType;
  typedef VectorDataTreeBrowser
  <VectorDataType>                           VectorDataTreeBrowserType;
  typedef VectorDataGlComponentType::ColorType ColorType;

  /** Event from the model */
  virtual void Notify();

  /** Set the controller */
  void SetController(VectorizationControllerInterface * controller);
  itkGetObjectMacro(Controller, VectorizationControllerInterface);

  /** Set the widgets controller */
  itkSetObjectMacro(WidgetController, ImageWidgetController);

  /** Get The image view */
  itkGetObjectMacro(ImageView, ImageViewType);

  /** Build the interface */
  virtual void BuildInterface();
  virtual void HideAll();
  virtual void NextGeometryIsPointCallback();
  virtual void NextGeometryIsLineCallback();
  virtual void NextGeometryIsPolygonExtCallback();
  virtual void NextGeometryIsPolygonIntCallback();
  virtual void ChangeNavigationModeCallback();
  virtual void UpdateColorCallback();
  virtual void UpdateAlphaCallback();
  virtual void OKCallback();

  /** GUI callbacks*/
  virtual void ButtonAutomaticCallbackOn();
  virtual void ButtonAutomaticCallbackOff();
  
  void Show();
  void RedrawWidgets();
  void SetModel(VectorizationModel* model);
  void UpdateModel();
  void InitColor();
  itkGetMacro(IsHide, bool);

protected:
  /** Constructor */
  VectorizationView();

  /** Destructor */
  virtual ~VectorizationView();

private:
  VectorizationView(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Pointer to the controller */
  VectorizationControllerInterface::Pointer m_Controller;
  /** Pointer to the widgets controller */
  ImageWidgetController::Pointer m_WidgetController;
  /** Pointer to the model */
  VectorizationModel::Pointer m_Model;
  /** Image view */
  ImageViewType::Pointer m_ImageView;
  /** VectorData Gl component */
  VectorDataGlComponentType::Pointer m_VectorDataGlComponent;
  /** VectorDataTreeBrowser */
  VectorDataTreeBrowserType::Pointer m_VectorDataTreeBrowser;
  /** Store the vector data representation color. */
  ColorType m_Color;
  bool m_IsHide;

  VectorDataGlComponentType::Pointer m_SelectedPolygonGLComponent;
};
} //end namespace otb

#endif
