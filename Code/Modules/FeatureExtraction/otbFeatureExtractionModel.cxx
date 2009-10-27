/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#include "otbFeatureExtractionModel.h"
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

FeatureExtractionModel::FeatureExtractionModel()
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
  m_MSImageListList          = ImageListObjectListType::New();
  m_MSListToVectorFilterList = ImageListToVectorObjectListType::New();
  m_MeanShiftFilterList      = MeanShiftFilterListType::New();


  // init input image + m_HasInput
  this->InitInput();

  // Instantiate the model
  m_VisuModel = VisuModelType::New();
  m_ResultVisuModel = VisuModelType::New();
  
  //Instantiate output image attributes
  m_image = SingleImageType::New();
  m_imageList = ImageListType::New();
  m_iL2VI = ImageListToVectorImageFilterType::New();

  m_HasChanged = false;
}


FeatureExtractionModel
::~FeatureExtractionModel() {}


void
FeatureExtractionModel
::NotifyListener(ListenerBase * listener)
{
  listener->Notify();
}



void
FeatureExtractionModel
::SetInputImage(InputImagePointerType image)
{
  // Set the input image
  m_InputImage = image;
  m_InputImage->UpdateOutputInformation();

  m_NumberOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();

  // Togle the valid flag
  m_HasInput = true;
  /*
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
  */
  // Notify the observers
  this->NotifyAll("SetInputImage");
}

void
    FeatureExtractionModel
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
  this->NotifyAll("GenerateLayers");
}
void
FeatureExtractionModel
::SetInputImage(std::string filename)
{
  m_Reader = ReaderType::New();
  this->SetInputFileName(filename);
  m_Reader->SetFileName(filename);
  this->SetInputImage( m_Reader->GetOutput() );
  m_Reader->GenerateOutputInformation();
}



void
FeatureExtractionModel
::InitInput()
{
  m_HasInput = false;
  m_NumberOfChannels = 0;
  m_InputImageList->Clear();
  m_InputImage = InputImageType::New();
}


void
FeatureExtractionModel
::InitMeanShiftLists()
{
  m_MSImageListList->Clear();
  m_MSListToVectorFilterList->Clear();
  m_MeanShiftFilterList->Clear();
}

/***************************************
 ****** SINGLE INPUT GENERATION ********
 ***************************************/
void
FeatureExtractionModel
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
FeatureExtractionModel
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
FeatureExtractionModel
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
FeatureExtractionModel
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
FeatureExtractionModel
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

    this->AddFeatureFilter( copy, FeatureInfo::ORIGINAL, i, 0, mess);
  }
}

void
FeatureExtractionModel
::AddTouziFilter(unsigned int radiusX)
{
  TouziFilterType::SizeType radius;
  radius[0] = radiusX;
  radius[1] = radiusX;

  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  {
    TouziFilterType::Pointer filterTouzi = TouziFilterType::New();
    filterTouzi->SetRadius( radius );
    filterTouzi->SetInput( m_InputImageList->GetNthElement(i) );

    itk::OStringStream oss;
    oss<<"TOUZI : "<<radiusX;
    std::string mess = oss.str();
    this->AddFeatureFilter( filterTouzi, FeatureInfo::TOUZI, i, 0, mess);
  }
}

void
FeatureExtractionModel
::AddHarrisFilter( double sigmaD, double sigmaI, double alpha )
{
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  {
    HarrisType::Pointer harris = HarrisType::New();

    harris->SetSigmaD( sigmaD );
    harris->SetSigmaI( sigmaI );
    harris->SetAlpha( alpha );
    harris->SetInput( m_InputImageList->GetNthElement(i) );

    itk::OStringStream oss;
    oss<<"HARRIS : "<<sigmaD<<" , "<<sigmaI<<" , "<<alpha;
    std::string mess = oss.str();
    this->AddFeatureFilter( harris, FeatureInfo::HARRIS, i, 0, mess);
  }
}

