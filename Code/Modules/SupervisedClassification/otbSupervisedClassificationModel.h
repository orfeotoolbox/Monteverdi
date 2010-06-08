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
#include "otbTypeManager.h"

#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbVectorData.h"
#include "otbListSampleGenerator.h"
#include "otbSVMSampleListModelEstimator.h"
#include "otbSVMImageClassificationFilter.h"
#include "otbSVMClassifier.h"
#include "itkListSample.h"
#include "otbConfusionMatrixCalculator.h"


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
  typedef SupervisedClassificationModel                             Self;
  typedef MVCModel<ListenerBase>                                    Superclass;
  typedef itk::SmartPointer<Self>                                   Pointer;
  typedef itk::SmartPointer<const Self>                             ConstPointer;

  /** Standard type macro */
  itkTypeMacro(SupervisedClassificationModel, Object);
  
  /** New macro */
  itkNewMacro(Self);

  typedef enum { RANDOM, TRAINING, VALIDATION }                                        ROISelectionModeType;

  typedef TypeManager::Floating_Point_Precision                                        PixelType;
  typedef int                                                                          LabeledPixelType;

  typedef VectorImage<PixelType,2>                                                     ImageType;
  typedef ImageType::Pointer                                                           ImagePointerType;
  typedef Image<LabeledPixelType,2>                                                    LabeledImageType;
  typedef LabeledImageType::Pointer                                                    LabeledImagePointerType;

  typedef otb::VectorData<double, 2>                                                   VectorDataType;
  typedef VectorDataType::Pointer                                                      VectorDataPointerType;

  typedef otb::ListSampleGenerator<ImageType, VectorDataType>                          ListSampleGeneratorType;
  typedef ListSampleGeneratorType::Pointer                                             ListSampleGeneratorPointerType;

  typedef itk::VariableLengthVector<PixelType>                                         SampleType;
  typedef itk::Statistics::ListSample<SampleType>                                      ListSampleType;
  typedef SVMClassifier<ListSampleType,LabeledPixelType>                               ClassifierType;
  typedef itk::FixedArray<LabeledPixelType,1>                                          TrainingSampleType;
  typedef itk::Statistics::ListSample<TrainingSampleType>                              TrainingListSampleType;
  typedef SVMSampleListModelEstimator<ListSampleType,TrainingListSampleType>           ModelEstimatorType;
  typedef ModelEstimatorType::Pointer                                                  ModelEstimatorPointerType;

  typedef std::map<LabeledPixelType,unsigned int>                                      ClassesMapType;

  typedef otb::ConfusionMatrixCalculator< ListSampleType >                             ConfusionMatrixCalculatorType;
  
  typedef ConfusionMatrixCalculatorType::ConfusionMatrixType                           ConfusionMatrixType;


  /** Get the unique instance of the model */
  static Pointer GetInstance();


  /** Input Image Pointer */
  itkGetConstObjectMacro(InputImage, ImageType);
  void SetImage(ImagePointerType image);

  /** Label Image Pointer */
  itkGetConstObjectMacro(LabeledImage, LabeledImageType);
  void SetLabeledImage(LabeledImagePointerType image);
  
  /** ROI manipulation. */
  itkGetConstObjectMacro(VectorROIs, VectorDataType);
  void SetVectorROIs(VectorDataPointerType vectorData);

  LabeledImageType::Pointer GetOutput()
  {
    //FIXME To be implemented
  }


  /** Train the classifier */
  void Train();
  
  /** Train the classifier */
  void Validate();

  /** SVM model manipulation */
  itkGetObjectMacro(ModelEstimator,ModelEstimatorType);
  itkGetMacro(NumberOfClasses,unsigned short);
  
  /** Update Output */
  void OK();

  void Quit();

  /** Get the output changed flag */
  itkGetMacro(OutputChanged,bool);
  
  /** Constructor */
  SupervisedClassificationModel();
  /** Destructor */
  ~SupervisedClassificationModel();


private:
  SupervisedClassificationModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  void GenerateSamples();

  /** Output changed */
  bool                                        m_OutputChanged;

  /** Singleton instance */
  static Pointer                              Instance;

  /** Input Images */
  ImagePointerType                            m_InputImage;
  LabeledImagePointerType                     m_LabeledImage;

  /** Vector data for the ROIs */
  VectorDataPointerType                       m_VectorROIs;

  /** The sample generator */
  ListSampleGeneratorPointerType              m_SampleGenerator;

  unsigned long int                           m_MaxTrainingSize;
  unsigned long int                           m_MaxValidationSize;
  double                                      m_ValidationTrainingProportion;

  unsigned short                              m_NumberOfClasses;

  /** The SVM model estimator */
  ModelEstimatorPointerType                   m_ModelEstimator;
  
};

}//end namespace otb
#endif
