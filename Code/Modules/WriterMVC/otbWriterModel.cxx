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

namespace otb
{

/** Initialize the singleton */
WriterModel::Pointer WriterModel::Instance = NULL;

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



/** Manage the singleton */
WriterModel::Pointer
WriterModel::GetInstance()
{
  if (!Instance)
  {
    Instance = WriterModel::New();
  }
  return Instance;
}

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
::GenerateOutputImage()
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
    m_imageList->PushBack( m_image );
    outputNb++;
    
    
  }//  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)

  if (todo == true)
  {
//     ImageListToVectorImageFilterType::Pointer iL2VI = ImageListToVectorImageFilterType::New();
    m_iL2VI->SetInput( m_imageList );

    m_OutputImage = m_iL2VI->GetOutput();
    
    //FIXME update during the pipeline!!!!! 
//     m_iL2VI->Update();
//     iL2VI->UpdateOutputInformation();

    m_OutputImage->UpdateOutputInformation();

    this->UpdateWriter(m_OutputFileName);
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
  ::UpdateWriter(std::string & fname)
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
    this->UpdateImageWriter(fname);
  }
}
void WriterModel
  ::UpdateVectorWriter(std::string & fname)
{
  m_VectorWriter->SetFileName(fname);
  m_VectorWriter->Update();
}
void WriterModel
  ::UpdateImageWriter(std::string & fname)   
{
  m_FPVWriter->SetFileName(fname);
  m_FPVWriter->Update();
}

}

