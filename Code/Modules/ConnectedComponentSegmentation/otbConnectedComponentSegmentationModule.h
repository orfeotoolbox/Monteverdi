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
#ifndef __otbConnectedComponentSegmentationModule_h
#define __otbConnectedComponentSegmentationModule_h

#include "otbModule.h"
#include "otbConnectedComponentSegmentationModuleGUI.h"

#include "itkRGBAPixel.h"

#include <itkScalarToRGBPixelFunctor.h>
#include <itkUnaryFunctorImageFilter.h>

#include "otbRelabelComponentImageFilter.h"

#include "otbImageLayer.h"
#include "otbImageLayerRenderingModel.h"
#include "otbImageView.h"

#include "otbImageWidgetController.h"
#include "otbImageLayerGenerator.h"
#include "otbWidgetResizingActionHandler.h"
// include the OTB elements
#include "otbMultiToMonoChannelExtractROI.h"

// Connected component filter
#include "otbConnectedComponentMuParserFunctor.h"
#include <itkConnectedComponentFunctorImageFilter.h>

//Mask Mu parser filter
#include "otbMaskMuParserFilter.h"

#include "itkMaskImageFilter.h"

// OBIA filtering
#include <otbAttributesMapLabelObject.h>
#include <otbLabelImageToLabelMapWithAdjacencyFilter.h>
#include "otbLabelObjectOpeningMuParserFilter.h"

#include <itkAttributeLabelObject.h>
#include "otbBandsStatisticsAttributesLabelMapFilter.h"
#include <itkStatisticsLabelObject.h>
#include <itkShapeLabelMapFilter.h>
#include <otbShapeAttributesLabelMapFilter.h>
#include <itkLabelObject.h>
#include "itkLabelImageToShapeLabelMapFilter.h"
#include <iostream>
#include <otbLabelMapToLabelImageFilter.h>

//#include <otbLabelImageToLabelMapWithAdjacencyFilter.h>

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbMouseClickActionHandler.h"

#include "otbImageLayerGenerator.h"
#include "otbPixelDescriptionModel.h"
#include "otbPixelDescriptionView.h"
#include "otbPixelDescriptionActionHandler.h"

#include "otbImage.h"
#include <otbLabelMapToVectorDataFilter.h>
#include "otbVectorDataProjectionFilter.h"
#include "otbVectorData.h"

#include "otbNoStretchRenderingFunction.h"

#include "otbStreamingConnectedComponentSegmentationOBIAToVectorDataFilter.h"

#include "otbObjectDescriptionActionHandler.h"

namespace otb
{
/** \class ConnectedComponentSegmentationModule
 *  \brief This is the Connected Component Segmentation module
 *
 *
 */
const unsigned int     Dimension = 2;

class ITK_EXPORT ConnectedComponentSegmentationModule
  : public Module, public ConnectedComponentSegmentationModuleGUI
{
public:

  /** Standard class typedefs */
  typedef ConnectedComponentSegmentationModule Self;
  typedef Module Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ConnectedComponentSegmentationModule, Module);

  // Convenient typedefs
  typedef TypeManager::Floating_Point_Image ImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef otb::Image<unsigned int, Dimension> LabelImageType;

  typedef itk::RGBPixel<unsigned char> RGBPixelType;
  typedef Image<RGBPixelType, Dimension> LayerOutputImageType;
  typedef Image<RGBPixelType, Dimension> RGBImageType;

  typedef otb::VectorData<double, Dimension> VectorDataType;
  typedef VectorDataType::Pointer VectorDataPointerType;

  typedef VectorImageType::PixelType PixelType;
  typedef Parser ParserType;
  typedef ImageType::RegionType RegionType;

  /** Image layer type */
  typedef ImageLayer<VectorImageType, LayerOutputImageType> VectorImageLayerType;
  typedef VectorImageLayerType::Pointer VectorImageLayerPointerType;

  typedef ImageLayer<ImageType, LayerOutputImageType> ImageLayerType;
  typedef ImageLayerType::Pointer ImageLayerPointerType;

  typedef ImageLayer<RGBImageType, LayerOutputImageType> RGBImageLayerType;
  typedef RGBImageLayerType::Pointer RGBImageLayerPointerType;

  typedef ImageLayer<LabelImageType, LayerOutputImageType> LabelLayerType;
  typedef LabelLayerType::Pointer LabelLayerPointerType;

