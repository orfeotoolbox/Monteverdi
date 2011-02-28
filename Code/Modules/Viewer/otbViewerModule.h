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
#ifndef __otbViewerModule_h
#define __otbViewerModule_h

// include the base class
#include "otbModule.h"

// include the OTB elements
#include "otbVectorImage.h"
#include "otbVectorData.h"

//standardImage Viewer
#include "otbViewerModuleGroup.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "itkRGBAPixel.h"
#include "otbImageLayer.h"
#include "otbImageLayerRenderingModel.h"
#include "otbImageView.h"
#include "otbImageWidgetController.h"
#include "otbImageLayerGenerator.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbArrowKeyMoveActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbHistogramActionHandler.h"
#include "otbCurves2DWidget.h"
#include "otbHistogramCurve.h"
#include "otbVerticalAsymptoteCurve.h"
#include "otbPixelDescriptionModel.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbPixelDescriptionView.h"
#include "otbStandardRenderingFunction.h"
#include "otbNoStretchRenderingFunction.h"
#include "otbSquareRootRenderingFunction.h"
#include "otbGaussianRenderingFunction.h"
#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"
#include "otbVectorDataProjectionFilter.h"
#include "otbVectorDataGlComponent.h"
#include "otbVectorDataExtractROI.h"
#include "otbRemoteSensingRegion.h"
#include "otbObjectList.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbDragFullWindowActionHandler.h"
#include "otbScalarBufferToImageFileWriter.h"

#include "otbAmplitudeFunctor.h"
#include "otbPhaseFunctor.h"

#include "otbWidgetManager.h"
#include "otbPackedWidgetManager.h"
#include "otbSplittedWidgetManager.h"
#include "otbMonteverdiEnum.h"
#include "otbUniformAlphaBlendingFunction.h"
#include "otbBlendingFunction.h"
#include "itkRescaleIntensityImageFilter.h"


namespace otb
{

const double ALPHA_BLENDING_DEFAULT   = 0.5;
const double ALPHA_BLENDING_OPAQUE    = 0.0;
const double ALPHA_BLENDING_INVISIBLE = 1.0;


/** \class ViewerModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */
class ITK_EXPORT ViewerModule
  : public Module, public ViewerModuleGroup
{
public:
  /** Standard class typedefs */
  typedef ViewerModule                  Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ViewerModule,Module);

  /** Dataset */
  typedef TypeManager::Floating_Point_Precision     PixelType;
  typedef TypeManager::Floating_Point_Image         SingleImageType;
  typedef TypeManager::Floating_Point_VectorImage   ImageType;
  typedef TypeManager::Vector_Data                  VectorDataType;

  /** WidgetManager related types definition */
  typedef WidgetManager                             WidgetManagerType;
  typedef WidgetManagerType::Pointer                WidgetManagerPointerType;

  typedef PackedWidgetManager                       PackedWidgetManagerType;
  typedef PackedWidgetManagerType::Pointer          PackedWidgetManagerPointerType;

  typedef SplittedWidgetManager                     SplittedWidgetManagerType;
  typedef SplittedWidgetManagerType::Pointer        SplittedWidgetManagerPointerType;

  /** Output image type */
  typedef itk::RGBAPixel<unsigned char>             RGBPixelType;
  typedef Image<RGBPixelType,2>                     OutputImageType;

  /** Image layer type */
  typedef ImageLayer<ImageType, OutputImageType>    ImageLayerType;
  typedef ImageLayerType::Pointer                   ImageLayerPointerType;
  typedef ImageLayerType::HistogramType             HistogramType;

  /** Image layer generator type */
  typedef ImageLayerGenerator<ImageLayerType>       ImageLayerGeneratorType;
  typedef ImageLayerGeneratorType::Pointer          ImageLayerGeneratorPointerType;

  /** Rendering model type */
  typedef ImageLayerRenderingModel<OutputImageType> RenderingModelType;
  typedef RenderingModelType::Pointer               RenderingModelPointerType;

  /** View type */
  typedef ImageView<RenderingModelType>             ViewType;
  typedef ViewType::Pointer                         ViewPointerType;

  /** Widget controller */
  typedef ImageWidgetController                     WidgetControllerType;
  typedef WidgetControllerType::Pointer             WidgetControllerPointerType;

  /** Curves 2D widget */
  typedef Curves2DWidget                            CurvesWidgetType;
  typedef CurvesWidgetType::Pointer                 CurvesWidgetPointerType;
  typedef HistogramCurve<HistogramType>             HistogramCurveType;
  typedef HistogramCurveType::ColorType             ColorType;
  typedef HistogramCurveType::Pointer               HistogramCurvePointerType;
  typedef VerticalAsymptoteCurve                    VerticalAsymptoteCurveType;
  typedef VerticalAsymptoteCurveType::Pointer       VerticalAsymptoteCurvePointerType;

  /** Rendering function */
  typedef Function::RenderingFunction<ImageType::PixelType, RGBPixelType> RenderingFunctionType;

