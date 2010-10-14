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
#ifndef __otbPolarimetricSynthesisView_h
#define __otbPolarimetricSynthesisView_h
// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbPolarimetricSynthesisGUI.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "otbEventsListener.h"
#include "otbTypeManager.h"
#include "otbPolarimetricSynthesisModel.h"
#include "otbPolarimetricSynthesisControllerInterface.h"

#include "otbImageWidgetController.h"
#include "otbInteractiveScrollWidget.h"
#include "otbPolarimetricSynthesisFullWidget.h"
#include "otbImageWidgetBoxForm.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbPoincareSphereWidget.h"

// View
#include "otbImageView.h"
#include "otbPixelDescriptionView.h"

#include "itkObject.h"

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>

namespace otb
{
/** \class PolarimetricSynthesisView
 *
 */
class PolarimetricSynthesisView
  : public EventsListener<std::string>,
    public PolarimetricSynthesisGUI,
    public itk::Object
{
public:
  /** Standard class typedefs */
  typedef PolarimetricSynthesisView      Self;
  typedef itk::Object                    Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(PolarimetricSynthesisView,Object);

  // Pixel type
//  typedef double PixelType;
  typedef TypeManager::Floating_Point_Precision                                    PixelType;
  typedef otb::InteractiveScrollWidget<PixelType,PolarimetricSynthesisControllerInterface>            ScrollWidgetType;
  typedef otb::PolarimetricSynthesisFullWidget<PixelType,PolarimetricSynthesisControllerInterface>    FullWidgetType;
  typedef FullWidgetType::ImageType                          ImageType;
  typedef ImageType::IndexType                               IndexType;
  typedef ImageType::SizeType                                SizeType;
  typedef ImageType::RegionType                              RegionType;
  typedef ImageWidgetBoxForm                                 BoxType;
  typedef BoxType::ColorType                                 ColorType;
  typedef PolarimetricSynthesisModel::OutputImageType SingleImageType;
  typedef ImageToVectorImageCastFilter<SingleImageType,ImageType> VectorCastFilterType;
  typedef PolarimetricSynthesisModel::HistogramType HistogramType;
  /*   typedef PolarimetricSynthesisHistogramWidget<HistogramType,PixelType> HistogramWidgetType; */
  typedef otb::PoincareSphereWidget                          PoincareSphereWidgetType;

  /** Visualization typedefs */
  typedef PolarimetricSynthesisModel::RGBPixelType              RGBPixelType;
  typedef PolarimetricSynthesisModel::VisualizationModelType    VisualizationModelType;
  typedef PolarimetricSynthesisModel::PixelDescriptionModelType PixelDescriptionModelType;
  typedef ImageView<VisualizationModelType>                         ImageViewType;
  typedef PixelDescriptionView<PixelDescriptionModelType>           PixelViewType;


  itkGetObjectMacro(FullWidget,FullWidgetType);

  /** Event from the model */
  virtual void Notify(const std::string & event);

  /** Build the interface */
  virtual void Build();

  /** Set the application controller */
  void SetController(PolarimetricSynthesisControllerInterface * controller);

  /** Set the widgets controller */
  void SetWidgetsController(ImageWidgetController * controller);

  /** Set the model */
  void SetModel(PolarimetricSynthesisModel * model)
  {
    m_Model = model;
    m_Model->RegisterListener(this);
  }


  /** Buttons callback */
  virtual void VectorBrowserCallback();
  virtual void HHBrowserCallback();
  virtual void HVBrowserCallback();
  virtual void VHBrowserCallback();
  virtual void VVBrowserCallback();
  virtual void HEmissionCallback();
  virtual void VEmissionCallback();
  virtual void OpenVectorImageCallback();
  virtual void OpenImagesCallback();
  virtual void OpenImageOkCallback();
  virtual void OpenImageCancelCallback();
  virtual void OpenCallback();
  virtual void SaveResultCallback();
  virtual void QuitCallback();
  virtual void DialGainCallback();
  virtual void InputGainCallback();

  virtual void RGBButtonCallback();
  virtual void RGBGroupCallback();
  virtual void GrayGroupCallback();

  virtual void RedPsiICallback();
  virtual void RedPsiRCallback();
  virtual void RedKhiICallback();
  virtual void RedKhiRCallback();
  virtual void RedCoPolarizationCallback();
  virtual void RedCrossPolarizationCallback();
  virtual void RedAnyPolarizationCallback();

  virtual void GreenPsiICallback();
  virtual void GreenPsiRCallback();
  virtual void GreenKhiICallback();
  virtual void GreenKhiRCallback();
  virtual void GreenCoPolarizationCallback();
  virtual void GreenCrossPolarizationCallback();
  virtual void GreenAnyPolarizationCallback();

  virtual void BluePsiICallback();
  virtual void BluePsiRCallback();
  virtual void BlueKhiICallback();
  virtual void BlueKhiRCallback();
  virtual void BlueCoPolarizationCallback();
  virtual void BlueCrossPolarizationCallback();
  virtual void BlueAnyPolarizationCallback();

  virtual void GrayPsiICallback();
  virtual void GrayPsiRCallback();
  virtual void GrayKhiICallback();
  virtual void GrayKhiRCallback();
  virtual void GrayCoPolarizationCallback();
  virtual void GrayCrossPolarizationCallback();
  virtual void GrayAnyPolarizationCallback();


protected:
  /** Constructor */
  PolarimetricSynthesisView();
  /** Destructor */
  ~PolarimetricSynthesisView() {};

  /** Refresh the image widgets */
  virtual void RefreshViewer();
  virtual void UpdateInterface();

private:
  PolarimetricSynthesisView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the model */
  PolarimetricSynthesisModel::Pointer m_Model;

  /** Pointer to the controller */
  PolarimetricSynthesisControllerInterface::Pointer m_Controller;

  /** Pointer to the widgets controller */
  ImageWidgetController::Pointer   m_WidgetsController;

  ColorType m_InterfaceColor;

  /// Vector cast filters
  VectorCastFilterType::Pointer m_VectorCastFilter;
  VectorCastFilterType::Pointer m_QuicklookVectorCastFilter;
  ScrollWidgetType::Pointer     m_ScrollWidget;
  FullWidgetType::Pointer       m_FullWidget;
  PoincareSphereWidgetType::Pointer m_PoincareSphereWidget;
  /*   HistogramWidgetType::Pointer  m_HistogramWidget; */

};
}//end namespace otb

#endif
