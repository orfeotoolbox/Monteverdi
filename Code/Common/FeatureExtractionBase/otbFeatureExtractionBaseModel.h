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
#ifndef __otbFeatureExtractionBaseModel_h
#define __otbFeatureExtractionBaseModel_h

#include "otbEventsSender.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbObjectList.h"
#include "itkImageSource.h"
#include "otbImageList.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbMultiToMonoChannelExtractROI.h"
#include "otbVectorImageToIntensityImageFilter.h"
#include "otbMultiChannelExtractROI.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbImageToVectorImageCastFilter.h"
#include "itkContinuousIndex.h"
#include "otbFeatureBase.h"

#include "otbTypeManager.h"

//Filter include:
// Feature includes
#include "otbTouziEdgeDetectorImageFilter.h"
#include "otbHarrisImageFilter.h"
#include "otbSpectralAngleDistanceImageFilter.h"
#include "otbMorphologicalOpeningProfileFilter.h"
#include "otbMorphologicalClosingProfileFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkVarianceImageFunction.h"
#include "itkUnaryFunctorImageFilter.h"
#include "itkMeanImageFilter.h"
#include "otbVarianceImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkConstNeighborhoodIterator.h"
#include "otbUnaryFunctorNeighborhoodImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "otbMeanShiftVectorImageFilter.h"
// Min max filter
#include "otbStreamingStatisticsVectorImageFilter.h"
#include "otbStreamingStatisticsImageFilter.h"
#include "itkRGBAPixel.h"
#include "otbImageLayer.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayerRenderingModel.h"

namespace otb
{
/** \class FeatureExtractionBaseModel
 *
 *
 *
 */

class ITK_EXPORT FeatureExtractionBaseModel
//       : public MVCModel<ListenerBase>, public itk::Object
  : public EventsSender<std::string>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef FeatureExtractionBaseModel        Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(FeatureExtractionBaseModel, MVCModel);

  /** Images typedefs */
  typedef TypeManager::Floating_Point_Precision PixelType;

  /** Input Image Type*/
  typedef TypeManager::Floating_Point_VectorImage InputImageType;
  typedef InputImageType::Pointer                 InputImagePointerType;
  typedef InputImageType::PixelType               InputImagePixelType;
  typedef InputImageType::InternalPixelType       InputImageInternalPixelType;
  typedef InputImageType::IndexType               IndexType;
  typedef InputImageType::SizeType                SizeType;
  typedef InputImageType::OffsetType              OffsetType;
  typedef itk::ContinuousIndex<>                  ContinuousIndexType;

  /** Vector Image Output Filter */
  typedef TypeManager::Floating_Point_VectorImage OutputImageType;
  typedef OutputImageType::PixelType              OutputImagePixelType;
  typedef OutputImageType::Pointer                OutputImagePointerType;

  /** Image Type */
  typedef TypeManager::Floating_Point_Image  SingleImageType;
  typedef SingleImageType::Pointer           SingleImagePointerType;
  typedef SingleImageType::PixelType         SinglePixelType;
  typedef SingleImageType::InternalPixelType SingleInternalPixelType;
  typedef SingleImageType::RegionType        SingleRegionType;
  typedef SingleImageType::IndexType         SingleIndexType;
  typedef SingleImageType::RegionType        SingleSpacingType;
  typedef SingleImageType::SizeType          SingleSizeType;

  /** Containers typedefs */
  typedef ObjectList<SingleImageType>  SingleImageListType;
  typedef SingleImageListType::Pointer SingleImageListPointerType;

  /** Input Manipulation typedefs */
  typedef MultiToMonoChannelExtractROI<PixelType, SinglePixelType>           ExtractROIFilterType;
  typedef VectorImageToIntensityImageFilter<InputImageType, SingleImageType> IntensityFilterType;
  typedef otb::ObjectList<ExtractROIFilterType>                              ExtractROIFilterListType;

  typedef itk::ProcessObject      FilterType;
  typedef FilterType::Pointer     FilterPointerType;
  typedef ObjectList<FilterType>  FilterListType;
  typedef FilterListType::Pointer FilterListPointerType;

