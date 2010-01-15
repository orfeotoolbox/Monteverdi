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
#include "otbOpticCalibrationModel.h"

#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>
#include "itkExceptionObject.h"
#include "otbMacro.h"
#include "otbImageMetadataInterfaceFactory.h"
#include "otbSpotImageMetadataInterface.h"
#include "otbImageFileWriter.h"
#include "otbSpectralSensitivityReader.h"
#include "ConfigureMonteverdi.h"


namespace otb
{
OpticCalibrationModel::OpticCalibrationModel()
{
  /** Visu */
  m_VisuModel = VisuModelType::New();
  m_RVisuModel = VisuModelType::New();
  m_ChangeReflectanceScale = false;

  this->Initialization();

  m_HasNewInput = false;
}

OpticCalibrationModel
::~OpticCalibrationModel() {}


/** Notify */
void
OpticCalibrationModel
::Notify(ListenerBase * listener)
{
  listener->Notify();
}


/** Init of the filters, the object lists and pixel description model */
void
OpticCalibrationModel
::Initialization()
{
  // Input image and reader
  m_InputImage = ImageType::New();

  // Init the lists
  m_ImageLayerList                        = ImageLayerListType::New();

  // Init Filters
  m_ImageToLuminanceFilter                = ImageToLuminanceImageFilterType::New();
  m_ImageToReflectanceFilter              = ImageToReflectanceImageFilterType::New();
  m_ReflectanceToSurfaceReflectanceFilter = ReflectanceToSurfaceReflectanceImageFilterType::New();
  m_DifferenceFilter                      = DifferenceImageFilterType::New();

  m_QLImageToLuminanceFilter                = ImageToLuminanceImageFilterType::New();
  m_QLImageToReflectanceFilter              = ImageToReflectanceImageFilterType::New();
  m_QLReflectanceToSurfaceReflectanceFilter = ReflectanceToSurfaceReflectanceImageFilterType::New();
  m_QLDifferenceFilter                      = DifferenceImageFilterType::New();
}


/** Layers and display */
void
OpticCalibrationModel
::DisplayImage()
{
  m_InputImage->UpdateOutputInformation();
  m_VisuModel->ClearLayers();
  m_RVisuModel->ClearLayers();
  m_VisuModel->Init();
  m_RVisuModel->Init();

  /// Generate input image layer
  /*ImageLayerGeneratorPointerType */lVisuGenerator = ImageLayerGeneratorType::New();
  FltkFilterWatcher qlwatcher(lVisuGenerator->GetResampler(),0,0,200,20,"Generating QuickLook ...");

  StandardRenderingFunctionType::Pointer renderer1 = StandardRenderingFunctionType::New();
  lVisuGenerator->SetImage(m_InputImage);
  lVisuGenerator->SetRenderingFunction(renderer1);
  lVisuGenerator->GenerateLayer();
  lVisuGenerator->GetLayer()->SetName("Input Image");
  // Save the Input Image Layer
  m_ImageLayerList->PushBack(lVisuGenerator->GetLayer());

  // Add the layer to the model
  m_VisuModel->AddLayer(lVisuGenerator->GetLayer());
  // Render
  m_VisuModel->Update();

  // Init the ql pipeline
  //lVisuGenerator->GetQuicklook()->SetMetaDataDictionary( m_InputImage->GetMetaDataDictionary() );
  ImageType::Pointer qlImage = lVisuGenerator->GetLayer()->GetQuicklook();
  lVisuGenerator = ImageLayerGeneratorType::New();

  m_QLImageToLuminanceFilter->SetInput(qlImage);
  m_QLImageToLuminanceFilter->Update();

  m_QLImageToReflectanceFilter->SetInput(qlImage);
  m_QLImageToReflectanceFilter->Update();

  m_QLReflectanceToSurfaceReflectanceFilter->SetInput(m_QLImageToReflectanceFilter->GetOutput());
  //m_QLReflectanceToSurfaceReflectanceFilter->UpdateOutputInformation();
  m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);
  m_QLReflectanceToSurfaceReflectanceFilter->Update();

