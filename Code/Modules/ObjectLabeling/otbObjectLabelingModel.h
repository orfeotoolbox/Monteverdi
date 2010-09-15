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
#ifndef __otbObjectLabelingModel_h
#define __otbObjectLabelingModel_h

#include "otbEventsSender.h"
#include "otbObjectLabelingEventsListener.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkFixedArray.h"
#include "otbTypeManager.h"
#include "itkLabelMap.h"
#include "otbAttributesMapLabelObjectWithClassLabel.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "otbShapeAttributesLabelMapFilter.h"
#include "otbRadiometricAttributesLabelMapFilter.h"
#include "otbLabelMapWithClassLabelToClassLabelImageFilter.h"
#include "otbChangeLabelImageFilter.h"
#include "otbSimplifyPathFunctor.h"

// Sample list
#include "otbLabelMapToSampleListFilter.h"
#include "otbLabelMapWithClassLabelToLabeledSampleListFilter.h"
#include "itkVariableLengthVector.h"
#include "itkListSample.h"

// SVMs
#include "otbSVMSampleListModelEstimator.h"
#include "otbSVMClassifier.h"
#include "otbSVMMarginSampler.h"

// Visu
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"
#include "otbPixelDescriptionModel.h"
#include "otbVectorData.h"
#include "otbLabelObjectToPolygonFunctor.h"
#include "otbSimplifyPathFunctor.h"

// KMeans
#include "itkEuclideanDistance.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkKdTreeBasedKmeansEstimator.h"

// Saving
#include "tinyxml.h"


namespace otb
{
// This is a struct class, so attributes are public
template <class TLabel> 
class ObjectClass
{
public:
  typedef TLabel                           LabelType;
  typedef std::vector<LabelType>           LabelVectorType;
  typedef itk::FixedArray<double,4>        ColorType;
  typedef otb::VectorData<double,2>        VectorDataType;
  typedef VectorDataType::DataNodeType     DataNodeType;

  /** Class label */
  LabelType       m_Label;
  /** Class name */
  std::string     m_Name;
  /** Class color */
  ColorType       m_Color;
  /** Label object samples */
  LabelVectorType m_Samples; 
  /** VectorData representing the samples */
  VectorDataType::Pointer m_VectorData;
  /** Pointer to the folder */
  DataNodeType::Pointer   m_Folder;
};

class ObjectLabelingModel
  : public  EventsSender<std::string>, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef ObjectLabelingModel                             Self;
  typedef itk::Object                                                Superclass;
  typedef itk::SmartPointer<Self>                                    Pointer;
  typedef itk::SmartPointer<const Self>                              ConstPointer;

  /** Standard type macro */
  itkTypeMacro(ObjectLabelingModel,Object);

  /** New macro*/
  itkNewMacro(Self);

  /** Algorithms typedef */
  typedef TypeManager::Floating_Point_Precision   LabelType;
  typedef TypeManager::Floating_Point_Precision   PixelType;
  typedef TypeManager::Floating_Point_Image       ImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  
  typedef ImageType::IndexType                    IndexType;
  typedef VectorImageType::PixelType              VectorPixelType;
  
  typedef AttributesMapLabelObjectWithClassLabel<LabelType,2,double,LabelType> LabelObjectType;
  typedef itk::LabelMap<LabelObjectType>                                       LabelMapType;
  typedef itk::LabelImageToLabelMapFilter<ImageType,LabelMapType>              LabelMapFilterType;
  typedef ShapeAttributesLabelMapFilter<LabelMapType>                          ShapeLabelMapFilterType;
  typedef RadiometricAttributesLabelMapFilter<LabelMapType,VectorImageType>    RadiometricLabelMapFilterType;
  
  typedef ObjectClass<LabelType>                     ObjectClassType;
  typedef ObjectClassType::ColorType                 ColorType;
  typedef std::vector<ObjectClassType>               ObjectClassVectorType;
  typedef ObjectClassType::VectorDataType            VectorDataType;
  typedef VectorDataType::DataNodeType               DataNodeType;
  typedef VectorDataType::DataTreeType::TreeNodeType TreeNodeType;
  typedef ObjectClassType::LabelVectorType           LabelVectorType;

  typedef LabelMapWithClassLabelToClassLabelImageFilter<LabelMapType,ImageType> ClassLabelFilterType;
  typedef ChangeLabelImageFilter<ImageType,VectorImageType>                     ChangeLabelFilterType;

  // Learning typedefs 
  typedef itk::VariableLengthVector<float>                                         VectorType;
  typedef itk::FixedArray<LabelType,1>                                             TrainingVectorType;
  typedef itk::Statistics::ListSample<VectorType>                                  ListSampleType;
  typedef itk::Statistics::ListSample<TrainingVectorType>                          TrainingListSampleType;
  typedef LabelMapWithClassLabelToLabeledSampleListFilter<LabelMapType,
    ListSampleType,TrainingListSampleType>                                         ClassLabelMap2ListSampleFilterType;
  typedef LabelMapToSampleListFilter<LabelMapType,ListSampleType>                  LabelMap2ListSampleFilterType;
  typedef std::map<std::string,bool>                                               AvailableFeaturesMapType;

