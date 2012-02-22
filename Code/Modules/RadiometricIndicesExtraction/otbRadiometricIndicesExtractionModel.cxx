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
      redId > m_NumberOfChannels || nirId > m_NumberOfChannels)
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << m_NumberOfChannels << "]");
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
      redId > m_NumberOfChannels || blueId > m_NumberOfChannels || nirId > m_NumberOfChannels)
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << m_NumberOfChannels << "]");
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
      redId > m_NumberOfChannels || greenId > m_NumberOfChannels || nirId > m_NumberOfChannels)
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << m_NumberOfChannels << "]");
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
      chan1Id > m_NumberOfChannels || chan2Id > m_NumberOfChannels)
    {
    itkExceptionMacro(<< "Channel indices must belong to range [1, " << m_NumberOfChannels << "]");
    }
  RadiometricIndicesGenerator lRadiometricIndicesGenerator;
  lRadiometricIndicesGenerator.Add2ChannelsFilter(this, type, chan1Id, chan2Id);
}

RadiometricIndicesExtractionModel
::SingleImagePointerType
RadiometricIndicesExtractionModel
::GetSingleImage(int i)
{
  RadiometricIndicesGenerator lRadiometricIndicesGenrator;
  SingleImagePointerType      image = SingleImageType::New();
  
  image = lRadiometricIndicesGenrator.GenerateRadiometricIndicesOutputImage(this, m_FilterTypeList[i], i);

  return image;
}

}

#endif