  /** Textures enumeration */
   typedef unsigned int                     FeatureType;
   typedef std::vector<FeatureType>         FilterTypeListType;
   typedef std::vector<std::string>         OutputFilterInformationType;
//   typedef HaralickTexture::TextureType     HaralickTextureType;
//   typedef AdvancedTexture::TextureType     AdvancedTextureType;
//   typedef std::vector<HaralickTextureType> HaralickTextureVectorType;
//   typedef std::vector<AdvancedTextureType> AdvancedTextureVectorType;

  /** Concatenation typedef */
  typedef ImageList<SingleImageType>                                     ImageListType;
  typedef ImageListToVectorImageFilter<ImageListType, OutputImageType>   ImageListToVectorImageFilterType;
  typedef ObjectList<ImageListType>                                      ImageListObjectListType;
  typedef ObjectList<ImageListToVectorImageFilterType>                   ImageListToVectorObjectListType;
  typedef VectorImageToImageListFilter<OutputImageType, ImageListType>   VectorToImageListType;
  typedef ImageToVectorImageCastFilter<SingleImageType, OutputImageType> CasterType;

  typedef itk::ConstNeighborhoodIterator<SingleImageType> IteratorType;

  /***************************/
  /** Filter type declaration*/
  /***************************/
//   // Copy of original data
//   typedef itk::ShiftScaleImageFilter<SingleImageType, SingleImageType> ShiftScaleFilterType;
//   // Touzi
//   typedef TouziEdgeDetectorImageFilter<SingleImageType, SingleImageType> TouziFilterType;
//   // Harris
//   typedef HarrisImageFilter<SingleImageType, SingleImageType> HarrisType;
//   // Spectral Angle
//   typedef SpectralAngleDistanceImageFilter<InputImageType, SingleImageType> DistanceFilterType;
//   // Morpho
//   typedef itk::BinaryBallStructuringElement<PixelType,
//       2>                      StructuringElementType;
//   typedef MorphologicalOpeningProfileFilter<SingleImageType, SingleImageType,
//       StructuringElementType> OpeningProfileFilterType;
//   typedef MorphologicalClosingProfileFilter<SingleImageType, SingleImageType,
//       StructuringElementType> ClosingProfileFilterType;
//   // Variance
//   typedef VarianceImageFilter<SingleImageType, SingleImageType> VarFilterType;
//   // Mean
//   typedef itk::MeanImageFilter<SingleImageType, SingleImageType> MeanFilterType;
//   // Gradient
//   typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<SingleImageType> GradientFilterType;
//   // Mean Shift
//   typedef MeanShiftVectorImageFilter<InputImageType, OutputImageType, SingleImageType> MeanShiftFilterType;
//   typedef ObjectList<MeanShiftFilterType>                                              MeanShiftFilterListType;

  // Min max filter
  typedef StreamingStatisticsVectorImageFilter<InputImageType> VectorImageMinMaxFilterType;
  typedef StreamingStatisticsImageFilter<SingleImageType>      ImageMinMaxFilterType;

  typedef itk::RGBAPixel<unsigned char>                    RGBPixelType;
  typedef otb::Image<RGBPixelType, 2>                      ViewerImageType;
  typedef otb::ImageLayer<InputImageType, ViewerImageType> LayerType;
  typedef otb::ImageLayerGenerator<LayerType>              LayerGeneratorType;
  typedef LayerGeneratorType::Pointer                      LayerGeneratorPointerType;

  typedef otb::ImageLayer<SingleImageType, ViewerImageType> SingleLayerType;
  typedef otb::ImageLayerGenerator<SingleLayerType>         SingleLayerGeneratorType;
  typedef SingleLayerGeneratorType::Pointer                 SingleLayerGeneratorPointerType;

  typedef otb::ImageLayerRenderingModel<ViewerImageType> VisuModelType;
  typedef VisuModelType::Pointer                         VisuModelPointerType;

  /** input image Pointer */
  void SetInputImage(InputImagePointerType image);

  /** Generate image layers */
  void GenerateLayers();

  InputImagePointerType GetInputImage()
  {
    return m_InputImage;
  }