  m_QLDifferenceFilter->SetValidInput(m_QLImageToReflectanceFilter->GetOutput());
  m_QLDifferenceFilter->SetTestInput(m_QLReflectanceToSurfaceReflectanceFilter->GetOutput());
  m_QLDifferenceFilter->Update();

 std::cout<<"phase 1 faite, cool"<<std::endl;

  /// Generate luminance image layer
  ImageLayerGeneratorPointerType lLVisuGenerator = ImageLayerGeneratorType::New();
  StandardRenderingFunctionType::Pointer renderer2 = StandardRenderingFunctionType::New();
  lLVisuGenerator->SetRenderingFunction(renderer2);
  lLVisuGenerator->SetImage(m_ImageToLuminanceFilter->GetOutput());
  lLVisuGenerator->SetGenerateQuicklook(false);
  lLVisuGenerator->SetQuicklook(m_QLImageToLuminanceFilter->GetOutput());
  lLVisuGenerator->GenerateLayer();
  lLVisuGenerator->GetLayer()->SetName("Luminance");
 
  // Save the Input Image Layer
  m_ImageLayerList->PushBack(lLVisuGenerator->GetLayer());
  // Add the layer
  m_RVisuModel->AddLayer(lLVisuGenerator->GetLayer());
  

  /// Generate reflectance TOA image layer
  ImageLayerGeneratorPointerType lRTOAVisuGenerator = ImageLayerGeneratorType::New();
  StandardRenderingFunctionType::Pointer renderer3 = StandardRenderingFunctionType::New();
  lRTOAVisuGenerator->SetImage(m_ImageToReflectanceFilter->GetOutput());
  lRTOAVisuGenerator->SetRenderingFunction(renderer3);
  lRTOAVisuGenerator->SetGenerateQuicklook(false);
  lRTOAVisuGenerator->SetQuicklook(m_QLImageToReflectanceFilter->GetOutput());
  lRTOAVisuGenerator->GenerateLayer();
  lRTOAVisuGenerator->GetLayer()->SetName("Reflectance TOA");

 // Save the Input Image Layer
  m_ImageLayerList->PushBack(lRTOAVisuGenerator->GetLayer());
  // Add the layer
  m_RVisuModel->AddLayer(lRTOAVisuGenerator->GetLayer());

  /// Generate reflectance TOC image layer
  ImageLayerGeneratorPointerType lRTOCVisuGenerator = ImageLayerGeneratorType::New();
  lRTOCVisuGenerator->SetImage(m_ReflectanceToSurfaceReflectanceFilter->GetOutput());
  lRTOAVisuGenerator->SetRenderingFunction(renderer3);
  lRTOCVisuGenerator->SetGenerateQuicklook(false);
  lRTOCVisuGenerator->SetQuicklook(m_QQLReflectanceToSurfaceReflectanceFilter->GetOutput());
  lRTOCVisuGenerator->GenerateLayer();
  lRTOCVisuGenerator->GetLayer()->SetName("Reflectance TOC");
  // Save the Input Image Layer
  m_ImageLayerList->PushBack(lRTOCVisuGenerator->GetLayer());
  // Add the layer
  m_RVisuModel->AddLayer(lRTOCVisuGenerator->GetLayer());


  /// Generate difference TOA-TOC image layer
  ImageLayerGeneratorPointerType lDiffVisuGenerator = ImageLayerGeneratorType::New();
  lDiffVisuGenerator->SetImage(m_DifferenceFilter->GetOutput());
  lDiffVisuGenerator->GenerateLayer();
  lDiffVisuGenerator->GetLayer()->SetName("Reflectances Difference");
  lDiffVisuGenerator->SetGenerateQuicklook(false);
  lDiffVisuGenerator->SetQuicklook(m_QLDifferenceFilter->GetOutput());
  // Save the Input Image Layer
  m_ImageLayerList->PushBack(lDiffVisuGenerator->GetLayer());
  // Add the layer
  m_RVisuModel->AddLayer(lDiffVisuGenerator->GetLayer());

 std::cout<<"phase 2 faite, cool"<<std::endl;

