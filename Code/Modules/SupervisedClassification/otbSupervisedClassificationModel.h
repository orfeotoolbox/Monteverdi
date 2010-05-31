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

#ifndef __otbSupervisedClassificationModel_h
#define __otbSupervisedClassificationModel_h

#include <iostream>
#include <utility>
#include "otbMVCModel.h"
#include "otbListenerBase.h"

#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbStreamingImageFileWriter.h"
#include "otbSVMSampleListModelEstimator.h"
#include "otbSVMImageClassificationFilter.h"
#include "otbSVMClassifier.h"
#include "itkListSample.h"

//Visu
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"


namespace otb {

/** \class SupervisedClassificationModel
 *
 *
 *
 */
class ITK_EXPORT SupervisedClassificationModel
      : public MVCModel<ListenerBase>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef SupervisedClassificationModel                                     Self;
  typedef MVCModel<ListenerBase>                                    Superclass;
  typedef itk::SmartPointer<Self>                                   Pointer;
  typedef itk::SmartPointer<const Self>                             ConstPointer;

  /** Standard type macro */
  itkTypeMacro(SupervisedClassificationModel, Object);
  
  /** New macro */
  itkNewMacro(Self);

  typedef enum { RANDOM, TRAINING, VALIDATION }                                        ROISelectionModeType;

  typedef float                                                                        PixelType;
  typedef unsigned short                                                               LabeledPixelType;

  typedef VectorImage<PixelType,2>                                                     ImageType;
  typedef Image<LabeledPixelType,2>                                                    LabeledImageType;
  typedef ImageFileReader<ImageType>                                                   ImageReaderType;
  typedef ImageFileReader<LabeledImageType>                                            LabeledImageReaderType;
  typedef StreamingImageFileWriter<LabeledImageType>                                   WriterType;

  typedef SVMImageClassificationFilter<ImageType,LabeledImageType,LabeledImageType>    ClassificationFilterType;
  typedef ClassificationFilterType::ModelType                                          ModelType;

  typedef itk::VariableLengthVector<PixelType>                                         SampleType;
  typedef itk::Statistics::ListSample<SampleType>                                      ListSampleType;

  typedef SVMClassifier<ListSampleType,LabeledPixelType>                               ClassifierType;
  typedef itk::FixedArray<LabeledPixelType,1>                                          TrainingSampleType;
  typedef itk::Statistics::ListSample<TrainingSampleType>                              TrainingListSampleType;
  typedef SVMSampleListModelEstimator<ListSampleType,TrainingListSampleType>           EstimatorType;

  typedef std::map<LabeledPixelType,unsigned int>                                      ClassesMapType;

  typedef itk::VariableSizeMatrix<double>                                              ConfusionMatrixType;


  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** ROI manipulation. */
  unsigned int AddROI(ROIType aRoi );
  void RemoveROI(unsigned int ROIId);

  /** SVM model manipulation */

  /** Update Output */
  void OK();

  /** Get the output changed flag */
  itkGetMacro(OutputChanged,bool);
  
  /** Notifiers */
  itkGetConstMacro(SelectedPointChanged, bool);

  /** Constructor */
  SupervisedClassificationModel();
  /** Destructor */
  ~SupervisedClassificationModel();


private:
  SupervisedClassificationModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);
  
  /** Update the local GCPs Container, refresh the sensor model, notify it */
  void UpdateContainer();

  /** Singleton instance */
  static Pointer                              Instance;


  /** Lists of Samples */
  
};

}//end namespace otb
#endif
