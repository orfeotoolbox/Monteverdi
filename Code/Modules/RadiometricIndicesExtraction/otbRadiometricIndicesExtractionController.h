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
#ifndef __otbRadiometricIndicesExtractionController_h
#define __otbRadiometricIndicesExtractionController_h

#include "otbFeatureExtractionBaseController.h"

#include "otbRadiometricIndicesExtractionViewGUI.h"
#include "otbRadiometricIndicesExtractionModel.h"

namespace otb
{
/** \class RadiometricIndicesExtractionController
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT RadiometricIndicesExtractionController
  : public FeatureExtractionBaseController
{
public:
  /** Standard class typedefs */
  typedef RadiometricIndicesExtractionController        Self;
  typedef FeatureExtractionBaseController        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef FeatureInfoBase::FeatureType  FeatureBaseType;

  /** Standard type macro */
  itkTypeMacro(RadiometricIndicesExtractionController, FeatureExtractionBaseController);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** Create a specific feature */
  virtual void CreateSpecificFeature(FeatureBaseType featureType);
  
  /** Set the specific view */
  void SetView(RadiometricIndicesExtractionViewGUI::Pointer specificView);
  
  /** Set the specific model */
  void SetModel(RadiometricIndicesExtractionModel::Pointer specificModel);

protected:
  /** Constructor */
  RadiometricIndicesExtractionController();
  /** Destrucor */
  virtual ~RadiometricIndicesExtractionController();

private:
  RadiometricIndicesExtractionViewGUI::Pointer m_ViewRI;
  RadiometricIndicesExtractionModel::Pointer m_ModelRI;

};

}

#endif
