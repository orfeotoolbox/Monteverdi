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
#ifndef __otbTextureExtractionController_h
#define __otbTextureExtractionController_h

#include "otbFeatureExtractionBaseController.h"

#include "otbTextureExtractionModel.h"
#include "otbTextureExtractionViewGUI.h"


namespace otb
{
/** \class TextureExtractionController
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT TextureExtractionController
  : public FeatureExtractionBaseController
{
public:
  /** Standard class typedefs */
  typedef TextureExtractionController        Self;
  typedef FeatureExtractionBaseController        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef FeatureInfoBase::FeatureType  FeatureBaseType;
  
//   typedef TextureExtractionModel::HaralickTextureType       HaralickTextureType;
//   typedef TextureExtractionModel::AdvancedTextureType       AdvancedTextureType;
  typedef TextureExtractionModel::HaralickTextureVectorType HaralickTextureVectorType;
  typedef TextureExtractionModel::AdvancedTextureVectorType AdvancedTextureVectorType;

  /** Standard type macro */
  itkTypeMacro(TextureExtractionController, FeatureExtractionBaseController);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** Create a specific feature */
  virtual void CreateSpecificFeature(FeatureBaseType featureType);
  
  /** Get a list of selected Haralick features */
  HaralickTextureVectorType GetHaralickTextList();
  
  /** Get a list of selected Haralick advanced features */
  AdvancedTextureVectorType GetAdvancedTextList();
  
  /** Set the specific view */
  void SetView(TextureExtractionViewGUI::Pointer specificView);
  
  /** Set the specific model */
  void SetModel(TextureExtractionModel::Pointer specificModel);

protected:
  /** Constructor */
  TextureExtractionController();
  /** Destrucor */
  virtual ~TextureExtractionController();

private:
  TextureExtractionViewGUI::Pointer m_ViewTex;
  TextureExtractionModel::Pointer m_ModelTex;

};

}

#endif
