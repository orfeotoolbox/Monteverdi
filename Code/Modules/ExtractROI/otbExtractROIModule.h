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
#ifndef __otbExtractROIModule_h
#define __otbExtractROIModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbExtractROIModuleGUI.h"

// include the OTB elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbExtractROI.h"
#include "otbMultiChannelExtractROI.h"

#include "itkRGBPixel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbImageView.h"
#include "otbImageLayerRenderingModel.h"
#include "otbImageWidgetController.h"

#include "otbWidgetResizingActionHandler.h"
#include "otbSelectAreaROIActionHandler.h"

#include "otbInverseSensorModel.h"
#include "otbGenericRSTransform.h"
namespace otb
{
/** \class ExtractROIModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT ExtractROIModule
  : public Module, public ExtractROIModuleGUI
{
public:
  /** Standard class typedefs */
  typedef ExtractROIModule              Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ExtractROIModule, Module);

  /** OTB typedefs */
  typedef TypeManager::Floating_Point_Precision   InternalPixelType;
  typedef TypeManager::Floating_Point_Image       FloatingImageType;
  typedef TypeManager::Floating_Point_VectorImage FloatingVectorImageType;

  /// Multi channels Extract ROI filter
  typedef MultiChannelExtractROI<InternalPixelType, InternalPixelType> VectorImageExtractROIFilterType;
  /// Mono channel Extract ROI filter
  typedef ExtractROI<InternalPixelType, InternalPixelType> ImageExtractROIFilterType;

  /** Inverse sensor model typedefs */
  typedef InverseSensorModel<double>         InverseSensorType;
  typedef InverseSensorType::InputPointType  InverseSensorInputPointType;
  typedef InverseSensorType::OutputPointType InverseSensorOutputPointType;

  typedef GenericRSTransform<>           TransformType;
  typedef TransformType::OutputPointType OutputPointType;

  /** Typedef to cast physical point to index*/
  typedef FloatingImageType::IndexType IndexType;
  typedef IndexType::OffsetType        OffsetType;
  typedef FloatingImageType::PointType PointType;

  /** Typedefs to add view */
  typedef itk::RGBPixel<unsigned char>                   RGBPixelType;
  typedef otb::Image<RGBPixelType, 2>                    OutputImageType;
  typedef otb::ImageLayerRenderingModel<OutputImageType> ModelType;
  typedef otb::ImageView<ModelType>                      ViewType;
  typedef otb::ImageWidgetController                     ControllerType;

  typedef otb::ImageLayer<FloatingImageType, OutputImageType> LayerType;
  typedef otb::ImageLayerGenerator<LayerType>                 LayerGeneratorType;

  typedef otb::ImageLayer<FloatingVectorImageType, OutputImageType> VectorLayerType;
  typedef otb::ImageLayerGenerator<VectorLayerType>                 VectorLayerGeneratorType;

  typedef otb::WidgetResizingActionHandler<ModelType, ViewType> ResizingHandlerType;

  typedef ViewType::ImageWidgetType                              WidgetType;
  typedef ViewType::RegionGlComponentType                        RegionGlComponentType;
  typedef RegionGlComponentType::ColorType                       ColorType;
  typedef otb::SelectAreaROIActionHandler<ModelType, WidgetType> SelectAreaHandlerType;

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }
  /** Show the Module GUI */
  virtual void Show()
  {
    wExtractROIWindow->show();
  }

protected:
  /** Constructor */
  ExtractROIModule();
  /** Destructor */
  virtual ~ExtractROIModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void Ok();
  virtual void Cancel();
  virtual void UpdateRegion();
  virtual void UpdateRegionWithLatLong();

private:

  ExtractROIModule(const Self&);   //purposely not implemented
  void operator =(const Self&);  //purposely not implemented

  /** Pointer to the image extract ROI filter object */
  ImageExtractROIFilterType::Pointer m_ImageExtractROIFilter;
  /** Pointer to the vector image extract ROI filter object */
  VectorImageExtractROIFilterType::Pointer m_VectorImageExtractROIFilter;

  TransformType::Pointer m_Transform;
  TransformType::Pointer m_InverseTransform;

  /** Pointer to the Model */
  ModelType::Pointer m_Model;

  /** Pointer to the Layer generator */
  LayerGeneratorType::Pointer       m_Generator;
  VectorLayerGeneratorType::Pointer m_VectorGenerator;

  /** Pointer to the view */
  ViewType::Pointer m_View;

  /** Pointer to the controller */
  ControllerType::Pointer m_Controller;

  /** Pointer to the handlers */
  ResizingHandlerType::Pointer   m_ResizingHandler;
  SelectAreaHandlerType::Pointer m_SelectAreaHandler;

  // Region for select area
  RegionGlComponentType::Pointer m_regionGl;

  bool isNotAProjection;

};

} // End namespace otb

#endif
