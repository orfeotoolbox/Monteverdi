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
  // Touzi
  typedef TouziEdgeDetectorImageFilter<SingleImageType, SingleImageType> TouziFilterType;
  
  void AddHarrisFilter(double sigmaD, double sigmaI, double alpha);
  void AddVarianceFilter(int radiusX, int radiusY);
  void AddGradientFilter(double sigma);
  void AddEdgeDensityFilter(FeatureType type, std::vector<double> params);
  void AddTouziFilter(unsigned int radiusX);
  
  /** Get filtered single output image */
  virtual SingleImagePointerType GetSingleImage(int i);
  
protected:
  /** Constructor */
  EdgeExtractionModel();
  /** Destructor */
  virtual ~EdgeExtractionModel();

private:

};

}

#endif
