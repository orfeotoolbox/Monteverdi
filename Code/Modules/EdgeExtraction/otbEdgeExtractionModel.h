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
#ifndef __otbEdgeExtractionModel_h
#define __otbEdgeExtractionModel_h

#include "otbFeatureExtractionBaseModel.h"

#include "otbTouziEdgeDetectorImageFilter.h"
#include "otbHarrisImageFilter.h"
#include "otbVarianceImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "otbMeanShiftSegmentationFilter.h"
#include "otbLabelToBoundaryImageFilter.h"


namespace otb
{
/** \class EdgeExtractionModel
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT EdgeExtractionModel
  : public FeatureExtractionBaseModel
{
public:
  /** Standard class typedefs */
  typedef EdgeExtractionModel        Self;
  typedef FeatureExtractionBaseModel        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(EdgeExtractionModel, FeatureExtractionBaseModel);
  
  /** Standard type macro */
  itkNewMacro(Self);
  //itkGetMacro(HasInput, bool);
  
  // Harris
  typedef HarrisImageFilter<SingleImageType, SingleImageType> HarrisType;
  // Variance
  typedef VarianceImageFilter<SingleImageType, SingleImageType> VarFilterType;
  // Gradient
  typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<SingleImageType> GradientFilterType;
  // Mean Shift
  typedef TypeManager::Labeled_Int_Image   LabeledImageType;
  typedef MeanShiftSegmentationFilter<InputImageType, LabeledImageType, OutputImageType> MeanShiftFilterType;
  typedef ObjectList<MeanShiftFilterType>                                              MeanShiftFilterListType;
  typedef LabelToBoundaryImageFilter<LabeledImageType,SingleImageType> BoundaryExtractorType;
  typedef ObjectList<BoundaryExtractorType>            BoundaryExtractorListType;
  
  // Touzi
  typedef TouziEdgeDetectorImageFilter<SingleImageType, SingleImageType> TouziFilterType;
  
  void AddHarrisFilter(double sigmaD, double sigmaI, double alpha);
  void AddVarianceFilter(int radiusX, int radiusY);
  void AddGradientFilter(double sigma);
  void AddEdgeDensityFilter(FeatureType type, std::vector<double> params);
  void AddMeanShiftFilter(FeatureType type, unsigned int spatial, double range, unsigned int minSize);
  void AddTouziFilter(unsigned int radiusX);
  
  /** Init mean shift */
  void InitMeanShiftLists();
  
  /** Get filtered single output image */
  virtual SingleImagePointerType GetSingleImage(int i);
  
protected:
  /** Constructor */
  EdgeExtractionModel();
  /** Destructor */
  virtual ~EdgeExtractionModel();

private:
  // For mean shift
  ImageListObjectListType::Pointer         m_MSImageListList;
  ImageListToVectorObjectListType::Pointer m_MSListToVectorFilterList;
  MeanShiftFilterListType::Pointer         m_MeanShiftFilterList;
  BoundaryExtractorListType::Pointer       m_BoundaryExtractorList;

};

}

#endif