void
FeatureExtractionModel
::AddSpectralAngleFilter(InputImageType::PixelType pix)
{

  if(m_NumberOfChannels < 2 )
  {
    itkExceptionMacro(<<"Spectral Angle Filter requires at least 2 channels, the selected picture does not have enough channel.");
  }

  DistanceFilterType::Pointer angle = DistanceFilterType::New();

  angle->SetInput(m_InputImage);
  angle->SetReferencePixel(pix);

  itk::OStringStream oss;
  oss<< "Full Image : Spect Angle : ";
  for (unsigned int i = 0; i<pix.Size();++i)
  {
    oss<<pix[i]<<" ";
  }
  std::string mess = oss.str();
  this->AddFeatureFilter( angle, FeatureInfo::SPECTRALANGLE, -1, 0, mess);
}

void
FeatureExtractionModel
::AddMorphoOpeningProfileFilter( unsigned int size, int value, int step )
{
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  {
    // Instantiation
    OpeningProfileFilterType::Pointer profileFilter = OpeningProfileFilterType::New();

    profileFilter->SetProfileSize(size);
    profileFilter->SetInitialValue(value);
    profileFilter->SetStep(step);

    profileFilter->SetInput(m_InputImageList->GetNthElement(i));

    profileFilter->GetOutput()->UpdateOutputInformation();

    for (unsigned int j = 0; j<size;++j)
    {
      itk::OStringStream oss;
      oss<<"Morpho Opening: "<<size<<" , "<<value<<" , "<<step<<", "<<value+j*step ;
      std::string mess = oss.str();
      this->AddFeatureFilter( profileFilter, FeatureInfo::MORPHOLOGICALOPENING, i, j,  mess);
    }
  }
}

void
FeatureExtractionModel
::AddMorphoClosingProfileFilter( unsigned int size, int value, int step )
{
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  {
    // Instantiation
    ClosingProfileFilterType::Pointer profileFilter = ClosingProfileFilterType::New();

    profileFilter->SetProfileSize(size);
    profileFilter->SetInitialValue(value);
    profileFilter->SetStep(step);

    profileFilter->SetInput(m_InputImageList->GetNthElement(i));
    profileFilter->GetOutput()->UpdateOutputInformation();

    for (unsigned int j = 0; j<size;++j)
    {
      itk::OStringStream oss;
      oss<<"Morpho Closing: "<<size<<" , "<<value<<" , "<<step<<", "<<value+j*step ;
      std::string mess = oss.str();
      this->AddFeatureFilter( profileFilter, FeatureInfo::MORPHOLOGICALCLOSING, i, j,  mess);
    }
  }
}

void
FeatureExtractionModel
::AddVarianceFilter(int radiusX, int radiusY)
{
  SingleSizeType rad;
  rad[0] = radiusX;
  rad[1] = radiusY;
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
    {
      VarFilterType::Pointer var = VarFilterType::New();
      var->SetRadius(rad);
      var->SetInput(m_InputImageList->GetNthElement(i));

      itk::OStringStream oss;
      oss<<"Variance : "<<radiusX<<", "<<radiusY;
      std::string mess = oss.str();
      this->AddFeatureFilter( var, FeatureInfo::VARIANCE, i, 0, mess);
    }
}

void
FeatureExtractionModel
::AddMeanFilter(int radiusX, int radiusY)
{
  SingleSizeType rad;
  rad[0] = radiusX;
  rad[1] = radiusY;
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
    {
      MeanFilterType::Pointer mean = MeanFilterType::New();
      mean->SetRadius(rad);
      mean->SetInput(m_InputImageList->GetNthElement(i));

      itk::OStringStream oss;
      oss<<"Mean : "<<radiusX<<", "<<radiusY;
      std::string mess = oss.str();
      this->AddFeatureFilter( mean, FeatureInfo::MEAN, i, 0, mess);
    }
}

void
FeatureExtractionModel
::AddGradientFilter(double sigma)
{
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  {
    GradientFilterType::Pointer grad =  GradientFilterType::New();
    grad->SetSigma(sigma);
    grad->SetInput(m_InputImageList->GetNthElement(i));


    itk::OStringStream oss;
    oss<<"Rec. Gradient : "<<sigma;
    std::string mess = oss.str();
    this->AddFeatureFilter( grad, FeatureInfo::GRADIENT, i, 0, mess);
  }
}

