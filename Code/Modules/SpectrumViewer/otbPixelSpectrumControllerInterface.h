/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbPixelSpectrumControllerInterface_h
#define __otbPixelSpectrumControllerInterface_h

#include "otbPixelSpectrumModel.h"
#include "itkObject.h"
#include "itkFixedArray.h"
#include <string>

namespace otb {

class PixelSpectrumControllerInterface
: public itk::Object

  {
public:
  /** Standard class typedefs */
  typedef PixelSpectrumControllerInterface Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef otb::PixelSpectrumModel ModelType;
  typedef ModelType::Pointer      ModelPointerType;

  typedef itk::FixedArray<double, 4> ColorType;
  typedef ModelType::IndexType       IndexType;

  /** Standard type macros */
  itkTypeMacro(PixelSpectrumControllerInterface, Superclass);

  itkSetObjectMacro(Model, ModelType);
  itkGetObjectMacro(Model, ModelType);

  /* pure virtual methods*/
  virtual void RemoveAPixelSpectrumById(int i) = 0;
  virtual void SetAxisLengthAtInit(double x, double y) = 0;
  virtual void DetermineMinMaxPixelValues() = 0;
  virtual void SetCurentCurveAlpha(float a) = 0;
  virtual int GetIdFromPosition(int i) = 0;
  virtual ColorType GetCurveColorByID(int i) = 0;
  virtual IndexType GetCurveIndexByID(int i) = 0;

  // relative to grid step
  virtual void SetXGridStep(float x) = 0;
  virtual void SetYGridStep(float y) = 0;

  // relative to grid origin
  virtual void SetGridOriginX(double x) = 0;
  virtual void SetGridOriginY(double y) = 0;
  virtual void SetGridOriginXTranslate(double x) = 0;
  virtual void SetGridOriginYTranslate(double y) = 0;
  virtual void SetCurveVisibleById(int id, float alpha) = 0;
  virtual void UpdateWidgetView() = 0;
  virtual void UpdateHCIView() = 0;
  virtual void ChangeCurveColor(int id, float r, float g, float b, float a) = 0;
  virtual int IsVisibleById(int id) = 0;
  virtual void SetVisibleById(int id, int v) = 0;

  // relative to handler
  virtual void UpdateCurrentPixelSpectrum(IndexType index) = 0;
  virtual void AddAPixelSpectrum(IndexType index) = 0;
  virtual void RemoveAllSpectrums() = 0;
  virtual void SetYAxisLenghtControl(float f, bool direct) = 0;
  virtual void SetAutomaticGridStep(bool b) = 0;
  virtual void SetGridStepY(double f) = 0;
  virtual void SetGridStepX(double f) = 0;
  virtual double GetGridOriginYTranslate() = 0;
  virtual double GetGridOriginXTranslate() = 0;
  virtual float GetYGridStep() = 0;
  virtual float GetYAxisLenghtControl() = 0;
  virtual void SetViewIndex(std::string s) = 0;

protected:
  PixelSpectrumControllerInterface()
    {
    }
  virtual ~PixelSpectrumControllerInterface(){}

  ModelPointerType m_Model;

private:

  };
}

#endif
