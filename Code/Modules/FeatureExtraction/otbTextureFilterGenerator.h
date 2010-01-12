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
#ifndef __otbTextureFilterGenerator_h
#define __otbTextureFilterGenerator_h

#include "otbTextureFunctors.h"
#include "otbTextureImageFunction.h"
#include "otbFunctionWithNeighborhoodToImageFilter.h"
#include "otbFeature.h"
#include "otbFeatureExtractionModel.h"
#include "otbPanTexTextureImageFunctionFilter.h"


namespace otb
{
class TextureFilterGenerator
{
 public:
  typedef FeatureExtractionModel            ModelType;
  typedef ModelType::Pointer                ModelPointerType;
  typedef ModelType::FilterType             FilterType;
  typedef ModelType::SizeType               SizeType;
  typedef ModelType::OffsetType             OffsetType;
  typedef ModelType::IteratorType           IteratorType;
  typedef ModelType::InputImagePixelType    InputImagePixelType;
  typedef ModelType::SingleImageType        SingleImageType;
  typedef ModelType::SinglePixelType        SinglePixelType;
  typedef ModelType::SingleImagePointerType SingleImagePointerType;
  typedef FeatureInfo::FeatureType          FeatureType;

  // Angular second momentum
  typedef Functor::AngularSecondMomentumTextureFunctor<SinglePixelType, SinglePixelType>            ASMTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, ASMTextureFunctorType>                              ASMFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, ASMFunctionType>  ASMFilterType;
  // Cluster shade
  typedef Functor::ClusterShadeTextureFunctor<SinglePixelType, SinglePixelType>                     CSHTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, CSHTextureFunctorType>                              CSHFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, CSHFunctionType>  CSHFilterType;
  // Cluster shade
  typedef Functor::ClusterProminenceTextureFunctor<SinglePixelType, SinglePixelType>                CPRTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, CPRTextureFunctorType>                              CPRFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, CPRFunctionType>  CPRFilterType;
  // Contrast
  typedef Functor::ContrastTextureFunctor<SinglePixelType, SinglePixelType>                         CONTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, CONTextureFunctorType>                              CONFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, CONFunctionType>  CONFilterType;
  // Correlation
  typedef Functor::CorrelationTextureFunctor<SinglePixelType, SinglePixelType>                      CORTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, CORTextureFunctorType>                              CORFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, CORFunctionType>  CORFilterType;
  // Difference Entropy
  typedef Functor::DifferenceEntropyTextureFunctor<SinglePixelType, SinglePixelType>                DENTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, DENTextureFunctorType>                              DENFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, DENFunctionType>  DENFilterType;
 // Difference Variance
  typedef Functor::DifferenceVarianceTextureFunctor<SinglePixelType, SinglePixelType>               DVATextureFunctorType;
  typedef TextureImageFunction<SingleImageType, DVATextureFunctorType>                              DVAFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, DVAFunctionType>  DVAFilterType;
  // Energy texture
  typedef Functor::EnergyTextureFunctor<SinglePixelType, SinglePixelType>                           ENJTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, ENJTextureFunctorType>                              ENJFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, ENJFunctionType>  ENJFilterType;
  // Entropy texture
  typedef Functor::EntropyTextureFunctor<SinglePixelType, SinglePixelType>                          ENTTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, ENTTextureFunctorType>                              ENTFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, ENTFunctionType>  ENTFilterType;
  // InformationMeasureOfCorrelation1TextureFunctor
  typedef Functor::InformationMeasureOfCorrelation1TextureFunctor<SinglePixelType, SinglePixelType> IC1TextureFunctorType;
  typedef TextureImageFunction<SingleImageType, IC1TextureFunctorType>                              IC1FunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, IC1FunctionType>  IC1FilterType;
  // InformationMeasureOfCorrelation2TextureFunctor
  typedef Functor::InformationMeasureOfCorrelation2TextureFunctor<SinglePixelType, SinglePixelType> IC2TextureFunctorType;
  typedef TextureImageFunction<SingleImageType, IC2TextureFunctorType>                              IC2FunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, IC2FunctionType>  IC2FilterType;
  // Inverse Difference Moment
  typedef Functor::InverseDifferenceMomentTextureFunctor<SinglePixelType, SinglePixelType>          IDMTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, IDMTextureFunctorType>                              IDMFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, IDMFunctionType>  IDMFilterType;
  // Mean
  typedef Functor::MeanTextureFunctor<SinglePixelType, SinglePixelType>                             MEATextureFunctorType;
  typedef TextureImageFunction<SingleImageType, MEATextureFunctorType>                              MEAFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, MEAFunctionType>  MEAFilterType;
  // Pan Tax
  typedef PanTexTextureImageFunctionFilter<SingleImageType, SingleImageType>                        PanTexFilterType;
  // Sum Average
  typedef Functor::SumAverageTextureFunctor<SinglePixelType, SinglePixelType>                       SAVTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, SAVTextureFunctorType>                              SAVFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, SAVFunctionType>  SAVFilterType;
  // Sum Entropy
  typedef Functor::SumEntropyTextureFunctor<SinglePixelType, SinglePixelType>                       SENTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, SENTextureFunctorType>                              SENFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, SENFunctionType>  SENFilterType;
  // Sum Variance
  typedef Functor::SumVarianceTextureFunctor<SinglePixelType, SinglePixelType>                      SVATextureFunctorType;
  typedef TextureImageFunction<SingleImageType, SVATextureFunctorType>                              SVAFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, SVAFunctionType>  SVAFilterType;
  // Variance
  typedef Functor::VarianceTextureFunctor<SinglePixelType, SinglePixelType>                         VARTextureFunctorType;
  typedef TextureImageFunction<SingleImageType, VARTextureFunctorType>                              VARFunctionType;
  typedef FunctionWithNeighborhoodToImageFilter<SingleImageType, SingleImageType, VARFunctionType>  VARFilterType;


template <class TFilterTypeMethod> void GenericAddTextureFilter( ModelPointerType model, FeatureType type, unsigned int inputListId, SizeType radius, OffsetType offset, std::string msg);

void GenerateTextureFilter( ModelPointerType model, FeatureType type, unsigned int inputListId, SizeType radius, OffsetType offset )
{
  //itk::OStringStream oss;
  std::string mess;
  switch (type)
    {
    case FeatureInfo::TEXT_ASM:
      {
        mess = "Angular 2nd momentum: ";
        this->GenericAddTextureFilter<ASMFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_CSH:
      {
        mess = "Cluster shade: ";
        this->GenericAddTextureFilter<CSHFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_CPR:
      {
        mess = "Cluster prominence: ";
        this->GenericAddTextureFilter<CPRFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_CON:
      {
        mess = "Contrast: ";
        this->GenericAddTextureFilter<CONFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_COR:
      {
        mess = "Correlation: ";
        this->GenericAddTextureFilter<CORFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_DEN:
      {
        mess = "Difference entropy: ";
        this->GenericAddTextureFilter<DENFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_DVA:
      {
        mess = "Difference variance: ";
        this->GenericAddTextureFilter<DVAFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_ENJ:
      {
        mess = "Energy: ";
        this->GenericAddTextureFilter<ENJFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_ENT:
      {
        mess = "Entropy: ";
        this->GenericAddTextureFilter<ENTFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_IC1:
      {
        mess = "Information measure of correlation 1: ";
        this->GenericAddTextureFilter<IC1FilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_IC2:
      {
        mess = "Information measure of correlation 2: ";
        this->GenericAddTextureFilter<IC2FilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_IDM:
      {
        mess = "Inverse difference moment: ";
        this->GenericAddTextureFilter<IDMFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_MEA:
      {
        mess = "Mean: ";
        this->GenericAddTextureFilter<MEAFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_PANTEX:
      {
        // Don't call generic method because don't take offset as input
        itk::OStringStream oss;
        oss<<"PanTex: "<<radius;
        PanTexFilterType::Pointer panTexFilter = PanTexFilterType::New();
        panTexFilter->SetInput(model->GetInputImageList()->GetNthElement(inputListId));
        panTexFilter->SetRadius(radius);
        std::string mess = oss.str();
        model->AddFeatureFilter( panTexFilter, type, inputListId, 0, mess);
        break;
      }
    case FeatureInfo::TEXT_SAV:
      {
        mess = "Sum average: ";
        this->GenericAddTextureFilter<SAVFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_SEN:
      {
        mess = "Sum entropy: ";
        this->GenericAddTextureFilter<SENFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_SVA:
      {
        mess = "Sum variance: ";
        this->GenericAddTextureFilter<SVAFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }
    case FeatureInfo::TEXT_VAR:
      {
        mess = "Variance: ";
        this->GenericAddTextureFilter<VARFilterType>( model, type, inputListId, radius, offset, mess);
        break;
      }

    default:
      {
        return;
      }
    }// if switch (type)
}

SingleImagePointerType GenerateTextureOutputImage( ModelPointerType model,  FeatureType type, unsigned int inputListId  )
{
  SingleImagePointerType image =  SingleImageType::New();
  switch (type)
    {
    case FeatureInfo::TEXT_ASM:
      {
        ASMFilterType::Pointer asmFilter = dynamic_cast<ASMFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = asmFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_CSH:
      {
        CSHFilterType::Pointer cshFilter = dynamic_cast<CSHFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = cshFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_CPR:
      {
        CPRFilterType::Pointer cprFilter = dynamic_cast<CPRFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = cprFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_CON:
      {
        CONFilterType::Pointer conFilter = dynamic_cast<CONFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = conFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_COR:
      {
        CORFilterType::Pointer corFilter = dynamic_cast<CORFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = corFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_DEN:
      {
        DENFilterType::Pointer denFilter = dynamic_cast<DENFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = denFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_DVA:
      {
        DVAFilterType::Pointer dvaFilter = dynamic_cast<DVAFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = dvaFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_ENJ:
      {

              ENJFilterType::Pointer enjFilter = dynamic_cast<ENJFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = enjFilter->GetOutput();break;
      }
    case FeatureInfo::TEXT_ENT:
      {
        ENTFilterType::Pointer entFilter = dynamic_cast<ENTFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = entFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_IC1:
      {
        IC1FilterType::Pointer ic2Filter = dynamic_cast<IC1FilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ic2Filter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_IC2:
      {
        IC2FilterType::Pointer ic1Filter = dynamic_cast<IC2FilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = ic1Filter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_IDM:
      {
        IDMFilterType::Pointer idmFilter = dynamic_cast<IDMFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = idmFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_MEA:
      {
        MEAFilterType::Pointer meaFilter = dynamic_cast<MEAFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = meaFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_PANTEX:
      {
        PanTexFilterType::Pointer panTexFilter = dynamic_cast<PanTexFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = panTexFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_SAV:
      {
        SAVFilterType::Pointer savFilter = dynamic_cast<SAVFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = savFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_SEN:
      {
        SENFilterType::Pointer senFilter = dynamic_cast<SENFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = senFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_SVA:
      {
        SVAFilterType::Pointer svaFilter = dynamic_cast<SVAFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = svaFilter->GetOutput();
        break;
      }
    case FeatureInfo::TEXT_VAR:
      {
        VARFilterType::Pointer varFilter = dynamic_cast<VARFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = varFilter->GetOutput();
        break;
      }
    default:
      {
      }
    }// if switch (type)
  return image;
}

/*
void GenericTextureConnectFilter( ModelPointerType model,  FeatureType type, int filterIndex )
{
switch (type)
    {
    case TEXT_ASM:
      {
        model->GenericConnectFilter<ASMFilterType>(filterIndex);
        break;
      }
    case TEXT_CSH:
      {
        model->GenericConnectFilter<CSHFilterType>(filterIndex);
        break;
      }
    case TEXT_CPR:
      {
        model->GenericConnectFilter<CPRFilterType>(filterIndex);
        break;
      }
    case TEXT_CON:
      {
        model->GenericConnectFilter<CONFilterType>(filterIndex);
        break;
      }
    case TEXT_COR:
      {
        model->GenericConnectFilter<CORFilterType>(filterIndex);
        break;
      }
    case TEXT_DEN:
      {
        model->GenericConnectFilter<DENFilterType>(filterIndex);
        break;
      }
    case TEXT_DVA:
      {
        model->GenericConnectFilter<DVAFilterType>(filterIndex);
        break;
      }
    case TEXT_ENJ:
      {
        model->GenericConnectFilter<ENJFilterType>(filterIndex);
        break;
      }
    case TEXT_ENT:
      {
        model->GenericConnectFilter<ENTFilterType>(filterIndex);
        break;
      }
    case TEXT_IC1:
      {
        model->GenericConnectFilter<IC1FilterType>(filterIndex);
        break;
      }
    case TEXT_IC2:
      {
        model->GenericConnectFilter<IC2FilterType>(filterIndex);
        break;
      }
    case TEXT_IDM:
      {
        model->GenericConnectFilter<IDMFilterType>(filterIndex);
        break;
      }
    case TEXT_MEA:
      {
        model->GenericConnectFilter<MEAFilterType>(filterIndex);
        break;
      }
    case TEXT_PANTEX:
      {
        model->GenericConnectFilter<PanTexFilterType>(filterIndex);
        break;
      }
    case TEXT_SAV:
      {
        model->GenericConnectFilter<SAVFilterType>(filterIndex);
        break;
      }
    case TEXT_SEN:
      {
        model->GenericConnectFilter<SENFilterType>(filterIndex);
        break;
      }
    case TEXT_SVA:
      {
        model->GenericConnectFilter<SVAFilterType>(filterIndex);
        break;
      }
    case TEXT_VAR:
      {
        model->GenericConnectFilter<VARFilterType>(filterIndex);
        break;
      }
    default:
      {
        return;
      }
    }// if switch (type)
}
*/

};


template <class TFilterTypeMethod>
void
TextureFilterGenerator::GenericAddTextureFilter( ModelPointerType model, FeatureType type, unsigned int inputListId, SizeType radius, OffsetType offset, std::string msg)
{
  itk::OStringStream oss;
  oss<<msg;
  oss<<radius<<" , "<<offset;
  typename TFilterTypeMethod::Pointer filter = TFilterTypeMethod::New();
  filter->SetInput(model->GetInputImageList()->GetNthElement(inputListId));
  filter->SetRadius(radius);
  filter->SetOffset(offset);
  std::string mess = oss.str();
  model->AddFeatureFilter( filter, type, inputListId, 0, mess);
}

}
#endif
