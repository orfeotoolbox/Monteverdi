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

#ifndef __otbHomologousPointExtractionModuleControllerInterface_h
#define __otbHomologousPointExtractionModuleControllerInterface_h

#include "itkObject.h"
#include "itkIndex.h"
#include "otbHomologousPointExtractionModuleModel.h"

namespace otb
{
class ITK_EXPORT HomologousPointExtractionModuleControllerInterface
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef HomologousPointExtractionModuleControllerInterface Self;
  typedef itk::Object                                        Superclass;
  typedef itk::SmartPointer<Self>                            Pointer;
  typedef itk::SmartPointer<const Self>                      ConstPointer;

  typedef HomologousPointExtractionModuleModel ModelType;
  typedef itk::Index<>                         IndexType;

  /** Standard type macros */
  itkTypeMacro(HomologousPointExtractionModuleControllerInterface, Superclass);

  /** Users actions */
  virtual void LinkPixelDescriptors() = 0;
  virtual void AddPoints(int x1, int y1, int x2, int y2) = 0;
  virtual void ClearPointList() = 0;
  virtual void DeletePointFromList(unsigned int id) = 0;
  virtual void LeftMouseButtonClicked(IndexType index, unsigned int viewId) = 0;
  virtual void ComputeTransform() = 0;
  virtual void FocusOn(IndexType id1, IndexType id2) = 0;
  virtual void Evaluate(IndexType id) = 0;
  virtual void SetRectifyMode(bool flag) = 0;

  virtual void OK() = 0;

  virtual ModelType* GetModel() = 0;

  itkSetMacro(TransformationAvailable, bool);
  itkGetMacro(TransformationAvailable, bool);

protected:
  /** Constructor */
  HomologousPointExtractionModuleControllerInterface()
  {
    m_TransformationAvailable = false;
  }
  /** Destructor */
  ~HomologousPointExtractionModuleControllerInterface() {}

private:
  HomologousPointExtractionModuleControllerInterface(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
  bool m_TransformationAvailable;
};
} // end namespace otb

#endif
