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
#include "otbWriterModel.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>
#include "itkExceptionObject.h"
#include "otbMacro.h"

#include "otbImageFileWriter.h"
#include "otbRadiometricIndicesGenerator.h"
#include "otbTextureFilterGenerator.h"
#include "otbSFSTexturesGenerator.h"
#include "otbEdgeDensityGenerator.h"

#include "otbStreamingImageFileWriter.h"
#include "otbVectorRescaleIntensityImageFilter.h"
#include "otbStandardFilterWatcher.h"
#include "otbStandardWriterWatcher.h"
namespace otb
{


WriterModel::WriterModel()
{
  /** Initial parameters */
  m_NumberOfInputImages = 0;
  m_NumberOfChannels = 0;
  m_InputImageList = SingleImageListType::New();


  m_FilterList = FilterListType::New();
  m_FilterTypeList.clear();
  m_OutputFilterInformation.clear();
  m_OutputListOrder.clear();

  m_ChannelExtractorList     = ExtractROIFilterListType::New();
  m_IntensityFilter          = IntensityFilterType::New();
  m_Reader                   = ReaderType::New();
  
  // init input image + m_HasInput
  this->InitInput();

  // Instantiate the model
  m_VisuModel = VisuModelType::New();
  m_ResultVisuModel = VisuModelType::New();
  
  //Instantiate output image attributes
  m_image = SingleImageType::New();
  m_imageList = ImageListType::New();
  m_iL2VI = ImageListToVectorImageFilterType::New();
  
  //Input and Writers
  m_InputImage = InputImageType::New(); 
  m_FPVWriter = FPVWriterType::New();
  m_VectorWriter = VectorWriterType::New();
}


WriterModel
::~WriterModel() {}

void
WriterModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}



void
WriterModel
::SetInputImage(InputImagePointerType image)
{
  // Set the input image
  m_InputImage = image;
  m_InputImage->UpdateOutputInformation();

  m_NumberOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();

  // Togle the valid flag
  m_HasInput = true;

  // Generate image layers
  LayerGeneratorPointerType lVisuGenerator = LayerGeneratorType::New();

  lVisuGenerator->SetImage(image);
  lVisuGenerator->GenerateLayer();
  lVisuGenerator->GetLayer()->SetName("Image");
  // Add the layer to the model
  m_VisuModel->ClearLayers();
  m_VisuModel->AddLayer(lVisuGenerator->GetLayer());

  // Render
  m_VisuModel->Update();
  
  //GenerateImageList
  
  //Set Input Writer
  m_FPVWriter->SetInput(m_InputImage);
  // Notify the observers
  this->NotifyAll();
}

void
    WriterModel
  ::GenerateLayers()
{
  // Generate image layers
  LayerGeneratorPointerType lVisuGenerator = LayerGeneratorType::New();

  lVisuGenerator->SetImage(m_InputImage);
  lVisuGenerator->GenerateLayer();
  lVisuGenerator->GetLayer()->SetName("Image");
  // Add the layer to the model
  m_VisuModel->ClearLayers();
  m_VisuModel->AddLayer(lVisuGenerator->GetLayer());

  // Render
  m_VisuModel->Update();
  
//   otbMsgDebugMacro (<< "image list size " << m_InputImageList->Size());
  std::cout << "Image list size " << m_InputImageList->Size() << std::endl;
  // Notify the observers
  this->NotifyAll();
}
void
WriterModel
::SetInputImage(std::string filename)
{
  m_Reader = ReaderType::New();
  this->SetInputFileName(filename);
  m_Reader->SetFileName(filename);
  this->SetInputImage( m_Reader->GetOutput() );
  m_Reader->GenerateOutputInformation();
}



void
WriterModel
::InitInput()
{
  m_HasInput = false;
  m_NumberOfChannels = 0;
  m_InputImageList->Clear();
  m_InputImage = InputImageType::New();
}




/***************************************
 ****** SINGLE INPUT GENERATION ********
 ***************************************/
void
WriterModel
::AddChannels(std::vector<unsigned int> chList)
{
  m_OutputChannelsInformation.clear();
  if (m_NumberOfChannels != 0 )
  {
    for ( unsigned int i = 0; i<chList.size(); i++)
    {
      if (chList[i] <=  m_NumberOfChannels)
      {
        this->AddChannel(chList[i]);
      }
      else
      {
        if (chList[i] ==  m_NumberOfChannels+1)
        {
          this->AddIntensityChannel();
        }
        else
          return;
      }
    }
  }
}


