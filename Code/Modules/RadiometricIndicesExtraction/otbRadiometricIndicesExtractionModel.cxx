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
#ifndef __otbRadiometricIndicesExtractionModel_cxx
#define __otbRadiometricIndicesExtractionModel_cxx

#include "otbRadiometricIndicesExtractionModel.h"

#include "otbRadiometricIndicesGenerator.h"

namespace otb
{

RadiometricIndicesExtractionModel::RadiometricIndicesExtractionModel()
{
}

RadiometricIndicesExtractionModel::~RadiometricIndicesExtractionModel()
{
}

/** Radiometric Indices */
/** RAndNIR */
void
RadiometricIndicesExtractionModel
::AddRAndNIRFilter(unsigned int redId, unsigned int nirId, FeatureType type, std::vector<double> params)
{
  if (redId < 1 || nirId < 1 ||
      redId > GetNumberOfChannels() || nirId > GetNumberOfChannels())
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << GetNumberOfChannels() << "]");
    }

  RadiometricIndicesGenerator lRadiometricIndicesGenerator;
  lRadiometricIndicesGenerator.AddRAndNIRFilter(this, type, redId, nirId, params);
}

/** RAndBAndNIR */
void
RadiometricIndicesExtractionModel
::AddRAndBAndNIRFilter(unsigned int redId,
                       unsigned int blueId,
                       unsigned int nirId,
                       FeatureType type,
                       std::vector<double> params)
{

  if (redId < 1 || blueId < 1 || nirId < 1 ||
      redId > GetNumberOfChannels() || blueId > GetNumberOfChannels() || nirId > GetNumberOfChannels())
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << GetNumberOfChannels() << "]");
    }
  RadiometricIndicesGenerator lRadiometricIndicesGenerator;
  lRadiometricIndicesGenerator.AddRAndBAndNIRFilter(this, type, redId, blueId, nirId, params);

}

/** RAndGAndNIR */
void
RadiometricIndicesExtractionModel
::AddRAndGAndNIRFilter(unsigned int redId,
                       unsigned int greenId,
                       unsigned int nirId,
                       FeatureType type,
                       std::vector<double> params)
{
  if (redId < 1 || greenId < 1 || nirId < 1 ||
      redId > GetNumberOfChannels() || greenId > GetNumberOfChannels() || nirId > GetNumberOfChannels())
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << GetNumberOfChannels() << "]");
    }
  RadiometricIndicesGenerator lRadiometricIndicesGenerator;
  lRadiometricIndicesGenerator.AddRAndGAndNIRFilter(this, type, redId, greenId, nirId, params);
}

/** Generic Add 2 Channels Filter without paramater */
void
RadiometricIndicesExtractionModel
::Add2ChannelsFilter(unsigned int chan1Id, unsigned int chan2Id, FeatureType type)
{
  if (chan1Id < 1 || chan2Id < 1 ||
      chan1Id > GetNumberOfChannels() || chan2Id > GetNumberOfChannels())
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << GetNumberOfChannels() << "]");
    }
  RadiometricIndicesGenerator lRadiometricIndicesGenerator;
  lRadiometricIndicesGenerator.Add2ChannelsFilter(this, type, chan1Id, chan2Id);
}

void
RadiometricIndicesExtractionModel
::AddSpectralAngleFilter(InputImageType::PixelType pix)
{

  if (GetNumberOfChannels() < 2)
    {
    itkExceptionMacro(
      << "Spectral Angle Filter requires at least 2 channels, the selected picture does not have enough channel.");
    }

  DistanceFilterType::Pointer angle = DistanceFilterType::New();

  angle->SetInput(this->GetInputImage());
  angle->SetReferencePixel(pix);

  std::ostringstream oss;
  oss << "Full Image: Spect Angle: ";
  for (unsigned int i = 0; i < pix.Size(); ++i)
    {
    oss << pix[i] << " ";
    }
  std::string mess = oss.str();
  this->AddFeatureFilter(angle, FeatureInfoRI::SPECTRALANGLE, -1, 0, mess);
}

RadiometricIndicesExtractionModel
::SingleImagePointerType
RadiometricIndicesExtractionModel
::GetSingleImage(int i)
{
  RadiometricIndicesGenerator lRadiometricIndicesGenrator;
  SingleImagePointerType      image = SingleImageType::New();
  
  switch ((GetFilterTypeList())[i])
    {
    case FeatureInfoRI::NDVI:
    case FeatureInfoRI::RVI:
    case FeatureInfoRI::ARVI:
    case FeatureInfoRI::EVI:
    case FeatureInfoRI::PVI:
    case FeatureInfoRI::SAVI:
    case FeatureInfoRI::MSAVI:
    case FeatureInfoRI::MSAVI2:
    case FeatureInfoRI::TSAVI:
    case FeatureInfoRI::TSARVI:
    case FeatureInfoRI::GEMI:
    case FeatureInfoRI::AVI:
    case FeatureInfoRI::WDVI:
    case FeatureInfoRI::IPVI:
    case FeatureInfoRI::TNDVI:
    case FeatureInfoRI::IR:
    case FeatureInfoRI::IC:
    case FeatureInfoRI::IB:
    case FeatureInfoRI::IB2:
    case FeatureInfoRI::NDBI:
    case FeatureInfoRI::ISU:
    case FeatureInfoRI::SRWI:
    case FeatureInfoRI::NDWI:
    case FeatureInfoRI::NDWI2:
    case FeatureInfoRI::MNDWI:
    case FeatureInfoRI::NDPI:
    case FeatureInfoRI::NDTI:
      {
      image = lRadiometricIndicesGenrator.GenerateRadiometricIndicesOutputImage(this, (GetFilterTypeList())[i], i);
      break;
      }
   case FeatureInfoRI::SPECTRALANGLE:
      {
      DistanceFilterType::Pointer angle =
        dynamic_cast<DistanceFilterType*>(static_cast<FilterType *>((GetFilterList())->GetNthElement(i)));
      image = angle->GetOutput();
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
