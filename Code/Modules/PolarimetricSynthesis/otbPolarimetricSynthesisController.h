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
#ifndef __otbPolarimetricSynthesisController_h
#define __otbPolarimetricSynthesisController_h

#include "otbPolarimetricSynthesisControllerInterface.h"
#include "otbPolarimetricSynthesisView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
//#include "otbMouseClickActionHandler.h"



namespace otb
{
class PolarimetricSynthesisController
      : public PolarimetricSynthesisControllerInterface
{
public:
  /** Standard class typedefs */
  typedef PolarimetricSynthesisController Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  typedef otb::PolarimetricSynthesisView   ViewType;
  typedef ViewType::Pointer                ViewPointerType;

  /** Widgets controller and action handlers */
   typedef PolarimetricSynthesisView::ImageViewType                  ImageViewType;
   typedef PolarimetricSynthesisModel::VisualizationModelType        VisualizationModelType;
   typedef PolarimetricSynthesisModel::PixelDescriptionModelType     PixelDescriptionModelType;

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
/*
  typedef otb::MouseClickActionHandler
   <PolarimetricSynthesisModel,ImageViewType>                        MouseClickActionHandlerType;
*/

  /** Standard type macros */
  itkTypeMacro(PolarimetricSynthesisController,Superclass);
  itkNewMacro(Self);

  /** Set the view */
  virtual void SetView(ViewPointerType view);

  virtual void SetModel(PolarimetricSynthesisModel * model)
  {
    PolarimetricSynthesisControllerInterface::SetModel(model);

#if 0
    // Register the model to the action handlers
    m_ResizingHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeScaleHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeRegionHandler->SetModel(m_Model->GetVisualizationModel());
    m_ChangeScaledRegionHandler->SetModel(m_Model->GetVisualizationModel());
    m_PixelDescriptionHandler->SetModel(m_Model->GetPixelDescriptionModel());
    m_MouseClickHandler->SetModel(m_Model);
#endif
  }


  /** User action */
  virtual void LoadImages();
  virtual void ChangeRGB(bool value);
  virtual void ChangeGain(double value);
  virtual void ChangeGrayPsiI(double value);
  virtual void ChangeGrayPsiR(double value);
  virtual void ChangeGrayKhiI(double value);
  virtual void ChangeGrayKhiR(double value);
  virtual void ChangeGrayPolarizationMode(PolarizationMode mode);
  virtual void ChangeRedPsiI(double value);
  virtual void ChangeRedPsiR(double value);
  virtual void ChangeRedKhiI(double value);
  virtual void ChangeRedKhiR(double value);
  virtual void ChangeRedPolarizationMode(PolarizationMode mode);
  virtual void ChangeGreenPsiI(double value);
  virtual void ChangeGreenPsiR(double value);
  virtual void ChangeGreenKhiI(double value);
  virtual void ChangeGreenKhiR(double value);
  virtual void ChangeGreenPolarizationMode(PolarizationMode mode);
  virtual void ChangeBluePsiI(double value);
  virtual void ChangeBluePsiR(double value);
  virtual void ChangeBlueKhiI(double value);
  virtual void ChangeBlueKhiR(double value);
  virtual void ChangeBluePolarizationMode(PolarizationMode mode);
  virtual void IndexClicked(const IndexType& index);
  virtual void MouseIn();
  virtual void MouseOut();
  virtual void PixelHovered(const IndexType & index);
  virtual bool IsHEmissionMode();
  virtual bool IsVEmissionMode();

  /** Get the widgets controller */
  itkGetObjectMacro(WidgetsController,WidgetsControllerType);


protected:
  /** Constructor */
  PolarimetricSynthesisController();
  /** Destructor */
  ~PolarimetricSynthesisController();

private:
  PolarimetricSynthesisController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;

  /** Widgets controller */
  WidgetsControllerType::Pointer             m_WidgetsController;

  /** Action handlers */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  ChangeScaleHandlerType::Pointer            m_ChangeScaleHandler;
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;
  ChangeScaledRegionHandlerType::Pointer     m_ChangeScaledRegionHandler;
  PixelDescriptionActionHandlerType::Pointer m_PixelDescriptionHandler;
//  MouseClickActionHandlerType::Pointer       m_MouseClickHandler;

};
} //end namespace otb

#endif