  /** layer generator type */
  typedef ImageLayerGenerator<VectorImageLayerType> VectorImageLayerGeneratorType;
  typedef VectorImageLayerGeneratorType::Pointer VectorImageLayerGeneratorPointerType;

  typedef ImageLayerGenerator<ImageLayerType> ImageLayerGeneratorType;
  typedef ImageLayerGeneratorType::Pointer ImageLayerGeneratorPointerType;

  typedef ImageLayerGenerator<RGBImageLayerType> RGBImageLayerGeneratorType;
  typedef ImageLayerGeneratorType::Pointer RGBImageLayerGeneratorPointerType;

  typedef ImageLayerGenerator<LabelLayerType> LabelLayerGeneratorType;
  typedef LabelLayerGeneratorType::Pointer LabelLayerGeneratorPointerType;

  /** Rendering model type */
  typedef ImageLayerRenderingModel<LayerOutputImageType> RenderingModelType;
  typedef RenderingModelType::Pointer RenderingModelPointerType;

  /** Pixel type */
  typedef PixelDescriptionModel<RGBImageType> PixelDescriptionModelType;
  typedef PixelDescriptionView<PixelDescriptionModelType> PixelViewType;

  /** View type */
  typedef ImageView<RenderingModelType> ViewType;

  typedef ViewType::Pointer ViewPointerType;

  /** Widget controller */
  typedef ImageWidgetController WidgetControllerType;
  typedef WidgetControllerType::Pointer WidgetControllerPointerType;
  typedef ViewType::ImageWidgetType                              WidgetType;

  /** handler **/
  typedef otb::WidgetResizingActionHandler<RenderingModelType, ViewType> ResizingHandlerType;
  typedef otb::ChangeExtractRegionActionHandler<RenderingModelType, ViewType> ChangeRegionHandlerType;
  typedef otb::ChangeScaledExtractRegionActionHandler<RenderingModelType, ViewType> ChangeScaledRegionHandlerType;
  typedef otb::ChangeScaleActionHandler<RenderingModelType, ViewType> ChangeScaleHandlerType;
  typedef otb::PixelDescriptionActionHandler<PixelDescriptionModelType, ViewType> PixelDescriptionActionHandlerType;
  typedef otb::ObjectDescriptionActionHandler<PixelDescriptionModelType, ViewType> ObjectDescriptionActionHandlerType;


  // colored label image typedef
  typedef itk::Functor::ScalarToRGBPixelFunctor<unsigned long> ColorMapFunctorType;
  typedef itk::UnaryFunctorImageFilter<LabelImageType, RGBImageType, ColorMapFunctorType> ColorMapFilterType;

  typedef Functor::ConnectedComponentMuParserFunctor<VectorImageType::PixelType> CCFunctorType;

  typedef itk::ConnectedComponentFunctorImageFilter<VectorImageType, LabelImageType, CCFunctorType, ImageType>
      ConnectedComponentFilterType;

  typedef otb::MaskMuParserFilter<VectorImageType, ImageType> MaskMuParserFilterType;
  typedef itk::MaskImageFilter<VectorImageType, ImageType, VectorImageType> MaskImageFilterType;


  typedef ImageLayerGeneratorType::ImageLayerType::OutputPixelType LayerOutputPixelType;
  typedef Function::UniformAlphaBlendingFunction<LayerOutputPixelType> BlendingFunctionType;

  // Labelization
  typedef otb::RelabelComponentImageFilter<LabelImageType, LabelImageType> RelabelComponentFilterType;
  typedef otb::AttributesMapLabelObject<unsigned int, Dimension, double> AttributesMapLabelObjectType;
  typedef itk::AttributeLabelObject<unsigned int, Dimension, double> AttributesLabelObjectType;

  typedef otb::LabelMapWithAdjacency<AttributesMapLabelObjectType> AttributesLabelMapType;
  typedef otb::LabelImageToLabelMapWithAdjacencyFilter<LabelImageType, AttributesLabelMapType>
      LabelImageToLabelMapFilterType;

  typedef otb::LabelObjectOpeningMuParserFilter<AttributesLabelMapType> LabelObjectOpeningFilterType;

