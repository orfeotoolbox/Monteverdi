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
#ifndef __otbRadiometricIndicesGenerator_h
#define __otbRadiometricIndicesGenerator_h

#include "otbVegetationIndicesFunctor.h"
#include "otbSoilIndicesFunctor.h"
#include "otbBuiltUpIndicesFunctor.h"
#include "otbWaterIndicesFunctor.h"
#include "otbFeature.h"
#include "otbFeatureExtractionModel.h"
#include "otbMultiChannelRAndNIRIndexImageFilter.h"
#include "otbMultiChannelGAndRIndexImageFilter.h"
#include "otbMultiChannelRAndGAndNIRIndexImageFilter.h"
#include "otbMultiChannelRAndBAndNIRIndexImageFilter.h"
#include "otbMultiChannelRAndGAndNIRIndexImageFilter.h"
#include "itkUnaryFunctorImageFilter.h"

namespace otb
{
/** \class otbRadiometricIndicesGenerator
 *
 *
 *
 */
class RadiometricIndicesGenerator
{
public:
  typedef FeatureExtractionModel                                                   ModelType;
  typedef ModelType::Pointer                                                       ModelPointerType;
  typedef ModelType::FilterType                                                    FilterType;
  typedef ModelType::PixelType                                                     PixelType;
  typedef ModelType::InputImageType                                                InputImageType;
  typedef ModelType::SingleImageType                                               SingleImageType;
  typedef ModelType::SingleImagePointerType                                        SingleImagePointerType;
  typedef FeatureInfo::FeatureType                                                 FeatureType;


  /****************************************/
  /** Vegetation Functor type declaration */
  /****************************************/

  /** RED NIR */

  // NDVI
  typedef Functor::NDVI< PixelType, PixelType, PixelType >                                          NDVIFunctorType;
  // RVI
  typedef Functor::RVI< PixelType, PixelType, PixelType >                                           RVIFunctorType;
  // PVI -> a b
  typedef Functor::PVI< PixelType, PixelType, PixelType >                                           PVIFunctorType;
  // SAVI -> L
  typedef Functor::SAVI< PixelType, PixelType, PixelType >                                          SAVIFunctorType;
  // MSAVI -> s
  typedef Functor::MSAVI< PixelType, PixelType, PixelType >                                         MSAVIFunctorType;
  // MSAVI2
  typedef Functor::MSAVI2< PixelType, PixelType, PixelType >                                        MSAVI2FunctorType;
  // TSAVI -> s, a, X
  typedef Functor::TSAVI< PixelType, PixelType, PixelType >                                         TSAVIFunctorType;
  // GEMI
  typedef Functor::GEMI< PixelType, PixelType, PixelType >                                          GEMIFunctorType;
  // WDVI -> s
  typedef Functor::WDVI< PixelType, PixelType, PixelType >                                          WDVIFunctorType;
  // IPVI
  typedef Functor::IPVI< PixelType, PixelType, PixelType >                                          IPVIFunctorType;
  // TNDVI
  typedef Functor::TNDVI< PixelType, PixelType, PixelType >                                         TNDVIFunctorType;

  /** RED BLUE NIR */

  // ARVI -> gamma
  typedef Functor::ARVI< PixelType, PixelType, PixelType, PixelType >                               ARVIFunctorType;
  // EVI -> G, L, C1, C2
  typedef Functor::EVI< PixelType, PixelType, PixelType, PixelType >                                EVIFunctorType;
  // TASRVI -> a_rb, b_rb
  typedef Functor::TSARVI< PixelType, PixelType, PixelType, PixelType >                             TSARVIFunctorType;

  /** RED GREEN NIR */

  // AVI -> lambda1, lambda2, lambda3
  typedef Functor::AVI< PixelType, PixelType, PixelType, PixelType >                                AVIFunctorType;

  /**********************************/
  /** Soil Functor type declaration */
  /**********************************/

