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
#ifndef __otbUncompressJpeg2000Module_h
#define __otbUncompressJpeg2000Module_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbUncompressJpeg2000ModuleGUI.h"

// include the OTB elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbMultiChannelExtractROI.h"

#include "itkRGBPixel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbImageView.h"
#include "otbImageLayerRenderingModel.h"
#include "otbImageWidgetController.h"

#include "otbWidgetResizingActionHandler.h"
#include "otbSelectTileROIActionHandler.h"

#include "otbImageFileWriter.h"
namespace otb
{
/** \class UncompressJpeg2000Module
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT UncompressJpeg2000Module
  : public Module, public UncompressJpeg2000ModuleGUI
{
public:
  /** Standard class typedefs */
  typedef UncompressJpeg2000Module              Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(UncompressJpeg2000Module, Module);

  /** OTB typedefs */
  typedef TypeManager::Floating_Point_Precision   InternalPixelType;
  typedef TypeManager::Floating_Point_Image       FloatingImageType;
  typedef TypeManager::Floating_Point_VectorImage FloatingVectorImageType;

  typedef TypeManager::FloatImageWithQuicklook    FloatImageWithQuicklook;

  typedef FloatingVectorImageType::RegionType  VectorImageRegionType;
  typedef FloatingImageType::RegionType        ImageRegionType;

  /** Multi channels Extract ROI filter */
  typedef MultiChannelExtractROI<InternalPixelType, InternalPixelType> VectorImageExtractROIFilterType;

  /** Typedef to cast physical point to index*/
  typedef FloatingImageType::IndexType IndexType;
  typedef FloatingImageType::SizeType SizeType;
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
  typedef otb::SelectTileROIActionHandler<ModelType, WidgetType> SelectAreaHandlerType;
  
  typedef otb::ImageFileWriter<FloatingVectorImageType> FPVWriterType;

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }
  
  /** Show the Module GUI */
  virtual void Show()
  {
    wExtractROIWindow->show();
    m_View->GetScrollWidget()->show();
    m_Model->Update();
  }

  /** Set/Get Macro for m_CheckFileExistance flag */
  itkSetMacro(CheckFileExistance, bool);
  itkGetMacro(CheckFileExistance, bool);

  /** Set function for the output filename */
  void SetFilename(std::string name)
    {
    m_Filename = name;
    }

protected:
  /** Constructor */
  UncompressJpeg2000Module();
  /** Destructor */
  virtual ~UncompressJpeg2000Module();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void Ok();
  virtual void Cancel();
  virtual void UpdateRegion();
  virtual void ThreadedRun();
  virtual void ThreadedWatch();

  /** Update the progress bar */
  void UpdateProgress();
  
  /** Close window */
  void HideWindow();

private:
  /** Callback to update the window label */
  static void UpdateProgressCallback(void * data);

  /** Callback to hide window */
  static void HideWindowCallback(void * data);

  /** Callback to Error reporter window */
  static void SendErrorCallback(void * data);

  UncompressJpeg2000Module(const Self&);   //purposely not implemented
  void operator =(const Self&);  //purposely not implemented

  /** Pointer to the vector image extract ROI filter object */
  VectorImageExtractROIFilterType::Pointer m_VectorImageExtractROIFilter;

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

  /** Region for selected area */
  RegionGlComponentType::Pointer m_RegionGl;

  /** Region for selected tiles */
  RegionGlComponentType::Pointer m_TileRegionGl;

  /** Image resolution */
  unsigned int m_ResolutionFactor;

  /** Tile sizes*/
  unsigned int m_TileHintX;
  unsigned int m_TileHintY;
  
  /** Number of bands */
  unsigned int m_NbBands;
  
  /** Pointer to a process object (containing the writer) */
  itk::ProcessObject::Pointer m_ProcessObject;
  
  /** Flag to check file existance */
  bool m_CheckFileExistance;
  
  /** Error message */
  std::string m_ErrorMsg;
  
  /** Output filename */
  std::string m_Filename;
};

} // End namespace otb

#endif
