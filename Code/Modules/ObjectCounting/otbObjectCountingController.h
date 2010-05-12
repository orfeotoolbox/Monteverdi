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
#ifndef __otbObjectCountingController_h
#define __otbObjectCountingController_h

#include "otbObjectCountingControllerInterface.h"
#include "otbObjectCountingViewGUI.h"


namespace otb
{
class ITK_EXPORT ObjectCountingController
      : public ObjectCountingControllerInterface
{
public:
  /** Standard class typedefs */
  typedef ObjectCountingController          Self;
  typedef ObjectCountingControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef ObjectCountingViewGUI          ViewType;
  typedef ViewType::Pointer              ViewPointerType;
  typedef Superclass::ModelType ModelType;
  typedef ModelType::SizeType SizeType;
  typedef ModelType::PixelType PixelType;

  /** Standard type macros */
  itkTypeMacro(ObjectCountingController,Superclass);
  itkNewMacro(Self);

  itkSetObjectMacro(View,ViewType);

  virtual void OpenImage();
  virtual void IndexClicked(const IndexType & index);
  virtual IndexType TransformIndex(const IndexType & index);
  virtual void DoubleLeftClick(const IndexType & index);
  virtual void SimpleLeftClick(const IndexType & index);
  virtual void DoubleRightClick(const IndexType & index);
  virtual void ComputeOverExtract();
  virtual void UseSVM(bool b);
  virtual void UpdateThresholdValue( double val );
  virtual void UpdateSpatialRadius(int val);
  virtual void UpdateRangeRadius(int val);
  virtual void UpdateScale(int val);
  virtual void UpdateNuParameter(double val);
  virtual void UpdateMinRegionSize(int val);
  virtual void SetOutputImageName(const char * name);
  virtual void SetOutputVectorFileName(const char * name);
  virtual void UseSmoothing(bool b);
  virtual void SaveResult( const char * cfname );
  virtual void SavePolygon( const char * cfname );
  
  virtual void Quit();

protected:
  /** Constructor */
  ObjectCountingController();
  /** Destructor */
  virtual ~ObjectCountingController();

private:
  ObjectCountingController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;

  /** bool to inhib multi access extraction*/
  bool m_IsExtractRunning;
  /** Used to know if need to create a new polygon*/
  bool m_HasDoubleClicked;

};
} //end namespace otb

#endif