  // Render
  m_RVisuModel->Update();
  std::cout<<"phase 3 faite, cool"<<std::endl;

}


/** Update the choosen layer and display it */
void
OpticCalibrationModel
::UpdateDisplay(EnumImageType pType)
{
  if( m_ImageLayerList->Size() < static_cast<unsigned int>(pType))
  {
    itkExceptionMacro(<<"Invalid Image layer list or Enum image type.");
  }

  ImageLayerListType::Iterator it;
  // Add the layer to the model ( begin+1 to avoid checking the input image layer )
  for ( it = m_ImageLayerList->Begin()+1;it!=m_ImageLayerList->End();++it)
  {
    it.Get()->SetVisible(false);
  }
  m_ImageLayerList->GetNthElement(pType)->SetVisible(true);
  // Render
  m_RVisuModel->Update();
}

/** Check meta data validity */
void
OpticCalibrationModel
::CheckMetadata(ImagePointerType img) const
{
  ImageMetadataInterfacePointerType lImageMetadataInterface = ImageMetadataInterfaceFactory::CreateIMI(img/*lReader->GetOutput()*/->GetMetaDataDictionary());
  // ImageToLuminance
  lImageMetadataInterface->GetPhysicalGain(img->GetMetaDataDictionary());
  lImageMetadataInterface->GetPhysicalBias(img->GetMetaDataDictionary());

  // ImageToReflectance
  lImageMetadataInterface->GetDay(img->GetMetaDataDictionary());
  lImageMetadataInterface->GetMonth(img->GetMetaDataDictionary());

  lImageMetadataInterface->GetSolarIrradiance(img->GetMetaDataDictionary());
  lImageMetadataInterface->GetSunElevation(img->GetMetaDataDictionary());
}

/** Open a new image */
void
OpticCalibrationModel
::OpenImage( ImagePointerType img )
{ 
  this->Initialization();
  
  // Set the input image
  m_InputImage = img;
  
  // Wire pipeline
  
  this->CalibrateExtract();
  // Init ReflectanceToSurfaceReflectanceFilter function values to be able to add the layer.
  AtmosphericRadiativeTerms::Pointer radTerms = AtmosphericRadiativeTerms::New();
  radTerms->ValuesInitialization( m_InputImage->GetNumberOfComponentsPerPixel() );
  m_ReflectanceToSurfaceReflectanceFilter->SetAtmosphericRadiativeTerms(radTerms);
  m_ReflectanceToSurfaceReflectanceFilter->UpdateOutputInformation();
  m_ReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);
  
  // QL pipeline
  //m_QLImageToLuminanceFilter->SetInput(lVisuGenerator->GetQuicklook());
  m_QLReflectanceToSurfaceReflectanceFilter->SetAtmosphericRadiativeTerms(radTerms);
  //m_QLReflectanceToSurfaceReflectanceFilter->UpdateOutputInformation();
  //m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);

  // Layers
  this->DisplayImage();
  this->NotifyAll("SetInputImage");
}

bool
OpticCalibrationModel
::LoadSpectralSensitivity()
{
  // Only have Spot data
  SpotImageMetadataInterface::Pointer lImageMetadataInterface = SpotImageMetadataInterface::New();
  
  if( !lImageMetadataInterface->CanRead( m_InputImage->GetMetaDataDictionary() ) )
    return false;
  else
    {
      std::string path=Monteverdi_SOURCE_DIR;
      path.append("/Code/Modules/OpticCalibration/data");
      SpectralSensitivityReader::Pointer spectSen = SpectralSensitivityReader::New();
      spectSen->SetDataPath( path );
      spectSen->SetImage( m_InputImage );//m_Reader->GetOutput() );
      spectSen->Update();

      FilterFunctionCoefVectorType ffvfVect;
      for(unsigned int i =0; i<spectSen->GetOutput()->Size(); i++)
	{
	  // loop because ObjectList to std::vector...
	  this->GetAtmosphericCorrectionParameters()->SetWavelenghtSpectralBandWithIndex( i, spectSen->GetOutput()->GetNthElement(i));
	  m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetWavelenghtSpectralBandWithIndex( i, spectSen->GetOutput()->GetNthElement(i));
	}

      m_ReflectanceToSurfaceReflectanceFilter->SetIsSetAtmosphericRadiativeTerms(false);
      m_ReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(true);
      m_ReflectanceToSurfaceReflectanceFilter->GenerateParameters();
      m_ReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);

      // QL pipeline
      m_QLReflectanceToSurfaceReflectanceFilter->SetIsSetAtmosphericRadiativeTerms(false);
      m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(true);
      m_QLReflectanceToSurfaceReflectanceFilter->GenerateParameters();
      m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);

      return true;
    }
}

