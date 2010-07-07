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

#ifndef __otbPointSetRegister_h
#define __otbPointSetRegister_h

#include "itkProcessObject.h"

// Estimation
#include "itkEuclideanDistancePointMetric.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSet.h"
#include "itkPointSetToPointSetRegistrationMethod.h"
// Transformation
#include "otbTransformEnumType.h"
#include "itkTransformBase.h"
#include "itkAffineTransform.h"
#include "itkTranslationTransform.h"

namespace otb {

/** \class PointSetRegister
 *
 * \brief Class to compute a transformation between 2 point sets.
 *
 */
template<class TTransform>
class ITK_EXPORT PointSetRegister : public itk::ProcessObject
{

public:
  /** Standard class typedefs */
  typedef PointSetRegister              Self;
  typedef itk::ProcessObject            Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(HPointSetRegister, itk::ProcessObject);

  /** New macro */
  itkNewMacro(Self);

  typedef itk::ProcessObject ProcessObjectType;

  /** Point Set */
  typedef itk::PointSet<float, 2> PointSetType;
  typedef PointSetType::Pointer   PointSetPointerType;
  typedef PointSetType::PointType PointType;

  /** Transformation type */
  typedef TTransform                                  TransformationType;
  typedef typename TransformationType::Pointer        TransformationPointerType;
  typedef typename TransformationType::ParametersType ParametersType;

  /** Transform estimation typedefs */
  typedef itk::EuclideanDistancePointMetric<PointSetType, PointSetType>         MetricType;
  typedef MetricType::TransformType                                             TransformType;
  typedef TransformType::Pointer                                                TransformPointerType;
  typedef TransformType::JacobianType                                           JacobianType;
  typedef itk::LevenbergMarquardtOptimizer                                      OptimizerType;
  typedef itk::PointSetToPointSetRegistrationMethod<PointSetType, PointSetType> RegistrationType;
  typedef OptimizerType::ScalesType                                             ScalesType;

  itkSetObjectMacro(FixPointSet, PointSetType);
  itkGetObjectMacro(FixPointSet, PointSetType);

  itkSetObjectMacro(MovingPointSet, PointSetType);
  itkGetObjectMacro(MovingPointSet, PointSetType);

  itkSetObjectMacro(Transform, TransformationType);
  itkGetObjectMacro(Transform, TransformationType);

  itkGetMacro(TransformParameters, ParametersType);

  /** Compute transformation */
  virtual void ComputeTransform();

  virtual void SetScales(const ScalesType& scales)
  {
    m_Scales = scales;
  }
protected:
  /** Constructor */
  PointSetRegister();
  /** Destructor */
  ~PointSetRegister(){}

private:
  PointSetRegister(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  OptimizerType::ScalesType m_Scales;

  /** Fix point set. */
  PointSetPointerType m_FixPointSet;
  /** Moving point set. */
  PointSetPointerType m_MovingPointSet;
  /** Transform base */
  TransformationPointerType m_Transform;
  /** Store transformation parameters*/
  ParametersType m_TransformParameters;

};

} //end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbPointSetRegister.txx"
#endif

#endif
