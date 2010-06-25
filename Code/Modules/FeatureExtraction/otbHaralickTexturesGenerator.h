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
#ifndef __otbHaralickTexturesGenerator_h
#define __otbHaralickTexturesGenerator_h

#include "otbFeature.h"
#include "otbFeatureExtractionModel.h"
#include "otbScalarImageToTexturesFilter.h"
#include "otbScalarImageToAdvancedTexturesFilter.h"
#include "itkMinimumMaximumImageCalculator.h"


namespace otb
{
/** \class HaralickTexturesGenerator
 *
 *
 *
 */
class HaralickTexturesGenerator
{
public:
  typedef FeatureExtractionModel                                                   ModelType;
  typedef ModelType::Pointer                                                       ModelPointerType;
  typedef ModelType::FilterType                                                    FilterType;
  typedef ModelType::PixelType                                                     PixelType;
  typedef ModelType::SizeType                                                      SizeType;
  typedef ModelType::OffsetType                                                    OffsetType;
  typedef ModelType::SingleImageType                                               SingleImageType;
  typedef ModelType::SingleImagePointerType                                        SingleImagePointerType;

  typedef FeatureInfo::FeatureType                                                 FeatureType;
  typedef HaralickTexture::TextureType                                             TextureType;
  typedef std::vector<TextureType>                                                 TextureVectorType;


  /***************************/
  /** Filter type declaration*/
  /***************************/

  typedef itk::MinimumMaximumImageCalculator<SingleImageType>                       MinMaxCalculatorType;

  typedef ScalarImageToTexturesFilter<SingleImageType,SingleImageType>              HarTexturesFilterType;
  typedef ScalarImageToAdvancedTexturesFilter<SingleImageType,SingleImageType>      AdvTexturesFilterType;


  /************/
  /** Methods */
  /************/

  //template <class TFilterTypeMethod> typename TFilterTypeMethod::Pointer GenericAddRAndNIRFilter( ModelPointerType model, FeatureType type, unsigned int redId, unsigned int nirId, std::string msg);

  // AddHarTexturesFilter
  void AddHarTexturesFilter( ModelPointerType pModel, TextureVectorType pHarList, SizeType pRadius, OffsetType pOff, unsigned int pBin)
    { 
      std::map<TextureType, FeatureType> textToHarMap;
      textToHarMap[HaralickTexture::ENERGY] = FeatureInfo::TEXT_HAR_ENERGY;
      textToHarMap[HaralickTexture::ENTROPY] = FeatureInfo::TEXT_HAR_ENTROPY;
      textToHarMap[HaralickTexture::CORRELATION] = FeatureInfo::TEXT_HAR_CORR;
      textToHarMap[HaralickTexture::INERTIA] = FeatureInfo::TEXT_HAR_INERTIA;
      textToHarMap[HaralickTexture::INVDIFMO] = FeatureInfo::TEXT_HAR_INVDIFMO;
      textToHarMap[HaralickTexture::CLUSPRO] = FeatureInfo::TEXT_HAR_CLUSPRO;
      textToHarMap[HaralickTexture::CLUSHA] = FeatureInfo::TEXT_HAR_CLUSHA;
      textToHarMap[HaralickTexture::HARCORR] = FeatureInfo::TEXT_HAR_HARCORR;
      textToHarMap[HaralickTexture::UNKNOWN] = FeatureInfo::TEXT_HAR_UNKNOWN;


      for (unsigned int i = 0; i < pModel->GetInputImageList()->Size(); i++)
	{
	  HarTexturesFilterType::Pointer filter = HarTexturesFilterType::New();
	  filter->SetRadius(pRadius);
	  filter->SetOffset(pOff);
	  filter->SetNumberOfBinsPerAxis(pBin);
	  // Compute input min, max
	 /*  MinMaxCalculatorType::Pointer minMaxFilter = MinMaxCalculatorType::New(); */
/* 	  minMaxFilter->SetImage(pModel->GetInputImageList()->GetNthElement(i)); */
/* 	  std::cout<<"avant"<<std::endl; */
/* 	  std::cout<<pModel->GetInputImageList()->GetNthElement(i)<<std::endl; */
/* 	  minMaxFilter->Compute(); */
/* 	  std::cout<<"avant"<<std::endl; */
	  
	  filter->SetInputImageMinimum(0);//minMaxFilter->GetMinimum());
	  filter->SetInputImageMaximum(255);//minMaxFilter->GetMaximum());
	  filter->SetInput(pModel->GetInputImageList()->GetNthElement(i));
	  
	  for(unsigned int textId = 0; textId<pHarList.size(); textId++)
	    {
	      itk::OStringStream oss;
	      
	      switch(pHarList[textId])
		{
		case HaralickTexture::ENERGY:
		  {
		    oss<<"Energy : ";
		    break;
		  }
		case HaralickTexture::ENTROPY:
		  {
		    oss<<"Entropy : ";
		    break;
		  }
		case HaralickTexture::CORRELATION:
		  {
		    oss<<"Correlation : ";
		    break;
		  }
		case HaralickTexture::INERTIA:
		  {
		    oss<<"Inertia : ";
		    break;
		  }
		case HaralickTexture::INVDIFMO:
		  {
		    oss<<"Inv. Diff. Moment : ";
		    break;
		  }
		case HaralickTexture::CLUSPRO:
		  {
		    oss<<"Cluster Pro. : ";
		    break;
		  }
		case HaralickTexture::CLUSHA:
		  {
		    oss<<"Cluster Shade : ";
		    break;
		  }
		case HaralickTexture::HARCORR:
		  {
		    oss<<"Har. Corr : ";
		    break;
		  }
		default:
		  {
		  }
		}
	      
	      oss<<"Haralick Text : ";
		oss<<pRadius<<","<<pOff<<","<<pBin;
	      std::string mess = oss.str();
	      pModel->AddFeatureFilter( filter, textToHarMap[pHarList.at(textId)], i, 0, mess);
	    }
	}
    }


  // GenerateHaralickTextureOutputImage
  SingleImagePointerType GenerateHaralickTextureOutputImage( ModelPointerType pModel,  FeatureType pType, unsigned int pInputListId  )
  {
    SingleImagePointerType image =  SingleImageType::New();
    HarTexturesFilterType::Pointer filter = dynamic_cast<HarTexturesFilterType*>(static_cast<FilterType *>(pModel->GetFilterList()->GetNthElement(pInputListId)));
    switch (pType)
    {
      case FeatureInfo::TEXT_HAR_ENERGY:
      {
        image = filter->GetEnergyOutput();
        break;
      }
      case FeatureInfo::TEXT_HAR_ENTROPY:
      {
        image = filter->GetEntropyOutput();
        break;
      }
      case FeatureInfo::TEXT_HAR_CORR:
      {
        image = filter->GetCorrelationOutput();
        break;
      }
      case FeatureInfo::TEXT_HAR_INVDIFMO:
      {
        image = filter->GetInverseDifferenceMomentOutput();
        break;
      }
      case FeatureInfo::TEXT_HAR_CLUSPRO:
      {
        image = filter->GetClusterProminenceOutput();
        break;
      }
      case FeatureInfo::TEXT_HAR_CLUSHA:
      {
        image = filter->GetClusterShadeOutput();
        break;
      }
      case FeatureInfo::TEXT_HAR_HARCORR:
      {
        image = filter->GetHaralickCorrelationOutput();
        break;
      }
    case FeatureInfo::TEXT_HAR_INERTIA:
      {
        image = filter->GetInertiaOutput();
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


}
#endif

