/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbObjectCountingControllerInterface_h
#define __otbObjectCountingControllerInterface_h

#include "otbObjectCountingModel.h"
#include "otbImageViewerFullResolutionEventsInterface.h"

namespace otb
{
class ITK_ABI_EXPORT ObjectCountingControllerInterface
  : public ImageViewerFullResolutionEventsInterface
{
public:
  /** Standard class typedefs */
  typedef ObjectCountingControllerInterface Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  typedef ObjectCountingModel ModelType;

  /** Standard type macros */
  itkTypeMacro(ObjectCountingControllerInterface, Superclass);

  /** Users actions */
  virtual void OpenImage() = 0;
  virtual IndexType TransformIndex(const IndexType& index) = 0;
  virtual void IndexClicked(const IndexType& index) = 0;
  virtual void SaveResult(const char * cfname) = 0;
  virtual void SavePolygon(const char * cfname) = 0;
  virtual void DoubleLeftClick(const IndexType& index) = 0;
  virtual void SimpleLeftClick(const IndexType& index) = 0;
  virtual void DoubleRightClick(const IndexType& index) = 0;
  virtual void ComputeOverExtract() = 0;
  virtual void UseSVM(bool b) = 0;
  virtual void UpdateThresholdValue(double val) = 0;
  virtual void UpdateSpatialRadius(int val) = 0;
  virtual void UpdateRangeRadius(int val) = 0;
  virtual void UpdateScale(int val) = 0;
  virtual void UpdateNuParameter(double val) = 0;
  virtual void UpdateMinRegionSize(int val) = 0;
  virtual void SetOutputImageName(const char * name) = 0;
  virtual void SetOutputVectorFileName(const char * name) = 0;
  virtual void UseSmoothing(bool b) = 0;

  virtual void Quit() = 0;

protected:
  /** Constructor */
  ObjectCountingControllerInterface()
  {
    m_Model = ModelType::GetInstance();
  }
  /** Destructor */
  ~ObjectCountingControllerInterface() {}

  /** The instance of the model (for commodity) */
  ModelType * m_Model;

private:
  ObjectCountingControllerInterface(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
