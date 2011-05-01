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
#ifndef __otbBayesianFusionModule_h
#define __otbBayesianFusionModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbBayesianFusionFilter.h"

namespace otb
{
/** \class BayesianFusionModule
 *  \brief This is the BayesianFusion module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT BayesianFusionModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef BayesianFusionModule           Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(BayesianFusionModule, Module);

  /** Data typedefs */
  typedef TypeManager::Floating_Point_VectorImage FloatingVectorImageType;
  typedef TypeManager::Floating_Point_Image       FloatingImageType;

  typedef otb::BayesianFusionFilter<FloatingVectorImageType, FloatingVectorImageType,
    FloatingImageType, FloatingVectorImageType> BayesianFusionFilterType;

protected:
  /** Constructor */
  BayesianFusionModule();

  /** Destructor */
  virtual ~BayesianFusionModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  BayesianFusionModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  BayesianFusionFilterType::Pointer m_BayesianFusionFilter;

};

} // End namespace otb

#endif