  /** Input image list */
  void SetInputImageList(SingleImageListPointerType lst)
  {
    m_InputImageList = lst;
  }
  SingleImageListPointerType GetInputImageList()
  {
    return m_InputImageList;
  }
  void AddInputImage(SingleImagePointerType image)
  {
    m_InputImageList->PushBack(image);
  }
  void ClearInputImageList()
  {
    m_InputImageList->Clear();
  }
  void EraseFromInputImageList(int i)
  {
    m_InputImageList->Erase(static_cast<unsigned int>(i));
  }
  void AddInputImageToIndex(SingleImagePointerType image, int i)
  {
    if (m_InputImageList->Size() < static_cast<unsigned int>(i + 1))
      {
      m_InputImageList->PushBack(image);
      }
    else
      {
      m_InputImageList->SetNthElement(i, image);
      }

  }

  void ClearOutputListOrder()
  {
    m_OutputListOrder.clear();
    this->Modified();
  }
  /**  Exchange the outputlist element (change output image channel order). */
  void ExchangeOutputListOrder(int id1, int id2)
  {
    unsigned int temp = m_OutputListOrder[id2];
    m_OutputListOrder[id2] = m_OutputListOrder[id1];
    m_OutputListOrder[id1] = temp;
    this->Modified();
  }
  /** Get m_OutputListOrder */
  std::vector<unsigned int> GetOutputListOrder()
  {
    return m_OutputListOrder;
  }

  void ClearSelectedFilters()
  {
    m_SelectedFilters.clear();
  }

  /** Get the output image */
  itkGetObjectMacro(OutputImage, OutputImageType);

  /** Filter list*/
  FilterListPointerType GetFilterList()
  {
    return m_FilterList;
  }
  FilterPointerType GetFilter(int id)
  {
    return m_FilterList->GetNthElement(id);
  }
  void SetFilterList(FilterListPointerType lst)
  {
    m_FilterList = lst;
  }
  void ClearFilterList()
  {
    m_FilterList->Clear();
  }
  void EraseFromFilterList(int i)
  {
    m_FilterList->Erase(static_cast<unsigned int>(i));
  }
  void AddFilter(FilterType * filter)
  {
    m_FilterList->PushBack(filter);
  }
  void AddFilterToIndex(FilterPointerType filter, int i)
  {
    if (m_FilterList->Size() < static_cast<unsigned int>(i + 1))
      {
      m_FilterList->PushBack(filter);
      }
    else
      {
      m_FilterList->SetNthElement(i, filter);
      }
  }

  /** Filter Type list*/
  FilterTypeListType GetFilterTypeList()
  {
    return m_FilterTypeList;
  }
  FeatureType GetFilterType(int id)
  {
    return m_FilterTypeList[id];
  }
  void SetFilterTypeList(FilterTypeListType lst)
  {
    m_FilterTypeList = lst;
  }
  void AddFilterType(FeatureType type)
  {
    m_FilterTypeList.push_back(type);
  }
  void ClearFilterTypeList()
  {
    m_FilterTypeList.clear();
  }
  void EraseFromFilterTypeList(int i)
  {
    m_FilterTypeList.erase(m_FilterTypeList.begin() + (i - 1));
  }
  void AddFilterTypeToIndex(FeatureType type, int i)
  {
    if (m_FilterTypeList.size() < static_cast<unsigned int>(i + 1))
      {
      m_FilterTypeList.push_back(type);
      }
    else
      {
      m_FilterTypeList[i] = type;
      }

  }

  /** Output Filter Information */
  OutputFilterInformationType GetOutputFilterInformation()
  {
    return m_OutputFilterInformation;
  }
  std::string GetOutputFilterInformationId(int id)
  {
    return m_OutputFilterInformation[id];
  }
  void SetOutputFilterInformation(OutputFilterInformationType lst)
  {
    m_OutputFilterInformation = lst;
  }
  void ClearOutputFilterInformation()
  {
    m_OutputFilterInformation.clear();
  }
  void EraseFromOutputFilterInformation(int i)
  {
    m_OutputFilterInformation.erase(m_OutputFilterInformation.begin() + (i - 1));
  }

