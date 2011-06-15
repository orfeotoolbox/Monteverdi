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
#include "otbMsgReporter.h"

namespace otb
{

FeatureExtractionController
::FeatureExtractionController()
{
  /** NewVisu */
  // Build a "visu"controller
  m_VisuController                   = VisuControllerType::New();
  m_ResultVisuController             = VisuControllerType::New();
  m_MouseClickedController           = MouseClickedController::New();
  m_LeftMouseClickedHandler          = MouseClickedHandlertype::New();
  m_ResizingHandler                  = ResizingHandlerType::New();
  m_ChangeExtractRegionHandler       = ChangeExtractRegionHandlerType::New();
  m_ResultResizingHandler            = ResizingHandlerType::New();
  m_ResultChangeExtractRegionHandler = MyChangeExtractRegionHandlerType::New();

  m_ResultChangeExtractRegionHandler->SetController(this);

  // Link pixel clicked model (controller in relity...)
  m_MouseClickedController->SetMouseButton(1);
  m_MouseClickedController->SetImageViewId(0);
  m_MouseClickedController->SetController(this);
  m_LeftMouseClickedHandler->SetModel(m_MouseClickedController);
  m_LeftMouseClickedHandler->SetActiveOnZoomWidget(false);
  m_LeftMouseClickedHandler->SetActiveOnScrollWidget(false);

  m_VisuController->AddActionHandler(m_ResizingHandler);
  m_VisuController->AddActionHandler(m_ChangeExtractRegionHandler);
  m_VisuController->AddActionHandler(m_ResultChangeExtractRegionHandler);
  m_ResultVisuController->AddActionHandler(m_ResultResizingHandler);
  m_VisuController->AddActionHandler(m_LeftMouseClickedHandler);
}

FeatureExtractionController
::~FeatureExtractionController()
{}

FeatureExtractionControllerInterface::ModelType*
FeatureExtractionController::
GetModel()
{
  return m_Model;
}

void
FeatureExtractionController
::SetModel(ModelType::Pointer model)
{
  m_Model = model;
// Register the model to the action handlers
  m_ResizingHandler->SetModel(m_Model->GetVisuModel());
  m_ChangeExtractRegionHandler->SetModel(m_Model->GetVisuModel());
  m_ResultResizingHandler->SetModel(m_Model->GetResultVisuModel());
  m_ResultChangeExtractRegionHandler->SetModel(m_Model->GetResultVisuModel());
}

void
FeatureExtractionController
::SetView(ViewPointerType pView)
{
  m_View = pView;

  m_ResizingHandler->SetView(m_View->GetVisuView());
  m_ChangeExtractRegionHandler->SetView(m_View->GetVisuView());
  m_ResultResizingHandler->SetView(m_View->GetResultVisuView());
  m_ResultChangeExtractRegionHandler->SetView(m_View->GetVisuView());
  m_LeftMouseClickedHandler->SetView(m_View->GetVisuView());
}

void
FeatureExtractionController
::LeftMouseButtonClicked(ContinuousIndexType index)
{
  IndexType id;
  id[0] = static_cast<long int>(index[0]);
  id[1] = static_cast<long int>(index[1]);
  m_View->UpdateSelectedPixel(id);
}

void
FeatureExtractionController
::ShowBarStatus()
{
  m_View->pBar->value(1);
  m_View->pBar->show();
  Fl::check();
}

void
FeatureExtractionController
::HideBarStatus()
{
  m_View->pBar->hide();
  Fl::check();
}

void
FeatureExtractionController
::SetOutputFileName(const char * name)
{
  try
    {
    m_Model->SetOutputFileName(name);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
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
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionController
::ExchangeOutputListOrder(int direction)
{
  int id = m_View->guiOutputFeatureList->value() - 1;
  // if switch with  next element
  if (direction > 0)
    {
    if (id != 0) m_Model->ExchangeOutputListOrder(id, id - 1);
    else m_Model->ExchangeOutputListOrder(id, m_View->guiOutputFeatureList->size() - 1);
    }
  // if switch with  previous element
  else
    {
    if (id != m_View->guiOutputFeatureList->size() - 1) m_Model->ExchangeOutputListOrder (id + 1, id);
    else m_Model->ExchangeOutputListOrder (id, 0);
    }
}

void FeatureExtractionController::ViewedRegionChanged()
{
  try
    {
    m_View->UpdateFeaturePreview();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void
FeatureExtractionController
::CreateFeature(FeatureType featureType)
{
  try
    {
    int                 beforeSize = m_Model->GetOutputFilterInformation().size();
    std::vector<double> params;

    switch (featureType)
      {
      case FeatureInfo::TOUZI:
        {
        unsigned int radius = static_cast<unsigned int>(m_View->guiTouziRadius->value());
        m_Model->AddTouziFilter(radius);
        break;
        }
      case FeatureInfo::HARRIS:
        {
        double sigmaD = m_View->guiHarrisSigmaD->value();
        double sigmaI = m_View->guiHarrisSigmaI->value();
        double alpha = m_View->guiHarrisAlpha->value();
        m_Model->AddHarrisFilter(sigmaD, sigmaI, alpha);
        break;
        }
      case FeatureInfo::SPECTRALANGLE:
        {
        if (m_View->GetSelectedPixel().GetSize() != m_Model->GetNumberOfChannels())
          {
          MsgReporter::GetInstance()->SendError("Reference pixel is not set!");
          return;
          }
        if (m_Model->GetInputImage()->GetNumberOfComponentsPerPixel() > 1)
          {
          m_Model->AddSpectralAngleFilter(m_View->GetSelectedPixel());
          }
        else
          {
          MsgReporter::GetInstance()->SendError("Spectral Angle invalid with mono channel images");
          return;
          }
        break;
        }
      case FeatureInfo::MORPHOLOGICALOPENING:
        {
        int size = static_cast<int>(m_View->guiMorphoProfileSize->value());
        int value = static_cast<int>(m_View->guiMorphoInitVal->value());
        int step = static_cast<int>(m_View->guiMorphoStep->value());
        m_Model->AddMorphoOpeningProfileFilter(size, value, step);
        break;
        }
      case FeatureInfo::MORPHOLOGICALCLOSING:
        {
        int size = static_cast<int>(m_View->guiMorphoProfileSize->value());
        int value = static_cast<int>(m_View->guiMorphoInitVal->value());
        int step = static_cast<int>(m_View->guiMorphoStep->value());
        m_Model->AddMorphoClosingProfileFilter(size, value, step);
        break;
        }
      case FeatureInfo::VARIANCE:
        {
        int radiusX = static_cast<int>(m_View->guiRadiusX->value());
        int radiusY = static_cast<int>(m_View->guiRadiusY->value());
        m_Model->AddVarianceFilter(radiusX, radiusY);
        break;
        }
      case FeatureInfo::MEAN:
        {
        int radiusX = static_cast<int>(m_View->guiRadiusX->value());
        int radiusY = static_cast<int>(m_View->guiRadiusY->value());
        m_Model->AddMeanFilter(radiusX, radiusY);
        break;
        }
      case FeatureInfo::GRADIENT:
        {
        double sigma = m_View->guiGradSigma->value();
        m_Model->AddGradientFilter(sigma);
        break;
        }
      case FeatureInfo::TEXT_PANTEX:
        {
        SizeType rad;
        rad[0] = static_cast<unsigned int>(m_View->guiPanTexRadiusX->value());
        rad[1] = static_cast<unsigned int>(m_View->guiPanTexRadiusY->value());
        // set offset to be able to call the generic texture add filter
        OffsetType off;
        off.Fill(0);

        unsigned int bin = static_cast<unsigned int>(atoi(m_View->guiPanTexBin->value()));

        m_Model->AddTextureFilter(featureType, rad, off, bin);
        break;
        }
      case FeatureInfo::TEXT_HAR:
        {
        SizeType rad;
        rad[0] = static_cast<unsigned int>(m_View->guiHarRadiusX->value());
        rad[1] = static_cast<unsigned int>(m_View->guiHarRadiusY->value());
        // set offset to be able to call the generic texture add filter
        OffsetType off;
        off[0] = static_cast<int>(m_View->guiHarOffsetX->value());
        off[1] = static_cast<int>(m_View->guiHarOffsetY->value());
        HaralickTextureVectorType harList;
        harList = this->GetHaralickTextList();

        unsigned int bin = static_cast<unsigned int>(atoi(m_View->guiHarBin->value()));

        if( m_View->guiHarCk->value() )
          {
            double minHar = static_cast<double>(m_View->guiHarMin->value());
            double maxHar =static_cast<double>(m_View->guiHarMax->value());
            m_Model->AddHaralickTextureFilter(harList, rad, off, minHar, maxHar, bin);
          }
        else
          {
            m_Model->AddHaralickTextureFilter(harList, rad, off, bin);
          }

        
        break;
        }
      case FeatureInfo::TEXT_ADV:
        {
        SizeType rad;
        rad[0] = static_cast<unsigned int>(m_View->guiAdvRadiusX->value());
        rad[1] = static_cast<unsigned int>(m_View->guiAdvRadiusY->value());
        // set offset to be able to call the generic texture add filter
        OffsetType off;
        off[0] = static_cast<int>(m_View->guiAdvOffsetX->value());
        off[1] = static_cast<int>(m_View->guiAdvOffsetY->value());
        AdvancedTextureVectorType advList;
        advList = this->GetAdvancedTextList();

        unsigned int bin = static_cast<unsigned int>(atoi(m_View->guiAdvBin->value()));

       if( m_View->guiAdvHarCk->value() )
          {
            double minHar = static_cast<double>(m_View->guiAdvHarMin->value());
            double maxHar =static_cast<double>(m_View->guiAdvHarMax->value());
            m_Model->AddAdvancedTextureFilter(advList, rad, off, minHar, maxHar, bin);
          }
        else
          {
            m_Model->AddAdvancedTextureFilter(advList, rad, off, bin);
          }


        break;
        }
      case FeatureInfo::NDVI:
      case FeatureInfo::RVI:
      case FeatureInfo::GEMI:
      case FeatureInfo::IPVI:
      case FeatureInfo::TNDVI:
        {
        int RId = static_cast<int>(m_View->guiRAndNIRR->value());
        int NIRId = static_cast<int>(m_View->guiRAndNIRNIR->value());
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::PVI:
        {
        int RId = static_cast<int>(m_View->guiPVIR->value());
        int NIRId = static_cast<int>(m_View->guiPVINIR->value());
        params.push_back(static_cast<double>(m_View->guiPVIa->value()));
        params.push_back(static_cast<double>(m_View->guiPVIb->value()));
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::SAVI:
        {
        int RId = static_cast<int>(m_View->guiSAVIR->value());
        int NIRId = static_cast<int>(m_View->guiSAVINIR->value());
        params.push_back(static_cast<double>(m_View->guiSAVIL->value()));
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::MSAVI:
        {
        int RId = static_cast<int>(m_View->guiMSAVIR->value());
        int NIRId = static_cast<int>(m_View->guiMSAVINIR->value());
        params.push_back(static_cast<double>(m_View->guiMSAVIs->value()));
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::MSAVI2:
        {
        int RId = static_cast<int>(m_View->guiRAndNIRR->value());
        int NIRId = static_cast<int>(m_View->guiRAndNIRNIR->value());
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::TSAVI:
        {
        int RId = static_cast<int>(m_View->guiTSAVIR->value());
        int NIRId = static_cast<int>(m_View->guiTSAVINIR->value());
        params.push_back(static_cast<double>(m_View->guiTSAVIs->value()));
        params.push_back(static_cast<double>(m_View->guiTSAVIa->value()));
        params.push_back(static_cast<double>(m_View->guiTSAVIx->value()));
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::WDVI:
        {
        int RId = static_cast<int>(m_View->guiWDVIR->value());
        int NIRId = static_cast<int>(m_View->guiWDVINIR->value());
        params.push_back(static_cast<double>(m_View->guiWDVIs->value()));
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::ARVI:
        {
        int RId = static_cast<int>(m_View->guiARVIR->value());
        int BId = static_cast<int>(m_View->guiARVIB->value());
        int NIRId = static_cast<int>(m_View->guiARVINIR->value());
        params.push_back(static_cast<double>(m_View->guiARVIgamma->value()));
        m_Model->AddRAndBAndNIRFilter(RId, BId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::EVI:
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
      case FeatureInfo::TSARVI:
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
      case FeatureInfo::AVI:
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
      case FeatureInfo::IR:
      case FeatureInfo::IC:
      case FeatureInfo::IB:
        {
        int GId = static_cast<int>(m_View->guiSoilG->value());
        int RId = static_cast<int>(m_View->guiSoilR->value());
        m_Model->Add2ChannelsFilter(GId, RId, featureType);
        break;
        }
      case FeatureInfo::IB2:
        {
        int GId = static_cast<int>(m_View->guiIB2G->value());
        int RId = static_cast<int>(m_View->guiIB2R->value());
        int NIRId = static_cast<int>(m_View->guiIB2NIR->value());
        m_Model->AddRAndGAndNIRFilter(RId, GId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::NDBI:
        {
        int TM4Id = static_cast<int>(m_View->guiNDBITM4->value());
        int TM5Id = static_cast<int>(m_View->guiNDBITM5->value());
        m_Model->Add2ChannelsFilter(TM4Id, TM5Id, featureType);
        break;
        }
      case FeatureInfo::ISU:
        {
        int RId = static_cast<int>(m_View->guiISUR->value());
        int NIRId = static_cast<int>(m_View->guiISUNIR->value());
        params.push_back(static_cast<double>(m_View->guiISUa->value()));
        params.push_back(static_cast<double>(m_View->guiISUb->value()));
        m_Model->AddRAndNIRFilter(RId, NIRId, featureType, params);
        break;
        }
      case FeatureInfo::SRWI:
        {
        int R860Id = static_cast<int>(m_View->guiSRWIR860->value());
        int R1240Id = static_cast<int>(m_View->guiSRWIR1240->value());
        m_Model->Add2ChannelsFilter(R860Id, R1240Id, featureType);
        break;
        }
      case FeatureInfo::NDWI:
        {
        int NIRId = static_cast<int>(m_View->guiNDWINIR->value());
        int MIRId = static_cast<int>(m_View->guiNDWIMIR->value());
        m_Model->Add2ChannelsFilter(NIRId, MIRId, featureType);
        break;
        }
      case FeatureInfo::NDWI2:
        {
        int GId = static_cast<int>(m_View->guiNDWI2G->value());
        int NIRId = static_cast<int>(m_View->guiNDWI2NIR->value());
        m_Model->Add2ChannelsFilter(GId, NIRId, featureType);
        break;
        }
      case FeatureInfo::MNDWI:
        {
        int GId = static_cast<int>(m_View->guiMNDWIG->value());
        int MIRId = static_cast<int>(m_View->guiMNDWIMIR->value());
        m_Model->Add2ChannelsFilter(GId, MIRId, featureType);
        break;
        }
      case FeatureInfo::NDPI:
        {
        int MIRId = static_cast<int>(m_View->guiNDPIMIR->value());
        int GId = static_cast<int>(m_View->guiNDPIG->value());
        m_Model->Add2ChannelsFilter(MIRId, GId, featureType);
        break;
        }
      case FeatureInfo::NDTI:
        {
        int RId = static_cast<int>(m_View->guiNDTIR->value());
        int GId = static_cast<int>(m_View->guiNDTIG->value());
        m_Model->Add2ChannelsFilter(RId, GId, featureType);
        break;
        }
      case FeatureInfo::SFS_LEN:
      case FeatureInfo::SFS_WID:
      case FeatureInfo::SFS_PSI:
      case FeatureInfo::SFS_WME:
      case FeatureInfo::SFS_RAT:
      case FeatureInfo::SFS_SD:
        {
        double       dSpectralThreshold = static_cast<double>(m_View->guiSFSSpectralThreshold->value());
        unsigned int uiSpectralThresh = static_cast<unsigned int>(m_View->guiSFSSpatialThreshold->value());
        unsigned int uiNbDirections = static_cast<unsigned int>(m_View->guiSFSNbDirections->value());
        double       dAlpha = static_cast<double>(m_View->guiSFSAlpha->value());
        unsigned int uiRatMaxConsNb = static_cast<unsigned int>(m_View->guiSFSRatMaxConsNb->value());
        m_Model->AddSFSTexturesFilter(featureType,
                                      dSpectralThreshold,
                                      uiSpectralThresh,
                                      uiNbDirections,
                                      dAlpha,
                                      uiRatMaxConsNb);
        break;
        }
      case FeatureInfo::EDGE_CANNY:
        {
        params.push_back(static_cast<double>(m_View->guiEdgeCannyRadius->value()));
        params.push_back(static_cast<double>(m_View->guiEdgeCannyMaxThresh->value()));
        params.push_back(static_cast<double>(m_View->guiEdgeCannyMinThresh->value()));
        params.push_back(static_cast<double>(m_View->guiEdgeCannyVariance->value()));
        params.push_back(static_cast<double>(m_View->guiEdgeCannyMaxError->value()));
        m_Model->AddEdgeDensityFilter(featureType, params);
        break;
        }
      case FeatureInfo::EDGE_SOBEL:
        {
        params.push_back(static_cast<double>(m_View->guiEdgeSobelRadiusX->value()));
        params.push_back(static_cast<double>(m_View->guiEdgeSobelRadiusY->value()));
        params.push_back(static_cast<double>(m_View->guiEgdeSobelLowThresh->value()));
        params.push_back(static_cast<double>(m_View->guiEgdeSobelUpThresh->value()));
        m_Model->AddEdgeDensityFilter(featureType, params);
        break;
        }
      case FeatureInfo::MS_SMOOTH:
      case FeatureInfo::MS_CLUSTERED:
      case FeatureInfo::MS_LABELED:
      case FeatureInfo::MS_BOUND:
        {
        unsigned int spatial = static_cast<unsigned int>(m_View->guiMSSpatial->value());
        double       range = static_cast<double>(m_View->guiMSRange->value());
        unsigned int minSize = static_cast<unsigned int>(m_View->guiMSSize->value());
        double       scale = static_cast<double>(m_View->guiMSScale->value());
        m_Model->AddMeanShiftFilter(featureType, spatial, range, minSize, scale);
        break;
        }
      case FeatureInfo::ORIGINAL:
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
    for (int i = beforeSize; i < afterSize; i++)
      {
      m_View->guiFeatureList->add(m_Model->GetOutputFilterInformationId(i).c_str());
      m_View->guiFeatureListAction->add(m_Model->GetOutputFilterInformationId(i).c_str());
      m_View->guiOutputFeatureList->add(m_Model->GetOutputFilterInformationId(i).c_str());
      m_View->AddToInputOutputFeatureLink(i);
      this->ChangeFilterStatus(i);
      }

    m_View->guiFeatureList->redraw();
    m_View->guiFeatureListAction->redraw();
    m_View->guiOutputFeatureList->redraw();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

FeatureExtractionController::HaralickTextureVectorType FeatureExtractionController::GetHaralickTextList()
{
  HaralickTextureVectorType harList(m_View->guiHarList->nchecked(), HaralickTexture::UNKNOWN);
  int                       j = 1;
  int                       count = 0;

  while (j <= m_View->guiHarList->nitems() && count < m_View->guiHarList->nchecked())
    {
    if (m_View->guiHarList->checked(j) != 0)
      {
      harList[count] = HaralickTexture::FindTextureType(j - 1);
      count++;
      }
    j++;
    }

  return harList;
}

FeatureExtractionController::AdvancedTextureVectorType FeatureExtractionController::GetAdvancedTextList()
{
  AdvancedTextureVectorType advList(m_View->guiAdvList->nchecked(), AdvancedTexture::UNKNOWN);

  int j = 1;
  int count = 0;

  while (j <= m_View->guiAdvList->nitems() && count < m_View->guiAdvList->nchecked())
    {
    if (m_View->guiAdvList->checked(j) != 0)
      {
      advList[count] = AdvancedTexture::FindTextureType(j - 1);
      count++;
      }
    j++;
    }

  return advList;
}

void
FeatureExtractionController::UpdateFeaturePreview(unsigned int id)
{
  try
    {
    this->ShowBarStatus();
    m_Model->GetSingleOutput(id);
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
  this->HideBarStatus();
}

void FeatureExtractionController::Quit()
{
  try
    {
    m_Model->Quit();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void FeatureExtractionController::Cancel()
{
  try
    {
    m_Model->Cancel();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

} // end namespace otb
