/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbSpectrumModuleView_h
#define __otbSpectrumModuleView_h

 #include "itkObject.h"
 #include "otbMsgReporter.h"
 #include "otbEventsListener.h"

 #include "otbImageView.h"
 #include "otbPixelSpectrumView.h"

#include "otbSpectrumModuleModel.h"
#include "otbSpectrumModuleControllerInterface.h"
#include "otbSpectrumModuleViewGroup.h"

#include "FL/Fl_File_Chooser.H"

namespace otb {

class SpectrumModuleView
: public EventsListener<std::string>, public SpectrumModuleViewGroup, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef SpectrumModuleView            Self;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef SpectrumModuleViewGroup       Superclass;

  typedef Fl_File_Chooser FileChooserType;

  /** New macro */
  itkNewMacro(Self);
  itkTypeMacro(SpectrumModuleView, itk::Object)

  /** relative to MVC */
  typedef otb::SpectrumModuleModel                SpectrumModuleModelType;
  typedef SpectrumModuleModelType::Pointer        SpectrumModuleModelPointerType;
  typedef SpectrumModuleModelType::ImageModelType ImageModelType;

  typedef otb::SpectrumModuleControllerInterface SpectrumModuleControllerType;
  typedef SpectrumModuleControllerType::Pointer  SpectrumModuleControllerPointerType;

  /** Views */
  typedef otb::ImageView<ImageModelType> ImageViewType;
  typedef ImageViewType::Pointer         ImageViewPointerType;

  typedef otb::PixelSpectrumView         PixelSpectrumViewType;
  typedef PixelSpectrumViewType::Pointer PixelSpectrumViewPointerType;

  /** Methods */
  itkGetObjectMacro(ImageView, ImageViewType);
  itkSetObjectMacro(ImageView, ImageViewType);

  itkGetObjectMacro(PixelSpectrumView, PixelSpectrumViewType);
  itkSetObjectMacro(PixelSpectrumView, PixelSpectrumViewType);

  itkGetObjectMacro(SpectrumModuleModel, SpectrumModuleModelType);
  void SetSpectrumModuleModel(SpectrumModuleModelType * m);

  itkGetObjectMacro(SpectrumModuleController, SpectrumModuleControllerType);
  //itkSetObjectMacro(SpectrumModuleController, SpectrumModuleControllerType);
  void SetSpectrumModuleController(SpectrumModuleControllerType *c);

  void Build();
  void Show();
  void Hide();
  void SaveAndQuit();
  void Exit();
  void ViewerSetupOkCallback();
  void ZoomPlusCallBack();
  void ZoomLessCallBack();
  void GridStep1CallBack();
  void GridStep2CallBack();
  void GridStep3CallBack();
  void GridStep4CallBack();
  void GridStep5CallBack();
  void AutoScaleCallBack();
  void AutoGridCallBack();
  void ResetListCallBack();
  void HideRollOverCurveCallBack();
  void ZoomRouletteCallBack();
  void StepRouletteCallBack();
  void DisplaySpectralAngleCallBack();
  void OpacityCallBack();
  void ExportSpectralImageCallBack();
  void ExportSpecralSpreadsheet();
  void IntensityCallBack();
  void NotMultispectralCallBack();
  void Notify(const std::string& event);
  void RefreshVisualization();
  void TriggerMonoChannelException();
  void ColorChannelCallBack();
  void ColorOrBWSpectralAngleCallBack();
  void ShowInfo();

protected:
  SpectrumModuleView();
  ~SpectrumModuleView();

private:
  /** Member (inner) */
  ImageViewPointerType         m_ImageView;
  PixelSpectrumViewPointerType m_PixelSpectrumView;

  /** Member (MVC relative) */
  SpectrumModuleModelPointerType      m_SpectrumModuleModel;
  SpectrumModuleControllerPointerType m_SpectrumModuleController;
  float                               m_Ylength;
  bool                                m_MonoChannelException;

};
}

#endif