  /** Channels Information */
  OutputFilterInformationType GetOutputChannelsInformation()
  {
    return m_OutputChannelsInformation;
  }
  std::string GetOutputChannelsInformationId(int id)
  {
    return m_OutputChannelsInformation[id];
  }
  void SetOutputChannelsInformation(OutputFilterInformationType list)
  {
    m_OutputChannelsInformation = list;
  }
  void ClearOutputChannelsInformation()
  {
    m_OutputChannelsInformation.clear();
  }
  void EraseFromOutputChannelsInformation(int i)
  {
    m_OutputChannelsInformation.erase(m_OutputChannelsInformation.begin() + (i - 1));
  }

  void AddToOutputListOrder(int val)
  {
    m_OutputListOrder.push_back(val);
    this->Modified();
  }
  void RemoveFromOutputListOrder(int id)
  {
    m_OutputListOrder.erase(m_OutputListOrder.begin() + id - 1);
    this->Modified();
  }

  /** Output filename */
  itkSetMacro(OutputFileName, std::string);
  itkGetMacro(OutputFileName, std::string);

  /** Need to Notify Monteverdi : output has changed */
  itkGetMacro(HasChanged, bool);

  /** Input filename */
  itkSetMacro(InputFileName, std::string);
  itkGetMacro(InputFileName, std::string);

  /** Number of channels */
  itkSetMacro(NumberOfChannels, unsigned int);
  itkGetMacro(NumberOfChannels, unsigned int);

  /** Selected output filters. */
  void SetSelectedFilters(std::vector<bool> vect)
  {
    m_SelectedFilters = vect;
    this->Modified();
  }
  std::vector<bool> GetSelectedFilters()
  {
    return m_SelectedFilters;
  }

  /** Standard type macro */
  itkNewMacro(Self);
  itkGetMacro(HasInput, bool);

  /** VisuModel */
  itkGetMacro(VisuModel, VisuModelPointerType);
  itkGetMacro(ResultVisuModel, VisuModelPointerType);

  /** Image max accessors*/
  const std::vector<double> GetMinValues()
  {
    return m_MinValues;
  }
  /** Image min accessors*/
  const std::vector<double> GetMaxValues()
  {
    return m_MaxValues;
  }

  /** Image selected max accessors*/
  const std::vector<double> GetSelectedMinValues()
  {
    return m_SelectedMinValues;
  }
  /** Image selected min accessors*/
  const std::vector<double> GetSelectedMaxValues()
  {
    return m_SelectedMaxValues;
  }

  /** Chain list */
  void CreateFilterList(int filterId);

  /** FILTER LIST*/
//   void AddOriginalData();
//   void AddTouziFilter(unsigned int radiusX);
//   void AddHarrisFilter(double sigmaD, double sigmaI, double alpha);
//
//   void AddRAndNIRFilter(unsigned int redId, unsigned int nirId, FeatureType type, std::vector<double> params);
//   void AddRAndBAndNIRFilter(unsigned int redId,
//                             unsigned int blueId,
//                             unsigned int nirId,
//                             FeatureType type,
//                             std::vector<double> params);
//   void AddRAndGAndNIRFilter(unsigned int redId,
//                             unsigned int greenId,
//                             unsigned int nirId,
//                             FeatureType type,
//                             std::vector<double> params);
//   void Add2ChannelsFilter(unsigned int chan1Id, unsigned int chan2Id, FeatureType type);
//
//   void AddSpectralAngleFilter(InputImagePixelType pix);
//   void AddMorphoOpeningProfileFilter(unsigned int size, int value, int step);
//   void AddMorphoClosingProfileFilter(unsigned int size, int value, int step);
//   void AddVarianceFilter(int radiusX, int radiusY);
//   void AddMeanFilter(int radiusX, int radiusY);
//   void AddGradientFilter(double sigma);
  /** Generic method to create a texture filter*/
//   void AddTextureFilter(FeatureType featureType, SizeType radius, OffsetType offset, unsigned int bin);
//
//   void AddSFSTexturesFilter(FeatureType type,
//                             double spectralThr,
//                             unsigned int spatialThr,
//                             unsigned int nbDirection,
//                             double alpha,
//                             unsigned int ratioMaxConsNb);
//   void AddEdgeDensityFilter(FeatureType type, std::vector<double> params);
//   void AddMeanShiftFilter(FeatureType type, unsigned int spatial, double range, unsigned int minSize, double scale);

