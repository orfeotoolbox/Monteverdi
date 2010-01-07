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

#include "otbOpticCalibrationController.h"
#include "otbMsgReporter.h"
#include "otbAtmosphericCorrectionParameters.h"

namespace otb
{

OpticCalibrationController
::OpticCalibrationController()
{
  // Build a "visu"controller
  m_VisuController = VisuControllerType::New();
  m_RVisuController = VisuControllerType::New();

  // Handlers
  // Input viewer
  m_ResizingHandler     = ResizingHandlerType::New();
  m_ChangeScaledHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeScaleHandler  = ChangeScaleHandlerType::New();
  m_ChangeRegionHandler = ChangeRegionHandlerType::New();
  m_PixelActionHandler  = PixelDescriptionActionHandlerType::New();
  m_PixelActionModel    = PixelDescriptionModelType::New();
  m_PixelActionView     = PixelDescriptionViewType::New();

  m_VisuController->AddActionHandler(m_ResizingHandler);
  m_VisuController->AddActionHandler(m_ChangeScaledHandler);
  m_VisuController->AddActionHandler(m_ChangeScaleHandler);
  m_VisuController->AddActionHandler(m_ChangeRegionHandler);
  m_VisuController->AddActionHandler(m_PixelActionHandler);

  // Result viewer
  m_ResultResizingHandler     = ResizingHandlerType::New();
  m_ResultChangeScaledHandler = ChangeScaledRegionHandlerType::New();
  m_ResultChangeScaleHandler  = ChangeScaleHandlerType::New();
  m_ResultChangeRegionHandler = ChangeRegionHandlerType::New();
  m_ResultPixelActionHandler  = PixelDescriptionActionHandlerType::New();
  m_ResultPixelActionModel    = PixelDescriptionModelType::New();
  m_ResultPixelActionView     = PixelDescriptionViewType::New();

  m_RVisuController->AddActionHandler(m_ResultResizingHandler);
  m_RVisuController->AddActionHandler(m_ResultChangeScaledHandler);
  m_RVisuController->AddActionHandler(m_ResultChangeScaleHandler);
  m_VisuController->AddActionHandler(m_ResultChangeRegionHandler);
  m_RVisuController->AddActionHandler(m_ResultPixelActionHandler);

  m_IsValidImage = false;
}

OpticCalibrationController
::~OpticCalibrationController()
{}

void
OpticCalibrationController
::LinkPixelDescriptors()
{
  // First image pixel descriptor
  m_PixelActionModel->SetLayers(m_Model->GetVisuModel()->GetLayers());
  m_PixelActionView->SetModel(m_PixelActionModel);
  m_PixelActionHandler->SetModel(m_PixelActionModel);
  m_PixelActionHandler->SetView(m_View->GetVisuView());
  m_View->m_PixelDescriptionGroup->add(m_PixelActionView->GetPixelDescriptionWidget());
  m_PixelActionView->GetPixelDescriptionWidget()->show();
  m_PixelActionView->GetPixelDescriptionWidget()->resize(m_View->m_PixelDescriptionGroup->x(),m_View->m_PixelDescriptionGroup->y(), m_View->m_PixelDescriptionGroup->w(), m_View->m_PixelDescriptionGroup->h());

  // Second image pixel descriptor
  m_ResultPixelActionModel->SetLayers(m_Model->GetRVisuModel()->GetLayers());
  m_ResultPixelActionView->SetModel(m_ResultPixelActionModel);
  m_ResultPixelActionHandler->SetModel(m_ResultPixelActionModel);
  m_ResultPixelActionHandler->SetView(m_View->GetRVisuView());
  m_View->m_RPixelDescriptionGroup->add(m_ResultPixelActionView->GetPixelDescriptionWidget());
  m_ResultPixelActionView->GetPixelDescriptionWidget()->show();
  m_ResultPixelActionView->GetPixelDescriptionWidget()->resize(m_View->m_RPixelDescriptionGroup->x(),m_View->m_RPixelDescriptionGroup->y(), m_View->m_RPixelDescriptionGroup->w(), m_View->m_RPixelDescriptionGroup->h());
}



void
OpticCalibrationController
::SetInputImage( ImagePointerType img )
{
  m_IsValidImage = true;
  try
    {
      m_Model->CheckMetadata(img);
    }
  catch (itk::ExceptionObject)
    {
      m_IsValidImage = false;
      MsgReporter::GetInstance()->SendError("Invalid or missing metadata. Please check your image is a SPOT4-5, IKONOS or Quickbird one");
      return;
    }

  try
    {
      m_Model->OpenImage( img );
      bool res = m_Model->LoadSpectralSensitivity();
      if(res)
	{
	  m_View->UpdateParamDisplay();
	  m_View->tCorrParam->show();
	}
    }
  catch (itk::ExceptionObject err)
    {
      m_IsValidImage = false;
      itk::OStringStream oss;
      oss.str("");
      oss<<"An error occurs at the application initialization. Please check your input data.";
      oss<<err;
      MsgReporter::GetInstance()->SendError(oss.str());
    }
}

void
OpticCalibrationController
::QuitModel()
{
  try
    {
      m_Model->Quit();
    }
  catch (itk::ExceptionObject err)
    {
      itk::OStringStream oss;
      oss.str("");
      oss<<"An error occurs while loading outputs.";
      oss<<err;
      MsgReporter::GetInstance()->SendError(oss.str());
    }
}




void
OpticCalibrationController
::SetView(ViewType* pView)
{
  m_View = pView;
  // Input Viewer
  m_ResizingHandler->SetView(m_View->GetVisuView()); 
  m_ChangeScaledHandler->SetView(m_View->GetVisuView());
  m_ChangeScaleHandler->SetView(m_View->GetVisuView());
  m_ChangeRegionHandler->SetView(m_View->GetVisuView()); 

  // Result Viewer
  m_ResultResizingHandler->SetView(m_View->GetRVisuView()); 
  m_ResultChangeScaledHandler->SetView(m_View->GetRVisuView());
  m_ResultChangeScaleHandler->SetView(m_View->GetRVisuView());
  m_ResultChangeRegionHandler->SetView(m_View->GetVisuView());
}


void
OpticCalibrationController
::SetModel(ModelType* pModel)
{
  m_Model = pModel;
  // Input Viewer
  m_ResizingHandler->SetModel(m_Model->GetVisuModel()); 
  m_ChangeScaledHandler->SetModel(m_Model->GetVisuModel()); 
  m_ChangeScaleHandler->SetModel(m_Model->GetVisuModel());
  m_ChangeRegionHandler->SetModel(m_Model->GetVisuModel());

  // Result Viewer
  m_ResultResizingHandler->SetModel(m_Model->GetRVisuModel()); 
  m_ResultChangeScaledHandler->SetModel(m_Model->GetRVisuModel());
  m_ResultChangeScaleHandler->SetModel(m_Model->GetRVisuModel());
  m_ResultChangeRegionHandler->SetModel(m_Model->GetRVisuModel());
}


void 
OpticCalibrationController
::UpdateRGBChannelOrder(unsigned int red,unsigned int green,unsigned int blue)
{
  try
  {
    m_Model->UpdateRGBChannelOrder(red,green,blue);
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void 
OpticCalibrationController
::UpdateGrayscaleChannel(unsigned int channel)
{
  try
  {
    m_Model->UpdateGrayscaleChannel(channel);
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
OpticCalibrationController
::UpdateDisplay(EnumImageType pType)
{
  if( m_IsValidImage == false )
    return;
  
  try
    {
      m_Model->UpdateDisplay(pType);
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}


void
OpticCalibrationController
::UpdateRadiativeTerms( bool updateIm )
{
  unsigned int ch = static_cast<unsigned int>(atoi(m_View->guiBandSelection->value()));
  double intRef   = m_View->gIntrinsicRef->value();
  double albedo   = m_View->guiAlbedo->value();
  double gasT     = m_View->guiGasT->value();
  double dT       = m_View->guiDT->value();
  double uT       = m_View->guiUT->value();
  double uDiffT   = m_View->guiUDiffT->value();
  double uDirR    = m_View->guiUDirT->value();
  double uDTR     = m_View->guiUDTR->value();
  double uDTA     = m_View->guiUDTA->value();

  try
    {
      m_Model->UpdateRadiativeTerm( ch, intRef, albedo, gasT, dT, uT, uDiffT, uDirR, uDTR, uDTA );
      // update display only ik OK button was pushed
      if(updateIm==true)
	{
	  m_Model->GetReflectanceToSurfaceReflectanceFilter()->SetIsSetAtmosphericRadiativeTerms(true);
	  m_Model->GetReflectanceToSurfaceReflectanceFilter()->GenerateParameters();
	  m_Model->GetReflectanceToSurfaceReflectanceFilter()->SetIsSetAtmosphericRadiativeTerms(false);
	}
    }
  catch (itk::ExceptionObject & err)
    {
      itk::OStringStream oss;
      oss.str("");
      oss<<"An error occurs update Radiatrive Terms.";
      oss<<err;
      MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
  m_View->UpdateParamDisplay();
} 


void
OpticCalibrationController
::UpdateCorrectionParameters()
{
  AerosolModelType aeroMod = AtmosphericCorrectionParameters::NO_AEROSOL;
  std::string aeroModStd = m_View->guiAerosolModel->value();

  if( aeroModStd == "NO AEROSOL")
    aeroMod = AtmosphericCorrectionParameters::NO_AEROSOL;
  else if( aeroModStd == "CONTINENTAL")
    aeroMod = AtmosphericCorrectionParameters::CONTINENTAL;
  else if( aeroModStd == "MARITIME")
    aeroMod = AtmosphericCorrectionParameters::MARITIME;
  else if( aeroModStd == "URBAN")
    aeroMod = AtmosphericCorrectionParameters::URBAN;
  else if( aeroModStd == "DESERTIC")
    aeroMod = AtmosphericCorrectionParameters::DESERTIC;
  else 
  {
    itkExceptionMacro(<<"Invalid Aerosol Model Type: "<<aeroModStd);
  }    

  double ozAmount = m_View->guiOzoneAmount->value();
  double atmoPres = m_View->guiAtmoPressure->value();
  double aeroTh   = m_View->guiAeroTh->value();
  double waterAm  = m_View->guiWater->value();

  bool aeronetFile = false;
  std::string aeroFile = m_View->teAeronetFile->value();
  if( aeroFile != m_Model->GetReflectanceToSurfaceReflectanceFilter()->GetAeronetFileName() && aeroFile != "")
    {
      m_Model->GetReflectanceToSurfaceReflectanceFilter()->SetAeronetFileName(aeroFile);
      aeronetFile = true;
    }
    
  std::string ffvFile = m_View->teFFVFile->value();
  if( ffvFile != m_Model->GetReflectanceToSurfaceReflectanceFilter()->GetFilterFunctionValuesFileName() &&  ffvFile != "" )
    m_Model->GetReflectanceToSurfaceReflectanceFilter()->SetFilterFunctionValuesFileName(ffvFile);

  try
    {
      if(aeronetFile)
	m_Model->UpdateCorrectionParameters(aeroMod, ozAmount, atmoPres);
      else
	m_Model->UpdateCorrectionParameters(aeroMod, ozAmount, atmoPres, aeroTh, waterAm);

      m_View->UpdateParamDisplay();
    }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
    }    
}

} // end namespace otb

