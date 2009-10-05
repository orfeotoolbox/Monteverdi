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
//#include "otbImageFileViewer.h"

#include "otbVectorData.h"

//standardImage Viewer
#include "otbViewerModuleGroup.h"

//
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
#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"
#include "otbVectorDataProjectionFilter.h"
#include "otbVectorDataGlComponent.h"
#include "otbVectorDataExtractROI.h"
#include "otbRemoteSensingRegion.h"
#include "otbObjectList.h"

#include "otbAmplitudeFunctor.h"
#include "otbPhaseFunctor.h"

#include "otbPackedWidgetManager.h"

namespace otb
{
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
  typedef double                                    PixelType;
  typedef VectorImage<PixelType,2>                  ImageType;
  typedef VectorData<PixelType>                     VectorDataType;
  
  /** PackedWidgetManager typedef*/
  typedef PackedWidgetManager                       WidgetManagerType;
  typedef WidgetManagerType::Pointer                WidgetManagerPointerType;

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
  typedef  ImageLayerGeneratorType::RenderingFunctionType    RenderingFunctionType;
  typedef  RenderingFunctionType::Pointer                    RenderingFunctionPointerType;
  
  typedef Function::StandardRenderingFunction<ImageType::PixelType,
                                              RGBPixelType>  StandardRenderingFunctionType;
  typedef  StandardRenderingFunctionType::Pointer            StandardRenderingFunctionPointerType;
  typedef  StandardRenderingFunctionType::ChannelListType    ChannelListType;
  typedef  StandardRenderingFunctionType::ParametersType     ParametersType;

  typedef Function::StandardRenderingFunction<ImageType::PixelType, RGBPixelType,
    otb::Function::AmplitudeFunctor<ImageType::PixelType> >                             AmplitudeRenderingFunction;
  typedef Function::StandardRenderingFunction<ImageType::PixelType, RGBPixelType,
    otb::Function::PhaseFunctor<ImageType::PixelType> >                                 PhaseRenderingFunction;
 
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
    <RenderingModelType,
    ViewType,
    StandardRenderingFunctionType>                  HistogramActionHandlerType;

    
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
  typedef  VectorDataExtractROIType::RegionType      RemoteSensingRegionType;
  typedef VectorDataGlComponent<VectorDataType>      VectorDataGlComponentType;
  typedef VectorDataFileReader<VectorDataType>       VectorDataFileReaderType;

  /** VectorData ObjectList*/
  typedef ObjectList<VectorDataType>                 VectorDataListType;

  /** Set InputImage*/
  itkSetObjectMacro(InputImage,ImageType);

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

  /** Callbacks */
  //virtual void OpenDataSet();

  /** Assign input by key.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  //virtual void AssignInputByKey(const std::string & key, const DataObjectWrapper & data);

  /** CallBacks Implementations*/
  virtual void ChangeROIColor();
  virtual void UpdateListSelectionColor();
  virtual void AddName( std::string name );
  virtual void ClearAll();
  virtual void DeleteVectorData();
  virtual void UpdateVectorData(unsigned int index);
  virtual ColorType SetRandomColor();
  virtual void RedrawWidget();
  virtual void UseDEM();
  virtual void UpdateDEMSettings();
  virtual void UpdateHistogramCurve();
  virtual void UpdateTabHistogram();
  

  /** Setup Color Composition Callbacks*/
  virtual void GrayScaleSet();
  virtual void RGBSet();
  virtual void ViewerSetupOk();
  virtual void ComplexSet();
  virtual void UpdateViewerSetupWindow();
  virtual void UpdateGrayScaleChannelOrder(int choice);
  virtual void UpdateRGBChannelOrder(int red, int green , int blue);
  virtual void UpdateAmplitudeChannelOrder(int realChoice, int imChoice);
  virtual void UpdatePhaseChannelOrder(int realChoice, int imChoice);
  virtual void TabSetupPosition();
  virtual void UpdateUpperQuantile();
  virtual void UpdateLowerQuantile();
  
  
private:
  ViewerModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  /** Pointer to the image */
  ImageType::Pointer                       m_InputImage;
  
  /** VectorData List pointer*/
  VectorDataListType::Pointer              m_VectorDataList;
  
  /** The image layer */
  ImageLayerPointerType                    m_InputImageLayer;
  
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

  /** StandardRenderingFunction */
  RenderingFunctionPointerType             m_RenderingFunction;

  /** StandardRendering Function*/
  StandardRenderingFunctionPointerType     m_StandardRenderingFunction;

  /** */
  std::string                              m_Label;

  /** Path to the DEMDirectory (used if a VectorData is rendered */
  std::string                             m_DEMDirectory;
  
  /** */
  WidgetManagerPointerType                m_DisplayWindow;
  
  /** Used colors*/
  HistogramCurveType::ColorType           m_Red;
  HistogramCurveType::ColorType           m_Green;
  HistogramCurveType::ColorType           m_Blue;
  HistogramCurveType::ColorType           m_Grey;

  /** Curve widget */
  CurvesWidgetPointerType                 m_BlueCurveWidgetGroup;
  CurvesWidgetPointerType                 m_GreenCurveWidgetGroup;
  CurvesWidgetPointerType                 m_RedCurveWidgetGroup;
  
  /** Vertical Asymptotes*/
  // Blue
  VerticalAsymptoteCurveType::Pointer     m_BlueVaCurveR;
  VerticalAsymptoteCurveType::Pointer     m_BlueVaCurveL;
  // Green
  VerticalAsymptoteCurveType::Pointer     m_GreenVaCurveR;
  VerticalAsymptoteCurveType::Pointer     m_GreenVaCurveL;
  // Red
  VerticalAsymptoteCurveType::Pointer     m_RedVaCurveR;
  VerticalAsymptoteCurveType::Pointer     m_RedVaCurveL;

  HistogramActionHandlerType::Pointer     m_HistogramHandler;
  HistogramActionHandlerType::Pointer     m_BlueHistogramHandler;
  HistogramActionHandlerType::Pointer     m_GreenHistogramHandler;
  HistogramActionHandlerType::Pointer     m_RedHistogramHandler;
  
  
  
};


} // End namespace otb

#endif