  // IR
  typedef Functor::IR< PixelType, PixelType, PixelType >                                            IRFunctorType;
  // IC
  typedef Functor::IC< PixelType, PixelType, PixelType >                                            ICFunctorType;
  // IB
  typedef Functor::IB< PixelType, PixelType, PixelType >                                            IBFunctorType;
  // IB2
  typedef Functor::IB2< PixelType, PixelType, PixelType, PixelType >                                IB2FunctorType;

  /**************************************/
  /** Built Up Functor type declaration */
  /**************************************/

  // NDBI
  typedef Functor::NDBI< PixelType, PixelType, PixelType>                                           NDBIFunctorType;
  // ISU
  typedef Functor::ISU< PixelType, PixelType, PixelType>                                            ISUFunctorType;

  /***********************************/
  /** Water Functor type declaration */
  /***********************************/

  // SRWI
  typedef Functor::SRWI< PixelType, PixelType, PixelType>                                           SRWIFunctorType;
  // NDWI
  typedef Functor::NDWI< PixelType, PixelType, PixelType>                                           NDWIFunctorType;
  // NDWI2
  typedef Functor::NDWI2< PixelType, PixelType, PixelType>                                          NDWI2FunctorType;
  // MNDWI
  typedef Functor::MNDWI< PixelType, PixelType, PixelType>                                          MNDWIFunctorType;
  // NDPI
  typedef Functor::NDPI< PixelType, PixelType, PixelType>                                           NDPIFunctorType;
  // NDTI
  typedef Functor::NDTI< PixelType, PixelType, PixelType>                                           NDTIFunctorType;

  /***************************/
  /** Filter type declaration*/
  /***************************/

  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,NDVIFunctorType>       NDVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,RVIFunctorType>        RVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,PVIFunctorType>        PVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,SAVIFunctorType>       SAVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,MSAVIFunctorType>      MSAVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,MSAVI2FunctorType>     MSAVI2FilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,TSAVIFunctorType>      TSAVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,GEMIFunctorType>       GEMIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,WDVIFunctorType>       WDVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,IPVIFunctorType>       IPVIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,TNDVIFunctorType>      TNDVIFilterType;
  typedef MultiChannelRAndBAndNIRIndexImageFilter<InputImageType,SingleImageType,ARVIFunctorType>   ARVIFilterType;
  typedef MultiChannelRAndBAndNIRIndexImageFilter<InputImageType,SingleImageType,EVIFunctorType>    EVIFilterType;
  typedef MultiChannelRAndBAndNIRIndexImageFilter<InputImageType,SingleImageType,TSARVIFunctorType> TSARVIFilterType;
  typedef MultiChannelRAndGAndNIRIndexImageFilter<InputImageType,SingleImageType,AVIFunctorType>    AVIFilterType;

  typedef MultiChannelGAndRIndexImageFilter<InputImageType,SingleImageType,IRFunctorType>           IRFilterType;
  typedef MultiChannelGAndRIndexImageFilter<InputImageType,SingleImageType,ICFunctorType>           ICFilterType;
  typedef MultiChannelGAndRIndexImageFilter<InputImageType,SingleImageType,IBFunctorType>           IBFilterType;
  typedef MultiChannelRAndGAndNIRIndexImageFilter<InputImageType,SingleImageType,IB2FunctorType>    IB2FilterType;

  typedef itk::UnaryFunctorImageFilter<InputImageType, SingleImageType,NDBIFunctorType>             NDBIFilterType;
  typedef MultiChannelRAndNIRIndexImageFilter<InputImageType,SingleImageType,ISUFunctorType>        ISUFilterType;

