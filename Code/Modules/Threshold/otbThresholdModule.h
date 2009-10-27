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
#ifndef __otbThresholdModule_h
#define __otbThresholdModule_h

// include the base class
#include "otbModule.h"
// include the GUI
#include "otbThresholdGroup.h"

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

// Threshold
#include "itkThresholdImageFilter.h"

//Extract ROI
#include "otbExtractROI.h"

namespace otb
{

/** \class ThresholdModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT ThresholdModule
  : public Module, public ThresholdGroup
{
public:
  /** Standard class typedefs */
  typedef ThresholdModule               Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ThresholdModule,Module);

  typedef double                                         PrecisionType;
  typedef unsigned char                                  LabelType;
  typedef Image<PrecisionType,2>                         ImageType;
  typedef Image<LabelType,2>                             LabeledImageType;
  
  
  /** Output image type */
  typedef itk::RGBAPixel<unsigned char>                  RGBPixelType;
  typedef Image<RGBPixelType,2>                          OutputImageType;

  /** Image layer type */
  typedef ImageLayer<ImageType, OutputImageType>         ImageLayerType;
  typedef ImageLayerType::Pointer                        ImageLayerPointerType;
  typedef ImageLayerType::HistogramType                  HistogramType;
  
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
    <RenderingModelType,ViewType>                        ResizingHandlerType;
  typedef otb::ChangeScaledExtractRegionActionHandler
    <RenderingModelType,ViewType>                        ChangeScaledRegionHandlerType;
  typedef otb::ChangeExtractRegionActionHandler
    <RenderingModelType,ViewType>                        ChangeRegionHandlerType;
  typedef otb::ChangeScaleActionHandler
    <RenderingModelType,ViewType>                        ChangeScaleHandlerType;
  typedef otb::ArrowKeyMoveActionHandler
    <RenderingModelType,ViewType>                        ArrowKeyMoveActionHandlerType;
  
  /** Filter for thresholding*/
  typedef itk::ThresholdImageFilter<ImageType> ThresholdFilterType;
  
  /** Extract ROI Filter */
  typedef ExtractROI<PrecisionType,PrecisionType>        ExtractROIFilterType;

  /** Set the input Image*/
  itkSetObjectMacro(InputImage,ImageType);
  
protected:
  /** Constructor */
  ThresholdModule();
  /** Destructor */
  virtual ~ThresholdModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();
  
  /** Callbacks */
  virtual void OK();

  /** Callback on the sliders*/
  virtual void UpdateDetails();

  /** Update the sliders*/
  virtual void UpdateSlidersExtremum();
  
  /** Show*/
  virtual void Show();

  /** */
  virtual void AlphaBlending();
  
private:
  ThresholdModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Threshold filter */
  ThresholdFilterType::Pointer             m_ThresholdFilter;
  ThresholdFilterType::Pointer             m_ThresholdQuicklook;
  
  /** Pointer to the image */
  ImageType::Pointer                       m_InputImage;
  
  /** The image layer */
  ImageLayerPointerType                    m_InputImageLayer;
  ImageLayerPointerType                    m_ThresholdImageLayer;
  
  
  /** The rendering model */
  RenderingModelPointerType                m_RenderingModel;

  /** The view */
  ViewPointerType                          m_View;
  
  /** The widget controller */
  WidgetControllerPointerType              m_Controller;

  /** Extract ROI filter*/
  ExtractROIFilterType::Pointer            m_ExtractROI;

  /** Layer Generator*/
  ImageLayerGeneratorType::Pointer         m_ThresholdGenerator;
  ImageLayerGeneratorType::Pointer         m_Generator;
};


} // End namespace otb

#endif
