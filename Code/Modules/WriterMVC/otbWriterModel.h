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
#ifndef __otbWriterModel_h
#define __otbWriterModel_h

#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageFileReader.h"
#include "otbObjectList.h"
#include "itkImageSource.h"
#include "otbImageList.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbStreamingImageFileWriter.h"
#include "otbMultiToMonoChannelExtractROI.h"
#include "otbVectorImageToIntensityImageFilter.h"
#include "otbMultiChannelExtractROI.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbImageToVectorImageCastFilter.h"

#include "otbPixelType.h"



#include "itkRGBAPixel.h"
#include "otbImageLayer.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayerRenderingModel.h"

#include "otbImageFileWriter.h"
#include "otbVectorDataFileWriter.h"

namespace otb
{
/** \class WriterModel
 *
 *
 *
 */



class ITK_EXPORT WriterModel
      : public MVCModel<ListenerBase>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef WriterModel                         Self;
  typedef MVCModel<ListenerBase> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Standard type macro */
  itkTypeMacro(WriterModel, MVCModel);

  /** Images typedefs */
  typedef double                                                          PixelType;
  /** Inpout Image Type*/
  typedef VectorImage<PixelType,2>                                        InputImageType;
  typedef InputImageType::Pointer                                         InputImagePointerType;
  typedef InputImageType::PixelType                                       InputImagePixelType;
  typedef InputImageType::IndexType                                       IndexType;
  typedef InputImageType::SizeType                                        SizeType;
  typedef InputImageType::OffsetType                                      OffsetType;
  /** Vector Image Output Filter */
  typedef VectorImage<PixelType,2>                                        OutputImageType;
  typedef OutputImageType::PixelType                                      OutputImagePixelType;
  typedef OutputImageType::Pointer                                        OutputImagePointerType;
  /** Image Type */
  typedef Image<PixelType,2>                                               SingleImageType;
  typedef SingleImageType::Pointer                                         SingleImagePointerType;
  typedef SingleImageType::PixelType                                       SinglePixelType;
  typedef SingleImageType::InternalPixelType                               SingleInternalPixelType;
  typedef SingleImageType::RegionType                                      SingleRegionType;
  typedef SingleImageType::IndexType                                       SingleIndexType;
  typedef SingleImageType::RegionType                                      SingleSpacingType;
  typedef SingleImageType::SizeType                                        SingleSizeType;

  /** Containers typedefs */
  typedef ObjectList<SingleImageType>                                      SingleImageListType;
  typedef SingleImageListType::Pointer                                     SingleImageListPointerType;

  /** Input Manipulation typedefs */
  typedef MultiToMonoChannelExtractROI<PixelType, SinglePixelType>           ExtractROIFilterType;
  typedef VectorImageToIntensityImageFilter<InputImageType, SingleImageType> IntensityFilterType;
  typedef otb::ObjectList<ExtractROIFilterType>                              ExtractROIFilterListType;

  typedef itk::ProcessObject                                                 FilterType;
  typedef FilterType::Pointer                                                FilterPointerType;
  typedef ObjectList<FilterType>                                             FilterListType;
  typedef FilterListType::Pointer                                            FilterListPointerType;
  typedef std::vector<PixelType>                                           FilterTypeListType;
  typedef std::vector<std::string>                                           OutputFilterInformationType;

  /** Concatenation typedef */
  typedef ImageList< SingleImageType >                                       ImageListType;
  typedef ImageListToVectorImageFilter< ImageListType, OutputImageType >     ImageListToVectorImageFilterType;
  typedef ObjectList<ImageListType>                                          ImageListObjectListType;
  typedef ObjectList<ImageListToVectorImageFilterType>                       ImageListToVectorObjectListType;
  typedef StreamingImageFileWriter<OutputImageType>                          WriterType;
  typedef ImageFileReader<InputImageType>                                    ReaderType;
  typedef ReaderType::Pointer                                                ReaderPointerType;
  typedef VectorImageToImageListFilter<OutputImageType, ImageListType>       VectorToImageListType;
  typedef ImageToVectorImageCastFilter<SingleImageType, OutputImageType>     CasterType;

  typedef itk::ConstNeighborhoodIterator<SingleImageType> IteratorType;

  
  typedef itk::RGBAPixel<unsigned char>                RGBPixelType;
  typedef otb::Image<RGBPixelType,2>                  ViewerImageType;
  typedef otb::ImageLayer<InputImageType, ViewerImageType> LayerType;
  typedef otb::ImageLayerGenerator<LayerType>         LayerGeneratorType;
  typedef LayerGeneratorType::Pointer                 LayerGeneratorPointerType;

  typedef otb::ImageLayer<SingleImageType, ViewerImageType> SingleLayerType;
  typedef otb::ImageLayerGenerator<SingleLayerType>   SingleLayerGeneratorType;
  typedef SingleLayerGeneratorType::Pointer           SingleLayerGeneratorPointerType;

  typedef otb::ImageLayerRenderingModel<ViewerImageType> VisuModelType;
  typedef VisuModelType::Pointer                         VisuModelPointerType;

  /** inpuyt vector data */
  typedef VectorData<double>            VectorType;
  /// Writers
  typedef ImageFileWriter<InputImageType>    FPVWriterType;
  typedef VectorDataFileWriter<VectorType> VectorWriterType;
  
  itkNewMacro(Self);
  
  

  /** Input Image Pointer */
  void SetInputImage(InputImagePointerType image);
  
  /** Generate image layers */
  void GenerateLayers();
  