void
FeatureExtractionModel
::AddTextureFilter( FeatureType featureType, SizeType radius, OffsetType offset )
{
  for (unsigned int i = 0; i<m_InputImageList->Size(); i++)
  {
     TextureFilterGenerator generator;
     generator.GenerateTextureFilter( this, featureType, i, radius, offset);
  }
}

void
FeatureExtractionModel
::AddSFSTexturesFilter(FeatureType type, double spectralThr, unsigned int spatialThr, unsigned int nbDirection, double alpha, unsigned int ratioMaxConsNb)
{
     SFSTexturesGenerator lSFSTexturesGenerator;
     lSFSTexturesGenerator.AddSFSTexturesFilter(this,type,spectralThr, spatialThr, nbDirection, alpha, ratioMaxConsNb);
}

/** Radiometric Indices */
/** RAndNIR */
void
FeatureExtractionModel
::AddRAndNIRFilter(unsigned int redId, unsigned int nirId, FeatureType type, std::vector<double> params )
{
    if(redId < 1 || nirId < 1 ||
       redId > m_NumberOfChannels || nirId > m_NumberOfChannels)
      {
      itkExceptionMacro(<<"Channel indices must belong to range [1, "<<m_NumberOfChannels<<"]");
      }

     RadiometricIndicesGenerator lRadiometricIndicesGenerator;
     lRadiometricIndicesGenerator.AddRAndNIRFilter(this,type, redId,nirId,params);
}

/** RAndBAndNIR */
void
FeatureExtractionModel
::AddRAndBAndNIRFilter(unsigned int redId, unsigned int blueId, unsigned int nirId, FeatureType type, std::vector<double> params)
{

    if(redId < 1 || blueId < 1 || nirId < 1 ||
       redId > m_NumberOfChannels || blueId > m_NumberOfChannels || nirId > m_NumberOfChannels)
      {
      itkExceptionMacro(<<"Channel indices must belong to range [1, "<<m_NumberOfChannels<<"]");
      }
     RadiometricIndicesGenerator lRadiometricIndicesGenerator;
     lRadiometricIndicesGenerator.AddRAndBAndNIRFilter(this,type, redId,blueId,nirId,params);

}

/** RAndGAndNIR */
void
FeatureExtractionModel
::AddRAndGAndNIRFilter(unsigned int redId, unsigned int greenId, unsigned int nirId, FeatureType type, std::vector<double> params )
{
    if(redId < 1 || greenId < 1 || nirId < 1 ||
       redId > m_NumberOfChannels || greenId > m_NumberOfChannels || nirId > m_NumberOfChannels)
      {
      itkExceptionMacro(<<"Channel indices must belong to range [1, "<<m_NumberOfChannels<<"]");
      }
     RadiometricIndicesGenerator lRadiometricIndicesGenerator;
     lRadiometricIndicesGenerator.AddRAndGAndNIRFilter(this,type, redId,greenId,nirId,params);
}

/** Generic Add 2 Channels Filter without paramater */
void
FeatureExtractionModel
::Add2ChannelsFilter( unsigned int chan1Id, unsigned int chan2Id, FeatureType type )
{
     if(chan1Id < 1 || chan2Id < 1 ||
       chan1Id > m_NumberOfChannels || chan2Id > m_NumberOfChannels)
      {
      itkExceptionMacro(<<"Channel indices must belong to range [1, "<<m_NumberOfChannels<<"]");
      }
     RadiometricIndicesGenerator lRadiometricIndicesGenerator;
     lRadiometricIndicesGenerator.Add2ChannelsFilter(this,type, chan1Id,chan2Id);
}

void
FeatureExtractionModel
::AddEdgeDensityFilter( FeatureType type, std::vector<double> params )
{
     EdgeDensityGenerator lEdgeDensityGenerator;
     lEdgeDensityGenerator.AddEdgeDensityFilter(this,type,params);
}