/** Pipeline : Optic Calibration */
void
OpticCalibrationModel
::CalibrateExtract()
{
  if(m_InputImage.IsNotNull())
  {
    /** Compute the reflectance image if all the needed parameters are present */
    m_ImageToLuminanceFilter->SetInput(m_InputImage);
    m_ImageToReflectanceFilter->SetInput(m_ImageToLuminanceFilter->GetOutput());
    m_ReflectanceToSurfaceReflectanceFilter->SetInput(m_ImageToReflectanceFilter->GetOutput());
    m_DifferenceFilter->SetValidInput(m_ImageToReflectanceFilter->GetOutput());
    m_DifferenceFilter->SetTestInput(m_ReflectanceToSurfaceReflectanceFilter->GetOutput());

    // QL pipeline
    //m_ImageToLuminanceFilter->SetInput(m_InputImage);
    m_QLImageToReflectanceFilter->SetInput(m_QLImageToLuminanceFilter->GetOutput());
    m_QLReflectanceToSurfaceReflectanceFilter->SetInput(m_QLImageToReflectanceFilter->GetOutput());
    m_QLDifferenceFilter->SetValidInput(m_QLImageToReflectanceFilter->GetOutput());
    m_QLDifferenceFilter->SetTestInput(m_QLReflectanceToSurfaceReflectanceFilter->GetOutput());
  }
  else
  {
    itkExceptionMacro(<<"Image is not initialized.");
  }
}

void
OpticCalibrationModel
::Quit()
{
  m_LuminanceImage          = ImageType::New();
  m_ReflectanceImage        = ImageType::New();
  m_SurfaceReflectanceImage = ImageType::New();
  m_DifferenceImage         = ImageType::New();

  this->CalibrateExtract();

  // Set output Image
  m_LuminanceImage          = m_ImageToLuminanceFilter->GetOutput();

  // If scale reflectance, add a multiplier
  if(m_ChangeReflectanceScale)
    {
      m_TOAMultiplier        = MultiplyByScalarImageFilterType::New();
      m_TOCMultiplier        = MultiplyByScalarImageFilterType::New();
      m_DiffTOATOCMultiplier = MultiplyByScalarImageFilterType::New();
      
      m_TOAMultiplier->SetCoef(1000.);
      m_TOCMultiplier->SetCoef(1000.);
      m_DiffTOATOCMultiplier->SetCoef(1000.);
      
      m_TOAMultiplier->SetInput(m_ImageToReflectanceFilter->GetOutput());
      m_TOCMultiplier->SetInput(m_ReflectanceToSurfaceReflectanceFilter->GetOutput());    
      m_DiffTOATOCMultiplier->SetInput(m_DifferenceFilter->GetOutput());

      m_ReflectanceImage        = m_TOAMultiplier->GetOutput();
      m_SurfaceReflectanceImage = m_TOCMultiplier->GetOutput();
      m_DifferenceImage         = m_DiffTOATOCMultiplier->GetOutput();
    }
  else
    {
      m_ReflectanceImage        = m_ImageToReflectanceFilter->GetOutput();
      m_SurfaceReflectanceImage = m_ReflectanceToSurfaceReflectanceFilter->GetOutput();
      m_DifferenceImage         = m_DifferenceFilter->GetOutput();
    }

  this->NotifyAll("OutputsUpdated");
  this->NotifyAll("BusyOff");
}


