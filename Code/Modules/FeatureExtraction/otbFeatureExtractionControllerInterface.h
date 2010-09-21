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
#ifndef __otbFeatureExtractionControllerInterface_h
#define __otbFeatureExtractionControllerInterface_h

#include "otbFeatureExtractionModel.h"
#include "itkProcessObject.h"
#include "otbImageWidgetController.h"

namespace otb
{
class ITK_EXPORT FeatureExtractionControllerInterface
  : public itk::ProcessObject
{
public:
  /** Standard class typedefs */
  typedef FeatureExtractionControllerInterface Self;
  typedef itk::ProcessObject                   Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;

  typedef FeatureExtractionModel           ModelType;
  typedef ModelType::InputImageType        InputImageType;
  typedef ModelType::InputImagePointerType InputImagePointerType;
  typedef ModelType::IndexType             IndexType;
  typedef ModelType::SizeType              SizeType;
  typedef ModelType::ContinuousIndexType   ContinuousIndexType;

  typedef FeatureInfo::FeatureType FeatureType;
/** NewVisu */
  typedef ImageWidgetController       VisuControllerType;
  typedef VisuControllerType::Pointer VisuControllerPointerType;

  /** Standard type macros */
  itkTypeMacro(FeatureExtractionControllerInterface, Superclass);

  /** Users actions */
  virtual void SaveOutput() = 0;
  virtual void InitInput() = 0;
  virtual void ClearFeatures() = 0;
  virtual void CreateFeature(FeatureType featureType) = 0;
  virtual void ClearSelectedChannels() = 0;
  virtual void SetOutputFileName(const char * name) = 0;
  virtual void AddInputChannels(std::vector<unsigned int> list) = 0;
  virtual void ChangeFilterStatus(int id) = 0;
  virtual void ViewedRegionChanged() = 0;
  virtual void AddToOutputListOrder(int id) = 0;
  virtual void RemoveFromOutputListOrder(int id) = 0;
  virtual void ExchangeOutputListOrder(int direction) = 0;
  virtual void LeftMouseButtonClicked(ContinuousIndexType index) = 0;
  virtual ModelType* GetModel() = 0;
  virtual void UpdateFeaturePreview(unsigned int id) = 0;
  virtual void ShowBarStatus() = 0;
  virtual void HideBarStatus() = 0;

  virtual void Quit() = 0;
  virtual void Cancel() = 0;

  virtual VisuControllerPointerType GetVisuController() = 0;
  virtual VisuControllerPointerType GetResultVisuController() = 0;

protected:
  /** Constructor */
  FeatureExtractionControllerInterface()
  {
  }
  /** Destructor */
  virtual ~FeatureExtractionControllerInterface() {}

  /** The instance of the model (for commodity) */
  ModelType::Pointer m_Model;

private:
  FeatureExtractionControllerInterface(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
