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

/** Initialize the singleton */
FeatureExtractionModel::Pointer FeatureExtractionModel::Instance = NULL;

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
}


FeatureExtractionModel
::~FeatureExtractionModel() {}



/** Manage the singleton */
FeatureExtractionModel::Pointer
FeatureExtractionModel::GetInstance()
{
  if (!Instance)
  {
    Instance = FeatureExtractionModel::New();
  }
  return Instance;
}

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
  
  // Notify the observers
  this->NotifyAll();
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

    this->AddFeatureFilter( copy, ORIGINAL, i, 0, mess);
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
    this->AddFeatureFilter( filterTouzi, TOUZI, i, 0, mess);
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
    this->AddFeatureFilter( harris, HARRIS, i, 0, mess);
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
  this->AddFeatureFilter( angle, SPECTRALANGLE, -1, 0, mess);
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
      this->AddFeatureFilter( profileFilter, MORPHOLOGICALOPENING, i, j,  mess);
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
      this->AddFeatureFilter( profileFilter, MORPHOLOGICALCLOSING, i, j,  mess);
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
      this->AddFeatureFilter( var, VARIANCE, i, 0, mess);
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
      this->AddFeatureFilter( mean, MEAN, i, 0, mess);
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
    this->AddFeatureFilter( grad, GRADIENT, i, 0, mess);
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

      if( type==MS_SMOOTH ||  type==MS_CLUSTERED )
	{
	  for(unsigned int k=0; k<m_InputImageList->Size(); k++)
	    {
	      ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
	      extract->SetChannel(k+1);
	      itk::OStringStream oss;
	      oss<<"Mean Shift ";
	      if( type==MS_SMOOTH )
		{
		  extract->SetInput( meanShift->GetOutput() );
		  oss<<"Smooth";
		}
	      else if( type==MS_CLUSTERED )
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
	  if( type==MS_BOUND )
	    oss<<"Clustered Boundries: ";
	  else if(type==MS_LABELED)
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
      if( type==MS_SMOOTH || type==MS_CLUSTERED )
	{
	  for(unsigned int k=0; k<m_InputImageList->Size(); k++)
	    {
	      ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
	      extract->SetChannel(k+1);
	      itk::OStringStream oss;
	      oss<<"Mean Shift ";
	      if( type==MS_SMOOTH )
		{
		  extract->SetInput( m_MeanShiftFilterList->GetNthElement(i-1)->GetOutput() );
		  oss<<"Smooth";
		}
	      else if( type==MS_CLUSTERED )
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
	  if( type==MS_LABELED )
	    oss<<"Labeled: ";
	  else if(type==MS_BOUND)
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
  SingleImagePointerType image = SingleImageType::New();
  ImageListType::Pointer imageList = ImageListType::New();

  bool todo = false;
   int outputNb = 0;
  int i = 0;

  if( !m_HasInput )
    itkExceptionMacro("Impossible to create output image : no image image selected.")
  if( m_OutputListOrder.size()==0 )
    itkExceptionMacro("Impossible to create output image : no feature selected.")

  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)
  {
    i = m_OutputListOrder[ii];
    if (m_SelectedFilters[i] == true)
    {
      todo = true;
      image = GetSingleImage(i);
      imageList->PushBack( image );
      outputNb++;
    }// if(m_SelectedFilters[i] == true)
  }//  for (unsigned int ii = 0; ii<m_OutputListOrder.size(); ii++)

  if (todo == true)
  {
    ImageListToVectorImageFilterType::Pointer iL2VI = ImageListToVectorImageFilterType::New();
    iL2VI->SetInput( imageList );

    m_OutputImage = iL2VI->GetOutput();
    iL2VI->UpdateOutputInformation();
    std::cout<<iL2VI->GetOutput()->GetLargestPossibleRegion()<<std::endl;


    //FIXME update during the pipeline!!!!! 
    //iL2VI->Update();
    /*
    WriterType::Pointer writer = WriterType::New();

    writer->SetFileName(m_OutputFileName);
    writer->SetInput( iL2VI->GetOutput() );

    writer->SetAutomaticNumberOfStreamDivisions();

    typedef FltkFilterWatcher WatcherType;
    WatcherType watcher(writer,100,100,200,20, "Generating Output...");

    writer->Update();
    
    watcher.EndFilter();
    */

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
      case TOUZI:
      {
        TouziFilterType::Pointer touzi = dynamic_cast<TouziFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = touzi->GetOutput();
        break;
      }
      case HARRIS:
      {
        HarrisType::Pointer harris = dynamic_cast<HarrisType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = harris->GetOutput();
        break;
      }
      case SPECTRALANGLE:
      {
        DistanceFilterType::Pointer angle = dynamic_cast<DistanceFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = angle->GetOutput();
        break;
      }
      case MORPHOLOGICALOPENING:
      {
        OpeningProfileFilterType::Pointer profile = dynamic_cast<OpeningProfileFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        profile->Update();
        image = profile->GetOutput()->GetNthElement(m_OutputIndexMap[i]);
        break;
      }

      case MORPHOLOGICALCLOSING:
      {
        ClosingProfileFilterType::Pointer profile = dynamic_cast<ClosingProfileFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        profile->Update();
        image = profile->GetOutput()->GetNthElement(m_OutputIndexMap[i]);
        break;
      }
      case VARIANCE:
      {
        VarFilterType::Pointer var = dynamic_cast<VarFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = var->GetOutput();
        break;
      }
      case MEAN:
      {
        MeanFilterType::Pointer mean = dynamic_cast<MeanFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = mean->GetOutput();
        break;
      }
      case GRADIENT:
      {
        GradientFilterType::Pointer grad = dynamic_cast<GradientFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
        image = grad->GetOutput();
        break;
      }
      case TEXT_ASM:
      case TEXT_CSH:
      case TEXT_CPR:
      case TEXT_CON:
      case TEXT_COR:
      case TEXT_DEN:
      case TEXT_DVA:
      case TEXT_ENJ:
      case TEXT_ENT:
      case TEXT_IC1:
      case TEXT_IC2:
      case TEXT_IDM:
      case TEXT_MEA:
      case TEXT_PANTEX:
      case TEXT_SAV:
      case TEXT_SEN:
      case TEXT_SVA:
      case TEXT_VAR:
      {
        image = featureGenerator.GenerateTextureOutputImage( this, m_FilterTypeList[i], i);
        break;
      }
      case NDVI:
      case RVI:
      case ARVI:
      case EVI:
      case PVI:
      case SAVI:
      case MSAVI:
      case MSAVI2:
      case TSAVI:
      case TSARVI:
      case GEMI:
      case AVI:
      case WDVI:
      case IPVI:
      case TNDVI:
      case IR:
      case IC:
      case IB:
      case IB2:
      case NDBI:
      case ISU:
      case SRWI:
      case NDWI:
      case NDWI2:
      case MNDWI:
      case NDPI:
      case NDTI:
      {
        image = lRadiometricIndicesGenrator.GenerateRadiometricIndicesOutputImage(this,m_FilterTypeList[i],i);
        break;
      }
      case SFS_LEN:
      case SFS_WID:
      case SFS_PSI:
      case SFS_WME:
      case SFS_RAT:
      case SFS_SD:
      {
        image = lSFSTexturesGenerator.GenerateSFSTextureOutputImage(this,m_FilterTypeList[i],i);
        break;
      }
    case EDGE_CANNY:
    case EDGE_SOBEL:
      {
        image = lEdgeDensityGenerator.GenerateEdgeDensityOutputImage(this,m_FilterTypeList[i],i);
        break;
      }
    case MS_SMOOTH:
    case MS_CLUSTERED:
      {
	ExtractROIFilterType::Pointer extract = dynamic_cast<ExtractROIFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
	image = extract->GetOutput();
	break;
      }
    case MS_LABELED:
      {
	MeanShiftFilterType::Pointer meanShift = dynamic_cast<MeanShiftFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
	image = meanShift->GetLabeledClusteredOutput();
	break;
      }
    case MS_BOUND:
      {
	MeanShiftFilterType::Pointer meanShift = dynamic_cast<MeanShiftFilterType*>(static_cast<FilterType *>(m_FilterList->GetNthElement(i)));
	image = meanShift->GetClusterBoundariesOutput();
	break;
      }
    case ORIGINAL:
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



}
