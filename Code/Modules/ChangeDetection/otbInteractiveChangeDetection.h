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
#ifndef __otbInteractiveChangeDetection_h
#define __otbInteractiveChangeDetection_h

#include <string>
#include "itkObject.h"
// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbInteractiveChangeDetectionGUI.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
//#include "otbICDImageWidget.h"
#include "otbSVMImageModelEstimator.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageViewerBase.h"
#include "otbMeanDifferenceImageFilter.h"
#include "otbMeanRatioImageFilter.h"
#include "itkAbsImageFilter.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbVectorImageToAmplitudeImageFilter.h"
#include "otbImageList.h"
#include "otbSVMImageClassificationFilter.h"
#include "otbSVMSampleListModelEstimator.h"
#include "otbChangeLabelImageFilter.h"
#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"
#include "otbVectorDataFileWriter.h"
#include "itkPreOrderTreeIterator.h"
#include "itkFixedArray.h"
#include "itkListSample.h"

#include "otbMVCModel.h"
#include "otbListenerBase.h"

// Overlay result display
#include "itkCastImageFilter.h"

namespace otb
{
/** \class InteractiveChangeDetection
 * \brief This class implements an interactive change detection application, based on
 * manual region selection, and joint classification using otb SVM's.
 *
 * Use FLTK gui.
 *
 */
template <class TPixel>
class ITK_EXPORT InteractiveChangeDetection
      : public itk::ProcessObject, public otbInteractiveChangeDetectionGUI, public MVCModel<ListenerBase>
{
public:
  /** Standard class typedefs */
  typedef InteractiveChangeDetection    Self;
  typedef itk::ProcessObject                 Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef itk::SmartPointer<const Self>      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(InteractiveChangeDetection,itk::ProcessObject);

  /// Template pixel type
  typedef TPixel InputPixelType;
  //typedef unsigned short LabeledPixelType;
  typedef double LabeledPixelType;
  
  /// Image widget
  typedef ImageViewerBase<InputPixelType,LabeledPixelType> ImageViewerType;
  typedef typename ImageViewerType::Pointer ImageViewerPointerType;
  typedef typename ImageViewerType::FormListType FormListType;
  typedef typename ImageViewerType::PolygonType PolygonType;
  typedef typename ImageViewerType::PolygonListType PolygonListType;
  typedef typename FormListType::Pointer FormListPointerType;
  typedef typename ImageViewerType::FullWidgetType FullWidgetType;
  typedef typename FullWidgetType::Pointer FullWidgetPointerType;
  typedef typename ImageViewerType::ScrollWidgetType ScrollWidgetType;
  typedef typename ScrollWidgetType::Pointer ScrollWidgetPointerType;
  typedef FixedSizeFullImageWidget<InputPixelType> ResultImageWidgetType;
  typedef typename ResultImageWidgetType::Pointer ResultImageWidgetPointerType;

  /// Image Type and related typedefs
  typedef typename FullWidgetType::ImageType                   ImageType;
  typedef typename FullWidgetType::OverlayImageType            OverlayImageType;
  typedef VectorImage<TPixel,2>                                OutputImageType;
  
  typedef typename OverlayImageType::Pointer                   OverlayImagePointerType;
  typedef typename ImageType::Pointer                          ImagePointerType;
  typedef typename ImageType::ConstPointer                     ImageConstPointerType;
  typedef typename ImageType::RegionType                       RegionType;
  typedef typename ImageType::SizeType                         SizeType;
  typedef typename ImageType::IndexType                        IndexType;
  typedef typename ImageType::OffsetType                       OffsetType;
  typedef typename ImageType::PixelType                        PixelType;
  typedef typename FullWidgetType::ColorType                   ColorType;
  typedef otb::Image<InputPixelType,2>                         SingleImageType;
  typedef typename SingleImageType::Pointer                    SingleImagePointerType;
  typedef otb::Image<LabeledPixelType,2>                       LabeledImageType;
  typedef otb::ImageList<SingleImageType>                      SingleImageListType;
  typedef typename SingleImageListType::Pointer                SingleImageListPointerType;
  typedef typename SingleImageListType::Iterator               SingleImageListIteratorType;

  /// Other image typedefs for learning
  typedef otb::VectorImageToAmplitudeImageFilter<ImageType,SingleImageType>               AmplitudeFilterType;
  typedef otb::VectorImageToImageListFilter<ImageType,SingleImageListType>                ImageToListFilterType;
  typedef otb::ImageListToVectorImageFilter<SingleImageListType,ImageType>                ListToImageFilterType;
  typedef otb::MeanRatioImageFilter<SingleImageType,SingleImageType,SingleImageType>      MeanRatioFilterType;
  typedef otb::MeanDifferenceImageFilter<SingleImageType,SingleImageType,SingleImageType> MeanDifferenceFilterType;
  typedef itk::AbsImageFilter<SingleImageType,SingleImageType>                            AbsFilterType;
  typedef otb::SVMImageClassificationFilter<ImageType,LabeledImageType>                   ClassificationFilterType;
  typedef typename ClassificationFilterType::ModelType                                    ModelType;
  typedef typename ImageType::PixelType                                                   MeasurementVectorType;
  typedef MeasurementVectorType                                                           SampleType;
  typedef itk::Statistics::ListSample< MeasurementVectorType >                                  ListSampleType;
  typedef itk::FixedArray<LabeledPixelType,1>                                             TrainingSampleType;
  typedef itk::Statistics::ListSample<TrainingSampleType>                                 TrainingListSampleType;
  typedef otb::SVMSampleListModelEstimator<ListSampleType,TrainingListSampleType>         EstimatorType;
  typedef otb::ChangeLabelImageFilter<LabeledImageType,OutputImageType>                   ChangeLabelFilterType;
  typedef typename EstimatorType::Pointer                                                 EstimatorPointerType;
  typedef typename AmplitudeFilterType::Pointer                                           AmplitudeFilterPointerType;
  typedef typename ImageToListFilterType::Pointer                                         ImageToListFilterPointerType;
  typedef typename ListToImageFilterType::Pointer                                         ListToImageFilterPointerType;
  typedef typename MeanRatioFilterType::Pointer                                           MeanRatioFilterPointerType;
  typedef typename MeanDifferenceFilterType::Pointer                                      MeanDifferenceFilterPointerType;
  typedef typename AbsFilterType::Pointer                                                 AbsFilterPointerType;
  typedef typename ClassificationFilterType::Pointer                                      ClassificationFilterPointerType;
  typedef typename ChangeLabelFilterType::Pointer                                         ChangeLabelFilterPointerType;
  typedef typename itk::CastImageFilter<OutputImageType, OverlayImageType>                         CastFilterType;
  typedef typename CastFilterType::Pointer                                                         CastFilterPointerType;

  /** Vector data handling */
  typedef VectorData<double,2,/*short unsigned int*/double>           VectorDataType;
  typedef typename VectorDataType::Pointer           VectorDataPointerType;
  typedef VectorDataFileReader<VectorDataType>       VectorDataFileReaderType;
  typedef typename VectorDataFileReaderType::Pointer VectorDataFileReaderPointerType;
  typedef VectorDataFileWriter<VectorDataType>       VectorDataFileWriterType;
  typedef typename VectorDataFileWriterType::Pointer VectorDataFileWriterPointerType;
  typedef VectorDataType::DataNodeType               DataNodeType;
  typedef typename DataNodeType::Pointer             DataNodePointerType;
  typedef typename VectorDataType::DataTreeType      DataTreeType;
  typedef typename DataTreeType::Pointer             DataTreePointerType;
  typedef itk::PreOrderTreeIterator<DataTreeType>    TreeIteratorType;

  /** Show the viewer (Update) */
  virtual void Show(void);

  virtual void Build(void);
  void ReInitButtons(void);
  virtual void Update(void);

  //ADDED Otmane: Set Left & Right Image
  itkSetObjectMacro(RightImage,ImageType);
  itkSetObjectMacro(LeftImage ,ImageType);
  itkGetObjectMacro(Output ,OutputImageType);
  
  //SVM Model FileName
  itkSetStringMacro(ModelFileName);

  // Get the HasOutput flag
  itkGetMacro(HasOutput,bool);

  void LoadLeftImage();
  void LoadRightImage();
  void LoadCenterImage();
  void SetFiltersInputs();
  
  void LoadSVMModel();

 /** Hide all Image View Windows */
  virtual void Hide(void);

protected:
  // Constructor and destructor
  InteractiveChangeDetection();
  ~InteractiveChangeDetection();

  /*   void EraseLastClickedPoint(); */
  void EraseAllClickedPoints();
  void EraseLastPoint();
  void EraseLastPolygon();
  void EndPolygon();
  void ChangeDetectionLearn();
  void AcquireFirstClass();
  void AcquireSecondClass();
  void DisplayResultsButtonToggled();
  void Quit();
  void Log(const char * log);
  void SaveSVMModel();
  void OpenSVMModel();
  void SaveResultImage();
  void ChangeClass1Color();
  void ChangeClass2Color();
  void ChangeTrainingSetOpacity();
  void Setup();
  void SVMSetupOk();
  void UseChangeDetectorsToggled();
  void PolygonalROIToggled();
  void HistogramTabSetup();
  void LeftViewerColorCompositionCallback();
  void RightViewerColorCompositionCallback();
  void CenterViewerColorCompositionCallback();

  virtual void ImportVectorData();
  virtual void ExportVectorData();

  virtual void PrincipalAction(const IndexType & index);

private:
  /// Left image viewer
  ImageViewerPointerType m_LeftViewer;
  /// Right image viewer
  ImageViewerPointerType m_RightViewer;
  /// Difference image viewer
  ImageViewerPointerType m_CenterViewer;
  /// Difference image viewer
  ImageViewerPointerType m_ResultViewer;
  /// Result widget component
  ResultImageWidgetPointerType m_ResultImageWidget;
  
  //Added : OTMANE
  typename ImageType::Pointer           m_RightImage;
  typename ImageType::Pointer           m_LeftImage;
  typename OutputImageType::Pointer     m_Output;
  
  std::string                           m_ModelFileName;

  /// Class form list
  FormListPointerType                   m_FormList;

  /// Amplitude filters
  AmplitudeFilterPointerType            m_AmplitudeFilter1;
  AmplitudeFilterPointerType            m_AmplitudeFilter2;

  /// VectorImage to ImageList filters
  ImageToListFilterPointerType          m_ImageToListFilter1;
  ImageToListFilterPointerType          m_ImageToListFilter2;

  /// Mean difference filter
  MeanDifferenceFilterPointerType       m_MeanDifferenceFilter;

  /// Abs filter
  AbsFilterPointerType                  m_AbsFilter;

  /// Mean ratio filter
  MeanRatioFilterPointerType            m_MeanRatioFilter;

  /// ImageList to VectorImage filters
  ListToImageFilterPointerType          m_ListToImageFilter1;
  ListToImageFilterPointerType          m_ListToImageFitler2;
  
  /// Classification filter
  ClassificationFilterPointerType       m_ClassificationFilter;
  
  /// Change label filter
  ChangeLabelFilterPointerType          m_ChangeLabelFilter;
  
  /// Image list
  SingleImageListPointerType            m_ImageList;
  
  /// SVM Estimator
  EstimatorPointerType                  m_Estimator;
  
  /// Caster
  CastFilterPointerType                 m_CastFilter
;

  /// Colors
  ColorType                             m_Class1Color;
  ColorType                             m_Class2Color;
  ColorType                             m_InterfaceColor;
  /// Labels
  unsigned short                       m_Label1;
  unsigned short                       m_Label2;
  /// Radius for the change detection filters
  unsigned int                         m_Radius;
  bool                                 m_DisplayResults;
  std::string                          m_LastPath;

  // Flag to determine if there is an output
  bool                                 m_HasOutput;
};

} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbInteractiveChangeDetection.txx"
#endif

#endif
