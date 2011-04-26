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
#include "otbPixelSpectrumView.h"

namespace otb
{

PixelSpectrumView
::PixelSpectrumView(): m_Model()
{
  m_Initiated = false;

  m_CurveWidget = WidgetType::New();
  m_Curve = SignatureCurveType::New();
  m_Curve->SetId(PixelSpectrumModel::NoneID);
  m_IdOfSelectedSpectralDisplay = PixelSpectrumModel::NoneID;

  // m_CurveWidget features management
  m_CurveWidget->SetXAxisLabel("Spectrum");
  m_CurveWidget->SetYAxisLabel("Intensity");
  m_CurveWidget->SetAutoScale(false);

  // fit the size of widgets
  int windowSize = 200;
  int sideBorder = 1;
  int thumbNailHeigth = 20 + sideBorder;
  int panelHeigth = windowSize - thumbNailHeigth - (2 * sideBorder);
  int maximumWidth = windowSize - (2 * sideBorder);

  // panel à onglet
  m_PixelSpectrumTab = new PixelSpectrumTab(0, 0, windowSize, windowSize);

  firstPan = new Fl_Group(sideBorder, thumbNailHeigth, maximumWidth, panelHeigth, "Curve list");
  secPan = new Fl_Group(sideBorder, thumbNailHeigth, maximumWidth, panelHeigth, "Index");
  m_IndexBox = new Fl_Output(1, 1, 0, 50);
  m_IndexBox->labelsize(10);
  m_IndexBox->align(FL_ALIGN_RIGHT);

  m_CurveWidget->AddCurve(m_Curve);
  m_CurveWidget->resize(firstPan->x(), firstPan->y(), firstPan->w() * 3, firstPan->h() * 2);
  m_ScrollCurveList = new Fl_Scroll(firstPan->x() + 1, firstPan->y() + 5, firstPan->w() - 1, firstPan->h() - 1);
  m_ScrollSpectralAngleCurveList = new Fl_Scroll(firstPan->x() + 1, firstPan->y() + 5, firstPan->w() - 1,
                                                 firstPan->h() - 1);

  m_ScrollCurveList->color((Fl_Color) 55);
  m_ScrollSpectralAngleCurveList->color((Fl_Color) 55);

  m_ScrollCurveList->type(Fl_Scroll::VERTICAL_ALWAYS);
  m_ScrollSpectralAngleCurveList->type(Fl_Scroll::VERTICAL_ALWAYS);

  m_PixelSpectrumTab->add(firstPan);
  m_PixelSpectrumTab->add(secPan);
}

PixelSpectrumView
::~PixelSpectrumView()
{
  m_SeveralSpectrums.clear();
  m_Labels.clear();
  m_SpectralRadioLabels.clear();
}

void
PixelSpectrumView
::SetScrollList(Fl_Scroll * f)
{
  m_ScrollCurveList = f;
}

void
PixelSpectrumView
::SetModel(ModelType * model)
{
  // Unregister from previous model if nay
  if (m_Model.IsNotNull())
    {
    m_Model->UnRegisterListener(this);
    }

  // Set and register with new model
  m_Model = model;
  m_Model->RegisterListener(this);
}

void
PixelSpectrumView
::SetController(ControllerType* controller)
{
  m_Controller = controller;
}

void
PixelSpectrumView
::Notify()
{
  this->Update();
}

void
PixelSpectrumView
::ScaleTheWidget()
{

  m_Controller->SetCurentCurveAlpha(1);

  // relative to the extrema, for displaying in the graph
  AffineTransformType::Pointer transform = AffineTransformType::New();
  PointType                    orig, lengthP, gridOrig;
  orig[0] = 0;
  orig[1] = 0;

  transform->TransformPoint(orig);

  VectorType lengthV, gridSpace;

  m_Controller->DetermineMinMaxPixelValues();
  m_Controller->SetAxisLengthAtInit(m_Curve->GetMaximum()[0] - 1, m_Model->GetMaxValue());

  lengthP[0] = m_Model->GetAxisLengthX();
  lengthP[1] = m_Model->GetAxisLengthY();

  transform->TransformPoint(lengthP);
  lengthV.Fill(0);
  lengthV[0] = lengthP[0];
  lengthV[1] = lengthP[1];

  m_CurveWidget->SetAxisOrigin(orig);
  m_CurveWidget->SetAxisLength(lengthV);

  gridSpace.Fill(0);
  gridSpace[0] = 1;
  gridSpace[1] = (int) (lengthV[1] / 10);
  m_CurveWidget->SetGridSpacing(gridSpace);

  m_Controller->SetGridOriginX(m_CurveWidget->GetGridOrigin()[0]);
  m_Controller->SetGridOriginY(0);
  gridOrig[0] = m_Model->GetGridOriginX();
  gridOrig[1] = m_Model->GetGridOriginY();
  m_CurveWidget->SetGridOrigin(gridOrig);

  if (m_Model->GetMaxValue() != 0)
    {
    m_Initiated = true;
    m_Controller->SetYGridStep(gridSpace[1]);

    }
}

void
PixelSpectrumView
::ReScaleAxisLength(bool automatic)
{

  AffineTransformType::Pointer transform = AffineTransformType::New();
  PointType                    lengthP, gridOrig;
  VectorType                   lengthV, gridSpace;

  /** AxisLength = Zoom */
  lengthP[0] = m_Curve->GetMaximum()[0] - 1;
  lengthP[1] = m_Model->GetAxisLengthY();
  transform->TransformPoint(lengthP);
  lengthV.Fill(0);
  lengthV[0] = lengthP[0];
  lengthV[1] = lengthP[1];
  m_CurveWidget->SetAxisLength(lengthV);

  /** GridSpacing = Displayed index*/
  gridSpace.Fill(0);

  // about the grid
  gridSpace[0] = m_Curve->GetSpectralCurve().GetSize() > 15 ? m_CurveWidget->GetAxisLength()[0] / 10: 1;

  if (!automatic)
    {
    gridSpace[1] = m_Model->GetGridStepY();
    }
  else
    {
    gridSpace[1] = (int) (m_CurveWidget->GetAxisLength()[1] / 10);
    }

  if (gridSpace[1] == 0) gridSpace[1] = 1;

  m_CurveWidget->SetGridSpacing(gridSpace);

  /** GridOrigin  = translation */
  gridOrig[0] = m_Model->GetGridOriginXTemp();
  gridOrig[1] = m_Model->GetGridOriginYTemp();
  m_CurveWidget->SetGridOrigin(gridOrig);
}

void
PixelSpectrumView
::ChangeCurveColor(unsigned int id, float r, float g, float b, float a)
{
  SignatureCurveType::ColorType tmpColor;
  tmpColor.Fill(0);
  tmpColor[0] = r;
  tmpColor[1] = g;
  tmpColor[2] = b;
  tmpColor[3] = a;

  GetCurveById(id)->SetSpectralCurveColor(tmpColor);

}

void
PixelSpectrumView
::UpdateCurvesList(unsigned int ID)
{
  bool                          click = false;
  int                           sizeModelVector = m_Model->GetSeveralSpectrumData().size();
  int                           sizeViewVector = m_SeveralSpectrums.size();
  int                           cur;
  CurveDescType                 tempDesc;
  SignatureCurveType::ColorType tmpColor;
  tmpColor.Fill(0);

  CurveFromWidgetType::Pointer tempCurve;

  // if number of curves in the model is higher than in the view
  // -> a curve has to be added
  if (sizeModelVector > sizeViewVector)
    {
    m_SeveralSpectrums.push_back(SignatureCurveType::New());
    cur = m_SeveralSpectrums.size() - 1;
    m_SeveralSpectrums.at(cur)->SetSpectralCurve(m_Model->GetSeveralSpectrumData().at(cur).sprectrum);
    m_SeveralSpectrums.at(cur)->SetId(m_Model->GetSeveralSpectrumData().at(cur).id);
    m_SeveralSpectrums.at(cur)->SetCoord(m_Model->GetSeveralSpectrumData().at(cur).coord);
    tmpColor[0] = (float) (rand() % 70) / 100 + 0.3;
    tmpColor[1] = (float) (rand() % 70) / 100 + 0.3;
    tmpColor[2] = (float) (rand() % 70) / 100 + 0.3;
    tmpColor[3] = 0.5;
    m_SeveralSpectrums.at(cur)->SetSpectralCurveColor(tmpColor);
    m_CurveWidget->AddCurve(m_SeveralSpectrums.at(cur));

    // relative to the HCI
    tempDesc.id = m_Model->GetSeveralSpectrumData().at(cur).id;
    std::ostringstream oss;
    oss << "Curve: [ID " << tempDesc.id << "] - coord: (" << m_Model->GetSeveralSpectrumData().at(cur).coord[0] <<
    ", " << m_Model->GetSeveralSpectrumData().at(cur).coord[1] << " )";

    tempDesc.name = oss.str();
    curveDescriptionList.push_back(tempDesc);

    click = true;
    }

  // if all curves are deleted from the model list
  // all have to be deleted from the view list

  if (sizeModelVector == 0 && sizeViewVector > 0 && ID == PixelSpectrumModel::NoneID)
    {
    tempCurve = m_CurveWidget->GetCurveByItsId(PixelSpectrumModel::NoneID);
    m_SeveralSpectrums.clear();
    m_CurveWidget->ClearAllCurves();
    m_CurveWidget->AddCurve(tempCurve);
    click = true;
    }

  if (sizeModelVector < sizeViewVector && ID != PixelSpectrumModel::NoneID)
    {
    m_CurveWidget->RemoveCurveByItsID(ID);
    for (int i = 0; i < sizeViewVector; i++)
      {
      if (m_SeveralSpectrums.at(i)->GetId() == ID)
        {
        m_SeveralSpectrums.erase(m_SeveralSpectrums.begin() + i);

        UpdateWidget();
        click = true;
        break;
        }
      }

    }

  if (click)
    {
    UpdateHCIDisplay();
    }
}

void
PixelSpectrumView
::RemoveCurvesToDisplay(unsigned int i)
{
}

void
PixelSpectrumView
::UpdateHCIDisplay()
{
  int i;
  int numberOfCurves = m_Model->GetSeveralSpectrumData().size();

  m_ScrollCurveList->clear();
  m_ScrollSpectralAngleCurveList->clear();
  m_Labels.clear();
  m_SpectralRadioLabels.clear();

  // adding the "none" with ID=PixelSpectrumModel::NoneID
  m_SpectralRadioLabels.push_back(new SpectralRadioButtonType(m_Controller, m_ScrollCurveList->x() + 1,
                                                              m_ScrollCurveList->y(), m_ScrollCurveList->w() - 20, 40,
                                                              PixelSpectrumModel::NoneID));
  m_SpectralRadioLabels.back()->type(FL_RADIO_BUTTON);
  m_ScrollSpectralAngleCurveList->add(m_SpectralRadioLabels.back());

  for (i = 0; i < numberOfCurves; i++)
    {

    // first scroll: m_ScrollCurveList
    m_Labels.push_back(new CurveLabel(i, m_Controller, m_ScrollCurveList->x() + 1, m_ScrollCurveList->y() + i * 60,
                                      m_ScrollCurveList->w() - 20, 60));
    m_Labels.back()->activate();
    m_ScrollCurveList->add(m_Labels.back());

    // second scroll: m_ScrollSpectralAngleCurveList
    m_SpectralRadioLabels.push_back(new SpectralRadioButtonType(m_Controller, m_ScrollCurveList->x() + 1,
                                                                m_ScrollCurveList->y() + (i + 1) * 40,
                                                                m_ScrollCurveList->w() - 20, 40,
                                                                m_Controller->GetIdFromPosition(i)));
    m_SpectralRadioLabels.back()->activate();

    if (m_SpectralRadioLabels.back()->GetId() == m_IdOfSelectedSpectralDisplay)
      {
      m_SpectralRadioLabels.back()->value(1);
      m_SpectralRadioLabels.at(0)->value(0);
      }

    m_SpectralRadioLabels.back()->type(FL_RADIO_BUTTON);
    m_ScrollSpectralAngleCurveList->add(m_SpectralRadioLabels.back());

    }

  i++;
  m_ScrollCurveList->add(new Fl_Box(m_ScrollCurveList->x() + 1, m_ScrollCurveList->y() + i * 50,
                                    m_ScrollCurveList->w() - 2, 0, ""));

  if (numberOfCurves == 0)
    {
    curveDescriptionList.clear();
    }

  m_ScrollCurveList->redraw();
  m_ScrollSpectralAngleCurveList->redraw();

  Fl::check();

}

PixelSpectrumView::SignatureCurvePointerType
PixelSpectrumView
::GetCurveById(unsigned int i)
{
  for (unsigned int j = 0; j < m_SeveralSpectrums.size(); j++)
    {
    if (m_SeveralSpectrums.at(j)->GetId() == i)
      {
      return m_SeveralSpectrums.at(j);
      }
    }
  itkExceptionMacro(<< "No curve found with id " << i)
}

void
PixelSpectrumView
::SetCurveVisibleById(unsigned int id, float alpha)
{
  GetCurveById(id)->SetTransparency(alpha);
}

void
PixelSpectrumView
::Update()
{
  if (!m_Initiated) ScaleTheWidget();

  m_Curve->SetSpectralCurve(m_Model->GetCurrentPixelSpectrum());
  m_Curve->SetTransparency(m_Model->GetCurentCurveAlpha());

  UpdateCurvesList(PixelSpectrumModel::NoneID);

  ReScaleAxisLength(m_Model->GetAutomaticGridStep()),
  FindWhoIsSelected();
  UpdateWidget();
}

void
PixelSpectrumView
::FindWhoIsSelected()
{
  for (unsigned int i = 0; i < m_SpectralRadioLabels.size(); i++)
    {
    if (m_SpectralRadioLabels.at(i)->value() == 1) m_IdOfSelectedSpectralDisplay = m_SpectralRadioLabels.at(i)->GetId();
    }
}

void
PixelSpectrumView
::UpdateWidget()
{
  m_CurveWidget->show();
  m_CurveWidget->redraw();
}

}
