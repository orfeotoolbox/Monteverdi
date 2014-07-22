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
#ifndef __otbMorphologyExtractionModel_h
#define __otbMorphologyExtractionModel_h

#include "otbFeatureExtractionBaseModel.h"

#include "otbMorphologicalOpeningProfileFilter.h"
#include "otbMorphologicalClosingProfileFilter.h"
#include "itkBinaryBallStructuringElement.h"

namespace otb
{
/** \class MorphologyExtractionModel
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT MorphologyExtractionModel
  : public FeatureExtractionBaseModel
{
public:
  /** Standard class typedefs */
  typedef MorphologyExtractionModel        Self;
  typedef FeatureExtractionBaseModel        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(MorphologyExtractionModel, FeatureExtractionBaseModel);
  
  /** Standard type macro */
  itkNewMacro(Self);
  //itkGetMacro(HasInput, bool);
  
  // Morpho
  typedef itk::BinaryBallStructuringElement<PixelType,
      2>                      StructuringElementType;
  typedef MorphologicalOpeningProfileFilter<SingleImageType, SingleImageType,
      StructuringElementType> OpeningProfileFilterType;
  typedef MorphologicalClosingProfileFilter<SingleImageType, SingleImageType,
      StructuringElementType> ClosingProfileFilterType;
  
  
  void AddMorphoOpeningProfileFilter(unsigned int size, int value, int step);
  void AddMorphoClosingProfileFilter(unsigned int size, int value, int step);
  
  /** Get filtered single output image */
  virtual SingleImagePointerType GetSingleImage(int i);
  
protected:
  /** Constructor */
  MorphologyExtractionModel();
  /** Destructor */
  virtual ~MorphologyExtractionModel();

private:

};

}

#endif