void
WriterModel
::AddChannel(int id)
{
  ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
  extract->SetInput( m_InputImage );
  extract->SetChannel(id);
  extract->GetOutput()->UpdateOutputInformation();

  this->AddInputImage( extract->GetOutput() );

  m_ChannelExtractorList->PushBack(extract);

  itk::OStringStream oss;
  oss<<"Ch"<<id;
  m_OutputChannelsInformation.push_back(oss.str());
}

void
WriterModel
::AddIntensityChannel()
{
  m_IntensityFilter->SetInput( m_InputImage );
  m_IntensityFilter->GetOutput()->UpdateOutputInformation();
  this->AddInputImage( m_IntensityFilter->GetOutput() );
  m_OutputChannelsInformation.push_back("Int.");
}


/************************************************
 ******************* FILTERS FUNCTIONS **********
 ************************************************/
 void
     WriterModel
  ::AddFeatureFilter(FilterType * filter, FeatureType type, int inputId, unsigned int indexMapval, std::string mess)
{
  this->AddFilter( filter );
  this->AddFilterType(type);
  
  itk::OStringStream oss;
  if( inputId!=-1 )
    oss<< m_OutputChannelsInformation[inputId]<<": "<<mess;
  else
    oss<<mess;
  m_OutputFilterInformation.push_back( oss.str() );
  m_OutputIndexMap.push_back(indexMapval);
  m_SelectedFilters.push_back(true);
  m_OutputListOrder.push_back(std::max(0, static_cast<int>(m_OutputListOrder.size())));
}
void
WriterModel
::AddFeature()
{
//   this->AddFilter( filter );
//   this->AddFilterType(type);

  //itk::OStringStream oss;
  
  //oss<< m_OutputChannelsInformation[inputId]<<": "<<mess;
//   unsigned int size = m_NumberOfChannels;
  for (unsigned int i=0;i<m_NumberOfChannels;++i)
  {
    m_OutputIndexMap.push_back(0);
//   m_SelectedFilters.push_back(true);
    m_OutputListOrder.push_back(std::max(0, static_cast<int>(m_OutputListOrder.size())));  
  }
  
}

void
WriterModel
::AddOriginalData()
{
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  {
    ShiftScaleFilterType::Pointer copy = ShiftScaleFilterType::New();
    copy->SetShift(0);
    copy->SetScale(1);
    copy->SetInput(m_InputImageList->GetNthElement(i));

    itk::OStringStream oss;
    oss<<"Original data";
    std::string mess = oss.str();

    this->AddFeatureFilter( copy, ORIGINAL, i, 0, mess);
  }
}

/****************************************************
 ******************* FIN FILTERS FUNCTIONS **********
 ****************************************************/


void
WriterModel
::GenerateOutputImage(const std::string & fname, const unsigned int pType, const bool useScale)
{
//   SingleImagePointerType image = SingleImageType::New();
//   ImageListType::Pointer imageList = ImageListType::New();

  bool todo = false;
  int outputNb = 0;
  int i = 0;

  if( !m_HasInput )
    itkExceptionMacro("Impossible to create output image : no image image selected.")
        if( m_OutputListOrder.size()==0 )
        itkExceptionMacro("Impossible to create output image : no feature selected.")

        for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)
  {
    /*
    i = m_OutputListOrder[ii];
    if (m_SelectedFilters[i] == true)
    {
      todo = true;
      m_image = GetSingleImage(i);
      m_imageList->PushBack( m_image );
      outputNb++;
    */
    i = m_OutputListOrder[ii];
    todo = true;
    m_image = this->GetInputImageList()->GetNthElement(i);
    std::cout << "list of image "<< i << std::endl;
    m_imageList->PushBack( m_image );
    outputNb++;
    
    
  }//  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)

  if (todo == true)
  {
//     ImageListToVectorImageFilterType::Pointer iL2VI = ImageListToVectorImageFilterType::New();
    //m_iL2VI->SetInput( m_imageList );

    //m_OutputImage = m_iL2VI->GetOutput();
    
    //Convert Image
    //switch case PType go to converter
    switch (pType)
    {
      case 0:
        genericImageConverter<unsigned char>(fname, useScale);
        break;
      case 1:
        genericImageConverter<short int>(fname, useScale);
        break;
      case 2:
        genericImageConverter<int>(fname, useScale);
        break;
      case 3:
        genericImageConverter<float>(fname, useScale);
        break;
      case 4:
        genericImageConverter<double>(fname, useScale);
        break;
      case 5:
        genericImageConverter<unsigned short int>(fname, useScale);
        break;
      case 6:
        genericImageConverter<unsigned int>(fname, useScale);
        break;
      default:
        genericImageConverter<unsigned char>(fname, useScale);
        break;
    }
//     this->UpdateWriter(fname);
  }
  
  
}