  typedef otb::BandsStatisticsAttributesLabelMapFilter<AttributesLabelMapType, VectorImageType>
      RadiometricLabelMapFilterType;
  typedef otb::ShapeAttributesLabelMapFilter<AttributesLabelMapType> ShapeLabelMapFilterType;
  typedef itk::ShapeLabelObject<unsigned int, Dimension> ShapeLabelObjectType;
  typedef itk::LabelObject<unsigned int, Dimension> LabelObjectType;
  typedef itk::LabelMap<ShapeLabelObjectType> ShapeLabelMapType;
  typedef otb::LabelMapToLabelImageFilter<AttributesLabelMapType, LabelImageType> LabelMapToLabelImageFilterType;

  typedef otb::LabelMapToVectorDataFilter<AttributesLabelMapType, VectorDataType> LabelMapToVectorDataFilterType;

  typedef otb::StreamingConnectedComponentSegmentationOBIAToVectorDataFilter<VectorImageType, LabelImageType,
      ImageType, VectorDataType> StreamingConnectedComponentSegmentationOBIAToVectorDataFilterType;

  typedef Function::RenderingFunction<ImageType::PixelType, RGBPixelType> RenderingFunctionType;

  typedef Function::StandardRenderingFunction<ImageType::PixelType,
                                                RGBPixelType> StandardRenderingFunctionType;

  typedef Function::NoStretchRenderingFunction<RGBPixelType,
      RGBPixelType> NoStretchRenderingFunctionType;

  typedef StandardRenderingFunctionType::ParametersType     ParametersType;

  typedef enum
  {
    INPUT_IMAGE,
    MASK,
    MASKED_IMAGE,
    CONNECTED_COMPONENT_SEGMENTATION_OUTPUT,
    SEGMENTATION_AFTER_SMALL_OBJECTS_REJECTION,
    OUTPUT,
  } TmpOutputEnumType;

public:

  /** Expression live checking */
  virtual void LiveCheckCC();
  virtual void LiveCheckMask();
  virtual void LiveCheckOBIA();

  /** DEM directory accessors. */
  itkGetConstMacro(DEMPath, std::string);
  itkSetMacro(DEMPath, std::string);

  /** Use DEM or not accessor. */
  itkGetMacro(UseDEM, bool);
  itkSetMacro(UseDEM, bool);

protected:
  /** Constructor */
  ConnectedComponentSegmentationModule();

  /** Destructor */
  virtual ~ConnectedComponentSegmentationModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow()
  {
    return true;
  }

  /** Show the Module GUI */
  virtual void Show();

  /** Hide the Module GUI */
  virtual void Hide();

  /** OK callback*/
  virtual void OK();

  /** Cancel callback*/
  virtual void Cancel();

  /** Help Initialization */
  virtual void InitHelp();

  /** Quick add a variable name into the expression */
  virtual void QuickAddMask();
  virtual void QuickAddCC();
  virtual void QuickAddOBIA();


  virtual void SetObjectMinArea();

  virtual void UpdateTmpOutput();

  /* test if connected component segmentation is possible*/
  virtual void CheckProcess();

  /** Process CC Callback*/
  virtual void Process();

  /** Tmp Output Choice **/
  virtual void TmpOutputSelection();

  /** Generate the mask layer **/
  virtual void UpdateMaskLayer();
  virtual void UpdateCCSegmentationLayer();
  virtual void UpdateRelabelLayer();
  virtual void OBIA_functor_init();
  virtual void UpdateOBIAOpeningLayer();

  /** update variable list **/
  virtual void UpdateMaskFormulaVariablesList();
  virtual void UpdateCCFormulaVariablesList();
  virtual void UpdateOBIAFormulaVariablesList();


private:
  ConnectedComponentSegmentationModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Class attributes

  itkGetObjectMacro(PixelView, PixelViewType)
;
  // itkGetObjectMacro(VisualizationModel, VisualizationModelType);
  itkGetObjectMacro(PixelDescriptionModel, PixelDescriptionModelType)
;


  // IO

  /** Pointer to the image */
  VectorImageType::Pointer m_InputImage;

  /** Pointer to the mask output */
  ImageType::Pointer m_InputMask;

  RGBImageType::Pointer m_OutputRGBLabelImage;
  VectorDataPointerType m_OutputVectorData;

  AttributesLabelMapType::Pointer m_OutputLabelMap;

