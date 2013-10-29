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
#ifndef __otbMorphologyExtractionModel_cxx
#define __otbMorphologyExtractionModel_cxx

#include "otbMorphologyExtractionModel.h"
#include "otbFeatureMorpho.h"

namespace otb
{

MorphologyExtractionModel::MorphologyExtractionModel()
{
}

MorphologyExtractionModel::~MorphologyExtractionModel()
{
}

void
MorphologyExtractionModel
::AddMorphoOpeningProfileFilter(unsigned int size, int value, int step)
{
  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    // Instantiation
    OpeningProfileFilterType::Pointer profileFilter = OpeningProfileFilterType::New();

    profileFilter->SetProfileSize(size);
    profileFilter->SetInitialValue(value);
    profileFilter->SetStep(step);

    profileFilter->SetInput(GetInputImageList()->GetNthElement(i));

    profileFilter->GetOutput()->UpdateOutputInformation();

    for (unsigned int j = 0; j < size; ++j)
      {
      std::ostringstream oss;
      oss << "Morpho Opening: " << size << " , " << value << " , " << step << ", " << value + j * step;
      std::string mess = oss.str();
      this->AddFeatureFilter(profileFilter, FeatureInfoMorpho::MORPHOLOGICALOPENING, i, j,  mess);
      }
    }
}

void
MorphologyExtractionModel
::AddMorphoClosingProfileFilter(unsigned int size, int value, int step)
{
  for (unsigned int i = 0; i < GetInputImageList()->Size(); i++)
    {
    // Instantiation
    ClosingProfileFilterType::Pointer profileFilter = ClosingProfileFilterType::New();

    profileFilter->SetProfileSize(size);
    profileFilter->SetInitialValue(value);
    profileFilter->SetStep(step);

    profileFilter->SetInput(GetInputImageList()->GetNthElement(i));
    profileFilter->GetOutput()->UpdateOutputInformation();

    for (unsigned int j = 0; j < size; ++j)
      {
      std::ostringstream oss;
      oss << "Morpho Closing: " << size << " , " << value << " , " << step << ", " << value + j * step;
      std::string mess = oss.str();
      this->AddFeatureFilter(profileFilter, FeatureInfoMorpho::MORPHOLOGICALCLOSING, i, j,  mess);
      }
    }
}

MorphologyExtractionModel
::SingleImagePointerType
MorphologyExtractionModel
::GetSingleImage(int i)
{
  SingleImagePointerType      image = SingleImageType::New();
  
  switch ((GetFilterTypeList())[i])
    {
    case FeatureInfoMorpho::MORPHOLOGICALOPENING:
      {
      OpeningProfileFilterType::Pointer profile =
        dynamic_cast<OpeningProfileFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      profile->Update();
      image = profile->GetOutput()->GetNthElement(m_OutputIndexMap[i]);
      break;
      }

    case FeatureInfoMorpho::MORPHOLOGICALCLOSING:
      {
      ClosingProfileFilterType::Pointer profile =
        dynamic_cast<ClosingProfileFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      profile->Update();
      image = profile->GetOutput()->GetNthElement(m_OutputIndexMap[i]);
      break;
      }
    default:
      {
      }
    }
  return image;
}

}

#endif
