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

#include "otbVectorRescaleIntensityImageFilter.h"
#include "otbStandardFilterWatcher.h"
#include "otbStandardWriterWatcher.h"
#include "otbMsgReporter.h"

namespace otb
{


WriterModel::WriterModel()
{
  /** Initial parameters */
  m_NumberOfInputImages = 0;
  m_NumberOfChannels = 0;
  m_InputImageList = SingleImageListType::New();

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
//   m_VectorWriter = VectorWriterType::New();
  
  m_ProcessObjectModel = m_FPVWriter;
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
  
  //Set Input Writer
  m_FPVWriter->SetInput(m_InputImage);
  // Notify the observers
  this->NotifyAll("SetInputImage");
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

  // keep a reference to the histogram list, to handle UseScale
  m_HistogramList = lVisuGenerator->GetLayer()->GetHistogramList();


  // Notify the observers
  this->NotifyAll("GenerateLayers");
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
::AddFeature()
{
  for (unsigned int i=0;i<m_NumberOfChannels;++i)
  {
    m_OutputIndexMap.push_back(0);
    m_OutputListOrder.push_back(std::max(0, static_cast<int>(m_OutputListOrder.size())));
  }
  
}

void
WriterModel
::ThreadedGenerateOutputImage(const std::string & fname, int pType, bool useScale)
{
  /** Set writer parameter*/
  this->SetOutputFileName(fname);
  this->SetPType(pType);
  this->SetUseScale(useScale);
  
  this->NotifyAll("SaveDataSet");
}


void
WriterModel
::GenerateOutputImage()
{
  bool todo = false;
  int outputNb = 0;
  int i = 0;

  if( !m_HasInput )
    itkExceptionMacro("Impossible to create output image : no image image selected.");
  if( m_OutputListOrder.size() == 0 )
    itkExceptionMacro("Impossible to create output image : no feature selected.");

  //m_VisuModel->GetLayer(0)->

  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)
  {
    i = m_OutputListOrder[ii];
    todo = true;
    m_image = this->GetInputImageList()->GetNthElement(i);
    m_imageList->PushBack( m_image );
    outputNb++;
  }
  
  if (todo == true)
  {
    switch ( this->GetPType() )
    {
      case 0:
        genericImageConverter<unsigned char>();
        break;
      case 1:
        genericImageConverter<short int>();
        break;
      case 2:
        genericImageConverter<int>();
        break;
      case 3:
        genericImageConverter<float>();
        break;
      case 4:
        genericImageConverter<double>();
        break;
      case 5:
        genericImageConverter<unsigned short int>();
        break;
      case 6:
        genericImageConverter<unsigned int>();
        break;
      default:
        genericImageConverter<unsigned char>();
        break;
    }
    
//     this->NotifyAll("OutputsUpdated");
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
  lResultVisuGenerator->SetImage( this->GetInputImageList()->GetNthElement(id) );
  lResultVisuGenerator->GenerateLayer();
  lResultVisuGenerator->GetLayer()->SetName("FeatureImage");

  // Add the layer to the model
  m_ResultVisuModel->ClearLayers();
  m_ResultVisuModel->AddLayer(lResultVisuGenerator->GetLayer());
  // Render
  m_ResultVisuModel->Update();
}


template <typename CastOutputPixelType>
void WriterModel::genericImageConverter(/*const std::string & fname, const bool useScale*/)
{
    typedef otb::VectorImage<CastOutputPixelType, 2> CastOutputImageType;
    typedef otb::ImageFileWriter<CastOutputImageType> CastWriterType;
    
    typename CastWriterType::Pointer writer=CastWriterType::New();
    
//     this->NotifyAll("SetWriter");
    this->SetProcessObjectModel ( writer );
    writer->SetFileName(this->GetOutputFileName().c_str());

    if ( this->GetUseScale() )
    {
      typedef ImageListToVectorImageFilter< ImageListType, InputImageType >               ImageListToVectorImageFilterType;
      typedef otb::VectorRescaleIntensityImageFilter<InputImageType, CastOutputImageType> RescalerType;

      typename ImageListToVectorImageFilterType::Pointer i2VI = ImageListToVectorImageFilterType::New();
      i2VI->SetInput( m_imageList );
      i2VI->UpdateOutputInformation();

      typedef typename InputImageType::PixelType InputPixelType;
      typedef typename InputImageType::InternalPixelType InputInternalPixelType;
      InputPixelType inputMinimum;
      InputPixelType inputMaximum;
      inputMinimum.SetSize(i2VI->GetOutput()->GetNumberOfComponentsPerPixel());
      inputMaximum.SetSize(i2VI->GetOutput()->GetNumberOfComponentsPerPixel());

      int i = 0;
      typename HistogramListType::ConstIterator it;
      for (it = m_HistogramList->Begin(); it != m_HistogramList->End(); ++it, ++i )
        {
        double iMin = it.Get()->Quantile(0, 0.02);
        double iMax = it.Get()->Quantile(0, 0.98);
        inputMinimum[ m_OutputListOrder[i] ] = static_cast<InputInternalPixelType>(iMin);
        inputMaximum[ m_OutputListOrder[i] ] = static_cast<InputInternalPixelType>(iMax);
        }

      typedef typename OutputImageType::PixelType OutputPixelType;
      OutputPixelType minimum;
      OutputPixelType maximum;
      minimum.SetSize(i2VI->GetOutput()->GetNumberOfComponentsPerPixel());
      maximum.SetSize(i2VI->GetOutput()->GetNumberOfComponentsPerPixel());
      minimum.Fill(itk::NumericTraits<CastOutputPixelType>::min());
      maximum.Fill(itk::NumericTraits<CastOutputPixelType>::max());

      typename RescalerType::Pointer rescaler=RescalerType::New();

      rescaler->SetOutputMinimum(minimum);
      rescaler->SetOutputMaximum(maximum);

      rescaler->SetInput(i2VI->GetOutput());
      writer->SetInput(rescaler->GetOutput());

      try
      {
        writer->Update();
      }
      catch (itk::ExceptionObject & err)
      {
        // Make the main fltk loop update Msg reporter
        m_ErrorMsg = err.GetDescription();
        Fl::awake(&SendErrorCallback,&m_ErrorMsg);
        this->Quit();
      }
    }
    else
    {
      typedef ImageListToVectorImageFilter< ImageListType, CastOutputImageType >     ImageListToVectorImageFilterType;
      typename ImageListToVectorImageFilterType::Pointer i2VI = ImageListToVectorImageFilterType::New();
      i2VI->SetInput( m_imageList );
      
      writer->SetInput(i2VI->GetOutput());
      
      try
      {
        writer->Update();
      }
      catch (itk::ExceptionObject & err)
      {
        // Make the main fltk loop update Msg reporter
        m_ErrorMsg = err.GetDescription();
        Fl::awake(&SendErrorCallback,&m_ErrorMsg);
        this->Quit();
      }
    }
}

void
WriterModel
::Quit()
{
  this->NotifyAll("Quit");
}

void WriterModel::SendErrorCallback(void * data)
{
  std::string *  error = static_cast<std::string *>(data);
  //TODO test if error is null
  if ( error == NULL )
  {
    MsgReporter::GetInstance()->SendError("Unknown error during update");
  }
  else
  {
    MsgReporter::GetInstance()->SendError(error->c_str());
  }
}
} //end namespace otb
