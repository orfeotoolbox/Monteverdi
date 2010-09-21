#ifndef __otbMeanShiftModuleModel_h
#define __otbMeanShiftModuleModel_h

#include "otbMVCModel.h"
#include "otbEventsSender.h"
#include "otbTypeManager.h"

#include "otbVectorImage.h"
#include "otbImage.h"

//Vis
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbMeanShiftVectorImageFilter.h"

namespace otb {

/** \class MeanShiftModuleModel
 *
 *
 *
 */
class ITK_EXPORT MeanShiftModuleModel
  : public EventsSender<std::string>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef MeanShiftModuleModel          Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(MeanShiftModuleModel, Object);

  typedef TypeManager::Floating_Point_Precision   PixelType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef VectorImageType::Pointer                VectorImagePointerType;

  typedef TypeManager::Labeled_Short_Image LabeledImageType;
  typedef LabeledImageType::Pointer        LabeledImagePointerType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                                 RGBPixelType;
  typedef Image<RGBPixelType, 2>                                       RGBImageType;
  typedef ImageLayer<VectorImageType, RGBImageType>                    LayerType;
  typedef ImageLayer<LabeledImageType, RGBImageType>                   LabelLayerType;
  typedef ImageLayerGenerator<LayerType>                               LayerGeneratorType;
  typedef ImageLayerGenerator<LabelLayerType>                          LabelLayerGeneratorType;
  typedef ImageLayerRenderingModel<RGBImageType>                       VisualizationModelType;
  typedef LayerGeneratorType::ImageLayerType::OutputPixelType          LayerOutputPixelType;
  typedef Function::UniformAlphaBlendingFunction<LayerOutputPixelType> BlendingFunctionType;

  /** Mean Shift Vector Image*/

  typedef MeanShiftVectorImageFilter<VectorImageType, VectorImageType, LabeledImageType> MSFilterType;
  typedef MSFilterType::Pointer                                                          MSFilterPointerType;

  /** New macro */
  itkNewMacro(Self);

  /** Get the visualization models */
  itkGetObjectMacro(VisualizationModel, VisualizationModelType);

  /** input image Pointer */
  void SetInputImage(VectorImagePointerType image);
  itkGetConstObjectMacro(InputImage, VectorImageType);

  /** Output Image Pointers */
  itkGetObjectMacro(OutputFilteredImage, VectorImageType);
  itkGetObjectMacro(OutputClusteredImage, VectorImageType);
  itkGetObjectMacro(OutputLabeledImage, LabeledImageType);
  itkGetObjectMacro(OutputBoundariesImage, LabeledImageType);

  itkGetMacro(IsImageReady, bool);

  std::vector<unsigned int> GetChannels()
  {
    return m_Channels;
  }

  /** Open an image */
  void RunSegmentation();
  void SwitchClusters(bool sc);
  void SwitchBoundaries(bool sc);

  void SetSpatialRadius(unsigned int sr);
  void SetSpectralRadius(unsigned int sr);
  void SetMinRegionSize(unsigned int mr);
  // change opacity between the image (input or cluster) and the boundaries one
  void SetOpacity(double op);
  // Change displayed channel order
  void UpdateViewerDisplay(std::vector<unsigned int> ch);

  bool IsUpdating() const
  {
    return m_IsUpdating;
  }

  void Quit();
protected:

  /** Constructor */
  MeanShiftModuleModel();
  /** Destructor */
  ~MeanShiftModuleModel();

  bool m_IsUpdating;
  bool m_ShowingResult;
  bool m_IsImageReady;

private:
  MeanShiftModuleModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Visualization model */
  VisualizationModelType::Pointer m_VisualizationModel;

  /** MS filter type */
  MSFilterPointerType m_MeanShift;

  LayerGeneratorType::Pointer      m_ImageGenerator;
  LayerGeneratorType::Pointer      m_ClustersGenerator;
  LabelLayerGeneratorType::Pointer m_BoundariesGenerator;

  unsigned int              m_SpatialRadius;
  unsigned int              m_SpectralRadius;
  unsigned int              m_MinRegionSize;
  std::vector<unsigned int> m_Channels;

  BlendingFunctionType::Pointer m_BlendingFunction;

  VectorImagePointerType  m_InputImage;
  VectorImagePointerType  m_OutputFilteredImage;
  VectorImagePointerType  m_OutputClusteredImage;
  LabeledImagePointerType m_OutputLabeledImage;
  LabeledImagePointerType m_OutputBoundariesImage;

};

} //end namespace otb
#endif
