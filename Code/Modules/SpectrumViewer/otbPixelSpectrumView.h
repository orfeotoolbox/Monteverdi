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
#ifndef __otbPixelSpectrumView_h
#define __otbPixelSpectrumView_h

#include "itkObject.h"

#include "otbImageWidget.h"
#include "otbImageLayerRenderingModelListener.h"
#include "otbImageWidgetController.h"
#include "otbSpectralSignatureCurve.h"
#include "otbCurves2DWidget.h"
#include "otbPixelSpectrumModel.h"
#include "otbPixelSpectrumControllerInterface.h"
#include "otbCurveLabel.h"
#include "otbCurveBarWidget.h"

//#include "otbCurveBarWidget.h"

#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>

#include "otbCurveBarWidget.h"

#include "otbRadioButtonId.h"

#include <vector>
#include <time.h>
#include <iostream>
#include <sstream>

namespace otb
{

/** \class PixelSpectrumView
*   \brief todo
*  \ingroup Visualization
 */

class PixelSpectrumView
: public PixelSpectrumModelListener, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef PixelSpectrumView             Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(PixelSpectrumView, itk::Object);

  /** Model typedef */
  typedef otb::PixelSpectrumModel ModelType;
  typedef  ModelType::Pointer     ModelPointerType;

  /** Controller typedef */
  typedef otb::PixelSpectrumControllerInterface ControllerType;
  typedef ControllerType::Pointer               ControllerPointerType;

  /** Pixel Spectrum widget typedef */
  typedef Fl_Tabs PixelSpectrumTab;

  // Curve Widget
  typedef float                 InternalPixelType;
  typedef otb::Curves2DWidget   WidgetType;
  typedef WidgetType::CurveType CurveFromWidgetType;

  typedef otb::VectorImage<InternalPixelType, 2>             VectorImageType;
  typedef VectorImageType::PixelType                         SpectralSignatureType;
  typedef otb::SpectralSignatureCurve<SpectralSignatureType> SignatureCurveType;
  typedef SignatureCurveType::Pointer                        SignatureCurvePointerType;

  typedef WidgetType::PointType           PointType;
  typedef WidgetType::VectorType          VectorType;
  typedef WidgetType::AffineTransformType AffineTransformType;

  typedef itk::VariableLengthVector<SignatureCurveType> SeveralSpectrumsType;
  typedef otb::CurveLabel                               CurveLabel;

  // test for display

  typedef struct {
    std::string name;
    int id;
  }
  CurveDescType;

  typedef otb::RadioButtonId SpectralRadioButtonType;


  /**
  * This method unregister with previous model if any, and
  * register with the new one.
  */
  void SetModel(ModelType * model);

  void SetController(ControllerType* controller);

  /** GET the curve widget */
  itkGetObjectMacro(CurveWidget, WidgetType);

  Fl_Output* GetIndexBox()
  {
    return m_IndexBox;
  }

  /**
  * Manually trigger a refresh
  */
  void Update();

  /**
   * Fill the vector of spectrums (m_SeveralSpectrums) from the model data
   */
  void UpdateCurvesList(unsigned int id);
  void RemoveCurvesToDisplay(unsigned int i);
  void UpdateHCIDisplay();

  /**
   * Init, triggered at the first Update
  */
  void ScaleTheWidget();
  void ReScaleAxisLength(bool automatic);

  Fl_Scroll * GetScrollCurveList()
  {
    return m_ScrollCurveList;
  }

  Fl_Scroll * GetScrollSpectralAngleCurveList()
  {
    return m_ScrollSpectralAngleCurveList;
  }

  unsigned int GetIdOfSelectedSpectralDisplay()
  {
    return m_IdOfSelectedSpectralDisplay;
  }

  void SetIdOfSelectedSpectralDisplay(unsigned int n)
  {
    m_IdOfSelectedSpectralDisplay = n;
  }

  /** get a curve by it id */
  SignatureCurvePointerType GetCurveById(unsigned int i);

  /** Set a curve visible or not , by its ID */
  void SetCurveVisibleById(unsigned int id, float alpha);

  void SetScrollList(Fl_Scroll * f);

  /** Get the pixel Spectrum Tab */
  PixelSpectrumTab * GetPixelSpectrumWidget()
  {
    return m_PixelSpectrumTab;
  }

  /** update widget display */
  void UpdateWidget();

  /** Find which radioSpectralButton is seleted */
  void FindWhoIsSelected();

  /** change curve color by Id      color: [0;1] */
  void ChangeCurveColor(unsigned int id, float r, float g, float b, float a);

  void ZoomRouletteCallBack();

  std::vector<SpectralRadioButtonType*> GetSpectralRadioLabels()
  {
    return m_SpectralRadioLabels;
  }

protected:
  /** Constructor */
  PixelSpectrumView();
  /** Destructor */
  virtual ~PixelSpectrumView();

  /** Handle notification from the viewer */
  virtual void Notify();

private:
  PixelSpectrumView(const Self &);            // purposely not implemented
  void                          operator = (const Self &); // purposely not implemented

  /** Relative to the basic HCI */
  Fl_Group *  firstPan;
  Fl_Group *  secPan;
  Fl_Scroll * m_ScrollCurveList;
  Fl_Scroll * m_ScrollSpectralAngleCurveList;
  Fl_Output * m_IndexBox;

  /** The Spectrum widget */
  PixelSpectrumTab *         m_PixelSpectrumTab;
  SignatureCurvePointerType  m_Curve;
  WidgetType::Pointer        m_CurveWidget;
  std::vector<CurveDescType> curveDescriptionList;

  std::vector<SignatureCurvePointerType> m_SeveralSpectrums;
  std::vector<CurveLabel*>               m_Labels;
  std::vector<SpectralRadioButtonType*>  m_SpectralRadioLabels;
  int                                    m_IdOfSelectedSpectralDisplay;

  bool m_Initiated;

  /** Model pointer */
  ModelPointerType      m_Model;
  ControllerPointerType m_Controller;

  int m_MaximumNumberOfBand;
  int m_MaximumAmplitude;



}; // end class
} // end namespace otb

#endif
