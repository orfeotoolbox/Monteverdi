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
#ifndef __otbOpticCalibrationView_h
#define __otbOpticCalibrationView_h


#include "otbListenerBase.h"

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbOpticCalibrationViewGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <FL/Fl_Group.H>

#include "otbOpticCalibrationModel.h"
#include "otbOpticCalibrationControllerInterface.h"

#include "otbImageViewerBase.h"
#include "otbImageView.h"

#include "otbCurves2DWidget.h"
#include "otbHistogramCurve.h"
#include "otbPixelDescriptionView.h"

namespace otb
{
/** \class OpticCalibrationView
 *
 */
class ITK_EXPORT OpticCalibrationView
      : public ListenerBase , public OpticCalibrationViewGroup, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef OpticCalibrationView          Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(OpticCalibrationView,itk::Object);

  /** Typedefs */
  typedef OpticCalibrationModel                        OpticCalibrationModelType;
  typedef OpticCalibrationModel::ImagePixelType        ModelPixelType;
  typedef OpticCalibrationModelType::VisuModelType     VisuModelType;
  typedef ImageView<VisuModelType>                           VisuViewType;
  typedef OpticCalibrationControllerInterface::Pointer OpticCalibrationControllerInterfacePointerType;
  typedef OpticCalibrationModelType::ImageType         ModelImageType;
  typedef VisuViewType::ImageType                            ImageType;
  typedef VisuViewType::Pointer                              VisuViewPointerType;
  typedef ImageType::RegionType                              RegionType;
  typedef RegionType::SizeType                               SizeType;
  typedef RegionType::IndexType                              IndexType;
  typedef ImageType::Pointer                                 ImagePointerType;

  typedef std::vector<std::string>                           StringVectorType;

  typedef OpticCalibrationModelType::RenderingFunctionType::Pointer
                                                             RenderingFunctionPointerType;
  typedef std::vector<unsigned int>                          ChannelListType;

  /** Histogram from Image layer type */
  typedef OpticCalibrationModelType::HistogramType     HistogramType;
  typedef OpticCalibrationModelType::ImageLayerPointerType ImageLayerPointerType;


  /** Curves 2D widget */
  typedef Curves2DWidget                            CurvesWidgetType;
  typedef CurvesWidgetType::Pointer                 CurvesWidgetPointerType;
  typedef ObjectList<CurvesWidgetType>              CurvesWidgetListType;
  typedef CurvesWidgetListType::Pointer             CurvesWidgetListPointerType;
  typedef HistogramCurve<HistogramType>             HistogramCurveType;
  typedef HistogramCurveType::Pointer               HistogramCurvePointerType;



  //--------------------//
  //      METHODS       //
  //--------------------//

  /** Render Curve Widget */
  void RenderCurveWidget(EnumImageType pType);
  /** Init window widgets */
  void InitWidgets();

  /** Standard Running macro */
  otbRunningMacro();

  /** Event from the model */
  virtual void Notify();
  void Show();

  /** Constructor */
  OpticCalibrationView();

  /** OpticCalibrationController Getter/Setter */
  itkGetObjectMacro(OpticCalibrationController,OpticCalibrationControllerInterface);
  void SetOpticCalibrationController(OpticCalibrationControllerInterface *pController)
    {
      m_OpticCalibrationController = pController;
      m_VisuView->SetController(m_OpticCalibrationController->GetVisuController());
      m_RVisuView->SetController(m_OpticCalibrationController->GetRVisuController());
    }
  
  /** OpticCalibrationModel Getter/Setter */
  itkGetObjectMacro(OpticCalibrationModel,OpticCalibrationModel);
  void SetOpticCalibrationModel( OpticCalibrationModel * pModel )
    {
      m_OpticCalibrationModel = pModel;
      m_VisuView->SetModel(m_OpticCalibrationModel->GetVisuModel());
      m_RVisuView->SetModel(m_OpticCalibrationModel->GetRVisuModel());
    }
 
  
 
  itkGetMacro(VisuView,VisuViewPointerType);
  itkGetMacro(RVisuView,VisuViewPointerType);

  virtual void QuitCallback();
  void UpdateParamDisplay();
  void UpdateDisplayResult();
  void HideAll();
  void Init();
  void BuildInterface();

protected:

  /** Destructor */
  virtual ~OpticCalibrationView();

  /** Callbacks */
  virtual void ViewerSetupCallback();
  virtual void ViewerSetupOkCallback();
  virtual void ViewerSetupCancelCallback();
  virtual void GrayscaleSetCallback();
  virtual void RGBSetCallback();
  virtual void SwitchCallback(EnumImageType pType);
  virtual void CoefSetupOkCallback();
  virtual void OpenAeronetFileCallback();
  virtual void OpenFFVFileCallback();
  virtual void UpdateRadiativeTermsCallback();
  virtual void ReloadChannelTermsCallback( bool updateIm );
  virtual void SaveAndQuitCallback();
  virtual void ChangeReflectanceScaleCallback();

  void ShowInputGroup();
  void ShowResultGroup();
  void UpdateViewerSetup();
  void UpdateInformation();
  void UpdateHistograms(EnumImageType pType);
  void UpdateCoefSetup();

//  virtual void OkConfigIndices();


private:
  OpticCalibrationView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  //--------------------//
  //     ATTRIBUTES     //
  //--------------------//

  /** Pointer to the model */
  OpticCalibrationModel::Pointer m_OpticCalibrationModel;
  /** Pointer to the controller */
  OpticCalibrationControllerInterface::Pointer m_OpticCalibrationController;
  /** Visu */
  VisuViewPointerType                 m_VisuView;
  VisuViewPointerType                 m_RVisuView;
  /** Curve widget list */
  CurvesWidgetListPointerType     m_CurvesWidgetList;
  /** Last input path */
  std::string              m_LastPath;

};
}//end namespace otb

#endif
