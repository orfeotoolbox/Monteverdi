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

  // Pan Tax
  typedef PanTexTextureImageFunctionFilter<SingleImageType, SingleImageType>                        PanTexFilterType;
 

template <class TFilterTypeMethod> void GenericAddTextureFilter( ModelPointerType model, FeatureType type, unsigned int inputListId, SizeType radius, OffsetType offset, std::string msg);

void GenerateTextureFilter( ModelPointerType model, FeatureType type, unsigned int inputListId, SizeType radius, OffsetType offset )
{
  //itk::OStringStream oss;
  std::string mess;
  switch (type)
    {
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
    case FeatureInfo::TEXT_PANTEX:
      {
        PanTexFilterType::Pointer panTexFilter = dynamic_cast<PanTexFilterType*>(static_cast<FilterType *>(model->GetFilterList()->GetNthElement(inputListId)));
        image = panTexFilter->GetOutput();
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
    case TEXT_PANTEX:
      {
        model->GenericConnectFilter<PanTexFilterType>(filterIndex);
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
