#ifndef __otbMeanShiftModuleModel_h
#define __otbMeanShiftModuleModel_h

#include "otbMVCModel.h"
// #include "otbListenerBase.h"
#include "otbEventsSender.h"
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

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
  typedef MeanShiftModuleModel                         Self;
  typedef MVCModel<ListenerBase> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Standard type macro */
  itkTypeMacro(MeanShiftModuleModel, Object);

  typedef      double                                                            PixelType;
  typedef      VectorImage<PixelType,2>                VectorImageType;
  typedef      VectorImageType::Pointer                VectorImagePointerType;
  typedef      Image<unsigned int>                     LabeledImageType;
  typedef      LabeledImageType::Pointer               LabeledImagePointerType;
  typedef      ImageFileReader<VectorImageType>        VectorReaderType;
  typedef      ImageFileWriter<VectorImageType>        VectorWriterType;
  typedef      ImageFileWriter<LabeledImageType>       LabeledWriterType;


  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                                   RGBPixelType;
  typedef      Image<RGBPixelType,2>                                     RGBImageType;
  typedef      ImageLayer<VectorImageType,RGBImageType>                  LayerType;
  typedef      ImageLayer<LabeledImageType,RGBImageType>                  LabelLayerType;
  typedef      ImageLayerGenerator<LayerType>                            LayerGeneratorType;
  typedef      ImageLayerGenerator<LabelLayerType>                            LabelLayerGeneratorType;
  typedef      ImageLayerRenderingModel<RGBImageType>                    VisualizationModelType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;

  /** Mean Shift Vector Image*/

  typedef MeanShiftVectorImageFilter<VectorImageType, VectorImageType, LabeledImageType>  MSFilterType;
  typedef MSFilterType::Pointer                                                  MSFilterPointerType;

  /** New macro */
  itkNewMacro(Self);
  
  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** Get the visualization models */
  itkGetObjectMacro(VisualizationModel,VisualizationModelType);


  /** Input Image Pointer */
  void SetInputImage(VectorImagePointerType image);

  /** Output Image Pointers */
  itkGetObjectMacro(OutputFilteredImage,VectorImageType);
  itkGetObjectMacro(OutputClusteredImage,VectorImageType);
  itkGetObjectMacro(OutputLabeledImage,LabeledImageType);
 
  /** Open an image */
  void OpenImage(const char * filename);
  void SaveLabelImage(const char * filename);
  void SaveClusterImage(const char * filename);
  void RunSegmentation();
  void SwitchClusters(bool sc);
  void SwitchBoundaries(bool sc);

  void SetSpatialRadius(unsigned int sr);
  void SetSpectralRadius(unsigned int sr);
  void SetMinRegionSize(unsigned int mr);

  itkSetMacro(GenerateFiltered, bool);
  itkSetMacro(GenerateClustered, bool);
  itkSetMacro(GenerateLabeled, bool);

  itkGetMacro(GenerateFiltered, bool);
  itkGetMacro(GenerateClustered, bool);
  itkGetMacro(GenerateLabeled, bool);

  /** Get the output changed flag */
//   itkGetMacro(OutputChanged,bool);
  

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
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Singleton instance */
  static Pointer                     Instance;

  /** Visualization model */
  VisualizationModelType::Pointer    m_VisualizationModel;

  /** Vector reader */
  VectorReaderType::Pointer            m_Reader;

  /** MS filter type */
  MSFilterPointerType m_MeanShift;

  LayerGeneratorType::Pointer m_ImageGenerator;
  LayerGeneratorType::Pointer m_ClustersGenerator;
  LabelLayerGeneratorType::Pointer m_BoundariesGenerator;

  unsigned int m_SpatialRadius;
  unsigned int m_SpectralRadius;
  unsigned int m_MinRegionSize;

  BlendingFunctionType::Pointer m_BlendingFunction;

  bool m_GenerateFiltered;
  bool m_GenerateClustered;
  bool m_GenerateLabeled;

  VectorImagePointerType m_InputImage;
  VectorImagePointerType m_OutputFilteredImage;
  VectorImagePointerType m_OutputClusteredImage;
  LabeledImagePointerType m_OutputLabeledImage;

//   bool m_OutputChanged;

};

}//end namespace otb
#endif
