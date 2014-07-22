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
#ifndef __otbResampleModule_h
#define __otbResampleModule_h

#include <FL/Fl_Pixmap.H>

#include "FLU/flu_export.h"

// include the base class
#include "otbModule.h"
// include the GUI
#include "otbResampleGroup.h"

//include the view elements
#include "otbImage.h"
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

// Resmaple
#include "itkResampleImageFilter.h"
#include "otbStreamingResampleImageFilter.h"
#include "itkTranslationTransform.h"
#include "itkIdentityTransform.h"
#include "itkScaleTransform.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "otbCompositeTransform.h"
#include "itkScalableAffineTransform.h"

#include "otbBCOInterpolateImageFunction.h"

namespace otb
{


/** \class ResampleModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor
 */

class ITK_ABI_EXPORT ResampleModule
  : public Module, public ResampleGroup
{
public:
  /** Standard class typedefs */
  typedef ResampleModule                Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ResampleModule, Module);

  typedef TypeManager::Floating_Point_Precision   PrecisionType;
  typedef TypeManager::Floating_Point_VectorImage ImageType;

  /** Output image type */
  typedef itk::RGBAPixel<unsigned char> RGBPixelType;
  typedef Image<RGBPixelType, 2>         OutputImageType;

  /** Image layer type */
  typedef ImageLayer<ImageType, OutputImageType>         ImageLayerType;
  typedef ImageLayerType::Pointer                        ImageLayerPointerType;

  /** Image layer generator type */
  typedef ImageLayerGenerator<ImageLayerType>            ImageLayerGeneratorType;
  typedef ImageLayerGeneratorType::Pointer               ImageLayerGeneratorPointerType;

  /** Rendering model type */
  typedef ImageLayerRenderingModel<OutputImageType>      RenderingModelType;
  typedef RenderingModelType::Pointer                    RenderingModelPointerType;

  /** View type */
  typedef ImageView<RenderingModelType>                  ViewType;
  typedef ViewType::Pointer                              ViewPointerType;

  /** Widget controller */
  typedef ImageWidgetController                          WidgetControllerType;
  typedef WidgetControllerType::Pointer                  WidgetControllerPointerType;

  /** Standard action handlers */
  typedef otb::WidgetResizingActionHandler
    <RenderingModelType, ViewType>                        ResizingHandlerType;
  typedef otb::ChangeScaledExtractRegionActionHandler
    <RenderingModelType, ViewType>                        ChangeScaledRegionHandlerType;
  typedef otb::ChangeExtractRegionActionHandler
    <RenderingModelType, ViewType>                        ChangeRegionHandlerType;
  typedef otb::ChangeScaleActionHandler
    <RenderingModelType, ViewType>                        ChangeScaleHandlerType;
  typedef otb::ArrowKeyMoveActionHandler
    <RenderingModelType, ViewType>                        ArrowKeyMoveActionHandlerType;
  typedef RenderingModelType::RegionType                 RegionType;

  /** Filter for resampling */
  //typedef itk::ResampleImageFilter<ImageType, ImageType, double>         ResampleFilterType;
  typedef otb::StreamingResampleImageFilter<ImageType, ImageType, double>         ResampleFilterType;
  typedef itk::TranslationTransform<double, ImageType::ImageDimension>   TransformType;
  typedef itk::IdentityTransform<double, ImageType::ImageDimension>      IdentityTransformType;

  typedef itk::ScalableAffineTransform<double, ImageType::ImageDimension> ScalableTransformType;
  typedef ScalableTransformType::OutputVectorType                         OutputVectorType;

  typedef itk::NearestNeighborInterpolateImageFunction<ImageType, double> NearestNeighborInterpolateType;

  typedef itk::LinearInterpolateImageFunction<ImageType, double>          LinearInterpolateType;

  typedef otb::BCOInterpolateImageFunction<ImageType>                    BCOInterpolatorType;

  /** Scale transform */
  typedef itk::ScaleTransform<double, ImageType::ImageDimension> ScaleTransformType;
  typedef ScaleTransformType::ScaleType                          ScaleType;

  /** Rotation transform */
  typedef itk::CenteredRigid2DTransform< double > RotationTransformType;
  typedef RotationTransformType::ScalarType              ScalarType;

  /** Composite Transform */
  typedef otb::CompositeTransform<ScaleTransformType, RotationTransformType,
    ScalarType, ImageType::ImageDimension, ImageType::ImageDimension> CompositeTransformType;

  /** Set the input Image*/
  itkSetObjectMacro(InputImage, ImageType);

  /** Show the Module GUI */
  virtual bool CanShow(){return true; };

  /** Hide window */
  virtual void Quit();

protected:
  /** Constructor */
  ResampleModule();
  /** Destructor */
  virtual ~ResampleModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void OK();

  /** Show*/
  virtual void Show();

  /** Callback change Angle Rotation. */
  virtual void ChangeAngleRotation();

  /** Callback Change Interpolator. */
  virtual void ChangeInterpolator();

  /** Callback Change XFactor. */
  virtual void ChangeXFactor();

  /** Callback Change YFactor. */
  virtual void ChangeYFactor();

  /** Callback Toggle Proportion. */
  virtual void ToggleProportion();

  /** UpdateThresholdLayer */
  virtual void UpdateResampleLayer();

  /** Callback on the sliders*/
  virtual void UpdateDetails();

  virtual void UpdateSize();

  typedef enum {
    NEARESTNEIGHBOR,
    BICUBIC,
    LINEAR
  } INTERPOLATOR_TYPE;

private:
  ResampleModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Resample filter. */
  ResampleFilterType::Pointer              m_ResampleFilter;
  ScalableTransformType::Pointer           m_ScalableTransform;

  /** Interpolator */
  NearestNeighborInterpolateType::Pointer  m_NearestNeighborInterpolator;
  BCOInterpolatorType::Pointer           m_BCOInterpolator;
  LinearInterpolateType::Pointer           m_LinearInterpolator;

  /** Pointer to the image */
  ImageType::Pointer                       m_InputImage;
  ImageType::Pointer                       m_InputQuickLookImage;

  /** The image layer */
  ImageLayerPointerType                    m_InputImageLayer;
  ImageLayerPointerType                    m_ResampleImageLayer;


  /** The rendering model */
  RenderingModelPointerType                m_RenderingModel;

  /** The view */
  ViewPointerType                          m_View;

  /** The widget controller */
  WidgetControllerPointerType              m_Controller;

  /** Layer Generator*/
  ImageLayerGeneratorType::Pointer         m_ResampleGenerator;
  ImageLayerGeneratorType::Pointer         m_Generator;

  /** Flag to allow layer regeneration*/
  bool                                     m_HasToGenerateLayer;

  /** Handler */
  ChangeScaleHandlerType::Pointer          m_ChangeScaleHandler;

};


} // End namespace otb

#endif
