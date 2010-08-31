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
#ifndef __otbPixelSpectrumController_h
#define __otbPixelSpectrumController_h

#include "otbPixelSpectrumControllerInterface.h"
#include "otbPixelSpectrumView.h"

namespace otb {

class PixelSpectrumController
: public PixelSpectrumControllerInterface
{
public:

  /** Standard class typedefs */
  typedef PixelSpectrumController          Self;
  typedef PixelSpectrumControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef otb::PixelSpectrumView              ViewType;
  typedef ViewType::Pointer                   ViewPointerType;
  typedef ViewType::SignatureCurvePointerType SignatureCurvePointerType;
  typedef ViewType::VectorType                VectorType;

  /** Standard type macros */
  itkTypeMacro(PixelSpectrumController, Superclass);
  itkNewMacro(Self);

  itkSetObjectMacro(View, ViewType);
  itkGetObjectMacro(View, ViewType);

  void RemoveAPixelSpectrumById(int i);
  void AddAPixelSpectrum(IndexType index);
  void SetAxisLengthAtInit(double x, double y);
  void DetermineMinMaxPixelValues();
  void SetCurentCurveAlpha(float a);

  // relative to grid Step
  void SetXGridStep(float x);
  void SetYGridStep(float y);

  // relative to grid origin
  void SetGridOriginX(double x);
  void SetGridOriginY(double y);
  void SetGridOriginXTranslate(double x);
  void SetGridOriginYTranslate(double y);

  int GetIdFromPosition(int i);

  void SetCurveVisibleById(int id, float alpha);

  void UpdateWidgetView();
  void UpdateHCIView();

  void ChangeCurveColor(int id, float r, float g, float b, float a);

  ColorType GetCurveColorByID(int i);
  IndexType GetCurveIndexByID(int i);

  int IsVisibleById(int id);
  void SetVisibleById(int id, int v);

  void UpdateCurrentPixelSpectrum(IndexType index);
  void RemoveAllSpectrums();
  void SetYAxisLenghtControl(float f, bool direct);
  void SetAutomaticGridStep(bool b);
  void SetGridStepY(double f);
  void SetGridStepX(double f);
  double GetGridOriginYTranslate();
  double GetGridOriginXTranslate();
  float GetYGridStep();
  float GetYAxisLenghtControl();
  void SetViewIndex(std::string s);

protected:
  PixelSpectrumController()  {}
  ~PixelSpectrumController() {}

private:
  ViewPointerType m_View;
};
}

#endif
