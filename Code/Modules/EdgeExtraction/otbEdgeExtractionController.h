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
#ifndef __otbEdgeExtractionController_h
#define __otbEdgeExtractionController_h

#include "otbFeatureExtractionBaseController.h"

#include "otbEdgeExtractionViewGUI.h"
#include "otbEdgeExtractionModel.h"

namespace otb
{
/** \class EdgeExtractionController
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT EdgeExtractionController
  : public FeatureExtractionBaseController
{
public:
  /** Standard class typedefs */
  typedef EdgeExtractionController        Self;
  typedef FeatureExtractionBaseController        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef FeatureInfoBase::FeatureType  FeatureBaseType;

  /** Standard type macro */
  itkTypeMacro(EdgeExtractionController, FeatureExtractionBaseController);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** clear also meanshift features */
  virtual void ClearFeatures();
  
  /** Create a specific feature */
  virtual void CreateSpecificFeature(FeatureBaseType featureType);
  
  /** Set the specific view */
  void SetView(EdgeExtractionViewGUI::Pointer specificView);
  
  /** Set the specific model */
  void SetModel(EdgeExtractionModel::Pointer specificModel);

protected:
  /** Constructor */
  EdgeExtractionController();
  /** Destrucor */
  virtual ~EdgeExtractionController();

private:
  EdgeExtractionViewGUI::Pointer m_ViewEdge;
  EdgeExtractionModel::Pointer m_ModelEdge;

};

}

#endif
