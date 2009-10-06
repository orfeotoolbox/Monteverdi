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

#ifndef __otbHomologousPointExtractionModuleModel_h
#define __otbHomologousPointExtractionModuleModel_h

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

namespace otb {

/** \class HomologousPointExtractionModuleModel
 *
 *
 *
 */
class ITK_EXPORT HomologousPointExtractionModuleModel
      : public MVCModel<ListenerBase>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef HomologousPointExtractionModuleModel                         Self;
  typedef MVCModel<ListenerBase> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Standard type macro */
  itkTypeMacro(HomologousPointExtractionModuleModel, Object);

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


  /** New macro */
  itkNewMacro(Self);
  
  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** Get the visualization models */
  itkGetObjectMacro(VisualizationModel,VisualizationModelType);


  /** Input Images Pointer */
  itkSetObjectMacro(FirstInputImage, VectorImageType);
  itkGetConstObjectMacro(FirstInputImage, VectorImageType);
  itkSetObjectMacro(SecondInputImage, VectorImageType);
  itkGetConstObjectMacro(SecondInputImage, VectorImageType);


  /** Open an image */
  //void OpenImage(const char * filename);

  /** Get the output changed flag */
  itkGetMacro(OutputChanged,bool);
  


protected:

  /** Constructor */
  HomologousPointExtractionModuleModel();
  /** Destructor */
  ~HomologousPointExtractionModuleModel();

  
private:
  HomologousPointExtractionModuleModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Singleton instance */
  static Pointer                     Instance;

  /** Visualization model */
  VisualizationModelType::Pointer    m_VisualizationModel;

  /** Vector reader */
  VectorReaderType::Pointer            m_Reader;

  LayerGeneratorType::Pointer m_ImageGenerator;

  BlendingFunctionType::Pointer m_BlendingFunction;

  VectorImagePointerType m_FirstInputImage;
  VectorImagePointerType m_SecondInputImage;

  bool m_OutputChanged;

};

}//end namespace otb
#endif