  typedef Function::StandardRenderingFunction<ImageType::PixelType,
                                              RGBPixelType> StandardRenderingFunctionType;
  typedef StandardRenderingFunctionType::ChannelListType    ChannelListType;
  typedef StandardRenderingFunctionType::ParametersType     ParametersType;

  typedef Function::NoStretchRenderingFunction<ImageType::PixelType,
                                               RGBPixelType> NoStretchRenderingFunctionType;

  typedef Function::SquareRootRenderingFunction<ImageType::PixelType,
                                                RGBPixelType> SquareRootRenderingFunctionType;

  typedef Function::GaussianRenderingFunction<ImageType::PixelType,
                                              RGBPixelType> GaussianRenderingFunctionType;

  /** Blending function */
  typedef Function::UniformAlphaBlendingFunction<ImageLayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;

  /** Standard action handlers */
  typedef otb::WidgetResizingActionHandler
  <RenderingModelType,ViewType>                     ResizingHandlerType;
  typedef otb::ChangeScaledExtractRegionActionHandler
  <RenderingModelType,ViewType>                     ChangeScaledRegionHandlerType;
  typedef otb::ChangeExtractRegionActionHandler
  <RenderingModelType,ViewType>                     ChangeRegionHandlerType;
  typedef otb::ChangeScaleActionHandler
  <RenderingModelType,ViewType>                     ChangeScaleHandlerType;
  typedef otb::ArrowKeyMoveActionHandler
  <RenderingModelType,ViewType>                     ArrowKeyMoveActionHandlerType;
  typedef otb::HistogramActionHandler
    <RenderingModelType, ViewType,
     RenderingFunctionType>                         HistogramActionHandlerType;
  typedef otb::DragFullWindowActionHandler
  <RenderingModelType, ViewType>                    DragFullActionHandlerType;

  /** Pixel description */
  typedef PixelDescriptionModel<OutputImageType>     PixelDescriptionModelType;
  typedef  PixelDescriptionModelType::Pointer        PixelDescriptionModelPointerType;
  typedef PixelDescriptionActionHandler
    < PixelDescriptionModelType, ViewType>           PixelDescriptionActionHandlerType;
  typedef otb::PixelDescriptionView
    < PixelDescriptionModelType >                    PixelDescriptionViewType;
  typedef  PixelDescriptionViewType::Pointer         PixelDescriptionViewPointerType;

    /** VectorData overlay */
  typedef VectorDataProjectionFilter
  <VectorDataType,VectorDataType>                    VectorDataProjectionFilterType;
  typedef VectorDataExtractROI<VectorDataType>       VectorDataExtractROIType;
  typedef VectorDataExtractROIType::RegionType       RemoteSensingRegionType;
  typedef VectorDataGlComponent<VectorDataType>      VectorDataGlComponentType;
  typedef VectorDataFileReader<VectorDataType>       VectorDataFileReaderType;

  /** Cast SingleImage to VectorImageType*/
  typedef ImageToVectorImageCastFilter<SingleImageType,ImageType>     CastSingleImageFilter;

  /** VectorData ObjectList*/
  typedef ObjectList<VectorDataType>                 VectorDataListType;

  /** Image pointer list*/
  typedef ObjectList<ImageType>                      ImageListType;
  typedef ObjectList<ImageLayerType>                 ImageLayerListType;
  typedef ObjectList<RenderingFunctionType>          RenderingFunctionListType;
  typedef ObjectList<CastSingleImageFilter>          CastFilterListType;

  /** Screen shot typedefs */
  typedef ScalarBufferToImageFileWriter<unsigned char, unsigned char> ScreenShotFilterType;

  /**
   * FIXME: code refactoring in progress...
   */
 

  /** Set/Get the DEM directory */
  itkSetStringMacro(DEMDirectory);
  itkGetStringMacro(DEMDirectory);

  /** Set/Get the Label */
  itkSetStringMacro(Label);
  itkGetStringMacro(Label);

 protected:
  /** Constructor */
  ViewerModule();

  /** Destructor */
  virtual ~ViewerModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** CallBacks Implementations*/
  virtual void ChangeROIColor();
  virtual void UpdateListSelectionColor();
  virtual void AddNameToVectorDataBrowser(const std::string & name);
  virtual void SynchronizeAll();
  virtual void SetAlphaBlending();
  virtual void ActivateTransparencyMode();
  virtual void ActivateSlideShowMode();
  virtual void SetOpaqueImage();
  virtual void SetTransparentImage();
  virtual void ShowPreviousImage();
  virtual void ShowNextImage();
  virtual void ShowSelectedImages();
  virtual void ClearAll();
  virtual void DeleteVectorData();
  virtual void DisplaySelectedVectorData();
  virtual void DisplayVectorData();
  virtual void UpdateVectorData(unsigned int index);
  virtual ColorType SetRandomColor();
  virtual void RedrawWidget();
  virtual void UseDEM();
  virtual void UpdateDEMSettings();
  virtual void UpdateHistogramCurve();
  virtual void UpdateTabHistogram();
  virtual void UpdatePixelInformationWindow();
  virtual void UpdateVectorDataListShowed(unsigned int selectedItem, std::string status);
  virtual void ScreenShot(unsigned int id);

