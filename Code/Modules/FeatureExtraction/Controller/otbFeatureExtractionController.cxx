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

#include "otbFeatureExtractionController.h"
// #include "otbMsgReporter.h"

namespace otb
{

FeatureExtractionController
::FeatureExtractionController()
{

/** NewVisu */
  // Build a "visu"controller
  m_VisuController = VisuControllerType::New();
  m_ResultVisuController = VisuControllerType::New();

}

FeatureExtractionController
::~FeatureExtractionController()
{}



void
FeatureExtractionController
::OpenInputImage(const char * filename)
{
  try
  {
    std::string strFilename = filename;
    this->ClearSelectedChannels();
    m_Model->SetInputImage( strFilename );
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}


void
FeatureExtractionController
::SetOutputFileName( const char * name )
{
  try
  {
    m_Model->SetOutputFileName( name );
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
FeatureExtractionController
::SaveOutput()
{
  try
  {
    m_Model->GenerateOutputImage();
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
FeatureExtractionController
::ClearFeatures()
{
  try
  {

    m_Model->ClearFilterList();
    m_Model->ClearFilterTypeList();
    m_Model->ClearOutputFilterInformation();
    m_Model->ClearSelectedFilters();
    m_Model->ClearOutputListOrder();
    m_Model->InitMeanShiftLists();
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }

}

void
FeatureExtractionController
::ClearSelectedChannels()
{
  try
  {
    this->ClearFeatures();
    m_Model->ClearOutputChannelsInformation();
    m_Model->ClearInputImageList();
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
FeatureExtractionController
::AddInputChannels(std::vector<unsigned int> chList)
{
  try
  {
    // channel index starts at 1
    m_Model->ClearInputImageList();
    m_Model->AddChannels(chList);
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}


void
FeatureExtractionController
::ChangeFilterStatus(int id)
{
  try
  {
    m_Model->GetSelectedFilters()[id] = !(m_Model->GetSelectedFilters()[id]);
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}



void
FeatureExtractionController
::InitInput()
{
  try
  {
    m_Model->InitInput();
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
FeatureExtractionController
::AddToOutputListOrder(int id)
{
  try
  {
    m_Model->AddToOutputListOrder(id);
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}


void
FeatureExtractionController
::RemoveFromOutputListOrder(int id)
{
  try
  {
    m_Model->RemoveFromOutputListOrder(id);
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
FeatureExtractionController
::ExchangeOutputListOrder( int direction )
{
  int id = m_View->guiOutputFeatureList->value()-1;
  // if switch with  next element
  if (direction > 0 )
    {
      if(id!=0)
	m_Model->ExchangeOutputListOrder( id,id-1 );
      else
	m_Model->ExchangeOutputListOrder( id, m_View->guiOutputFeatureList->size()-1 );
    }
  // if switch with  previous element
  else
    {
      if(id!=m_View->guiOutputFeatureList->size()-1)
	m_Model->ExchangeOutputListOrder (id+1,id );
      else
	m_Model->ExchangeOutputListOrder ( id, 0 );
    }
}


void FeatureExtractionController::ViewedRegionChanged()
{
  try
  {
    m_View->UpdateFeaturePreview();
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
FeatureExtractionController
::CreateFeature(FeatureType featureType)
{
  try
  {
//     if( !m_Model->GetHasInput() )
//       MsgReporter::GetInstance()->SendError("Please select an input image");
    
    //itkExceptionMacro(<<"Please select an input image" );

    int beforeSize = m_Model->GetOutputFilterInformation().size();
    std::vector<double> params;

    switch (featureType)
    {
    case TOUZI:
    {
      unsigned int radius = static_cast<unsigned int>(m_View->guiTouziRadius->value());
      m_Model->AddTouziFilter(radius);
      break;
    }
    case HARRIS:
    {
      double sigmaD = m_View->guiHarrisSigmaD->value();
      double sigmaI = m_View->guiHarrisSigmaI->value();
      double alpha = m_View->guiHarrisAlpha->value();
      m_Model->AddHarrisFilter(sigmaD,sigmaI, alpha);
      break;
    }
    case SPECTRALANGLE:
    {
      if ( m_View->GetSelectedPixel().GetSize() != m_Model->GetNumberOfChannels() )
      {
        itkExceptionMacro(<<"Reference pixel is not set!");
      }
      m_Model->AddSpectralAngleFilter(m_View->GetSelectedPixel());
      break;
    }
    case MORPHOLOGICALOPENING:
    {
      int size = static_cast<int>(m_View->guiMorphoProfileSize->value());
      int value = static_cast<int>(m_View->guiMorphoInitVal->value());
      int step = static_cast<int>(m_View->guiMorphoStep->value());
      m_Model->AddMorphoOpeningProfileFilter(size, value, step);
      break;
    }
    case MORPHOLOGICALCLOSING:
    {
      int size = static_cast<int>(m_View->guiMorphoProfileSize->value());
      int value = static_cast<int>(m_View->guiMorphoInitVal->value());
      int step = static_cast<int>(m_View->guiMorphoStep->value());
      m_Model->AddMorphoClosingProfileFilter(size, value, step);
      break;
    }
    case VARIANCE:
    {
      int radiusX = static_cast<int>(m_View->guiRadiusX->value());
      int radiusY = static_cast<int>(m_View->guiRadiusY->value());
      m_Model->AddVarianceFilter(radiusX, radiusY);
      break;
    }
    case MEAN:
    {
      int radiusX = static_cast<int>(m_View->guiRadiusX->value());
      int radiusY = static_cast<int>(m_View->guiRadiusY->value());
      m_Model->AddMeanFilter(radiusX, radiusY);
      break;
    }
    case GRADIENT:
    {
      double sigma = m_View->guiGradSigma->value();
      m_Model->AddGradientFilter(sigma);
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
    case TEXT_SAV:
    case TEXT_SEN:
    case TEXT_SVA:
    case TEXT_VAR:
    {
      SizeType rad;
      rad[0] = static_cast<unsigned int>(m_View->guiTextRadiusX->value());
      rad[1] = static_cast<unsigned int>(m_View->guiTextRadiusY->value());
      OffsetType off;
      off[0] = static_cast<int>(m_View->guiTextOffsetX->value());
      off[1] = static_cast<int>(m_View->guiTextOffsetY->value());
      m_Model->AddTextureFilter(featureType, rad, off);
      break;
    }
    case TEXT_PANTEX:
      {
        SizeType rad;
      rad[0] = static_cast<unsigned int>(m_View->guiRadiusX->value());
      rad[1] = static_cast<unsigned int>(m_View->guiRadiusY->value());
      // set offset to be able to call the generic texture add filter
      OffsetType off;
      off.Fill(0);
      m_Model->AddTextureFilter(featureType, rad, off);
      break;
      }
    case NDVI:
    case RVI:
    case GEMI:
    case IPVI:
    case TNDVI:
    {
      int RId = static_cast<int>(m_View->guiRAndNIRR->value());
      int NIRId = static_cast<int>(m_View->guiRAndNIRNIR->value());
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case PVI:
    {
      int RId = static_cast<int>(m_View->guiPVIR->value());
      int NIRId = static_cast<int>(m_View->guiPVINIR->value());
      params.push_back(static_cast<double>(m_View->guiPVIa->value()));
      params.push_back(static_cast<double>(m_View->guiPVIb->value()));
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case SAVI:
    {
      int RId = static_cast<int>(m_View->guiSAVIR->value());
      int NIRId = static_cast<int>(m_View->guiSAVINIR->value());
      params.push_back(static_cast<double>(m_View->guiSAVIL->value()));
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case MSAVI:
    {
      int RId = static_cast<int>(m_View->guiMSAVIR->value());
      int NIRId = static_cast<int>(m_View->guiMSAVINIR->value());
      params.push_back(static_cast<double>(m_View->guiMSAVIs->value()));
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case MSAVI2:
    {
      int RId = static_cast<int>(m_View->guiRAndNIRR->value());
      int NIRId = static_cast<int>(m_View->guiRAndNIRNIR->value());
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case TSAVI:
    {
      int RId = static_cast<int>(m_View->guiTSAVIR->value());
      int NIRId = static_cast<int>(m_View->guiTSAVINIR->value());
      params.push_back(static_cast<double>(m_View->guiTSAVIs->value()));
      params.push_back(static_cast<double>(m_View->guiTSAVIa->value()));
      params.push_back(static_cast<double>(m_View->guiTSAVIx->value()));
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case WDVI:
    {
      int RId = static_cast<int>(m_View->guiWDVIR->value());
      int NIRId = static_cast<int>(m_View->guiWDVINIR->value());
      params.push_back(static_cast<double>(m_View->guiWDVIs->value()));
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case ARVI:
    {
      int RId = static_cast<int>(m_View->guiARVIR->value());
      int BId = static_cast<int>(m_View->guiARVIB->value());
      int NIRId = static_cast<int>(m_View->guiARVINIR->value());
      params.push_back(static_cast<double>(m_View->guiARVIgamma->value()));
      m_Model->AddRAndBAndNIRFilter(RId, BId, NIRId, featureType, params);
      break;
    }
    case EVI:
    {
      int RId = static_cast<int>(m_View->guiEVIR->value());
      int BId = static_cast<int>(m_View->guiEVIB->value());
      int NIRId = static_cast<int>(m_View->guiEVINIR->value());
      params.push_back(static_cast<double>(m_View->guiEVIG->value()));
      params.push_back(static_cast<double>(m_View->guiEVIL->value()));
      params.push_back(static_cast<double>(m_View->guiEVIC1->value()));
      params.push_back(static_cast<double>(m_View->guiEVIC2->value()));
      m_Model->AddRAndBAndNIRFilter(RId, BId, NIRId, featureType, params);
      break;
    }
    case TSARVI:
    {
      int RId = static_cast<int>(m_View->guiTSARVIR->value());
      int BId = static_cast<int>(m_View->guiTSARVIB->value());
      int NIRId = static_cast<int>(m_View->guiTSARVINIR->value());
      params.push_back(static_cast<double>(m_View->guiTSARVIa_rb->value()));
      params.push_back(static_cast<double>(m_View->guiTSARVIb_rb->value()));
      params.push_back(static_cast<double>(m_View->guiTSARVIx->value()));
      m_Model->AddRAndBAndNIRFilter(RId, BId, NIRId, featureType, params);
      break;
    }
    case AVI:
    {
      int RId = static_cast<int>(m_View->guiAVIR->value());
      int GId = static_cast<int>(m_View->guiAVIG->value());
      int NIRId = static_cast<int>(m_View->guiAVINIR->value());
      params.push_back(static_cast<double>(m_View->guiAVIlambda1->value()));
      params.push_back(static_cast<double>(m_View->guiAVIlambda2->value()));
      params.push_back(static_cast<double>(m_View->guiAVIlambda3->value()));
      m_Model->AddRAndGAndNIRFilter(RId, GId, NIRId, featureType, params);
      break;
    }
    case IR:
    case IC:
    case IB:
    {
      int GId = static_cast<int>(m_View->guiSoilG->value());
      int RId = static_cast<int>(m_View->guiSoilR->value());
      m_Model->Add2ChannelsFilter( GId, RId, featureType);
      break;
    }
    case IB2:
    {
      int GId = static_cast<int>(m_View->guiIB2G->value());
      int RId = static_cast<int>(m_View->guiIB2R->value());
      int NIRId = static_cast<int>(m_View->guiIB2NIR->value());
      m_Model->AddRAndGAndNIRFilter(RId, GId, NIRId, featureType, params);
      break;
    }
    case NDBI:
    {
      int TM4Id = static_cast<int>(m_View->guiNDBITM4->value());
      int TM5Id = static_cast<int>(m_View->guiNDBITM5->value());
      m_Model->Add2ChannelsFilter(TM4Id, TM5Id, featureType);
      break;
    }
    case ISU:
    {
      int RId = static_cast<int>(m_View->guiISUR->value());
      int NIRId = static_cast<int>(m_View->guiISUNIR->value());
      params.push_back(static_cast<double>(m_View->guiISUa->value()));
      params.push_back(static_cast<double>(m_View->guiISUb->value()));
      m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
      break;
    }
    case SRWI:
    {
      int R860Id = static_cast<int>(m_View->guiSRWIR860->value());
      int R1240Id = static_cast<int>(m_View->guiSRWIR1240->value());
      m_Model->Add2ChannelsFilter(R860Id, R1240Id, featureType);
      break;
    }
    case NDWI:
    {
      int NIRId = static_cast<int>(m_View->guiNDWINIR->value());
      int MIRId = static_cast<int>(m_View->guiNDWIMIR->value());
      m_Model->Add2ChannelsFilter(NIRId, MIRId, featureType);
      break;
    }
    case NDWI2:
    {
      int GId = static_cast<int>(m_View->guiNDWI2G->value());
      int NIRId = static_cast<int>(m_View->guiNDWI2NIR->value());
      m_Model->Add2ChannelsFilter(GId, NIRId, featureType);
      break;
    }
    case MNDWI:
    {
      int GId = static_cast<int>(m_View->guiMNDWIG->value());
      int MIRId = static_cast<int>(m_View->guiMNDWIMIR->value());
      m_Model->Add2ChannelsFilter(GId, MIRId, featureType);
      break;
    }
    case NDPI:
    {
      int MIRId = static_cast<int>(m_View->guiNDPIMIR->value());
      int GId = static_cast<int>(m_View->guiNDPIG->value());
      m_Model->Add2ChannelsFilter(MIRId, GId, featureType);
      break;
    }
    case NDTI:
    {
      int RId = static_cast<int>(m_View->guiNDTIR->value());
      int GId = static_cast<int>(m_View->guiNDTIG->value());
      m_Model->Add2ChannelsFilter(RId, GId, featureType);
      break;
    }
    case SFS_LEN:
    case SFS_WID:
    case SFS_PSI:
    case SFS_WME:
    case SFS_RAT:
    case SFS_SD:
    {
      double dSpectralThreshold = static_cast<double>(m_View->guiSFSSpectralThreshold->value());
      unsigned int uiSpectralThresh = static_cast<unsigned int>(m_View->guiSFSSpatialThreshold->value());
      unsigned int uiNbDirections = static_cast<unsigned int>(m_View->guiSFSNbDirections->value());
      double dAlpha = static_cast<double>(m_View->guiSFSAlpha->value());
      unsigned int uiRatMaxConsNb = static_cast<unsigned int>(m_View->guiSFSRatMaxConsNb->value());
      m_Model->AddSFSTexturesFilter(featureType,dSpectralThreshold,uiSpectralThresh,uiNbDirections,dAlpha,uiRatMaxConsNb);
      break;
    }
    case EDGE_CANNY:
    {
      params.push_back(static_cast<double>(m_View->guiEdgeCannyRadius->value()));
      params.push_back(static_cast<double>(m_View->guiEdgeCannyMaxThresh->value()));
      params.push_back(static_cast<double>(m_View->guiEdgeCannyMinThresh->value()));
      params.push_back(static_cast<double>(m_View->guiEdgeCannyVariance->value()));
      params.push_back(static_cast<double>(m_View->guiEdgeCannyMaxError->value()));
      m_Model->AddEdgeDensityFilter(featureType,params);
      break;
    }
    case EDGE_SOBEL:
    {
      params.push_back(static_cast<double>(m_View->guiEdgeSobelRadiusX->value()));
      params.push_back(static_cast<double>(m_View->guiEdgeSobelRadiusY->value()));
      params.push_back(static_cast<double>(m_View->guiEgdeSobelLowThresh->value()));
      params.push_back(static_cast<double>(m_View->guiEgdeSobelUpThresh->value()));
      m_Model->AddEdgeDensityFilter(featureType,params);
      break;
    }
    case MS_SMOOTH:
    case MS_CLUSTERED:
    case MS_LABELED:
    case MS_BOUND:
    {
      unsigned int spatial = static_cast<unsigned int>(m_View->guiMSSpatial->value());
      double range = static_cast<double>(m_View->guiMSRange->value());
      unsigned int minSize = static_cast<unsigned int>(m_View->guiMSSize->value());
      double scale = static_cast<double>(m_View->guiMSScale->value());
      m_Model->AddMeanShiftFilter(featureType, spatial, range, minSize, scale);
      break;
    }
    case ORIGINAL:
    {
      m_Model->AddOriginalData();
      break;
    }
    default:
    {
      return;
    }
    }
    int afterSize = m_Model->GetOutputFilterInformation().size();
    for (int i=beforeSize; i<afterSize; i++)
    {
      m_View->guiFeatureList->add( m_Model->GetOutputFilterInformationId(i).c_str() );
      m_View->guiFeatureListAction->add( m_Model->GetOutputFilterInformationId(i).c_str() );
      m_View->guiOutputFeatureList->add( m_Model->GetOutputFilterInformationId(i).c_str() );
      m_View->AddToInputOutputFeatureLink(i);
      this->ChangeFilterStatus(i);
    }
  
    m_View->guiFeatureList->redraw();
    m_View->guiFeatureListAction->redraw();
    m_View->guiOutputFeatureList->redraw();
  }
  catch (itk::ExceptionObject & err)
  {
//     MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}


} // end namespace otb

