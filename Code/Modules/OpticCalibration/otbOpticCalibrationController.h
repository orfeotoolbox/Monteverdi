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
#ifndef __otbOpticCalibrationController_h
#define __otbOpticCalibrationController_h

#include "otbOpticCalibrationControllerInterface.h"
#include "otbOpticCalibrationView.h"


#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbStandardRenderingFunction.h"


namespace otb
{
class ITK_EXPORT OpticCalibrationController
      : public OpticCalibrationControllerInterface
{
public:
  /** Standard class typedefs */
  typedef OpticCalibrationController          Self;
  typedef OpticCalibrationControllerInterface Superclass;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::SmartPointer<const Self>       ConstPointer;

  typedef OpticCalibrationView         ViewType;
  typedef ViewType::Pointer            ViewPointerType;
  typedef Superclass::ImageType        ImageType;
  typedef Superclass::ImagePointerType ImagePointerType;
  typedef ModelType::IndexType         IndexType;
  typedef ModelType::SizeType          SizeType;
  typedef ModelType::OffsetType        OffsetType;
  typedef ModelType::AerosolModelType  AerosolModelType;

  typedef ModelType::VisuModelType     VisuModelType;
  typedef ViewType::VisuViewType       VisuViewType;


  /** Action handlers */
  typedef otb::ChangeScaledExtractRegionActionHandler
  <VisuModelType,VisuViewType>                     ChangeScaledRegionHandlerType;
  typedef otb::ChangeExtractRegionActionHandler
  <VisuModelType,VisuViewType>                     ChangeRegionHandlerType;
  typedef otb::ChangeScaleActionHandler
  <VisuModelType,VisuViewType>                     ChangeScaleHandlerType;
  typedef WidgetResizingActionHandler
  <VisuModelType,VisuViewType>                     ResizingHandlerType;
  typedef ResizingHandlerType::Pointer             ResizingHandlerPointerType;
  typedef ChangeExtractRegionActionHandler
  <VisuModelType,VisuViewType>                     ChangeExtractRegionHandlerType;
  typedef ChangeExtractRegionHandlerType::Pointer  ChangeExtractRegionHandlerPointerType;

  /** Pixel description */
  typedef ModelType::ViewerImageType ViewerImageType;
  typedef PixelDescriptionModel<ViewerImageType>           PixelDescriptionModelType;
  typedef PixelDescriptionView<PixelDescriptionModelType> PixelDescriptionViewType;
  typedef PixelDescriptionActionHandler
    < PixelDescriptionModelType, VisuViewType>            PixelDescriptionActionHandlerType;


  /** Standard type macros */
  itkTypeMacro(OpticCalibrationController,Superclass);
  itkNewMacro(Self);

  /** Visu */
  VisuControllerPointerType GetVisuController()
  {
    return m_VisuController;
  }

  VisuControllerPointerType GetRVisuController()
  {
    return m_RVisuController;
  }

  /** View of the MVC application */
  void SetView(ViewType* pView);
  /** Model of the MVC application */ 
  void SetModel(ModelType* model);


  virtual void UpdateRGBChannelOrder(unsigned int red,unsigned int green,unsigned int blue);
  virtual void UpdateGrayscaleChannel(unsigned int channel);
  virtual void UpdateDisplay(EnumImageType pType);
  virtual void UpdateCorrectionParameters();
  virtual void UpdateRadiativeTerms( bool updateIm );
  virtual void QuitModel();
 
  void SetInputImage( ImagePointerType img );
  void LinkPixelDescriptors();

protected:
  /** Constructor */
  OpticCalibrationController();
  /** Destructor */
  virtual ~OpticCalibrationController();

private:
  OpticCalibrationController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewType * m_View;

  /** The instance of the model */
  ModelType * m_Model;

  /** Visu */
  VisuControllerPointerType m_VisuController;
  VisuControllerPointerType m_RVisuController;

  bool m_IsValidImage;
  
  /** Handlers */
  /// Input Image Viewer
  /** Add the resizing handler to the viewer */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  /** Add the change scaled region handler */
  ChangeScaledRegionHandlerType::Pointer     m_ChangeScaledHandler;
  /** Add the change scale handler */
  ChangeScaleHandlerType::Pointer            m_ChangeScaleHandler;
  /** Add the change region action handler */
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;
  /** Add the pixel description action handler */
  PixelDescriptionActionHandlerType::Pointer m_PixelActionHandler;
  /** Add the pixel description model */
  PixelDescriptionModelType::Pointer         m_PixelActionModel;
  /** Add the pixel description view */
  PixelDescriptionViewType::Pointer          m_PixelActionView;

  /// Result ImageViewer
  /** Add the resizing handler to the viewer */
  ResizingHandlerType::Pointer               m_ResultResizingHandler;
  /** Add the change region action handler */
  ChangeRegionHandlerType::Pointer           m_ResultChangeRegionHandler;
  /** Add the change scaled region handler */
  ChangeScaledRegionHandlerType::Pointer     m_ResultChangeScaledHandler;
  /** Add the change scale handler */
  ChangeScaleHandlerType::Pointer            m_ResultChangeScaleHandler;
  /** Add the pixel description action handler */
  PixelDescriptionActionHandlerType::Pointer m_ResultPixelActionHandler;
  /** Add the pixel description model */
  PixelDescriptionModelType::Pointer         m_ResultPixelActionModel;
  /** Add the pixel description view */
  PixelDescriptionViewType::Pointer          m_ResultPixelActionView;
};
} //end namespace otb

#endif
