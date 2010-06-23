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
#include "otbImageToVectorImageCastFilter.h"
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

  typedef ClassifierType::OutputType                                                   ValidationListSampleType;
  typedef otb::ConfusionMatrixCalculator< TrainingListSampleType,
                                                        TrainingListSampleType >       ConfusionMatrixCalculatorType;
  
  typedef ConfusionMatrixCalculatorType::ConfusionMatrixType                           ConfusionMatrixType;
  typedef ConfusionMatrixCalculatorType::ClassLabelType                                ClassLabelType;

  typedef otb::SVMImageClassificationFilter<ImageType,LabeledImageType,
                                            LabeledImageType>                          ClassificationFilterType;

  typedef otb::ImageToVectorImageCastFilter<LabeledImageType, ImageType>               CasterType;

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

  ImageType::Pointer GetOutput()
  {

    return m_Caster->GetOutput();
  }


  /** Train the classifier */
  void Train();
  
  /** Train the classifier */
  void Validate();

  /** Update the vector data stat */
  void UpdateVectorDataInformation();

  /** Update the description */
  void UpdateDescription();

  /** Update the validation results */
  void UpdateMatrixString();

  /** SVM model manipulation */
  itkGetObjectMacro(ModelEstimator,ModelEstimatorType);
  itkGetObjectMacro(ClassificationFilter,ClassificationFilterType);
  itkGetConstMacro(NumberOfClasses, unsigned short);
  itkGetConstMacro(ClassMinSize, long int);

  itkSetMacro(MaxTrainingSize, long int)
  itkGetConstMacro(MaxTrainingSize, long int)
  itkSetMacro(MaxValidationSize, long int)
  itkGetConstMacro(MaxValidationSize, long int)
  itkSetMacro(ValidationTrainingProportion, double)
  itkGetConstMacro(ValidationTrainingProportion, double)

  itkGetConstMacro(ConfusionMatrix,ConfusionMatrixType);
  itkGetConstMacro(OverallAccuracy,double);
  itkGetConstMacro(KappaIndex,double);
  itkGetStringMacro(Description);
  itkGetStringMacro(MatrixString);
  
  itkGetStringMacro(ClassKey);
  void SetClassKey(std::string key)
  {
    otbMsgDevMacro( << "Setting the key to " << key);
    m_ClassKey = key;
    this->UpdateVectorDataInformation();
    this->UpdateDescription();
  }

  //itkGetConstMacro(ClassKeyList, std::vector<std::string>);
  std::vector<std::string> GetClassKeyList() const
    {
    return m_ClassKeyList;
    }

  /** Update Output */
  void Ok();

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

  // FIXME Maybe all this should just be in the sample generator
  // and accessed through convenient accessors
  long int                                    m_MaxTrainingSize;
  long int                                    m_MaxValidationSize;
  double                                      m_ValidationTrainingProportion;

  std::string                                 m_ClassKey;
  std::vector<std::string>                    m_ClassKeyList;


  // computed from the vector data
  long int                                    m_ClassMinSize;//this is the size of the smallest class
  unsigned short                              m_NumberOfClasses;


  /** The description to present the results */
  std::string                                 m_Description;

  std::string                                 m_MatrixString;

  /** The SVM model estimator */
  ModelEstimatorPointerType                   m_ModelEstimator;

  /** The confusion matrix */
  ConfusionMatrixType                         m_ConfusionMatrix;
  double                                      m_OverallAccuracy;
  double                                      m_KappaIndex;
  std::map<ClassLabelType, int>               m_MapOfClasses;

  /** The SVM classifier */
  ClassificationFilterType::Pointer           m_ClassificationFilter;

  /** The caster to get a vector image as output */
  CasterType::Pointer                         m_Caster;
  

};

}//end namespace otb
#endif