  /** Setup Color Composition Callbacks*/
  virtual void GrayScaleSet();
  virtual void RGBSet();
  virtual void SplittedLayout();
  virtual void PackedLayout();
  virtual void UpdateWindowsLayout(const WindowsLayoutEnumType windowsLayout);
  virtual void ViewerSetupOk();
  virtual void UpdateViewerSetupWindow();
  virtual void UpdateGrayScaleChannelOrder(int choice);
  virtual void UpdateRGBChannelOrder(int red, int green , int blue);
  virtual void TabSetupPosition();
  virtual void SetContrastStretch();
  virtual void UpdateQuantiles();
  virtual void UpdateQuantiles(double lowerQuantile, double upperQuantile);
  virtual void UpdateStandardDeviation();
  virtual void ShowHide();
  virtual void Quit();

  /** Update image size visu */
  virtual void UpdateInformation();

  /** Hide every window */
  virtual void Hide();

  /**
   * FIXME: code refactoring in progress...
   */
  virtual void RefreshTabData();
  virtual void RefreshTabSetup();
  virtual void RefreshTabHistogram();
  virtual void RefreshTabPixelDescription();


private:
  ViewerModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ImageListType::Pointer                   m_InputImageList;

  /** The image layers */
  ImageLayerListType::Pointer              m_InputImageLayerList;
  int                                      m_CurrentOpaqueImage;
  int                                      m_CurrentTransparentImage;
  std::vector<double>                      m_AlphaBlendingList;
  std::vector<ContrastStretchEnumType>     m_ContrastStretchList;

  /** VectorData List pointer*/
  VectorDataListType::Pointer              m_VectorDataList;

   /** The rendering model */
  RenderingModelPointerType                m_RenderingModel;

  /** The pixel description model */
  PixelDescriptionModelPointerType         m_PixelDescriptionModel;

  /** The view */
  ViewPointerType                          m_View;

    /** The pixel description view */
  PixelDescriptionViewPointerType          m_PixelDescriptionView;

  /** Curve widget */
  CurvesWidgetPointerType                  m_CurveWidget;

  /** The widget controller */
  WidgetControllerPointerType              m_Controller;

  /** Rendering function */
  RenderingFunctionListType::Pointer       m_RenderingFunctionList;

  /** FIXME: remove this from otbViewerModule.cxx file */
  std::string                              m_Label;

  /** Path to the DEMDirectory (used if a VectorData is rendered */
  std::string                              m_DEMDirectory;

  /** Management of the windows layout (splitted or packed windows) */
  WidgetManagerPointerType                 m_DisplayWindow;
  PackedWidgetManagerPointerType           m_PackedWindows;
  SplittedWidgetManagerPointerType         m_SplittedWindows;
  WindowsLayoutEnumType                    m_WindowsLayout;

  /** Management of the display mode (image overlay with transparency or slide show) */
  DisplayModeEnumType                      m_DisplayMode;

  /** Used colors*/
  HistogramCurveType::ColorType            m_Red;
  HistogramCurveType::ColorType            m_Green;
  HistogramCurveType::ColorType            m_Blue;
  HistogramCurveType::ColorType            m_Grey;

  /** Curve widget */
  CurvesWidgetPointerType                  m_BlueCurveWidgetGroup;
  CurvesWidgetPointerType                  m_GreenCurveWidgetGroup;
  CurvesWidgetPointerType                  m_RedCurveWidgetGroup;

  /** Vertical Asymptotes*/
  // Blue
  VerticalAsymptoteCurveType::Pointer      m_BlueVaCurveR;
  VerticalAsymptoteCurveType::Pointer      m_BlueVaCurveL;
  // Green
  VerticalAsymptoteCurveType::Pointer      m_GreenVaCurveR;
  VerticalAsymptoteCurveType::Pointer      m_GreenVaCurveL;
  // Red
  VerticalAsymptoteCurveType::Pointer      m_RedVaCurveR;
  VerticalAsymptoteCurveType::Pointer      m_RedVaCurveL;

  // Histogram Handlers
  HistogramActionHandlerType::Pointer      m_BlueHistogramHandler;
  HistogramActionHandlerType::Pointer      m_GreenHistogramHandler;
  HistogramActionHandlerType::Pointer      m_RedHistogramHandler;

  // Vector to handle the dispalyed vector data
  std::vector<bool>                        m_DisplayedVectorData;

  // Cast Filter
  CastFilterListType::Pointer              m_CastFilterList;

  // Labels
  std::string                              m_DisplayedLabel;
  std::string                              m_UndisplayedLabel;

  // Color List
  std::vector<ColorType>                   m_ColorList;

  bool                                     m_GenerateRandomColor;

  /**
   * FIXME: code refactoring in progress...
   */
  /*
  std::vector<ImageLayerInfo> m_ImageLayerInfoList;
  */


};


} // End namespace otb

#endif /* __otbViewerModule_h */
