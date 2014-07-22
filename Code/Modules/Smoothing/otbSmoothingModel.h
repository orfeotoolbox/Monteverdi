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
#ifndef __otbSmoothingModel_h
#define __otbSmoothingModel_h

#include "otbFeatureExtractionBaseModel.h"

#include "itkMeanImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "otbMeanShiftVectorImageFilter.h"

namespace otb
{
/** \class SmoothingModel
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT SmoothingModel
  : public FeatureExtractionBaseModel
{
public:
  /** Standard class typedefs */
  typedef SmoothingModel        Self;
  typedef FeatureExtractionBaseModel        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(SmoothingModel, FeatureExtractionBaseModel);
  
  /** Standard type macro */
  itkNewMacro(Self);
  //itkGetMacro(HasInput, bool);
  
  // Copy of original data
  typedef itk::ShiftScaleImageFilter<SingleImageType, SingleImageType> ShiftScaleFilterType;
  // Mean
  typedef itk::MeanImageFilter<SingleImageType, SingleImageType> MeanFilterType;
  // Mean Shift
  typedef MeanShiftVectorImageFilter<InputImageType, OutputImageType, SingleImageType> MeanShiftFilterType;
  typedef ObjectList<MeanShiftFilterType>                                              MeanShiftFilterListType;
  
  void AddOriginalData();
  void AddMeanFilter(int radiusX, int radiusY);
  void AddMeanShiftFilter(FeatureType type, unsigned int spatial, double range, unsigned int minSize, double scale);
  
  /** Init mean shift */
  void InitMeanShiftLists();
  
  /** Get filtered single output image */
  virtual SingleImagePointerType GetSingleImage(int i);
  
protected:
  /** Constructor */
  SmoothingModel();
  /** Destructor */
  virtual ~SmoothingModel();

private:
  // For mean shift
  ImageListObjectListType::Pointer         m_MSImageListList;
  ImageListToVectorObjectListType::Pointer m_MSListToVectorFilterList;
  MeanShiftFilterListType::Pointer         m_MeanShiftFilterList;

};

}

#endif