  typedef itk::UnaryFunctorImageFilter<InputImageType, SingleImageType,SRWIFunctorType>             SRWIFilterType;
  typedef itk::UnaryFunctorImageFilter<InputImageType, SingleImageType,NDWIFunctorType>             NDWIFilterType;
  typedef itk::UnaryFunctorImageFilter<InputImageType, SingleImageType,NDWI2FunctorType>            NDWI2FilterType;
  typedef itk::UnaryFunctorImageFilter<InputImageType, SingleImageType,MNDWIFunctorType>            MNDWIFilterType;
  typedef itk::UnaryFunctorImageFilter<InputImageType, SingleImageType,NDPIFunctorType>             NDPIFilterType;
  typedef itk::UnaryFunctorImageFilter<InputImageType, SingleImageType,NDTIFunctorType>             NDTIFilterType;


  /************/
  /** Methods */
  /************/

  template <class TFilterTypeMethod> typename TFilterTypeMethod::Pointer GenericAddRAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int nirId, std::string msg);
  template <class TFilterTypeMethod> typename TFilterTypeMethod::Pointer GenericAddRAndBAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int blueId, unsigned int nirId, std::string msg);
  template <class TFilterTypeMethod> typename TFilterTypeMethod::Pointer GenericAdd2ChannelsFilter( ModelPointerType model, FeatureType type, unsigned int chan1Id, unsigned int chan2Id, std::string msg);

  // AddRAndNIRFilter
  void AddRAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int nirId , std::vector<double> params)
  {
    std::string mess;
    switch(type)
    {
      case FeatureInfo::NDVI:
      {
        mess = "Full Image : NDVI : ";
        this->GenericAddRAndNIRFilter<NDVIFilterType>(model, type, redId, nirId, mess);
        break;
      }
      case FeatureInfo::RVI:
      {
        mess = "Full Image : RVI : ";
        this->GenericAddRAndNIRFilter<RVIFilterType>(model, type, redId, nirId, mess);
        break;
      }
      case FeatureInfo::PVI:
      {
        itk::OStringStream oss;
        oss<<"Full Image : PVI : "<<params[0]<<","<<params[1]<<" ";
        std::string mess = oss.str();
        PVIFilterType::Pointer filter;
        filter = this->GenericAddRAndNIRFilter<PVIFilterType>(model, type, redId, nirId, mess);
        filter->GetFunctor().SetA(params[0]);
        filter->GetFunctor().SetB(params[1]);
        break;
      }
      case FeatureInfo::SAVI:
      {
        itk::OStringStream oss;
        oss<<"Full Image : SAVI : "<<params[0]<<" ";
        std::string mess = oss.str();
        SAVIFilterType::Pointer filter;
        filter = this->GenericAddRAndNIRFilter<SAVIFilterType>(model, type, redId, nirId, mess);
        filter->GetFunctor().SetL(params[0]);
        break;
      }
      case FeatureInfo::MSAVI:
      {
        itk::OStringStream oss;
        oss<<"Full Image : MSAVI : "<<params[0]<<" ";
        std::string mess = oss.str();
        MSAVIFilterType::Pointer filter;
        filter = this->GenericAddRAndNIRFilter<MSAVIFilterType>(model, type, redId, nirId, mess);
        filter->GetFunctor().SetS(params[0]);
        break;
      }
      case FeatureInfo::MSAVI2:
      {
        mess = "Full Image : MSAVI2 : ";
        this->GenericAddRAndNIRFilter<MSAVI2FilterType>(model, type, redId, nirId, mess);
        break;
      }
      case FeatureInfo::TSAVI:
      {
        itk::OStringStream oss;
        oss<<"Full Image : TSAVI : "<<params[0]<<","<<params[1]<<","<<params[2]<<" ";
        std::string mess = oss.str();
        TSAVIFilterType::Pointer filter;
        filter = this->GenericAddRAndNIRFilter<TSAVIFilterType>(model, type, redId, nirId, mess);
        filter->GetFunctor().SetS(params[0]);
        filter->GetFunctor().SetA(params[1]);
        filter->GetFunctor().SetX(params[2]);
        break;
      }
      case FeatureInfo::GEMI:
      {
        mess = "Full Image : GEMI : ";
        this->GenericAddRAndNIRFilter<GEMIFilterType>(model, type, redId, nirId, mess);
        break;
      }
      case FeatureInfo::WDVI:
      {
        itk::OStringStream oss;
        oss<<"Full Image : WDVI : "<<params[0]<<" ";
        std::string mess = oss.str();
        WDVIFilterType::Pointer filter;
        filter = this->GenericAddRAndNIRFilter<WDVIFilterType>(model, type, redId, nirId, mess);
        filter->GetFunctor().SetS(params[0]);
        break;
      }
      case FeatureInfo::IPVI:
      {
        mess = "Full Image : IPVI : ";
        this->GenericAddRAndNIRFilter<IPVIFilterType>(model, type, redId, nirId, mess);
        break;
      }
      case FeatureInfo::TNDVI:
      {
        mess = "Full Image : TNDVI : ";
        this->GenericAddRAndNIRFilter<TNDVIFilterType>(model, type, redId, nirId, mess);
        break;
      }
      case FeatureInfo::ISU:
      {
        itk::OStringStream oss;
        oss<<"Full Image : ISU : "<<params[0]<<","<<params[1]<<" ";
        std::string mess = oss.str();
        ISUFilterType::Pointer filter;
        filter = this->GenericAddRAndNIRFilter<ISUFilterType>(model, type, redId, nirId, mess);
        filter->GetFunctor().SetA(params[0]);
        filter->GetFunctor().SetB(params[1]);
        break;
      }
      default:
      {
      }
    }
  }

  // AddRAndBAndNIRFilter
  void AddRAndBAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int blueId, unsigned int nirId, std::vector<double> params )
  {
    std::string mess;
    switch(type)
    {
      case FeatureInfo::ARVI:
      {
        mess = "Full Image : ARVI : ";
        ARVIFilterType::Pointer filter;
        filter = this->GenericAddRAndBAndNIRFilter<ARVIFilterType>(model, type, redId, blueId, nirId, mess);
        filter->GetFunctor().SetGamma(params[0]);
        break;
      }
      case FeatureInfo::EVI:
      {
        itk::OStringStream oss;
        oss<<"Full Image : EVI : "<<params[0]<<","<<params[1]<<","<<params[2]<<","<<params[3]<<" ";
        std::string mess = oss.str();
        EVIFilterType::Pointer filter;
        filter = this->GenericAddRAndBAndNIRFilter<EVIFilterType>(model, type, redId, blueId, nirId, mess);
        filter->GetFunctor().SetG(params[0]);
        filter->GetFunctor().SetL(params[1]);
        filter->GetFunctor().SetC1(params[2]);
        filter->GetFunctor().SetC2(params[3]);
        break;
      }
      case FeatureInfo::TSARVI:
      {
        itk::OStringStream oss;
        oss<<"Full Image : TSARVI : "<<params[0]<<","<<params[1]<<","<<params[2]<<" ";
        std::string mess = oss.str();
        TSARVIFilterType::Pointer filter;
        filter = this->GenericAddRAndBAndNIRFilter<TSARVIFilterType>(model, type, redId, blueId, nirId, mess);
        filter->GetFunctor().SetA(params[0]);
        filter->GetFunctor().SetB(params[1]);
        filter->GetFunctor().SetX(params[2]);
        break;
      }
      default:
      {
      }
    }
  }

  // AddRAndGAndNIRFilter
  void AddRAndGAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int greenId, unsigned int nirId, std::vector<double> params )
  {
    switch(type)
    {
      case FeatureInfo::AVI:
      {
        AVIFilterType::Pointer avi = AVIFilterType::New();
        avi->SetInput(model->GetInputImage());
        avi->SetRedIndex(redId);
        avi->SetGreenIndex(greenId);
        avi->SetNIRIndex(nirId);

        avi->GetFunctor().SetLambdaG(params[0]);
        avi->GetFunctor().SetLambdaR(params[1]);
        avi->GetFunctor().SetLambdaNir(params[2]);

        itk::OStringStream oss;
        oss<<"Full Image : AVI : "<<params[0]<<","<<params[1]<<","<<params[2]<<" "<<greenId<<","<<redId<<","<<nirId;
        std::string mess = oss.str();
        model->AddFeatureFilter( avi, FeatureInfo::AVI, itk::NumericTraits<unsigned int>::max(), 0, mess);
        break;
      }
      case FeatureInfo::IB2:
      {
        IB2FilterType::Pointer ib2 = IB2FilterType::New();
        ib2->SetInput(model->GetInputImage());
        ib2->SetRedIndex(redId);
        ib2->SetGreenIndex(greenId);
        ib2->SetNIRIndex(nirId);

        itk::OStringStream oss;
        oss<<"Full Image : IB2 : "<<redId<<", "<<greenId<<", "<<nirId;
        std::string mess = oss.str();
        model->AddFeatureFilter( ib2, FeatureInfo::IB2, itk::NumericTraits<unsigned int>::max(), 0, mess);
        break;
      }
      default:
      {
      }
    }
  }


 // Add2ChannelsFilter
  void Add2ChannelsFilter( ModelPointerType model, FeatureType type, unsigned int chan1Id, unsigned int chan2Id )
  {
    std::string mess;
    switch(type)
    {
      case FeatureInfo::IR:
      {
        IRFilterType::Pointer ir = IRFilterType::New();
        ir->SetInput(model->GetInputImage());
        ir->SetGreenIndex(chan1Id);
        ir->SetRedIndex(chan2Id);

        itk::OStringStream oss;
        oss<<"Full Image : IR : "<<chan1Id<<", "<<chan2Id;
        std::string mess = oss.str();
        model->AddFeatureFilter( ir, FeatureInfo::IR, itk::NumericTraits<unsigned int>::max(), 0, mess);
        break;
      }
      case FeatureInfo::IC:
      {
        ICFilterType::Pointer ic = ICFilterType::New();
        ic->SetInput(model->GetInputImage());
        ic->SetGreenIndex(chan1Id);
        ic->SetRedIndex(chan2Id);

        itk::OStringStream oss;
        oss<<"Full Image : IC : "<<chan1Id<<", "<<chan2Id;
        std::string mess = oss.str();
        model->AddFeatureFilter( ic, FeatureInfo::IC, itk::NumericTraits<unsigned int>::max(), 0, mess);
        break;
      }
      case FeatureInfo::IB:
      {
        IBFilterType::Pointer ib = IBFilterType::New();
        ib->SetInput(model->GetInputImage());
        ib->SetGreenIndex(chan1Id);
        ib->SetRedIndex(chan2Id);

        itk::OStringStream oss;
        oss<<"Full Image : IB : "<<chan1Id<<", "<<chan2Id;
        std::string mess = oss.str();
        model->AddFeatureFilter( ib, FeatureInfo::IB, itk::NumericTraits<unsigned int>::max(), 0, mess);
        break;
      }
      case FeatureInfo::NDBI:
      {
        mess = "Full Image : NDBI : ";
        this->GenericAdd2ChannelsFilter<NDBIFilterType>(model, type, chan1Id, chan2Id, mess);
        break;
      }
      case FeatureInfo::SRWI:
      {
        mess = "Full Image : SRWI : ";
        this->GenericAdd2ChannelsFilter<SRWIFilterType>(model, type, chan1Id, chan2Id, mess);
        break;
      }
      case FeatureInfo::NDWI:
      {
        mess = "Full Image : NDWI : ";
        this->GenericAdd2ChannelsFilter<NDWIFilterType>(model, type, chan1Id, chan2Id, mess);
        break;
      }
      case FeatureInfo::NDWI2:
      {
        mess = "Full Image : NDWI2 : ";
        this->GenericAdd2ChannelsFilter<NDWI2FilterType>(model, type, chan1Id, chan2Id, mess);
        break;
      }
      case FeatureInfo::MNDWI:
      {
        mess = "Full Image : MNDWI : ";
        this->GenericAdd2ChannelsFilter<MNDWIFilterType>(model, type, chan1Id, chan2Id, mess);
        break;
      }
      case FeatureInfo::NDPI:
      {
        mess = "Full Image : NDPI : ";
        this->GenericAdd2ChannelsFilter<NDPIFilterType>(model, type, chan1Id, chan2Id, mess);
        break;
      }
      case FeatureInfo::NDTI:
      {
        mess = "Full Image : NDTI : ";
        this->GenericAdd2ChannelsFilter<NDTIFilterType>(model, type, chan1Id, chan2Id, mess);
        break;
      }
      default:
      {
      }
    }
  }

  // GenerateRadiometricIndicesOutputImage
  SingleImagePointerType GenerateRadiometricIndicesOutputImage( ModelPointerType model,  FeatureType type, unsigned int inputListId  )
  {
    SingleImagePointerType image =  SingleImageType::New();
    switch (type)
    {
      case FeatureInfo::NDVI:
      {
        NDVIFilterType::Pointer ndvi = dynamic_cast<NDVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ndvi->GetOutput();
        break;
      }
      case FeatureInfo::RVI:
      {
        RVIFilterType::Pointer rvi = dynamic_cast<RVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = rvi->GetOutput();
        break;
      }
      case FeatureInfo::ARVI:
      {
        ARVIFilterType::Pointer arvi = dynamic_cast<ARVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = arvi->GetOutput();
        break;
      }
      case FeatureInfo::EVI:
      {
        EVIFilterType::Pointer evi = dynamic_cast<EVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = evi->GetOutput();
        break;
      }
      case FeatureInfo::PVI:
      {
        PVIFilterType::Pointer pvi = dynamic_cast<PVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = pvi->GetOutput();
        break;
      }
      case FeatureInfo::SAVI:
      {
        SAVIFilterType::Pointer savi = dynamic_cast<SAVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = savi->GetOutput();
        break;
      }
      case FeatureInfo::MSAVI:
      {
        MSAVIFilterType::Pointer msavi = dynamic_cast<MSAVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = msavi->GetOutput();
        break;
      }
      case FeatureInfo::MSAVI2:
      {
        MSAVI2FilterType::Pointer msavi2 = dynamic_cast<MSAVI2FilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = msavi2->GetOutput();
        break;
      }
      case FeatureInfo::TSAVI:
      {
        TSAVIFilterType::Pointer tsavi = dynamic_cast<TSAVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = tsavi->GetOutput();
        break;
      }
      case FeatureInfo::TSARVI:
      {
        TSARVIFilterType::Pointer tsarvi = dynamic_cast<TSARVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = tsarvi->GetOutput();
        break;
      }
      case FeatureInfo::GEMI:
      {
        GEMIFilterType::Pointer gemi = dynamic_cast<GEMIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = gemi->GetOutput();
        break;
      }
      case FeatureInfo::AVI:
      {
        AVIFilterType::Pointer avi = dynamic_cast<AVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = avi->GetOutput();
        break;
      }
      case FeatureInfo::WDVI:
      {
        WDVIFilterType::Pointer wdvi = dynamic_cast<WDVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = wdvi->GetOutput();
        break;
      }
      case FeatureInfo::IPVI:
      {
        IPVIFilterType::Pointer ipvi = dynamic_cast<IPVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ipvi->GetOutput();
        break;
      }
      case FeatureInfo::TNDVI:
      {
        TNDVIFilterType::Pointer tndvi = dynamic_cast<TNDVIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = tndvi->GetOutput();
        break;
      }
      case FeatureInfo::IR:
      {
        IRFilterType::Pointer ir = dynamic_cast<IRFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ir->GetOutput();
        break;
      }
      case FeatureInfo::IC:
      {
        ICFilterType::Pointer ic = dynamic_cast<ICFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ic->GetOutput();
        break;
      }
      case FeatureInfo::IB:
      {
        IBFilterType::Pointer ib = dynamic_cast<IBFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ib->GetOutput();
        break;
      }
      case FeatureInfo::IB2:
      {
        IB2FilterType::Pointer ib2 = dynamic_cast<IB2FilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ib2->GetOutput();
        break;
      }
      case FeatureInfo::NDBI:
      {
        NDBIFilterType::Pointer ndbi = dynamic_cast<NDBIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ndbi->GetOutput();
        break;
      }
      case FeatureInfo::ISU:
      {
        ISUFilterType::Pointer isu = dynamic_cast<ISUFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = isu->GetOutput();
        break;
      }
      case FeatureInfo::SRWI:
      {
        SRWIFilterType::Pointer filter = dynamic_cast<SRWIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = filter->GetOutput();
        break;
      }
      case FeatureInfo::NDWI:
      {
        NDWIFilterType::Pointer filter = dynamic_cast<NDWIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = filter->GetOutput();
        break;
      }
      case FeatureInfo::NDWI2:
      {
        NDWI2FilterType::Pointer filter = dynamic_cast<NDWI2FilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = filter->GetOutput();
        break;
      }
      case FeatureInfo::MNDWI:
      {
        MNDWIFilterType::Pointer filter = dynamic_cast<MNDWIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = filter->GetOutput();
        break;
      }
      case FeatureInfo::NDPI:
      {
        NDPIFilterType::Pointer filter = dynamic_cast<NDPIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = filter->GetOutput();
        break;
      }
      case FeatureInfo::NDTI:
      {
        NDTIFilterType::Pointer filter = dynamic_cast<NDTIFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = filter->GetOutput();
        break;
      }
      default:
      {
      }
    }
    return image;
  }


