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
#ifndef __otbObjectCountingViewGUI_h
#define __otbObjectCountingViewGUI_h

#include "otbEventsListener.h"
// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbObjectCountingViewGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include "otbObjectCountingModel.h"
#include "otbObjectCountingControllerInterface.h"

// Visu includes
#include "otbInteractiveScrollWidget.h"
#include "otbInteractiveFullResolutionImageWidget.h"
#include "otbImageWidgetBoxForm.h"
#include "otbImageWidgetPolygonForm.h"
#include "otbImageWidgetPolylineForm.h"
#include "otbVisualizationMacro.h"

namespace otb
{
/** \class ObjectCountingViewGUI
 *
 */
class ITK_ABI_EXPORT ObjectCountingViewGUI
  : public EventsListener<std::string>, public ObjectCountingViewGroup, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef ObjectCountingViewGUI         Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(ObjectCountingViewGUI, itk::Object);

  typedef ObjectCountingModel                                                                ObjectCountingModelType;
  typedef ObjectCountingModel::PixelType                                                     PixelType;
  typedef ObjectCountingModel::ImageType                                                     SingleImageType;
  typedef InteractiveScrollWidget<PixelType, ObjectCountingControllerInterface>              ScrollWidgetType;
  typedef InteractiveFullResolutionImageWidget<PixelType, ObjectCountingControllerInterface> FullWidgetType;

  typedef FullWidgetType::ImageType ImageType;
  typedef ImageType::IndexType      IndexType;
  typedef ImageType::SizeType       SizeType;
  typedef ImageType::RegionType     RegionType;
  typedef ImageWidgetBoxForm        BoxType;

  typedef ImageWidgetPolygonForm<>  PolygonType;
  typedef PolygonType::Pointer      PolygonPointerType;
  typedef ImageWidgetPolylineForm<> PolylineType;
  typedef PolylineType::Pointer     PolylinePointerType;

  /** Set the controller */
  itkSetObjectMacro(Controller, ObjectCountingControllerInterface);
  itkGetObjectMacro(Controller, ObjectCountingControllerInterface);
  /** Display window accessors */
  itkGetObjectMacro(FullImageWidget, FullWidgetType);
  itkGetObjectMacro(ScrollWidget, ScrollWidgetType);

  /** Standard Running macro */
  otbRunningMacro();

  /** Event from the model */
  virtual void Notify(const std::string& event);

  /** Fluid call backs*/
  virtual void OpenImage();
  virtual void Quit();
  virtual void Exit();
  virtual void RunPreview();
  virtual void SaveResult();
  virtual void SavePolygon();
  virtual void ActivateClassifArea(int i);
  virtual void UpdateThresholdValue();
  virtual void UpdateSpatialRadius();
  virtual void UpdateRangeRadius();
  virtual void UpdateScale();
  virtual void UpdateMinRegionSize();
  virtual void UpdateNuParameter();
  virtual void UseSmoothing();

  /** Constructor */
  ObjectCountingViewGUI();

  /** Show the main interface */
  void Show();
  /** Hide the main interface */
  void Hide();
  /** Polygons printing*/
  void UpdatePolygon();
  /** Output object boundaries */
  void UpdateOutputPolyBoundaries();
  /** Clean pre computed result */
  void CleanResultDisplay();

  /** Build the interface */
  virtual void Build();

  void SetModel(ObjectCountingModelType* model);
protected:

  /** Destructor */
  virtual ~ObjectCountingViewGUI();
  /** Refresh the image widgets */
  virtual void RefreshViewer();
  virtual void UpdateInterface();

private:
  ObjectCountingViewGUI(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  //void ReInitAllViews();

  /** Pointer to the model */
  ObjectCountingModel::Pointer m_Model;
  /** Pointer to the controller */
  ObjectCountingControllerInterface::Pointer m_Controller;

  ScrollWidgetType::Pointer m_ScrollWidget;
  FullWidgetType::Pointer   m_FullImageWidget;
};
} //end namespace otb

#endif
