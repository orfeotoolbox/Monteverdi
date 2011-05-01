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
#ifndef __otbInteractiveChangeDetection_txx
#define __otbInteractiveChangeDetection_txx

#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Text_Buffer.H>
#include "otbInteractiveChangeDetection.h"
#include "otbMacro.h"
#include <iostream>
#include "itkImageRegionIteratorWithIndex.h"
#include "otbStreamingImageFileWriter.h"
#include "otbMsgReporter.h"

// Overlay result display
#include "itkCastImageFilter.h"

namespace otb
{
/// Constructor
template <class TPixel>
InteractiveChangeDetection<TPixel>
::InteractiveChangeDetection()
{
  m_FormList = FormListType::New();
  m_Estimator = EstimatorType::New();
  m_Label1 = 1;
  m_Label2 = 2;

  m_Class1Color[0] = 1.0;
  m_Class1Color[1] = 0;
  m_Class1Color[2] = 0;
  m_Class2Color[0] = 0;
  m_Class2Color[1] = 0;
  m_Class2Color[2] = 1.;

  m_Radius = 2;

  m_ImageList = SingleImageListType::New();

  m_RightViewer = ImageViewerType::New();
  m_LeftViewer = ImageViewerType::New();
  m_CenterViewer = ImageViewerType::New();

  m_LeftImage = ImageType::New();
  m_RightImage = ImageType::New();

  m_AmplitudeFilter1 = AmplitudeFilterType::New();
  m_AmplitudeFilter2 = AmplitudeFilterType::New();
  m_ImageToListFilter1 = ImageToListFilterType::New();
  m_ImageToListFilter2 = ImageToListFilterType::New();
  m_MeanDifferenceFilter = MeanDifferenceFilterType::New();
  m_MeanRatioFilter = MeanRatioFilterType::New();
  m_ListToImageFilter1 = ListToImageFilterType::New();
  m_ClassificationFilter = ClassificationFilterType::New();
  m_AbsFilter            = AbsFilterType::New();
  m_ChangeLabelFilter    = ChangeLabelFilterType::New();
  m_Estimator = EstimatorType::New();
  m_CastFilter = CastFilterType::New();

  m_HasOutput = false;

  m_LastPath = ".";
}

/// Destructor
template <class TPixel>
InteractiveChangeDetection<TPixel>
::~InteractiveChangeDetection()
{
}
/// Build the HMI
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::Build(void)
{
  SizeType  size;
  IndexType index;
  size.Fill(10);
  index.Fill(0);
  RegionType region;
  region.SetSize(size);
  region.SetIndex(index);
  PixelType blackPixel;
  blackPixel.SetSize(3);
  blackPixel.Fill(0);

  CreateGUI();

  this->svmSVMType->value(m_Estimator->GetSVMType());
  this->svmKernelType->value(m_Estimator->GetKernelType());
  this->svmKernelDegree->value(m_Estimator->GetPolynomialKernelDegree());
  this->svmGamma->value(m_Estimator->GetKernelGamma());
  this->svmNu->value(m_Estimator->GetNu());
  this->svmCoef0->value(m_Estimator->GetKernelCoef0());
  this->svmC->value(m_Estimator->GetC());
  this->svmEpsilon->value(m_Estimator->GetEpsilon());
  this->svmP->value(m_Estimator->GetP());
  this->svmCacheSize->value(m_Estimator->GetCacheSize());
  this->svmPE->value(m_Estimator->GetDoProbabilityEstimates());
  this->svmShrinking->value(m_Estimator->GetDoShrinking());
  this->iviewWindowPrincipal->label("Interactive Change Detection.");
  Fl_Text_Buffer * buffer = new Fl_Text_Buffer();
  this->bOutputLogs->buffer(buffer);

  m_LeftViewer->SetPixLocOutput(oPixLocValue);
  m_RightViewer->SetPixLocOutput(oPixLocValue);
  m_CenterViewer->SetPixLocOutput(oPixLocValue);

  m_LeftViewer->SetShowZoomWidget(false);
  m_RightViewer->SetShowZoomWidget(false);
  m_CenterViewer->SetShowZoomWidget(false);

  m_InterfaceColor[0] = 0;
  m_InterfaceColor[1] = 1;
  m_InterfaceColor[2] = 0;
  m_InterfaceColor[3] = 1.;

  ColorType ROIDefaultColor;
  ROIDefaultColor[0] = 0;
  ROIDefaultColor[1] = 0;
  ROIDefaultColor[2] = 0;
  ROIDefaultColor[3] = 0;

  m_Class1Color[3] = (float) this->slTrainingSetOpacity->value();
  m_Class2Color[3] = (float) this->slTrainingSetOpacity->value();

  m_LeftViewer->SetInterfaceBoxesColor(m_InterfaceColor);
  m_LeftViewer->SetDefaultROIColor(ROIDefaultColor);
  m_LeftViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
  m_LeftViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
  m_LeftViewer->SetScrollMaxInitialSize(iViewScrollLeftWindow->w());
  m_LeftViewer->SetLabel("Before image");

  m_RightViewer->SetInterfaceBoxesColor(m_InterfaceColor);
  m_RightViewer->SetDefaultROIColor(ROIDefaultColor);
  m_RightViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
  m_RightViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
  m_RightViewer->SetScrollMaxInitialSize(iViewScrollRightWindow->w());
  m_RightViewer->SetLabel("After image");

  m_CenterViewer->SetInterfaceBoxesColor(m_InterfaceColor);
  m_CenterViewer->SetDefaultROIColor(ROIDefaultColor);
  m_CenterViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
  m_CenterViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
  m_CenterViewer->SetScrollMaxInitialSize(iViewScrollCenterWindow->w());
  m_CenterViewer->SetLabel("Center image");

  m_LeftViewer->SetPolygonalROISelectionMode(true);
  m_RightViewer->SetPolygonalROISelectionMode(true);
  m_CenterViewer->SetPolygonalROISelectionMode(true);

  m_RightViewer->SetPolygonROIList(m_LeftViewer->GetPolygonROIList());
  m_CenterViewer->SetPolygonROIList(m_LeftViewer->GetPolygonROIList());

  m_LeftViewer->SetNextROILabel(1);
  m_RightViewer->SetNextROILabel(1);
  m_CenterViewer->SetNextROILabel(1);

  bC2->clear();
  bC1->set();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::PolygonalROIToggled()
{
  typename PolygonType::Pointer pol = PolygonType::New();
  pol->SetValue(m_LeftViewer->GetNextROILabel());
  m_LeftViewer->GetPolygonROIList()->PushBack(pol);

  if (bPolygonalROI->value())
    {
    m_LeftViewer->SetPolygonalROISelectionMode(true);
    m_RightViewer->SetPolygonalROISelectionMode(true);
    m_CenterViewer->SetPolygonalROISelectionMode(true);
    m_LeftViewer->SetRectangularROISelectionMode(false);
    m_RightViewer->SetRectangularROISelectionMode(false);
    m_CenterViewer->SetRectangularROISelectionMode(false);
    Log("Switching to polygonal ROI selection mode.");
    bEndPolygon->activate();
    bEraseLastPoint->activate();

    }
  else
    {
    m_LeftViewer->SetPolygonalROISelectionMode(false);
    m_RightViewer->SetPolygonalROISelectionMode(false);
    m_CenterViewer->SetPolygonalROISelectionMode(false);
    m_LeftViewer->SetRectangularROISelectionMode(true);
    m_RightViewer->SetRectangularROISelectionMode(true);
    m_CenterViewer->SetRectangularROISelectionMode(true);
    Log("Switching to rectangular ROI selection mode.");
    bEndPolygon->deactivate();
    bEraseLastPoint->deactivate();
    }

}
/// What to do on principal action
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::PrincipalAction(const IndexType&)
{
  Update();
}
/// Show the app
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::Show(void)
{
  Fl::check();
  this->iviewWindowPrincipal->show();
  Fl::check();
}

/// Show the app
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::SetFiltersInputs(void)
{
  m_AmplitudeFilter1->SetInput(m_LeftImage);
  m_AmplitudeFilter2->SetInput(m_RightImage);
  m_ImageToListFilter1->SetInput(m_LeftImage);
  m_ImageToListFilter2->SetInput(m_RightImage);
  m_MeanRatioFilter->SetInput1(m_AmplitudeFilter1->GetOutput());
  m_MeanRatioFilter->SetInput2(m_AmplitudeFilter2->GetOutput());
  m_MeanDifferenceFilter->SetInput1(m_AmplitudeFilter1->GetOutput());
  m_MeanDifferenceFilter->SetInput2(m_AmplitudeFilter2->GetOutput());
  m_AbsFilter->SetInput(m_MeanDifferenceFilter->GetOutput());

}

/// Update the display
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::Update(void)
{
  Fl::check();
  if (m_LeftViewer->GetBuilt())
    {
    m_LeftViewer->Update();
    }
  if (m_RightViewer->GetBuilt())
    {
    m_RightViewer->Update();
    }
  if (bDisplayResults->value())
    {
    if (m_ResultViewer->GetBuilt())
      {
      m_ResultViewer->Update();
      }
    }
  else
    {
    if (m_CenterViewer->GetBuilt())
      {
      m_CenterViewer->Update();
      }
    }

  Fl::check();
}

// Gestion des �v�nements boutons
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::Quit()
{
  this->Hide();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::Log(const char * log)
{
  this->bOutputLogs->insert(log);
  this->bOutputLogs->insert("\n");
  this->bOutputLogs->show_insert_position();
  Fl::check();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::SaveSVMModel()
{
  const char* filename = fl_file_chooser("SVM model file:", "*.svm", m_LastPath.c_str());
  if (filename == NULL || strlen(filename) < 1)
    {
    return;
    }
  Log("Saving SVM model.");
  m_Estimator->GetModel()->SaveModel(filename);

  m_LastPath = itksys::SystemTools::GetFilenamePath(filename);
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::OpenSVMModel()
{
  const char* filename = fl_file_chooser("SVM model file:", "*.svm", m_LastPath.c_str());
  if (filename == NULL || strlen(filename) < 1)
    {
    return;
    }
  m_ModelFileName = std::string(filename);
  m_LastPath = itksys::SystemTools::GetFilenamePath(m_ModelFileName.c_str());
  this->LoadSVMModel();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::LoadSVMModel()
{

  this->ReInitButtons();
  typename ModelType::Pointer model = ModelType::New();
  model->LoadModel(m_ModelFileName.c_str());
  m_ClassificationFilter->SetModel(model);
  this->bLearn->value(1);
  this->miSaveModel->activate();
  this->bDisplayResults->activate();

  Log("SVM Model loaded");
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::SaveResultImage()
{
  // Pipeline wiring (correction bug #98)
  m_ClassificationFilter->SetInput(m_ListToImageFilter1->GetOutput());

  typename OverlayImageType::PixelType color1(3), color2(3);

  color1[0] = static_cast<unsigned char>(m_Class1Color[0] * 255);
  color1[1] = static_cast<unsigned char>(m_Class1Color[1] * 255);
  color1[2] = static_cast<unsigned char>(m_Class1Color[2] * 255);

  color2[0] = static_cast<unsigned char>(m_Class2Color[0] * 255);
  color2[1] = static_cast<unsigned char>(m_Class2Color[1] * 255);
  color2[2] = static_cast<unsigned char>(m_Class2Color[2] * 255);

  m_ChangeLabelFilter->SetChange(m_Label1, color1);
  m_ChangeLabelFilter->SetChange(m_Label2, color2);
  m_ChangeLabelFilter->SetInput(m_ClassificationFilter->GetOutput());
  m_ChangeLabelFilter->SetNumberOfComponentsPerPixel(3);
  m_Output = m_ChangeLabelFilter->GetOutput();

  m_HasOutput = true;
  this->NotifyAll();

  //hide the gui
  this->Hide();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::ImportVectorData()
{
  const char* filename = fl_file_chooser("Vector data file:", "*.shp\t*.kml", m_LastPath.c_str());
  if (filename == NULL || strlen(filename) < 1)
    {
    return;
    }

  VectorDataFileReaderPointerType reader = VectorDataFileReaderType::New();
  reader->SetFileName(filename);
  reader->Update();

  TreeIteratorType it(reader->GetOutput()->GetDataTree());

  it.GoToBegin();

  while (!it.IsAtEnd())
    {
    if (it.Get()->IsPolygonFeature())
      {
      m_LeftViewer->GetPolygonROIList()->PushBack(it.Get()->GetPolygonExteriorRing());
      m_LeftViewer->GetPolygonROIList()->Back()->SetValue(m_LeftViewer->GetNextROILabel());
      }
    ++it;
    }
  m_LeftViewer->GetPolygonROIList()->PushBack(PolygonType::New());
  m_LeftViewer->GetPolygonROIList()->Back()->SetValue(m_LeftViewer->GetNextROILabel());

  try
    {
    this->Update();
    Log("Polygons imported from vector file.");
    }
  catch (itk::ExceptionObject & err)
    {

    Log("Error while reading data file.");
    Log(err.GetDescription());
    }

  m_LastPath = itksys::SystemTools::GetFilenamePath(filename);
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::ExportVectorData()
{
  const char* filename = fl_file_chooser("Vector data file:", "*.shp", m_LastPath.c_str());
  if (filename == NULL || strlen(filename) < 1)
    {
    return;
    }

  VectorDataPointerType vectorData = VectorDataType::New();

  DataNodePointerType document = DataNodeType::New();
  document->SetNodeType(DOCUMENT);
  document->SetNodeId("polygon");
  DataNodePointerType folder = DataNodeType::New();
  folder->SetNodeType(FOLDER);
  DataNodePointerType multiPolygon = DataNodeType::New();
  multiPolygon->SetNodeType(FEATURE_MULTIPOLYGON);

  DataTreePointerType tree = vectorData->GetDataTree();
  DataNodePointerType root = tree->GetRoot()->Get();

  tree->Add(document, root);
  tree->Add(folder, document);
  tree->Add(multiPolygon, folder);

  typename PolygonListType::Pointer list = m_LeftViewer->GetPolygonROIList();

  for (typename PolygonListType::Iterator it = list->Begin(); it != list->End(); ++it)
    {
    if (it.Get()->GetValue() == m_LeftViewer->GetNextROILabel())
      {
      DataNodePointerType newPolygon = DataNodeType::New();
      newPolygon->SetPolygonExteriorRing(it.Get());
      tree->Add(newPolygon, multiPolygon);
      }
    }

  VectorDataFileWriterPointerType writer = VectorDataFileWriterType::New();
  writer->SetInput(vectorData);
  writer->SetFileName(filename);

  try
    {
    writer->Update();
    Log("Vector data file written");
    }
  catch (itk::ExceptionObject & err)
    {
    Log("Error while writing data file.");
    Log(err.GetDescription());
    }

  m_LastPath = itksys::SystemTools::GetFilenamePath(filename);
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::EraseAllClickedPoints()
{
  m_LeftViewer->GetPolygonROIList()->Clear();
  this->Update();
  this->ReInitButtons();
  Log("All regions erased.");
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::EraseLastPoint()
{
  if (m_LeftViewer->GetPolygonROIList()->Size() > 0)
    {
    unsigned int sizeOfThePolygon = m_LeftViewer->GetPolygonROIList()->Back()->GetVertexList()->Size();
    if (sizeOfThePolygon > 0)
      {
      typedef typename PolygonType::VertexListType VertexListType;
      typedef typename VertexListType::Pointer     VertexListPointerType;

      // itk::PolylineParametricPath does not provide a RemoveVertex() method, and the access to the vertex list is const, so we have no other choice to remove a vertex.
      VertexListPointerType list = const_cast<VertexListType *>(
        m_LeftViewer->GetPolygonROIList()->Back()->GetVertexList());
      list->pop_back();
      }
    }
  this->Update();
  this->ReInitButtons();
  Log("Last polygon point erased.");
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::EraseLastPolygon()
{
  if (m_LeftViewer->GetPolygonROIList()->Size() > 0)
    {
    m_LeftViewer->GetPolygonROIList()->Erase(m_LeftViewer->GetPolygonROIList()->Size() - 1);
    }
  this->Update();
  this->ReInitButtons();
  Log("Last polygon erased.");
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::EndPolygon()
{
  m_LeftViewer->GetPolygonROIList()->PushBack(PolygonType::New());
  m_LeftViewer->GetPolygonROIList()->Back()->SetValue(m_LeftViewer->GetNextROILabel());
  this->Update();
  this->ReInitButtons();
  Log("Polygon ended.");
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::UseChangeDetectorsToggled()
{
  this->ReInitButtons();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::ReInitButtons()
{
  this->miSaveModel->deactivate();
  this->bLearn->clear();
  this->bDisplayResults->deactivate();
  this->bDisplayResults->clear();
  m_Estimator = EstimatorType::New();
  m_Estimator->SetSVMType(static_cast<int>(this->svmSVMType->value()));
  m_Estimator->SetKernelType(static_cast<int>(this->svmKernelType->value()));
  m_Estimator->SetPolynomialKernelDegree(static_cast<int>(this->svmKernelDegree->value()));
  m_Estimator->SetKernelGamma(static_cast<double>(this->svmGamma->value()));
  m_Estimator->SetNu(static_cast<double>(this->svmNu->value()));
  m_Estimator->SetKernelCoef0(static_cast<double>(this->svmCoef0->value()));
  m_Estimator->SetC(static_cast<double>(this->svmC->value()));
  m_Estimator->SetEpsilon(static_cast<double>(this->svmEpsilon->value()));
  m_Estimator->SetP(static_cast<double>(this->svmP->value()));
  m_Estimator->SetCacheSize(static_cast<int>(this->svmCacheSize->value()));
  m_Estimator->DoProbabilityEstimates(static_cast<bool>(this->svmPE->value()));
  m_Estimator->DoShrinking(static_cast<bool>(this->svmShrinking->value()));
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::ChangeDetectionLearn()
{
  this->bLearn->clear();

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  typename ListSampleType::Pointer         sampleList = ListSampleType::New();
  typename TrainingListSampleType::Pointer trainingSampleList = TrainingListSampleType::New();
  typename PolygonListType::Pointer        polygonList = m_LeftViewer->GetPolygonROIList();
  ImagePointerType                         image = m_ListToImageFilter1->GetOutput();
  unsigned int                             sampleSize = image->GetNumberOfComponentsPerPixel();

  for (typename PolygonListType::Iterator polygIt = polygonList->Begin();
       polygIt != polygonList->End(); ++polygIt)
    {
    image->SetRequestedRegion(polygIt.Get()->GetBoundingRegion().GetImageRegion());
    image->PropagateRequestedRegion();
    image->UpdateOutputData();

    IteratorType imageIt(image, polygIt.Get()->GetBoundingRegion().GetImageRegion());
    imageIt.GoToBegin();

    while (!imageIt.IsAtEnd())
      {
      typename PolygonType::VertexType point;
      point[0] = imageIt.GetIndex()[0];
      point[1] = imageIt.GetIndex()[1];

      if (polygIt.Get()->IsInside(point))
        {

        SampleType newSample(sampleSize);
        TrainingSampleType newTrainingSample;

        newSample.Fill(0);

        for (unsigned int i = 0; i < sampleSize; ++i)
          {
          newSample[i] = imageIt.Get()[i];
          }

        newTrainingSample[0] = polygIt.Get()->GetValue();

        sampleList->PushBack(newSample);
        trainingSampleList->PushBack(newTrainingSample);
        }
      ++imageIt;
      }
    }

  try
    {

    m_Estimator->SetInputSampleList(sampleList);
    m_Estimator->SetTrainingSampleList(trainingSampleList);
    m_Estimator->Update();
    m_ClassificationFilter->SetModel(m_Estimator->GetModel());

    //
    Log("Estimation completed.");
    this->bLearn->set();
    this->miSaveModel->activate();
    this->bDisplayResults->activate();
    }
  catch(itk::ExceptionObject & err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::AcquireFirstClass()
{
  bC2->clear();
  bC1->set();
  m_LeftViewer->SetNextROILabel(m_Label1);
  m_RightViewer->SetNextROILabel(m_Label1);
  typename PolygonType::Pointer pol = PolygonType::New();
  pol->SetValue(m_LeftViewer->GetNextROILabel());
  m_LeftViewer->GetPolygonROIList()->PushBack(pol);
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::ChangeClass1Color()
{
  double r = (double) m_Class1Color[0];
  double g = (double) m_Class1Color[1];
  double b = (double) m_Class1Color[2];

  int ok = fl_color_chooser("Changed class color:", r, g, b);

  if (ok)
    {
    m_Class1Color[0] = (float) r;
    m_Class1Color[1] = (float) g;
    m_Class1Color[2] = (float) b;
    m_Class1Color[3] = (float) this->slTrainingSetOpacity->value();
    m_LeftViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
    m_RightViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
    m_CenterViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
    this->Update();
    }
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::AcquireSecondClass()
{
  bC1->clear();
  bC2->set();
  m_LeftViewer->SetNextROILabel(m_Label2);
  m_RightViewer->SetNextROILabel(m_Label2);
  typename PolygonType::Pointer pol = PolygonType::New();
  pol->SetValue(m_LeftViewer->GetNextROILabel());
  m_LeftViewer->GetPolygonROIList()->PushBack(pol);
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::ChangeClass2Color()
{
  double r = (double) m_Class2Color[0];
  double g = (double) m_Class2Color[1];
  double b = (double) m_Class2Color[2];

  int ok = fl_color_chooser("Changed class color:", r, g, b);

  if (ok)
    {
    m_Class2Color[0] = (float) r;
    m_Class2Color[1] = (float) g;
    m_Class2Color[2] = (float) b;
    m_Class2Color[3] = (float) this->slTrainingSetOpacity->value();
    m_LeftViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
    m_RightViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
    m_CenterViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
    this->Update();
    }
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::ChangeTrainingSetOpacity()
{
  m_Class1Color[3] = (float) this->slTrainingSetOpacity->value();
  m_Class2Color[3] = (float) this->slTrainingSetOpacity->value();
  m_LeftViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
  m_LeftViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
  m_RightViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
  m_RightViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
  m_CenterViewer->AddROIColorMapEntry(m_Label2, m_Class2Color);
  m_CenterViewer->AddROIColorMapEntry(m_Label1, m_Class1Color);
  if (bDisplayResults->value())
    {
    m_ResultViewer->SetImageOverlayOpacity(static_cast<unsigned char>(this->slTrainingSetOpacity->value() * 255));
    }
  this->Update();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::Setup()
{
  if (this->iViewSetup->visible())
    {
    this->iViewSetup->hide();
    }
  else
    {
    this->svmSVMType->value(m_Estimator->GetSVMType());
    this->svmKernelType->value(m_Estimator->GetKernelType());
    this->svmKernelDegree->value(m_Estimator->GetPolynomialKernelDegree());
    this->svmGamma->value(m_Estimator->GetKernelGamma());
    this->svmNu->value(m_Estimator->GetNu());
    this->svmCoef0->value(m_Estimator->GetKernelCoef0());
    this->svmC->value(m_Estimator->GetC());
    this->svmEpsilon->value(m_Estimator->GetEpsilon());
    this->svmP->value(m_Estimator->GetP());
    this->svmCacheSize->value(m_Estimator->GetCacheSize());
    this->svmPE->value(m_Estimator->GetDoProbabilityEstimates());
    this->svmShrinking->value(m_Estimator->GetDoShrinking());
    this->iViewSetup->show();

    this->iViewSetup->redraw();
    HistogramTabSetup();
    }
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::HistogramTabSetup()
{
  if (this->tHistogram->visible())
    {
    if (m_LeftViewer->GetBuilt())
      {
      m_LeftViewer->GetRedHistogramWidget()->show();
      if (m_LeftViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
        {
        m_LeftViewer->GetGreenHistogramWidget()->show();
        m_LeftViewer->GetBlueHistogramWidget()->show();
        }
      }
    if (m_RightViewer->GetBuilt())
      {
      m_RightViewer->GetRedHistogramWidget()->show();
      if (m_RightViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
        {
        m_RightViewer->GetGreenHistogramWidget()->show();
        m_RightViewer->GetBlueHistogramWidget()->show();
        }
      }
    if (m_CenterViewer->GetBuilt())
      {
      m_CenterViewer->GetRedHistogramWidget()->show();
      if (m_CenterViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
        {
        m_CenterViewer->GetGreenHistogramWidget()->show();
        m_CenterViewer->GetBlueHistogramWidget()->show();
        }
      }
    }
  else
    {
    if (m_LeftViewer->GetBuilt())
      {
      m_LeftViewer->GetRedHistogramWidget()->hide();
      if (m_LeftViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
        {
        m_LeftViewer->GetGreenHistogramWidget()->hide();
        m_LeftViewer->GetBlueHistogramWidget()->hide();
        }
      }
    if (m_RightViewer->GetBuilt())
      {
      m_RightViewer->GetRedHistogramWidget()->hide();
      if (m_RightViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
        {
        m_RightViewer->GetGreenHistogramWidget()->hide();
        m_RightViewer->GetBlueHistogramWidget()->hide();
        }
      }
    if (m_CenterViewer->GetBuilt())
      {
      m_CenterViewer->GetRedHistogramWidget()->hide();
      if (m_CenterViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
        {
        m_CenterViewer->GetGreenHistogramWidget()->hide();
        m_CenterViewer->GetBlueHistogramWidget()->hide();
        }
      }
    }
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::LeftViewerColorCompositionCallback()
{
  if (bLeftViewerColorRGB->value())
    {
    m_LeftViewer->SetRedChannelIndex(bLeftViewerRedChannel->value());
    m_LeftViewer->SetGreenChannelIndex(bLeftViewerGreenChannel->value());
    m_LeftViewer->SetBlueChannelIndex(bLeftViewerBlueChannel->value());
    m_LeftViewer->GetRedHistogramWidget()->resize(aLeftViewerHistogram->x(),
                                                  aLeftViewerHistogram->y(),
                                                  aLeftViewerHistogram->w() / 3, aLeftViewerHistogram->h());
    m_LeftViewer->SetViewModel(ImageViewerType::ScrollWidgetType::RGB);
    }
  else
    {
    m_LeftViewer->SetRedChannelIndex(bLeftViewerGrayscaleChannel->value());
    m_LeftViewer->GetRedHistogramWidget()->resize(aLeftViewerHistogram->x(),
                                                  aLeftViewerHistogram->y(),
                                                  aLeftViewerHistogram->w(), aLeftViewerHistogram->h());
    m_LeftViewer->SetViewModel(ImageViewerType::ScrollWidgetType::GRAYSCALE);
    }

  m_LeftViewer->Reset();
  m_LeftViewer->Update();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::RightViewerColorCompositionCallback()
{
  if (bRightViewerColorRGB->value())
    {
    m_RightViewer->SetRedChannelIndex(bRightViewerRedChannel->value());
    m_RightViewer->SetGreenChannelIndex(bRightViewerGreenChannel->value());
    m_RightViewer->SetBlueChannelIndex(bRightViewerBlueChannel->value());
    m_RightViewer->GetRedHistogramWidget()->resize(aRightViewerHistogram->x(),
                                                   aRightViewerHistogram->y(),
                                                   aRightViewerHistogram->w() / 3, aRightViewerHistogram->h());
    m_RightViewer->SetViewModel(ImageViewerType::ScrollWidgetType::RGB);
    }
  else
    {
    m_RightViewer->SetRedChannelIndex(bRightViewerGrayscaleChannel->value());
    m_RightViewer->GetRedHistogramWidget()->resize(aRightViewerHistogram->x(),
                                                   aRightViewerHistogram->y(),
                                                   aRightViewerHistogram->w(), aRightViewerHistogram->h());
    m_RightViewer->SetViewModel(ImageViewerType::ScrollWidgetType::GRAYSCALE);
    }

  m_RightViewer->Reset();
  m_RightViewer->Update();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::CenterViewerColorCompositionCallback()
{
  if (bCenterViewerColorRGB->value())
    {
    m_CenterViewer->SetRedChannelIndex(bCenterViewerRedChannel->value());
    m_CenterViewer->SetGreenChannelIndex(bCenterViewerGreenChannel->value());
    m_CenterViewer->SetBlueChannelIndex(bCenterViewerBlueChannel->value());
    m_CenterViewer->GetRedHistogramWidget()->resize(aCenterViewerHistogram->x(),
                                                    aCenterViewerHistogram->y(),
                                                    aCenterViewerHistogram->w() / 3, aCenterViewerHistogram->h());
    m_CenterViewer->SetViewModel(ImageViewerType::ScrollWidgetType::RGB);
    }
  else
    {
    m_CenterViewer->SetRedChannelIndex(bCenterViewerGrayscaleChannel->value());
    m_CenterViewer->GetRedHistogramWidget()->resize(aCenterViewerHistogram->x(),
                                                    aCenterViewerHistogram->y(),
                                                    aCenterViewerHistogram->w(), aCenterViewerHistogram->h());
    m_CenterViewer->SetViewModel(ImageViewerType::ScrollWidgetType::GRAYSCALE);
    }

  m_CenterViewer->Reset();
  m_CenterViewer->Update();
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::SVMSetupOk()
{
  m_Estimator->SetSVMType(this->svmSVMType->value());
  m_Estimator->SetKernelType(this->svmKernelType->value());
  m_Estimator->SetPolynomialKernelDegree(static_cast<int>(this->svmKernelDegree->value()));
  m_Estimator->SetKernelGamma(this->svmGamma->value());
  m_Estimator->SetNu(this->svmNu->value());
  m_Estimator->SetKernelCoef0(this->svmCoef0->value());
  m_Estimator->SetC(this->svmC->value());
  m_Estimator->SetEpsilon(this->svmEpsilon->value());
  m_Estimator->SetP(this->svmP->value());
  m_Estimator->SetCacheSize(static_cast<int>(this->svmCacheSize->value()));
  m_Estimator->DoProbabilityEstimates(this->svmPE->value());
  m_Estimator->DoShrinking(this->svmShrinking->value());
  this->ReInitButtons();
  Log("SVM parameters changed.");
}
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::DisplayResultsButtonToggled()
{
  if (bDisplayResults->value())
    {
    bDisplayResults->clear();
    FullWidgetPointerType full = m_ResultViewer->GetFullWidget();
    iViewFullCenterWindow->remove(full);

    if (m_CenterViewer->GetUseScroll())
      {
      ScrollWidgetPointerType scroll = m_ResultViewer->GetScrollWidget();
      iViewScrollCenterWindow->remove(scroll);
      }

    full = m_CenterViewer->GetFullWidget();
    full->resize(0, 0, iViewFullCenterWindow->w(), iViewFullCenterWindow->h());
    iViewFullCenterWindow->add(full);
    iViewFullCenterWindow->resizable(full);
    iViewFullCenterWindow->show();
    full->show();

    if (m_CenterViewer->GetUseScroll())
      {
      ScrollWidgetPointerType scroll = m_CenterViewer->GetScrollWidget();
      iViewScrollCenterWindow->add(scroll);
      iViewScrollCenterWindow->resizable(scroll);
      iViewScrollCenterWindow->show();
      scroll->show();
      }
    m_CenterViewer->Update();
    OffsetType off;
    off.Fill(0);
    m_CenterViewer->Link(m_LeftViewer, off);
    Fl::check();
    this->Update();
    Fl::check();
    this->Update();
    }
  else
    {
    bDisplayResults->set();
    m_ClassificationFilter->SetInput(m_ListToImageFilter1->GetOutput());

    typename OverlayImageType::PixelType color1(3), color2(3);

    color1[0] = static_cast<unsigned char>(m_Class1Color[0] * 255);
    color1[1] = static_cast<unsigned char>(m_Class1Color[1] * 255);
    color1[2] = static_cast<unsigned char>(m_Class1Color[2] * 255);

    color2[0] = static_cast<unsigned char>(m_Class2Color[0] * 255);
    color2[1] = static_cast<unsigned char>(m_Class2Color[1] * 255);
    color2[2] = static_cast<unsigned char>(m_Class2Color[2] * 255);

    m_ChangeLabelFilter->SetChange(m_Label1, color1);
    m_ChangeLabelFilter->SetChange(m_Label2, color2);
    m_ChangeLabelFilter->SetInput(m_ClassificationFilter->GetOutput());
    m_ChangeLabelFilter->SetNumberOfComponentsPerPixel(3);

    // Cast for result viewer display
    m_CastFilter = CastFilterType::New();
    m_CastFilter->SetInput(m_ChangeLabelFilter->GetOutput());
    m_CastFilter->UpdateOutputInformation();

    FullWidgetPointerType full = m_CenterViewer->GetFullWidget();
    iViewFullCenterWindow->remove(full);
    iViewFullCenterWindow->redraw();

    if (m_CenterViewer->GetUseScroll())
      {
      ScrollWidgetPointerType scroll = m_CenterViewer->GetScrollWidget();
      iViewScrollCenterWindow->remove(scroll);
      }

    m_ResultViewer = ImageViewerType::New();
    m_ResultViewer->SetImage(m_LeftImage);
    m_ResultViewer->SetImageOverlay(m_CastFilter->GetOutput());
    m_ResultViewer->SetUseImageOverlay(true);
    m_ResultViewer->SetShowHistograms(false);
    m_ResultViewer->SetInterfaceBoxesColor(m_InterfaceColor);
    m_ResultViewer->SetPixLocOutput(oPixLocValue);
    m_ResultViewer->SetShowZoomWidget(false);
    m_ResultViewer->SetLabel("Result image");

    m_ResultViewer->Build();

    m_ResultViewer->SetImageOverlayOpacity(static_cast<unsigned char>(this->slTrainingSetOpacity->value() * 255));

    full = m_ResultViewer->GetFullWidget();
    full->resize(0, 0, iViewFullCenterWindow->w(), iViewFullCenterWindow->h());
    iViewFullCenterWindow->add(full);
    iViewFullCenterWindow->resizable(full);
    iViewFullCenterWindow->show();
    full->show();

    if (m_ResultViewer->GetUseScroll())
      {
      ScrollWidgetPointerType scroll = m_ResultViewer->GetScrollWidget();
      scroll->resize(0, 0, iViewScrollCenterWindow->w(), iViewScrollCenterWindow->h());
      iViewScrollCenterWindow->add(scroll);
      iViewScrollCenterWindow->resizable(scroll);
      iViewScrollCenterWindow->show();
      scroll->show();
      }
    m_ResultViewer->Show();
    m_ResultViewer->Update();
    OffsetType off;
    off.Fill(0);
    m_ResultViewer->Link(m_LeftViewer, off);
    Fl::check();
    this->Update();
    Fl::check();
    this->Update();
    }
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::LoadCenterImage()
{
  m_ImageToListFilter1->GetOutput()->UpdateOutputInformation();
  m_ImageToListFilter2->GetOutput()->UpdateOutputInformation();
  m_AbsFilter->GetOutput()->UpdateOutputInformation();
  m_MeanRatioFilter->GetOutput()->UpdateOutputInformation();

  m_ImageList->PushBack(m_AbsFilter->GetOutput());
  m_ImageList->PushBack(m_MeanRatioFilter->GetOutput());

  SingleImageListIteratorType it;

  for (it =  m_ImageToListFilter1->GetOutput()->Begin();
       it != m_ImageToListFilter1->GetOutput()->End();
       ++it)
    {
    it.Get()->UpdateOutputInformation();
    m_ImageList->PushBack(it.Get());
    }

  for (it =  m_ImageToListFilter2->GetOutput()->Begin();
       it != m_ImageToListFilter2->GetOutput()->End();
       ++it)
    {
    it.Get()->UpdateOutputInformation();
    m_ImageList->PushBack(it.Get());
    }

  m_ListToImageFilter1->SetInput(m_ImageList);
  m_ListToImageFilter1->GetOutput()->UpdateOutputInformation();

  m_CenterViewer->SetImage(m_ListToImageFilter1->GetOutput());
  m_CenterViewer->Build();

  FullWidgetPointerType full = m_CenterViewer->GetFullWidget();
  full->resize(0, 0, iViewFullCenterWindow->h(), iViewFullCenterWindow->w());
  iViewFullCenterWindow->resize(iViewFullCenterWindow->x(), iViewFullCenterWindow->y(), full->w(), full->h());
  iViewFullCenterWindow->add(full);
  iViewFullCenterWindow->resizable(full);
  iViewFullCenterWindow->show();
  full->show();

  if (m_CenterViewer->GetUseScroll())
    {
    ScrollWidgetPointerType scroll = m_CenterViewer->GetScrollWidget();
    iViewScrollCenterWindow->resize(iViewScrollCenterWindow->x(), iViewScrollCenterWindow->y(), scroll->w(), scroll->h());
    iViewScrollCenterWindow->add(scroll);
    iViewScrollCenterWindow->resizable(scroll);
    iViewScrollCenterWindow->show();
    scroll->show();
    }

  //  /// Add the choice for color composition
  itk::OStringStream oss;

  oss << "Intensity difference";
  bCenterViewerGrayscaleChannel->add(oss.str().c_str());
  bCenterViewerRedChannel->add(oss.str().c_str());
  bCenterViewerGreenChannel->add(oss.str().c_str());
  bCenterViewerBlueChannel->add(oss.str().c_str());
  oss.str("");

  oss << "Intensity ratio";
  bCenterViewerGrayscaleChannel->add(oss.str().c_str());
  bCenterViewerRedChannel->add(oss.str().c_str());
  bCenterViewerGreenChannel->add(oss.str().c_str());
  bCenterViewerBlueChannel->add(oss.str().c_str());
  oss.str("");

  for (unsigned int i = 0; i < m_LeftImage->GetNumberOfComponentsPerPixel(); ++i)
    {
    oss << "Left image channel " << i + 1;
    bCenterViewerGrayscaleChannel->add(oss.str().c_str());
    bCenterViewerRedChannel->add(oss.str().c_str());
    bCenterViewerGreenChannel->add(oss.str().c_str());
    bCenterViewerBlueChannel->add(oss.str().c_str());
    oss.str("");
    }

  for (unsigned int i = 0; i < m_RightImage->GetNumberOfComponentsPerPixel(); ++i)
    {
    oss << "Right image channel " << i + 1;
    bCenterViewerGrayscaleChannel->add(oss.str().c_str());
    bCenterViewerRedChannel->add(oss.str().c_str());
    bCenterViewerGreenChannel->add(oss.str().c_str());
    bCenterViewerBlueChannel->add(oss.str().c_str());
    oss.str("");
    }

  bCenterViewerRedChannel->value(m_CenterViewer->GetRedChannelIndex());
  bCenterViewerGreenChannel->value(m_CenterViewer->GetGreenChannelIndex());
  bCenterViewerBlueChannel->value(m_CenterViewer->GetBlueChannelIndex());
  bCenterViewerGrayscaleChannel->value(m_CenterViewer->GetRedChannelIndex());

  aCenterViewerHistogram->add(m_CenterViewer->GetRedHistogramWidget());
  //int offset = tTabs->h()-tHistogram->h();

  if (m_CenterViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
    {
    int width = aCenterViewerHistogram->w() / 3;
    aCenterViewerHistogram->add(m_CenterViewer->GetGreenHistogramWidget());
    aCenterViewerHistogram->add(m_CenterViewer->GetBlueHistogramWidget());

    m_CenterViewer->GetRedHistogramWidget()->resize(aCenterViewerHistogram->x(),
                                                    aCenterViewerHistogram->y(), width, aCenterViewerHistogram->h());
    m_CenterViewer->GetGreenHistogramWidget()->resize(aCenterViewerHistogram->x() + width,
                                                      aCenterViewerHistogram->y(), width, aCenterViewerHistogram->h());
    m_CenterViewer->GetBlueHistogramWidget()->resize(2 * width + aCenterViewerHistogram->x(), aCenterViewerHistogram->y(
                                                       ), width, aCenterViewerHistogram->h());

    bCenterViewerColorGrayscale->clear();
    bCenterViewerColorRGB->set();
    bCenterViewerRedChannel->activate();
    bCenterViewerGreenChannel->activate();
    bCenterViewerBlueChannel->activate();
    bCenterViewerGrayscaleChannel->deactivate();
    }
  else
    {
    bCenterViewerColorGrayscale->set();
    bCenterViewerColorRGB->clear();
    bCenterViewerRedChannel->deactivate();
    bCenterViewerGreenChannel->deactivate();
    bCenterViewerBlueChannel->deactivate();
    bCenterViewerGrayscaleChannel->activate();
    m_CenterViewer->GetRedHistogramWidget()->resize(aCenterViewerHistogram->x(),
                                                    aCenterViewerHistogram->y(),
                                                    aCenterViewerHistogram->w(), aCenterViewerHistogram->h());
    }

  m_CenterViewer->Update();

  OffsetType off;
  off.Fill(0);
  m_CenterViewer->Link(m_LeftViewer, off);
  Fl::check();
  this->Update();
  Fl::check();
  Log("Center image computed");

}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::LoadLeftImage()
{
  m_LeftViewer->SetImage(m_LeftImage);
  m_LeftViewer->Build();

  FullWidgetPointerType full = m_LeftViewer->GetFullWidget();
  full->resize(0, 0, iViewFullLeftWindow->h(), iViewFullLeftWindow->w());
  iViewFullLeftWindow->resize(iViewFullLeftWindow->x(), iViewFullLeftWindow->y(), full->w(), full->h());
  iViewFullLeftWindow->add(full);
  iViewFullLeftWindow->resizable(full);
  iViewFullLeftWindow->show();
  full->show();

  if (m_LeftViewer->GetUseScroll())
    {
    ScrollWidgetPointerType scroll = m_LeftViewer->GetScrollWidget();
    iViewScrollLeftWindow->resize(iViewScrollLeftWindow->x(), iViewScrollLeftWindow->y(), scroll->w(), scroll->h());
    iViewScrollLeftWindow->add(scroll);
    iViewScrollLeftWindow->resizable(scroll);
    iViewScrollLeftWindow->show();
    scroll->show();
    }
  aLeftViewerHistogram->add(m_LeftViewer->GetRedHistogramWidget());

  /// Add the choice for color composition
  itk::OStringStream oss;
  for (unsigned int i = 0; i < m_LeftImage->GetNumberOfComponentsPerPixel(); ++i)
    {
    oss << "Left image channel " << i + 1;
    bLeftViewerGrayscaleChannel->add(oss.str().c_str());
    bLeftViewerRedChannel->add(oss.str().c_str());
    bLeftViewerGreenChannel->add(oss.str().c_str());
    bLeftViewerBlueChannel->add(oss.str().c_str());
    oss.str("");
    }

  bLeftViewerRedChannel->value(m_LeftViewer->GetRedChannelIndex());
  bLeftViewerGreenChannel->value(m_LeftViewer->GetGreenChannelIndex());
  bLeftViewerBlueChannel->value(m_LeftViewer->GetBlueChannelIndex());
  bLeftViewerGrayscaleChannel->value(m_LeftViewer->GetRedChannelIndex());

  if (m_LeftViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
    {
    int width = aLeftViewerHistogram->w() / 3;
    aLeftViewerHistogram->add(m_LeftViewer->GetGreenHistogramWidget());
    aLeftViewerHistogram->add(m_LeftViewer->GetBlueHistogramWidget());

    m_LeftViewer->GetRedHistogramWidget()->resize(aLeftViewerHistogram->x(),
                                                  aLeftViewerHistogram->y(), width, aLeftViewerHistogram->h());
    m_LeftViewer->GetGreenHistogramWidget()->resize(width + aLeftViewerHistogram->x(),
                                                    aLeftViewerHistogram->y(), width, aLeftViewerHistogram->h());
    m_LeftViewer->GetBlueHistogramWidget()->resize(2 * width + aLeftViewerHistogram->x(),
                                                   aLeftViewerHistogram->y(), width, aLeftViewerHistogram->h());
    bLeftViewerColorGrayscale->clear();
    bLeftViewerGrayscaleChannel->deactivate();
    bLeftViewerRedChannel->activate();
    bLeftViewerGreenChannel->activate();
    bLeftViewerBlueChannel->activate();
    bLeftViewerColorRGB->set();
    }
  else
    {
    bLeftViewerColorGrayscale->set();
    bLeftViewerColorRGB->clear();
    bLeftViewerRedChannel->deactivate();
    bLeftViewerGreenChannel->deactivate();
    bLeftViewerBlueChannel->deactivate();
    bLeftViewerGrayscaleChannel->activate();
    bLeftViewerGrayscaleChannel->value(m_LeftViewer->GetRedChannelIndex());
    m_LeftViewer->GetRedHistogramWidget()->resize(aLeftViewerHistogram->x(),
                                                  aLeftViewerHistogram->y(),
                                                  aLeftViewerHistogram->w(), aLeftViewerHistogram->h());
    }

  oss.str("");
  m_LeftViewer->Update();
  if (m_RightViewer->GetBuilt())
    {
    OffsetType off;
    off.Fill(0);
    m_LeftViewer->Link(m_RightViewer, off);
    }

  this->ReInitButtons();
  Fl::check();
  this->Update();
  Fl::check();
  Log("Left image loaded");
}

template <class TPixel>
void
InteractiveChangeDetection<TPixel>::LoadRightImage()
{
  m_RightViewer->SetImage(m_RightImage);
  m_RightViewer->Build();

  FullWidgetPointerType full = m_RightViewer->GetFullWidget();
  full->resize(0, 0, iViewFullRightWindow->h(), iViewFullRightWindow->w());
  iViewFullRightWindow->resize(iViewFullRightWindow->x(), iViewFullRightWindow->y(), full->w(), full->h());
  iViewFullRightWindow->add(full);
  iViewFullRightWindow->resizable(full);
  iViewFullRightWindow->show();
  full->show();

  if (m_RightViewer->GetUseScroll())
    {
    ScrollWidgetPointerType scroll = m_RightViewer->GetScrollWidget();
    iViewScrollRightWindow->resize(iViewScrollRightWindow->x(), iViewScrollRightWindow->y(), scroll->w(), scroll->h());
    iViewScrollRightWindow->add(scroll);
    iViewScrollRightWindow->resizable(scroll);
    iViewScrollRightWindow->show();
    scroll->show();
    }

  /// Add the choice for color composition
  itk::OStringStream oss;
  for (unsigned int i = 0; i < m_RightImage->GetNumberOfComponentsPerPixel(); ++i)
    {
    oss << "Right image channel " << i + 1;
    bRightViewerGrayscaleChannel->add(oss.str().c_str());
    bRightViewerRedChannel->add(oss.str().c_str());
    bRightViewerGreenChannel->add(oss.str().c_str());
    bRightViewerBlueChannel->add(oss.str().c_str());
    oss.str("");
    }

  bRightViewerRedChannel->value(m_RightViewer->GetRedChannelIndex());
  bRightViewerGreenChannel->value(m_RightViewer->GetGreenChannelIndex());
  bRightViewerBlueChannel->value(m_RightViewer->GetBlueChannelIndex());
  bRightViewerGrayscaleChannel->value(m_RightViewer->GetRedChannelIndex());

  // histogram rendering
  aRightViewerHistogram->add(m_RightViewer->GetRedHistogramWidget());
  if (m_RightViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
    {
    int width = aRightViewerHistogram->w() / 3;
    aRightViewerHistogram->add(m_RightViewer->GetGreenHistogramWidget());
    aRightViewerHistogram->add(m_RightViewer->GetBlueHistogramWidget());

    m_RightViewer->GetRedHistogramWidget()->resize(aRightViewerHistogram->x(),
                                                   aRightViewerHistogram->y(), width, aRightViewerHistogram->h());
    m_RightViewer->GetGreenHistogramWidget()->resize(width + aRightViewerHistogram->x(),
                                                     aRightViewerHistogram->y(), width, aRightViewerHistogram->h());
    m_RightViewer->GetBlueHistogramWidget()->resize(2 * width + aRightViewerHistogram->x(),
                                                    aRightViewerHistogram->y(), width, aRightViewerHistogram->h());

    bRightViewerColorGrayscale->clear();
    bRightViewerColorRGB->set();

    bRightViewerRedChannel->activate();
    bRightViewerGreenChannel->activate();
    bRightViewerBlueChannel->activate();
    bRightViewerGrayscaleChannel->deactivate();
    }
  else
    {
    bRightViewerColorGrayscale->set();
    bRightViewerColorRGB->clear();

    bRightViewerRedChannel->deactivate();
    bRightViewerGreenChannel->deactivate();
    bRightViewerBlueChannel->deactivate();
    bRightViewerGrayscaleChannel->activate();

    m_RightViewer->GetRedHistogramWidget()->resize(aRightViewerHistogram->x(),
                                                   aRightViewerHistogram->y(),
                                                   aRightViewerHistogram->w(), aRightViewerHistogram->h());
    }

  m_RightViewer->Update();

  if (m_LeftViewer->GetBuilt())
    {
    OffsetType off;
    off.Fill(0);
    m_RightViewer->Link(m_LeftViewer, off);
    }

  this->ReInitButtons();
  Fl::check();
  this->Update();
  Fl::check();
  Log("Right image loaded");
}
/// Hide the app
template <class TPixel>
void
InteractiveChangeDetection<TPixel>
::Hide(void)
{
  // hide the main windo
  iviewWindowPrincipal->hide();

  // hide the full windows
  iViewFullRightWindow->hide();
  iViewFullLeftWindow->hide();
  iViewFullCenterWindow->hide();

  // hide scrolls if any
  if (m_RightViewer->GetUseScroll()) iViewScrollRightWindow->hide();
  if (m_LeftViewer->GetUseScroll()) iViewScrollLeftWindow->hide();
  if (m_CenterViewer->GetUseScroll()) iViewScrollCenterWindow->hide();

  // hide the Setup Window
  tTabs->hide();
}

} // end namespace otb
#endif