void 
OpticCalibrationModel
::UpdateRadiativeTerm(unsigned int ch, double intRef, double albebo, double gasT, double dT, double uT, double uDiffT, double uDirT, double uDTR, double uDTA )
{
  try
    {
      this->GetAtmosphericRadiativeTerms()->SetIntrinsicAtmosphericReflectance( ch, intRef );
      this->GetAtmosphericRadiativeTerms()->SetSphericalAlbedo( ch, albebo );
      this->GetAtmosphericRadiativeTerms()->SetTotalGaseousTransmission( ch, gasT );
      this->GetAtmosphericRadiativeTerms()->SetDownwardTransmittance( ch, dT );
      this->GetAtmosphericRadiativeTerms()->SetUpwardTransmittance( ch, uT );
      this->GetAtmosphericRadiativeTerms()->SetUpwardDiffuseTransmittance( ch, uDiffT );
      this->GetAtmosphericRadiativeTerms()->SetUpwardDirectTransmittance( ch, uDirT );
      this->GetAtmosphericRadiativeTerms()->SetUpwardDiffuseTransmittanceForRayleigh( ch, uDTR );
      this->GetAtmosphericRadiativeTerms()->SetUpwardDiffuseTransmittanceForAerosol( ch, uDTA );
      

      m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetIntrinsicAtmosphericReflectance( ch, intRef );
m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetSphericalAlbedo( ch, albebo );
m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetTotalGaseousTransmission( ch, gasT );
 m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetDownwardTransmittance( ch, dT );
m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetUpwardTransmittance( ch, uT );
m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetUpwardDiffuseTransmittance( ch, uDiffT );
m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetUpwardDirectTransmittance( ch, uDirT );
m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetUpwardDiffuseTransmittanceForRayleigh( ch, uDTR );
m_QLReflectanceToSurfaceReflectanceFilter->GetAtmosphericRadiativeTerms()->SetUpwardDiffuseTransmittanceForAerosol( ch, uDTA );

      /** param update is done by the controller, that allows to change each channel without computed is parameters each time*/
    }
  catch (itk::ExceptionObject & err)
    {
      itkExceptionMacro(<<"The Input Image of the ReflectanceToSurfaceReflectanceFilter is not initialized."<<std::endl<<err);
    }     
}


void
OpticCalibrationModel
::UpdateCorrectionParameters(AerosolModelType aeroMod, double ozAmount, double atmoPres)
{
  try
    {
      this->GetAtmosphericCorrectionParameters()->SetAerosolModel( static_cast<AerosolModelType>(aeroMod) );
      this->GetAtmosphericCorrectionParameters()->SetOzoneAmount( ozAmount );
      this->GetAtmosphericCorrectionParameters()->SetAtmosphericPressure( atmoPres );
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetAerosolModel( static_cast<AerosolModelType>(aeroMod) );
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetOzoneAmount( ozAmount );
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetAtmosphericPressure( atmoPres );

      m_ReflectanceToSurfaceReflectanceFilter->SetIsSetAtmosphericRadiativeTerms(false);
      m_ReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(true);
      m_ReflectanceToSurfaceReflectanceFilter->GenerateParameters();
      m_ReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);

      // QL pipeline
      m_QLReflectanceToSurfaceReflectanceFilter->SetIsSetAtmosphericRadiativeTerms(false);
      m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(true);
      m_QLReflectanceToSurfaceReflectanceFilter->GenerateParameters();
      m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);
    }
  catch (itk::ExceptionObject & err)
    {
      itkExceptionMacro(<<"The Input Image of the ReflectanceToSurfaceReflectanceFilter is not initialized."<<std::endl<<err);
    }
}