  /** This have for aim to factorized the previous AddFunctions code*/
   void AddFeatureFilter(FilterType * filter, FeatureType type, int inputId, unsigned int indexMapval, std::string mess);

  /** Add Haralick testure with given min max */
//   void AddHaralickTextureFilter(HaralickTextureVectorType harList, SizeType radius, OffsetType offset, double minHar, double maxHar, unsigned int bin);
  /** Add Haralick testure with default min max */
//  void AddHaralickTextureFilter(HaralickTextureVectorType harList, SizeType radius, OffsetType offset, unsigned int bin);

  /** Add Haralick testure with given min max */
//   void AddAdvancedTextureFilter(AdvancedTextureVectorType advList, SizeType radius, OffsetType offset, double minHar, double maxHar, unsigned int bin);
  /** Add Haralick testure with default min max */
//   void AddAdvancedTextureFilter(AdvancedTextureVectorType harList, SizeType radius, OffsetType offset, unsigned int bin);

  /** Generate output image */
  void GenerateOutputImage();
  void GetSingleOutput(int id);
  void AddChannels(std::vector<unsigned int> chListx);
  void AddChannel(int id);
  void AddIntensityChannel();

  /** Open input image */
  //void SetInputImage(std::string strfilename);
  /** Init input image */
  void InitInput();
  /** Init mean shift */
//  void InitMeanShiftLists();

  /** Get filtered single output image */
  virtual SingleImagePointerType GetSingleImage(int i);

  void Quit();
  void Cancel();
protected:

  /** Constructor */
  FeatureExtractionBaseModel();
  /** Destructor */
  virtual ~FeatureExtractionBaseModel();
  
  /** Input number of channels */
  unsigned int m_NumberOfChannels;
  
  /** Filter Type list*/
  FilterTypeListType m_FilterTypeList;

private:
  FeatureExtractionBaseModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);

  /** Imput Image Pointer */
  InputImagePointerType m_InputImage;
  /** input image List*/
  SingleImageListPointerType m_InputImageList;
  /** Output Image */
  OutputImagePointerType m_OutputImage;
  /** Filter list*/
  FilterListPointerType m_FilterList;

  
  /** Output Filter Information */
  OutputFilterInformationType m_OutputFilterInformation;
  /** Output Filter Information */
  OutputFilterInformationType m_OutputChannelsInformation;
  /** Number of input images*/
  int m_NumberOfInputImages;
  /** Output filename*/
  std::string m_OutputFileName;
  
  /** Input filename*/
  std::string m_InputFileName;
  /** Output filters selected to generate the output image. */
  std::vector<bool> m_SelectedFilters;
  /** Flags to activate/deactivate the preprocessings */
  bool m_HasInput;
  /** Min channels value, the last one is the value off the intensity image */
  std::vector<double> m_MinValues;
  /** Max channels value, the last one is the value off the intensity image */
  std::vector<double> m_MaxValues;
  /** Min of the select channel */
  std::vector<double> m_SelectedMinValues;
  /** Min of the select channel */
  std::vector<double> m_SelectedMaxValues;

  ExtractROIFilterListType::Pointer m_ChannelExtractorList;
  IntensityFilterType::Pointer      m_IntensityFilter;
  CasterType::Pointer               m_Caster;

  std::vector<unsigned int> m_OutputIndexMap;
  /** Contains the filter list order for outputs */
  std::vector<unsigned int> m_OutputListOrder;

//   // For mean shift
//   ImageListObjectListType::Pointer         m_MSImageListList;
//   ImageListToVectorObjectListType::Pointer m_MSListToVectorFilterList;
//   MeanShiftFilterListType::Pointer         m_MeanShiftFilterList;

  // Instantiate the model
  VisuModelPointerType m_VisuModel;
  VisuModelPointerType m_ResultVisuModel;

  //Generation of the output image attributes (to maintain pipeline)
  SingleImagePointerType                    m_image;
  ImageListType::Pointer                    m_imageList;
  ImageListToVectorImageFilterType::Pointer m_iL2VI;

  bool m_HasChanged;
};

}
#endif