  // SVMs
  typedef Functor::VariableLengthVectorToMeasurementVectorFunctor<VectorType> MeasurementVectorFunctorType;
  typedef SVMSampleListModelEstimator<ListSampleType,TrainingListSampleType,
    MeasurementVectorFunctorType>                                             SVMEstimatorType;
  typedef SVMClassifier<ListSampleType,LabelType>                             SVMClassifierType;
  typedef SVMMarginSampler<ListSampleType,SVMClassifierType::SVMModelType>    MarginSamplerType;

  // KMeans
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator<ListSampleType>         TreeGeneratorType;
  typedef TreeGeneratorType::KdTreeType                                            TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType>                    EstimatorType;
  typedef itk::Statistics::EuclideanDistance<VectorType>                           DistanceType;
  typedef std::vector<VectorType>                                                  CentroidsVectorType;

  /** Badn id list typedef */
  typedef itk::FixedArray<unsigned int, 4>                               BandIdListType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                                   RGBPixelType;
  typedef Image<RGBPixelType,2>                                          RGBImageType;
  typedef ImageLayer<VectorImageType,RGBImageType>                       LayerType;
  typedef ImageLayer<ImageType,RGBImageType>                             LabeledLayerType;
  typedef ImageLayerGenerator<LayerType>                                 LayerGeneratorType;
  typedef ImageLayerGenerator<LabeledLayerType>                          LabeledLayerGeneratorType;
  typedef ImageLayerRenderingModel<RGBImageType>                         VisualizationModelType;
  typedef PixelDescriptionModel<RGBImageType>                            PixelDescriptionModelType;
  typedef DataNodeType::PolygonType                                      PolygonType;
  typedef SimplifyPathFunctor<PolygonType,PolygonType>                   SimplifyPolygonFunctorType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;
  typedef LayerGeneratorType::ImageLayerType::RenderingFunctionType      RenderingFunctionType;

  /** Get the origin */
  itkGetConstReferenceMacro(Origin,VectorImageType::PointType);
  
  /** Get the spacing */
  itkGetConstReferenceMacro(Spacing,VectorImageType::SpacingType);

  /** Open an image with its associated label map */
  void OpenImage(VectorImageType* vimage, ImageType* limage);

  /** Initialize the filters */
  void Init(VectorImageType* vimage, ImageType* limage);

  /** Add sample */
  void AddSampleToClass(const IndexType & sampleIndex, unsigned int classIndex);
  void AddSampleToClass(const LabelType & label,       unsigned int classIndex);
  
  /** Get number of classes */
  unsigned int GetNumberOfClasses();

  /** Remove label */
  void RemoveSampleFromClass(const LabelType & label, unsigned int classIndex);

  /** Clear samples from class */
  void ClearSamplesFromClass(unsigned int classIndex);

  /** Focus on sample */
  void FocusOnSample(const LabelType & label);

  /** Get the current class */
  const ObjectClassType & GetClass(unsigned int classIndex) const;
  
  /** Get class index from label */
  unsigned int GetClassIndex(LabelType label) const;
  
  /** Add a new class */
  void AddClass();

  /** Remove the given class */
  void RemoveClass(unsigned int index);

  /** Clear all classes */
  void ClearClasses();
  
  /** Get a const reference on the classes vector */
  const ObjectClassVectorType & GetClasses() const;

  /** Is there any classes available ? */
  bool HasClasses() const;

  /** Set class name */
  void SetClassName(const char * name, unsigned int classIndex);

  /** Set class label */
  void SetClassLabel(const LabelType & label, unsigned int classIndex);
  
  /** Set class color */
  void SetClassColor(const ColorType & color, unsigned int classIndex);

  /** Select a class */
  void SelectClass(unsigned int classIndex);

  /** Clear selected class */
  void ClearSelectedClass();

  /** Return true if there is a selected class */
  bool HasSelectedClass();

  /** Return the selected class */
  unsigned int GetSelectedClass();

  /** Select sample */
  void SelectSample(const IndexType & index);
  void SelectSample(const LabelType & label);

  /** Is the given sample selected */
  bool IsSampleSelected(const IndexType & index);
  bool IsSampleSelected(const LabelType & label);
  bool IsSampleSelected();

  /** Clear selected sample */
  void ClearSelectedSample();

  /** An index has been clicked */
  void IndexClicked(const IndexType & index);

  /** Save samples to XML file */
  void SaveSamplesToXMLFile(const char * fname);

  /** Load samples to XML file */
  void LoadSamplesFromXMLFile(const char * fname);

  /** Save classification parameters to XML file */
  void SaveClassificationParametersToXMLFile(const char * fname);

  /** Save classification (image) to a file */
  void SaveClassification();

  /** Get the visualization models */
  itkGetObjectMacro(VisualizationModel,VisualizationModelType);
  itkGetObjectMacro(PixelDescriptionModel,PixelDescriptionModelType);

  /** Get the selected label polygon */
  itkGetObjectMacro(SelectedPolygon,VectorDataType);
  