protected:

private:

};

template <class TFilterTypeMethod>
typename TFilterTypeMethod::Pointer
RadiometricIndicesGenerator::GenericAddRAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int nirId, std::string msg )
{
  itk::OStringStream oss;
  oss<<msg;
  oss<<redId<<" , "<<nirId;
  typename TFilterTypeMethod::Pointer filter = TFilterTypeMethod::New();
  filter->SetInput(model->GetInputImage());
  filter->SetRedIndex(redId);
  filter->SetNIRIndex(nirId);
  std::string mess = oss.str();
  model->AddFeatureFilter( filter, type, itk::NumericTraits<unsigned int>::max(), 0, mess);
  return filter;
}

template <class TFilterTypeMethod>
typename TFilterTypeMethod::Pointer
RadiometricIndicesGenerator::GenericAddRAndBAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int blueId, unsigned int nirId, std::string msg )
{
  itk::OStringStream oss;
  oss<<msg;
  oss<<redId<<" , "<<blueId<<" , "<<nirId;
  typename TFilterTypeMethod::Pointer filter = TFilterTypeMethod::New();
  filter->SetInput(model->GetInputImage());
  filter->SetRedIndex(redId);
  filter->SetBlueIndex(blueId);
  filter->SetNIRIndex(nirId);
  std::string mess = oss.str();
  model->AddFeatureFilter( filter, type, itk::NumericTraits<unsigned int>::max(), 0, mess);
  return filter;
}

template <class TFilterTypeMethod>
typename TFilterTypeMethod::Pointer
RadiometricIndicesGenerator::GenericAdd2ChannelsFilter( ModelPointerType model, FeatureType type, unsigned int chan1Id, unsigned int chan2Id, std::string msg )
{
  itk::OStringStream oss;
  oss<<msg;
  oss<<chan1Id<<" , "<<chan2Id;
  typename TFilterTypeMethod::Pointer filter = TFilterTypeMethod::New();
  filter->SetInput(model->GetInputImage());
  filter->GetFunctor().SetIndex1(chan1Id);
  filter->GetFunctor().SetIndex2(chan2Id);
  std::string mess = oss.str();
  model->AddFeatureFilter( filter, type, itk::NumericTraits<unsigned int>::max(), 0, mess);
  return filter;
}

}
#endif

