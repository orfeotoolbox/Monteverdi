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
#ifndef __otbRadiometricIndicesExtractionModel_h
#define __otbRadiometricIndicesExtractionModel_h

#include "otbFeatureExtractionBaseModel.h"

#include "otbSpectralAngleDistanceImageFilter.h"

namespace otb
{
/** \class RadiometricIndicesExtractionModel
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT RadiometricIndicesExtractionModel
  : public FeatureExtractionBaseModel
{
public:
  /** Standard class typedefs */
  typedef RadiometricIndicesExtractionModel        Self;
  typedef FeatureExtractionBaseModel        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(RadiometricIndicesExtractionModel, FeatureExtractionBaseModel);
  
  /** Standard type macro */
  itkNewMacro(Self);
  //itkGetMacro(HasInput, bool);
  
  // Spectral Angle
  typedef SpectralAngleDistanceImageFilter<InputImageType, SingleImageType> DistanceFilterType;
  
  void AddRAndNIRFilter(unsigned int redId, unsigned int nirId, FeatureType type, std::vector<double> params);
  void AddRAndBAndNIRFilter(unsigned int redId,
                            unsigned int blueId,
                            unsigned int nirId,
                            FeatureType type,
                            std::vector<double> params);
  void AddRAndGAndNIRFilter(unsigned int redId,
                            unsigned int greenId,
                            unsigned int nirId,
                            FeatureType type,
                            std::vector<double> params);
  void Add2ChannelsFilter(unsigned int chan1Id, unsigned int chan2Id, FeatureType type);
  
  void AddSpectralAngleFilter(InputImagePixelType pix);
  
  /** Get filtered single output image */
  virtual SingleImagePointerType GetSingleImage(int i);
  
protected:
  /** Constructor */
  RadiometricIndicesExtractionModel();
  /** Destructor */
  virtual ~RadiometricIndicesExtractionModel();

private:

};

}

#endif