void
OpticCalibrationModel
::UpdateCorrectionParameters(AerosolModelType aeroMod, double ozAmount, double atmoPres, double aeroTh, double waterAm)
{
  try
    {
      this->GetAtmosphericCorrectionParameters()->SetAerosolModel( static_cast<AerosolModelType>(aeroMod) );
      this->GetAtmosphericCorrectionParameters()->SetOzoneAmount( ozAmount );
      this->GetAtmosphericCorrectionParameters()->SetAtmosphericPressure( atmoPres );
      this->GetAtmosphericCorrectionParameters()->SetAerosolOptical(aeroTh);
      this->GetAtmosphericCorrectionParameters()->SetWaterVaporAmount(waterAm);
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetAerosolModel( static_cast<AerosolModelType>(aeroMod) );
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetOzoneAmount( ozAmount );
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetAtmosphericPressure( atmoPres );
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetAerosolOptical(aeroTh);
m_QLReflectanceToSurfaceReflectanceFilter->GetCorrectionParameters()->SetWaterVaporAmount(waterAm);

      m_ReflectanceToSurfaceReflectanceFilter->SetIsSetAtmosphericRadiativeTerms(false);
      m_ReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(true);
      m_ReflectanceToSurfaceReflectanceFilter->GenerateParameters();
      m_ReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);

      m_QLReflectanceToSurfaceReflectanceFilter->SetIsSetAtmosphericRadiativeTerms(false);
      m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(true);
      m_QLReflectanceToSurfaceReflectanceFilter->GenerateParameters();
      m_QLReflectanceToSurfaceReflectanceFilter->SetUseGenerateParameters(false);

    }
  catch (itk::ExceptionObject & err)
    {
      itkExceptionMacro(<<"The Input Image of the ReflectanceToSurfaceReflectanceFilter is not initialized."<<std::endl<<err);
    }

}


void
OpticCalibrationModel
::UpdateRGBChannelOrder(int redChoice , int greenChoice, int blueChoice)
{
  ChannelListType channels;
  channels.push_back(redChoice);
  channels.push_back(greenChoice);
  channels.push_back(blueChoice);

  StandardRenderingFunctionType::Pointer inputRendering = StandardRenderingFunctionType::New();
  inputRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(INPUT_IMAGE)->SetRenderingFunction(inputRendering);

  StandardRenderingFunctionType::Pointer luminanceRendering = StandardRenderingFunctionType::New();
  luminanceRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(LUMINANCE)->SetRenderingFunction(luminanceRendering);

  StandardRenderingFunctionType::Pointer reflectanceRendering = StandardRenderingFunctionType::New();
  reflectanceRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(REFLECTANCE_TOA)->SetRenderingFunction(reflectanceRendering);

  StandardRenderingFunctionType::Pointer reflectanceSurfRendering = StandardRenderingFunctionType::New();
  reflectanceSurfRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(REFLECTANCE_TOC)->SetRenderingFunction(reflectanceSurfRendering);
 
  StandardRenderingFunctionType::Pointer diffRendering = StandardRenderingFunctionType::New();
  diffRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(DIFF_REFL)->SetRenderingFunction(diffRendering);

  // Update the view
  m_VisuModel->Update();
  // Update the result
  m_RVisuModel->Update();

}

void
OpticCalibrationModel
::UpdateGrayscaleChannel(int choice)
{
  ChannelListType channels;
  channels.push_back(choice);

  StandardRenderingFunctionType::Pointer inputRendering = StandardRenderingFunctionType::New();
  inputRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(INPUT_IMAGE)->SetRenderingFunction(inputRendering);

  StandardRenderingFunctionType::Pointer luminanceRendering = StandardRenderingFunctionType::New();
  luminanceRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(LUMINANCE)->SetRenderingFunction(luminanceRendering);

  StandardRenderingFunctionType::Pointer reflectanceRendering = StandardRenderingFunctionType::New();
  reflectanceRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(REFLECTANCE_TOA)->SetRenderingFunction(reflectanceRendering);
  
  StandardRenderingFunctionType::Pointer reflectanceSurfRendering = StandardRenderingFunctionType::New();
  reflectanceSurfRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(REFLECTANCE_TOC)->SetRenderingFunction(reflectanceSurfRendering);

  StandardRenderingFunctionType::Pointer diffRendering = StandardRenderingFunctionType::New();
  diffRendering->SetChannelList(channels);
  m_ImageLayerList->GetNthElement(DIFF_REFL)->SetRenderingFunction(diffRendering);

  // Update the view
  m_VisuModel->Update();
  // Update the result
  m_RVisuModel->Update();

}


}

