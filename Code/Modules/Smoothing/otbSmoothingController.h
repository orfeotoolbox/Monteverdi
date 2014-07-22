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
#ifndef __otbSmoothingController_h
#define __otbSmoothingController_h

#include "otbFeatureExtractionBaseController.h"

#include "otbSmoothingViewGUI.h"
#include "otbSmoothingModel.h"

namespace otb
{
/** \class SmoothingController
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT SmoothingController
  : public FeatureExtractionBaseController
{
public:
  /** Standard class typedefs */
  typedef SmoothingController        Self;
  typedef FeatureExtractionBaseController        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef FeatureInfoBase::FeatureType  FeatureBaseType;

  /** Standard type macro */
  itkTypeMacro(SmoothingController, FeatureExtractionBaseController);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** clear also meanshift features */
  virtual void ClearFeatures();
  
  /** Create a specific feature */
  virtual void CreateSpecificFeature(FeatureBaseType featureType);
  
  /** Set the specific view */
  void SetView(SmoothingViewGUI::Pointer specificView);
  
  /** Set the specific model */
  void SetModel(SmoothingModel::Pointer specificModel);

protected:
  /** Constructor */
  SmoothingController();
  /** Destrucor */
  virtual ~SmoothingController();

private:
  SmoothingViewGUI::Pointer m_ViewSmooth;
  SmoothingModel::Pointer m_ModelSmooth;

};

}

#endif