  InputImagePointerType GetInputImage()
  {
    return m_InputImage;
  };

  /** Input image list */
  void SetInputImageList(SingleImageListPointerType lst)
  {
    m_InputImageList = lst;
  };
  SingleImageListPointerType GetInputImageList()
  {
    return m_InputImageList;
  };
  void AddInputImage(SingleImagePointerType image)
  {
    m_InputImageList->PushBack(image);
  };
  void ClearInputImageList()
  {
    m_InputImageList->Clear();
  };
  void EraseFromInputImageList(int i)
  {
    m_InputImageList->Erase( static_cast<unsigned int>(i) );
  };
  void AddInputImageToIndex(SingleImagePointerType image, int i)
  {
    if ( m_InputImageList->Size()<static_cast<unsigned int>(i+1) )
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
  void ExchangeOutputListOrder(  int id1, int id2 )
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

  

  /** Get the output image */
  itkGetObjectMacro(OutputImage,OutputImageType);

   /** Channels Information */
  OutputFilterInformationType GetOutputChannelsInformation()
  {
    return m_OutputChannelsInformation;
  };
  std::string GetOutputChannelsInformationId( int id )
  {
    return m_OutputChannelsInformation[id];
  };
  void SetOutputChannelsInformation(OutputFilterInformationType list)
  {
    m_OutputChannelsInformation=list;
  };
  void ClearOutputChannelsInformation()
  {
    m_OutputChannelsInformation.clear();
  };
  void EraseFromOutputChannelsInformation(int i)
  {
    m_OutputChannelsInformation.erase( m_OutputChannelsInformation.begin()+(i-1) );
  };


  void AddToOutputListOrder(int val)
  {
    m_OutputListOrder.push_back(val);
    this->Modified();
  };
  void RemoveFromOutputListOrder(int id)
  {
    m_OutputListOrder.erase(m_OutputListOrder.begin()+id-1);
    this->Modified();
  };

  /** Output filename */
  itkSetMacro(OutputFileName, std:: string);
  itkGetMacro(OutputFileName, std:: string);

  /** Input filename */
  itkSetMacro(InputFileName, std:: string);
  itkGetMacro(InputFileName, std:: string);

  /** Number of channels */
  itkSetMacro(NumberOfChannels, unsigned int);
  itkGetMacro(NumberOfChannels, unsigned int);

  
  /** Need to Notify Monteverdi : output has changed */
  itkGetMacro(HasChanged, bool);

  itkGetMacro(HasInput,bool);

  /** VisuModel */
  itkGetMacro(VisuModel,VisuModelPointerType );
  itkGetMacro(ResultVisuModel,VisuModelPointerType );

  
  /** Get writer */
  itkGetObjectMacro(FPVWriter,FPVWriterType);
  itkGetObjectMacro(VectorWriter,VectorWriterType);

  /** Chain lsit */
  void CreateFilterList( int filterId );


  /** This have for aim to factorized the previous AddFunctions code*/
  void AddFeature();

  /** Generate output image */
  void GenerateOutputImage(const std::string & fname, const unsigned int pType, const bool useScale);
  void GetSingleOutput(int id);
  void AddChannels(std::vector<unsigned int> chListx);
  void AddChannel(int id);
  void AddIntensityChannel();

  /** Open Input Image */
  void SetInputImage(std::string strfilename);
  /** Init Input Image */
  void InitInput();

  /** Convert OutputImage*/
  template<typename CastOutputPixelType> void genericImageConverter(const std::string & fname, const bool useScale);
  /** update writers*/
  void UpdateWriter(const std::string & fname);
  void UpdateVectorWriter(const std::string & fname);
//   void UpdateImageWriter(const std::string & fname, bool useScale);
protected:
  /** Constructor */
  WriterModel();
  /** Destructor */
  virtual ~WriterModel();

private:
  WriterModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);

  /** Imput Image Pointer */
  InputImagePointerType m_InputImage;
  /** Input Image List*/
  SingleImageListPointerType m_InputImageList;
  /** Output Image */
  OutputImagePointerType m_OutputImage;
  
  /** Output Filter Information */
  OutputFilterInformationType m_OutputChannelsInformation;
  /** The instance singleton */
  static Pointer Instance;
  /** Number Of Input Images*/
  int m_NumberOfInputImages;
  /** Output filename*/
  std:: string m_OutputFileName;
  /** Input number of channels */
  unsigned int m_NumberOfChannels;
  /** Input filename*/
  std:: string m_InputFileName;
  /** Flags to activate/deactivate the preprocessings */
  bool m_HasInput;

  ExtractROIFilterListType::Pointer m_ChannelExtractorList;
  IntensityFilterType::Pointer      m_IntensityFilter;
  ReaderPointerType                 m_Reader;
  CasterType::Pointer               m_Caster;

  std::vector<unsigned int>         m_OutputIndexMap;
  /** Contains the filter list order for outputs */
  std::vector<unsigned int>         m_OutputListOrder;

  

  // Instantiate the model
  VisuModelPointerType              m_VisuModel;
  VisuModelPointerType              m_ResultVisuModel;
  
  
  //Generation of the output image attributes (to maintain pipeline)
  SingleImagePointerType m_image;
  ImageListType::Pointer m_imageList;
  ImageListToVectorImageFilterType::Pointer m_iL2VI;
  
  //Writers
  FPVWriterType::Pointer m_FPVWriter;
  VectorWriterType::Pointer m_VectorWriter;
  
  bool m_HasChanged;
};

}
#endif
