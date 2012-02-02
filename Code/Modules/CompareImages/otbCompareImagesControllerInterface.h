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

#ifndef __otbCompareImagesControllerInterface_h
#define __otbCompareImagesControllerInterface_h

#include "otbCompareImagesModel.h"
#include "otbImageWidgetController.h"

namespace otb
{
class ITK_EXPORT CompareImagesControllerInterface
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef CompareImagesControllerInterface      Self;
  typedef itk::Object                              Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  typedef CompareImagesModel ModelType;
  typedef ImageWidgetController       VisuControllerType;
  typedef VisuControllerType::Pointer VisuControllerPointerType;
  typedef ModelType::IndexType             IndexType;
  typedef ModelType::ContinuousIndexType   ContinuousIndexType;
  /** Standard type macros */
  itkTypeMacro(CompareImagesControllerInterface, Superclass);

  /*itkSetObjectMacro(Model, CompareImagesModel);
  itkGetObjectMacro(Model, CompareImagesModel); */
  
  virtual void SetModel(CompareImagesModel* model) = 0;
  virtual ModelType* GetModel() = 0;

  virtual void ProcessCompareImagesFilter() = 0;
  virtual void SaveQuit() = 0;

  virtual VisuControllerPointerType GetLeftVisuController() = 0;
  virtual VisuControllerPointerType GetRightVisuController() = 0;
  
protected:
  /** Constructor */
  CompareImagesControllerInterface() {}
  /** Destructor */
  virtual ~CompareImagesControllerInterface() {}

  /** The instance of the model (for commodity) */
  ModelType::Pointer m_Model;

private:
  CompareImagesControllerInterface(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