  // Filter
  ConnectedComponentFilterType::Pointer m_CCFilter;
  MaskMuParserFilterType::Pointer m_MaskFilter;
  MaskImageFilterType::Pointer m_MaskImageFilter;
  RelabelComponentFilterType::Pointer m_CCRelabelFilter;
  ColorMapFilterType::Pointer m_CCColorMapper;
  ColorMapFilterType::Pointer m_RelabelColorMapper;
  LabelObjectOpeningFilterType::Pointer m_OBIAOpeningFilter;
  LabelMapToLabelImageFilterType::Pointer m_OBIAOpeningLabelMapToLabelImageFilter;
  ColorMapFilterType::Pointer m_OBIAOpeningColorMapper;
  LabelImageToLabelMapFilterType::Pointer m_CCImageToCCLabelMapFilter;
  ShapeLabelMapFilterType::Pointer m_ShapeLabelMapFilter;
  RadiometricLabelMapFilterType::Pointer m_RadiometricLabelMapFilter;
  LabelMapToVectorDataFilterType::Pointer m_OBIAOpeningLabelMapToVectorDataFilter;

  AttributesLabelMapType::Pointer m_AttributesLabelMap;

  bool m_IsMaskExpressionOK;
  bool m_NoMask;
  bool m_NoOBIAOpening;
  bool m_IsCCExpressionOK;
  bool m_IsOBIAExpressionOK;

  std::string m_CurrentExpressionMask;
  std::string m_CurrentExpressionCC;
  std::string m_CurrentExpressionOBIA;

  std::ostringstream m_FunList;

  /** DEM directory path. */
   std::string                   m_DEMPath;

   /** Use DEM or not. */
   bool                          m_UseDEM;


  unsigned int m_MinObjectSize;

  unsigned int m_NumberOfInputBands;

  // MVC Variables

  /** Action handlers */
  ResizingHandlerType::Pointer m_ResizingHandler;
  ChangeRegionHandlerType::Pointer m_ChangeRegionHandler;
  ChangeScaledRegionHandlerType::Pointer m_ChangeScaledRegionHandler;
  ChangeScaleHandlerType::Pointer m_ChangeScaleHandler;
  PixelDescriptionActionHandlerType::Pointer m_PixelActionHandler;
  ObjectDescriptionActionHandlerType::Pointer m_ObjectDescriptionActionHandler;

  /** The widget controller */
  WidgetControllerPointerType m_WidgetsController;

  /** The view */
  ViewPointerType m_View;

  /** Pixel view */
  PixelViewType::Pointer m_PixelView;

  /** Layer Generator*/
  /** The image layer */
  VectorImageLayerPointerType m_InputImageLayer;
  VectorImageLayerGeneratorType::Pointer m_ImageGenerator;
  ImageLayerGeneratorType::Pointer m_MaskGenerator;
  VectorImageLayerGeneratorType::Pointer m_MaskedImageGenerator;
  RGBImageLayerGeneratorType::Pointer m_CCSegmentationGenerator;
  RGBImageLayerGeneratorType::Pointer m_RelabelRGBGenerator;
  RGBImageLayerGeneratorType::Pointer m_OBIAOpeningGenerator;

  RenderingFunctionType::Pointer m_MaskRenderer;

  LabelLayerGeneratorType::Pointer m_CCSegmentationLabelGenerator;
  LabelLayerGeneratorType::Pointer m_RelabelGenerator;
  LabelLayerGeneratorType::Pointer m_OBIAOpeningLabelGenerator;

  /** Pixel description model */
  PixelDescriptionModelType::Pointer m_PixelDescriptionModel;

  // flag for layer generation
  bool m_HasToGenerateMaskLayer;
  bool m_HasToGenerateCCSegmentationLayer;
  bool m_HasToGenerateRelabelLayer;
  bool m_HasToGenerateOBIAOpeningLayer;

  // output first generation flag
  bool m_MaskOutputReady;
  bool m_CCSegmentationReady;
  bool m_RelabelOutputReady;
  bool m_OBIAOpeningOutputReady;

  // attributes
  bool m_ShapeReducedSetOfAttributes;
  bool m_ComputeFlusser;
  bool m_ComputePolygon;
  bool m_StatsReducedSetOfAttributes;
  bool m_ComputeFeretDiameter;
  bool m_ComputePerimeter;

  /** The rendering model */
  RenderingModelPointerType m_RenderingModel;

  std::vector<std::string> m_ShapeAttributes;
  std::vector<std::string> m_StatAttributes;

  unsigned int m_NbOfObjectsAfterCC;
  unsigned int m_NbOfObjectsAfterOBIA;

};

} // End namespace otb

#endif