void
WriterModel
::GetSingleOutput(int id)
{

  // Generate image layers
  SingleLayerGeneratorPointerType lResultVisuGenerator = SingleLayerGeneratorType::New();
  // To avoid drawing a quicklook( ScrollView) for nothing
  lResultVisuGenerator->SetGenerateQuicklook(false);
  //lResultVisuGenerator->SetImage(this->GetSingleImage(id));
  std::cout << "render feature number " << " "<< id <<this->GetInputImageList()->GetNthElement(id)<< " size " << this->GetInputImageList()->Size() << std::endl;
  lResultVisuGenerator->SetImage( this->GetInputImageList()->GetNthElement(id) );
  lResultVisuGenerator->GenerateLayer();
  lResultVisuGenerator->GetLayer()->SetName("FeatureImage");

  // Add the layer to the model
  m_ResultVisuModel->ClearLayers();
  m_ResultVisuModel->AddLayer(lResultVisuGenerator->GetLayer());
  // Render
  m_ResultVisuModel->Update();
}

void WriterModel
  ::UpdateWriter(const std::string & fname)
{
  typedef OGRVectorDataIO<VectorType> OGRVectorDataIOType;
  OGRVectorDataIOType::Pointer test=OGRVectorDataIOType::New() ;
  
  if ( test->CanWriteFile(fname.c_str()) ) 
  {
//     m_VectorWriter->SetFileName(filepath);
//     m_VectorWriter->Update();
    this->UpdateVectorWriter(fname);
  }
  else
  {
//     m_FPVWriter->SetFileName(filepath);
//     m_FPVWriter->Update();
//     this->UpdateImageWriter(fname);
    return;
  }
}
void WriterModel
  ::UpdateVectorWriter(const std::string & fname)
{
  m_VectorWriter->SetFileName(fname);
  //Need tio set input //TODO
  m_VectorWriter->Update();
}



template <typename CastOutputPixelType> 
void WriterModel::genericImageConverter(const std::string & fname, const bool useScale)
{
  typedef OGRVectorDataIO<VectorType> OGRVectorDataIOType;
  OGRVectorDataIOType::Pointer test=OGRVectorDataIOType::New() ;
  
  if ( test->CanWriteFile(fname.c_str()) ) 
  {
    this->UpdateVectorWriter(fname);
  }
  else
  {
    typedef otb::VectorImage<CastOutputPixelType, 2> CastOutputImageType;
    typedef otb::StreamingImageFileWriter<CastOutputImageType> CastWriterType;
    
    typename CastWriterType::Pointer writer=CastWriterType::New();
    
    writer->SetFileName(fname.c_str());

    if ( useScale )
    {
      m_iL2VI->SetInput( m_imageList );
      
      typedef otb::VectorRescaleIntensityImageFilter<InputImageType, CastOutputImageType> RescalerType;
      
      m_iL2VI->UpdateOutputInformation();
      
      typename OutputImageType::PixelType minimum;
      typename OutputImageType::PixelType maximum;
      minimum.SetSize(m_iL2VI->GetOutput()->GetNumberOfComponentsPerPixel());
      maximum.SetSize(m_iL2VI->GetOutput()->GetNumberOfComponentsPerPixel());
      minimum.Fill(itk::NumericTraits<CastOutputPixelType>::min());
      maximum.Fill(itk::NumericTraits<CastOutputPixelType>::max());

      typename RescalerType::Pointer rescaler=RescalerType::New();

      rescaler->SetOutputMinimum(minimum);
      rescaler->SetOutputMaximum(maximum);

      rescaler->SetInput(m_iL2VI->GetOutput());
      writer->SetInput(rescaler->GetOutput());

      otb::StandardWriterWatcher watcher(writer,rescaler,"Conversion");

      writer->Update();
    }
    else
    {
      typedef ImageListToVectorImageFilter< ImageListType, CastOutputImageType >     ImageListToCastVectorImageFilterType;
      typename ImageListToCastVectorImageFilterType::Pointer i2CastVI = ImageListToCastVectorImageFilterType::New();
      i2CastVI->SetInput( m_imageList );
      
      otb::StandardFilterWatcher watcher(writer,"Conversion");
      
      writer->SetInput(i2CastVI->GetOutput());
      writer->Update();
    }
  }
}

} //end namespace otb

