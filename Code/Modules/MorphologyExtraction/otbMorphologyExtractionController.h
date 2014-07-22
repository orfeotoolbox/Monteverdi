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
#ifndef __otbMorphologyExtractionController_h
#define __otbMorphologyExtractionController_h

#include "otbFeatureExtractionBaseController.h"

#include "otbMorphologyExtractionViewGUI.h"
#include "otbMorphologyExtractionModel.h"

namespace otb
{
/** \class MorphologyExtractionController
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT MorphologyExtractionController
  : public FeatureExtractionBaseController
{
public:
  /** Standard class typedefs */
  typedef MorphologyExtractionController        Self;
  typedef FeatureExtractionBaseController        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef FeatureInfoBase::FeatureType  FeatureBaseType;

  /** Standard type macro */
  itkTypeMacro(MorphologyExtractionController, FeatureExtractionBaseController);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** Create a specific feature */
  virtual void CreateSpecificFeature(FeatureBaseType featureType);
  
  /** Set the specific view */
  void SetView(MorphologyExtractionViewGUI::Pointer specificView);
  
  /** Set the specific model */
  void SetModel(MorphologyExtractionModel::Pointer specificModel);

protected:
  /** Constructor */
  MorphologyExtractionController();
  /** Destrucor */
  virtual ~MorphologyExtractionController();

private:
  MorphologyExtractionViewGUI::Pointer m_ViewMorpho;
  MorphologyExtractionModel::Pointer m_ModelMorpho;

};

}

#endif