  /** Get the margin sampled polygons */
  itkGetObjectMacro(MarginSampledPolygon,VectorDataType);

 /** Get the list of available features */
  AvailableFeaturesMapType GetAvailableFeatures() const
  {
    return m_AvailableFeatures;
  }

  /** Compute features statistics */
  void ComputeFeaturesStatistics();

  /** Estimate the centroids */
  void EstimateCentroids();

  /** Convert to sample list */
  void BuildSampleList();

  /** Convert to training sample list */
  void BuildTrainingSampleList();

  /** Margin sampler */
  void SampleMargin();

  /** Clear margin samples */
  void ClearMarginSamples();

  /** Train SVM */
  void Train();

  /** Classify */
  void Classify();

  /** Clear classifyed image */
  void ClearClassification();

  /** Get current accuracy */
  itkGetMacro(Accuracy,double);  

  /** Get the SVM Estimator */
  itkGetObjectMacro(SVMEstimator,SVMEstimatorType);
  itkGetObjectMacro(MarginSampler,MarginSamplerType);

  /** Get margin samples */
  const LabelVectorType & GetMarginSamples() const
  {
    return m_MarginSamples;
  }

  /** Change a feature state */
  void ChangeFeatureState(const std::string &, bool state);

  /** Change classification opacity */
  void ChangeClassificationOpacity(double opacity);

  /** Get vector image */
  itkGetConstObjectMacro(VectorImage,VectorImageType);
  itkGetObjectMacro(LabeledOutput,ImageType);
  itkGetObjectMacro(ColoredOutput,VectorImageType);

  /** Get displayed channels list */
  std::vector<unsigned int> GetChannels();

  /** Change displayed channel order */
  void UpdateViewerDisplay(std::vector<unsigned int> ch);
  
  /** Check image label relability. */
  bool CheckLabelImage(ImageType* limage);
  
  /** Link the pipeline and the visu. */
  void Link();

protected:
  /** Constructor */
  ObjectLabelingModel();
  /** Destructor */
  ~ObjectLabelingModel();

  /** Extract the relevant features and normalize to range [0,1] */
  VectorType BuildSample(const LabelObjectType * lo) const;

private:
  ObjectLabelingModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Create a new class */
  void AddClass(const LabelType& label, const std::string& name, const ColorType& color);
  
  /**  */
  LabelType GetNextAvailableClassLabel();

  /** Singleton instance */
  static Pointer                     Instance;

  /** Visualization model */
  VisualizationModelType::Pointer    m_VisualizationModel;

  /** Pixel description model */
  PixelDescriptionModelType::Pointer m_PixelDescriptionModel;

  /** Vector of object classes */
  ObjectClassVectorType              m_Classes;

  /** Labeled reader */
  ImageType::Pointer                 m_LabeledImage;

  /** Labeled Image source file */
  std::string                        m_LabeledImageFile;

  /** Vector reader */
  VectorImageType::Pointer           m_VectorImage; 

  /** Vector Image source file */
  std::string                        m_VectorImageFile;

  /** Label object map */
  LabelMapType::Pointer              m_LabelMap;

  /** Selected label */
  LabelType                          m_SelectedLabel; 

  /** Selected class */
  unsigned int                       m_SelectedClass;

  /** is there any class selected ? */
  bool                               m_HasSelectedClass;

  /** The VectorData to store the currently selected object */
  VectorDataType::Pointer            m_SelectedPolygon;
  DataNodeType::Pointer              m_SelectedPolygonNode;

  VectorDataType::Pointer            m_MarginSampledPolygon;
  DataNodeType::Pointer              m_MarginSampledFolder;

  /** List of available features */
  AvailableFeaturesMapType           m_AvailableFeatures;

  /** Stats */
  std::map<std::string,double>       m_FeaturesMeans;
  std::map<std::string,double>       m_FeaturesVariances;
  std::map<std::string,double>       m_FeaturesMinimum;
  std::map<std::string,double>       m_FeaturesMaximum;

  /** Vector Image layer generator */
  LayerGeneratorType::Pointer        m_ImageGenerator;

  /** Labeled Image layer generator */
  LayerGeneratorType::Pointer        m_LabeledImageGenerator;
  
  /** Samples */
  ListSampleType::Pointer            m_ListSample;

  ListSampleType::Pointer            m_TrainingListSample;

  TrainingListSampleType::Pointer    m_LabelsListSample;

  SVMEstimatorType::Pointer          m_SVMEstimator;

  MarginSamplerType::Pointer         m_MarginSampler;

  LabelVectorType                    m_MarginSamples;

  double                             m_Accuracy;

  /** Classification */
  ClassLabelFilterType::Pointer      m_ClassLabelFilter;
  ChangeLabelFilterType::Pointer     m_ColorMapper;

  /** Centroids vector */
  CentroidsVectorType                m_CentroidsVector;

  RenderingFunctionType::Pointer     m_ImageLayerRenderingFunction;

  VectorImageType::PointType   m_Origin;
  VectorImageType::SpacingType m_Spacing;

  ImageType::Pointer           m_LabeledOutput;
  VectorImageType::Pointer     m_ColoredOutput;
};
}
#endif