void
FeatureExtractionModel
::AddMeanShiftFilter(  FeatureType type, unsigned int spatial, double range, unsigned int minSize, double scale )
{
  /*
     Mean shift clustered output is a vector image. We only deals with filter that have image as output.
     To avoid huge modification, in the  case clustered image, we extract each channel of the output
     those channels (ie extractor) are added the the filterlist.
  */
  bool alreadyLinked = false;
  unsigned int i = 0;

  // if not already exist
  while( i<m_MeanShiftFilterList->Size() && alreadyLinked==false )
    {
      // check same parameters
      MeanShiftFilterType::Pointer meanShiftTemp = m_MeanShiftFilterList->GetNthElement(i);
      unsigned int spatialCur = meanShiftTemp->GetSpatialRadius();
      double rangeCur = meanShiftTemp->GetRangeRadius();
      unsigned int minSizeCur = meanShiftTemp->GetMinimumRegionSize();
      double scaleCur = meanShiftTemp->GetScale();

      if( spatialCur == spatial && rangeCur == range && minSizeCur == minSize && scaleCur == scale    )
       alreadyLinked = true;
      i++;
    }

  if( alreadyLinked==false )
    {
      ImageListType::Pointer imList = ImageListType::New();
      ImageListToVectorImageFilterType::Pointer list2Vec = ImageListToVectorImageFilterType::New();

      // Create input vector image from selectd channels
      for (unsigned int j = 0; j<m_InputImageList->Size(); j++)
       {
         imList->PushBack( m_InputImageList->GetNthElement(j) );
       }
      list2Vec->SetInput( imList );

      MeanShiftFilterType::Pointer meanShift = MeanShiftFilterType::New();
      meanShift->SetInput(list2Vec->GetOutput() );
      meanShift->SetSpatialRadius( spatial );
      meanShift->SetRangeRadius( range );
      meanShift->SetMinimumRegionSize( minSize );
      meanShift->SetScale( scale );

      if( type==FeatureInfo::MS_SMOOTH ||  type==FeatureInfo::MS_CLUSTERED )
       {
         for(unsigned int k=0; k<m_InputImageList->Size(); k++)
           {
             ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
             extract->SetChannel(k+1);
             itk::OStringStream oss;
             oss<<"Mean Shift ";
             if( type==FeatureInfo::MS_SMOOTH )
              {
                extract->SetInput( meanShift->GetOutput() );
                oss<<"Smooth";
              }
             else if( type==FeatureInfo::MS_CLUSTERED )
              {
                extract->SetInput( meanShift->GetClusteredOutput() );
                oss<<"Clustered";
              }
             oss<<" (Ch."<<k+1<<"): ";
             oss<<spatial<<", "<<range<<", "<<minSize<<", "<<scale;
             std::string mess = oss.str();
             this->AddFeatureFilter( extract, type, -1, 0, mess);
           }
       }
      else
       {
         itk::OStringStream oss;
         oss<<"Mean Shift ";
         if( type==FeatureInfo::MS_BOUND )
           oss<<"Clustered Boundries: ";
         else if(type==FeatureInfo::MS_LABELED)
           oss<<"Labeled: ";
         oss<<spatial<<", "<<range<<", "<<minSize<<", "<<scale;
         std::string mess = oss.str();

         this->AddFeatureFilter( meanShift, type, -1, 0, mess);
       }
      m_MSImageListList->PushBack(imList);
      m_MSListToVectorFilterList->PushBack(list2Vec);
      m_MeanShiftFilterList->PushBack(meanShift);
    }
  else
    {
      if( type==FeatureInfo::MS_SMOOTH || type==FeatureInfo::MS_CLUSTERED )
       {
         for(unsigned int k=0; k<m_InputImageList->Size(); k++)
           {
             ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
             extract->SetChannel(k+1);
             itk::OStringStream oss;
             oss<<"Mean Shift ";
             if( type==FeatureInfo::MS_SMOOTH )
              {
                extract->SetInput( m_MeanShiftFilterList->GetNthElement(i-1)->GetOutput() );
                oss<<"Smooth";
              }
             else if( type==FeatureInfo::MS_CLUSTERED )
              {
                extract->SetInput( m_MeanShiftFilterList->GetNthElement(i-1)->GetClusteredOutput() );
                oss<<"Clustered";
              }
             oss<<" (Ch."<<k+1<<"): ";
             oss<<spatial<<", "<<range<<", "<<minSize<<", "<<scale;
             std::string mess = oss.str();
             this->AddFeatureFilter( extract, type, -1, 0, mess);
           }
       }
      else
       {
         itk::OStringStream oss;
         oss<<"Mean Shift ";
         if( type==FeatureInfo::MS_LABELED )
           oss<<"Labeled: ";
         else if(type==FeatureInfo::MS_BOUND)
           oss<<"Clustered Boundries: ";
         oss<<spatial<<", "<<range<<", "<<minSize<<", "<<scale;
         std::string mess = oss.str();

         this->AddFeatureFilter( m_MeanShiftFilterList->GetNthElement(i-1), type, -1, 0, mess);
       }
    }
}

