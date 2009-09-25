#ifndef __otbBasicApplicationModel_h
#define __otbBasicApplicationModel_h

#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

//Vis
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbMeanShiftVectorImageFilter.h"

#include "otbAsynchronousProcessBase.h"

namespace otb {

/** \class BasicApplicationModel
 *
 *
 *
 */
class ITK_EXPORT BasicApplicationModel
  : public MVCModel<ListenerBase>, public itk::Object, public Process::AsynchronousProcessBase
{

public:
  /** Standard class typedefs */
  typedef BasicApplicationModel                         Self;
  typedef MVCModel<ListenerBase> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Standard type macro */
  itkTypeMacro(BasicApplicationModel, Object);

  typedef      double                                                            PixelType;
  typedef      VectorImage<PixelType,2>                                          VectorImageType;
  typedef      ImageFileReader<VectorImageType>                                  VectorReaderType;
  typedef      ImageFileWriter<VectorImageType>                                  VectorWriterType;
 
  

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                                   RGBPixelType;
  typedef      Image<RGBPixelType,2>                                     RGBImageType;
  typedef      ImageLayer<VectorImageType,RGBImageType>                  LayerType;
  typedef      ImageLayerGenerator<LayerType>                            LayerGeneratorType;
  typedef      ImageLayerRenderingModel<RGBImageType>                    VisualizationModelType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;

  /** Mean Shift Vector Image*/
  typedef  Image<unsigned int>                                                            LabeledImageType;
  typedef MeanShiftVectorImageFilter<VectorImageType, VectorImageType, LabeledImageType>  MSFilterType;
  typedef MSFilterType::Pointer                                                  MSFilterPointerType;

  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** Get the visualization models */
  itkGetObjectMacro(VisualizationModel,VisualizationModelType);

  /** Open an image */
  void OpenImage(const char * filename);
  //void RunLoop();
  virtual void Run( void * v );

  bool IsUpdating() const
  {
    return m_IsUpdating;
  }

protected:
  /** This is protected for the singleton. Use GetInstance() instead. */
  itkNewMacro(Self);
  /** Constructor */
  BasicApplicationModel();
  /** Destructor */
  ~BasicApplicationModel();

  bool m_IsUpdating;

private:
  BasicApplicationModel(const Self&); //purposely not implemented
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



};

}//end namespace otb
#endif