/****************************************************
 ******************* FIN FILTERS FUNCTIONS **********
 ****************************************************/


void
FeatureExtractionModel
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
  
  m_imageList->Clear();
  
  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)
  {
    i = m_OutputListOrder[ii];
    if (m_SelectedFilters[i] == true)
    {
      todo = true;
      m_image = GetSingleImage(i);
      m_imageList->PushBack( m_image );
      outputNb++;
    }// if(m_SelectedFilters[i] == true)
  }//  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)

  if (todo == true)
  {
    m_iL2VI->SetInput( m_imageList );
    
    m_OutputImage = m_iL2VI->GetOutput();
    
    m_OutputImage->UpdateOutputInformation();
    
    m_HasChanged = true;
    this->NotifyAll("OutputsUpdated");
  }
}

FeatureExtractionModel
::SingleImagePointerType
FeatureExtractionModel
::GetSingleImage(int i)
{

  TextureFilterGenerator featureGenerator;
  RadiometricIndicesGenerator lRadiometricIndicesGenrator;
  SFSTexturesGenerator lSFSTexturesGenerator;
  EdgeDensityGenerator lEdgeDensityGenerator;
  SingleImagePointerType image = SingleImageType::New();

  switch (m_FilterTypeList[i])
    {
      case FeatureInfo::TOUZI:
      {
        TouziFilterType::Pointer touzi = dynamic_cast<TouziFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = touzi->GetOutput();
        break;
      }
      case FeatureInfo::HARRIS:
      {
        HarrisType::Pointer harris = dynamic_cast<HarrisType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = harris->GetOutput();
        break;
      }
      case FeatureInfo::SPECTRALANGLE:
      {
        DistanceFilterType::Pointer angle = dynamic_cast<DistanceFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = angle->GetOutput();
        break;
      }
      case FeatureInfo::MORPHOLOGICALOPENING:
      {
        OpeningProfileFilterType::Pointer profile = dynamic_cast<OpeningProfileFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        profile->Update();
        image = profile->GetOutput()->GetNthElement(m_OutputIndexMap[i]);
        break;
      }

      case FeatureInfo::MORPHOLOGICALCLOSING:
      {
        ClosingProfileFilterType::Pointer profile = dynamic_cast<ClosingProfileFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        profile->Update();
        image = profile->GetOutput()->GetNthElement(m_OutputIndexMap[i]);
        break;
      }
      case FeatureInfo::VARIANCE:
      {
        VarFilterType::Pointer var = dynamic_cast<VarFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = var->GetOutput();
        break;
      }
      case FeatureInfo::MEAN:
      {
        MeanFilterType::Pointer mean = dynamic_cast<MeanFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = mean->GetOutput();
        break;
      }
      case FeatureInfo::GRADIENT:
      {
        GradientFilterType::Pointer grad = dynamic_cast<GradientFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = grad->GetOutput();
        break;
      }
      case FeatureInfo::TEXT_ASM:
      case FeatureInfo::TEXT_CSH:
      case FeatureInfo::TEXT_CPR:
      case FeatureInfo::TEXT_CON:
      case FeatureInfo::TEXT_COR:
      case FeatureInfo::TEXT_DEN:
      case FeatureInfo::TEXT_DVA:
      case FeatureInfo::TEXT_ENJ:
      case FeatureInfo::TEXT_ENT:
      case FeatureInfo::TEXT_IC1:
      case FeatureInfo::TEXT_IC2:
      case FeatureInfo::TEXT_IDM:
      case FeatureInfo::TEXT_MEA:
      case FeatureInfo::TEXT_PANTEX:
      case FeatureInfo::TEXT_SAV:
      case FeatureInfo::TEXT_SEN:
      case FeatureInfo::TEXT_SVA:
      case FeatureInfo::TEXT_VAR:
      {
        image = featureGenerator.GenerateTextureOutputImage( this, m_FilterTypeList[i], i);
        break;
      }
      case FeatureInfo::NDVI:
      case FeatureInfo::RVI:
      case FeatureInfo::ARVI:
      case FeatureInfo::EVI:
      case FeatureInfo::PVI:
      case FeatureInfo::SAVI:
      case FeatureInfo::MSAVI:
      case FeatureInfo::MSAVI2:
      case FeatureInfo::TSAVI:
      case FeatureInfo::TSARVI:
      case FeatureInfo::GEMI:
      case FeatureInfo::AVI:
      case FeatureInfo::WDVI:
      case FeatureInfo::IPVI:
      case FeatureInfo::TNDVI:
      case FeatureInfo::IR:
      case FeatureInfo::IC:
      case FeatureInfo::IB:
      case FeatureInfo::IB2:
      case FeatureInfo::NDBI:
      case FeatureInfo::ISU:
      case FeatureInfo::SRWI:
      case FeatureInfo::NDWI:
      case FeatureInfo::NDWI2:
      case FeatureInfo::MNDWI:
      case FeatureInfo::NDPI:
      case FeatureInfo::NDTI:
      {
        image = lRadiometricIndicesGenrator.GenerateRadiometricIndicesOutputImage(this,m_FilterTypeList[i],i);
        break;
      }
      case FeatureInfo::SFS_LEN:
      case FeatureInfo::SFS_WID:
      case FeatureInfo::SFS_PSI:
      case FeatureInfo::SFS_WME:
      case FeatureInfo::SFS_RAT:
      case FeatureInfo::SFS_SD:
      {
        image = lSFSTexturesGenerator.GenerateSFSTextureOutputImage(this,m_FilterTypeList[i],i);
        break;
      }
    case FeatureInfo::EDGE_CANNY:
    case FeatureInfo::EDGE_SOBEL:
      {
        image = lEdgeDensityGenerator.GenerateEdgeDensityOutputImage(this,m_FilterTypeList[i],i);
        break;
      }
    case FeatureInfo::MS_SMOOTH:
    case FeatureInfo::MS_CLUSTERED:
      {
       ExtractROIFilterType::Pointer extract = dynamic_cast<ExtractROIFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
       image = extract->GetOutput();
       break;
      }
    case FeatureInfo::MS_LABELED:
      {
       MeanShiftFilterType::Pointer meanShift = dynamic_cast<MeanShiftFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
       image = meanShift->GetLabeledClusteredOutput();
       break;
      }
    case FeatureInfo::MS_BOUND:
      {
       MeanShiftFilterType::Pointer meanShift = dynamic_cast<MeanShiftFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
       image = meanShift->GetClusterBoundariesOutput();
       break;
      }
    case FeatureInfo::ORIGINAL:
      {
        ShiftScaleFilterType::Pointer copy = dynamic_cast<ShiftScaleFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = copy->GetOutput();
        break;
      }
      default:
      {
      }
    }
return image;

}

void
FeatureExtractionModel
::GetSingleOutput(int id)
{

  // Generate image layers
  SingleLayerGeneratorPointerType lResultVisuGenerator = SingleLayerGeneratorType::New();
  // To avoid drawing a quicklook( ScrollView) for nothing
  lResultVisuGenerator->SetGenerateQuicklook(false);
  lResultVisuGenerator->SetImage(this->GetSingleImage(id));
  lResultVisuGenerator->GenerateLayer();
  lResultVisuGenerator->GetLayer()->SetName("FeatureImage");

  // Add the layer to the model
  m_ResultVisuModel->ClearLayers();
  m_ResultVisuModel->AddLayer(lResultVisuGenerator->GetLayer());
  // Render
  m_ResultVisuModel->Update();
}

void
    FeatureExtractionModel
  ::Quit()
{

  this->NotifyAll("BusyOff");
}

}

