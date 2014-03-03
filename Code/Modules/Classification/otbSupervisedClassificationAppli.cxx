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

#include "otbSupervisedClassificationAppli.h"

#include <FLU/Flu_File_Chooser.h>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Color_Chooser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Group.H>

#include "otbMacro.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "otbStreamingTraits.h"
#include "otbExtractROI.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "otbImageToEdgePathFilter.h"
#include "otbSimplifyPathListFilter.h"
#include "otbFltkWriterWatcher.h"
#include "otbVectorDataProjectionFilter.h"
#include "otbVectorDataExtractROI.h"

#include "otbImageMetadataInterfaceBase.h"
#include "otbImageMetadataInterfaceFactory.h"
#include "otbVectorDataIntoImageProjectionFilter.h"

#include "otbMsgReporter.h"

namespace otb
{
/// Constructor
SupervisedClassificationAppli
::SupervisedClassificationAppli()
{
  srand(time(0));

  m_LastPath = ".";
  m_CurrentLabel = 1;
  m_Model = NULL;

  m_HasOutput = false;
  m_HasOutputVector = false;
  m_HasCloseModule = false;
  m_ResultShown = false;
  m_UpToDateResult = false;

  m_ImageViewer               = ImageViewerType::New();
  m_Output                    = OutputImageType::New();
  m_Estimator                 = EstimatorType::New();
  m_ClassificationFilter      = ClassificationFilterType::New();
  m_ChangeLabelFilter         = ChangeLabelFilterType::New();
  m_TrainingSet               = PolygonListType::New();
  m_ValidationSet             = PolygonListType::New();
  m_TrainingListSample        = ListSampleType::New();
  m_ValidationListSample      = ListSampleType::New();
  m_TrainingListLabelSample   = TrainingListSampleType::New();
  m_ValidationListLabelSample = TrainingListSampleType::New();
  m_CastFilter                = CastFilterType::New();
}

/// Destructor
SupervisedClassificationAppli
::~SupervisedClassificationAppli()
{}

/** Main methods */

/**
*
*/
void
SupervisedClassificationAppli
::Show()
{
  Fl::check();
  this->bClassesBrowser->show();
  Fl::check();
}

/**
*
*/
void
SupervisedClassificationAppli
::Build()
{
  this->CreateGUI();
  this->ReInitButtons();

  m_ImageViewer->SetPixLocOutput(oPixLocValue);
  m_ImageViewer->SetShowZoomWidget(false);
  m_ImageViewer->SetPolygonalROISelectionMode(false);
  m_ImageViewer->SetRectangularROISelectionMode(false);
  m_ImageViewer->SetPolygonROIList(m_TrainingSet);

  Fl_Text_Buffer * buffer1 = new Fl_Text_Buffer();
  this->dClassInfo->buffer(buffer1);

  Fl_Text_Buffer * buffer2 = new Fl_Text_Buffer();
  this->dImageInfo->buffer(buffer2);

  bTrainingSet->set();
  bValidationSet->clear();

  this->LoadSVMParams();
}

void
SupervisedClassificationAppli
::LoadSVMParams()
{
   // SVM Setup
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
}

/**
*
*/
void
SupervisedClassificationAppli
::ReInitButtons()
{
  bSaveClassifAsVectorData->deactivate();
  bSaveSVMModel->deactivate();
  bVisualisationSetup->deactivate();
  bDisplay->deactivate();
  bEraseLastPoint->deactivate();
}

/**
*
*/
void
SupervisedClassificationAppli
::Update()
{
  m_ImageViewer->Update();

  if (static_cast<int>(bDisplay->value()) == 1)
    {
    m_ResultViewer->Update();
    }
  this->UpdateClassInfo();
}

/**
*
*/
void
SupervisedClassificationAppli
::LoadImage()
{
  this->ClearROIs();

  // Removing widgets from the display group, since they will be destroyed
  if (m_ImageViewer.IsNotNull())
    {
    FullWidgetPointerType full = m_ImageViewer->GetFullWidget();
    guiFullWindow->remove(full);
    if (m_ImageViewer->GetUseScroll())
      {
      ScrollWidgetPointerType scroll = m_ImageViewer->GetScrollWidget();
      guiScrollWindow->remove(scroll);
      }
    }

  m_ImageViewer = ImageViewerType::New();
  m_ImageViewer->SetPixLocOutput(oPixLocValue);
  m_ImageViewer->SetShowZoomWidget(false);
  m_ImageViewer->SetPolygonalROISelectionMode(false);
  m_ImageViewer->SetRectangularROISelectionMode(false);
  m_ImageViewer->SetPolygonROIList(m_TrainingSet);
  m_ImageViewer->SetImage(m_InputImage);

  m_InputImage->UpdateOutputInformation();
  otb::ImageMetadataInterfaceBase::Pointer lImageMetadata = otb::ImageMetadataInterfaceFactory::CreateIMI(
      m_InputImage->GetMetaDataDictionary());

  std::vector<unsigned int> defaultDisplay(3);
  defaultDisplay = lImageMetadata->GetDefaultDisplay();

  m_ImageViewer->SetRedChannelIndex(defaultDisplay[0]);
  m_ImageViewer->SetGreenChannelIndex(defaultDisplay[1]);
  m_ImageViewer->SetBlueChannelIndex(defaultDisplay[2]);

  m_ImageViewer->Build();

  FullWidgetPointerType full = m_ImageViewer->GetFullWidget();
  full->resize(0, 0, guiFullWindow->h(), guiFullWindow->w());
  guiFullWindow->resize(guiFullWindow->x(), guiFullWindow->y(), full->w(), full->h());
  guiFullWindow->add(full);
  guiFullWindow->resizable(full);
  guiFullWindow->show();
  full->show();

  if (m_ImageViewer->GetUseScroll())
    {
    ScrollWidgetPointerType scroll = m_ImageViewer->GetScrollWidget();
    guiScrollWindow->resize(guiScrollWindow->x(), guiScrollWindow->y(), scroll->w(), scroll->h());
    guiScrollWindow->add(scroll);
    guiScrollWindow->resizable(scroll);
    guiScrollWindow->show();
    scroll->show();
    }

  /// Add the choice for color composition
  std::ostringstream oss;
  for (unsigned int i = 0; i < m_InputImage->GetNumberOfComponentsPerPixel(); ++i)
    {
    oss << i + 1;
    viewerGrayscaleChannelChoice->add(oss.str().c_str());
    viewerRedChannelChoice->add(oss.str().c_str());
    viewerGreenChannelChoice->add(oss.str().c_str());
    viewerBlueChannelChoice->add(oss.str().c_str());
    viewerRealChannelChoice->add(oss.str().c_str());
    viewerImaginaryChannelChoice->add(oss.str().c_str());
    oss.str("");
    }

  viewerRedChannelChoice->value(m_ImageViewer->GetRedChannelIndex());
  if (m_ImageViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
    {
    viewerGreenChannelChoice->value(m_ImageViewer->GetGreenChannelIndex());
    viewerBlueChannelChoice->value(m_ImageViewer->GetBlueChannelIndex());
    }
  viewerGrayscaleChannelChoice->value(m_ImageViewer->GetRedChannelIndex());
  viewerRealChannelChoice->value(m_ImageViewer->GetRedChannelIndex());
  viewerImaginaryChannelChoice->value(m_ImageViewer->GetRedChannelIndex());

  if (m_ImageViewer->GetFullWidget()->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
    {
    viewerGrayscaleMode->clear();
    viewerGrayscaleChannelChoice->deactivate();
    viewerComplexMode->clear();
    viewerRealChannelChoice->deactivate();
    viewerImaginaryChannelChoice->deactivate();
    viewerColorMode->set();
    viewerRedChannelChoice->activate();
    viewerGreenChannelChoice->activate();
    viewerBlueChannelChoice->activate();

    }
  else
    {
    viewerGrayscaleMode->set();
    viewerGrayscaleChannelChoice->activate();
    viewerComplexMode->clear();
    viewerRealChannelChoice->deactivate();
    viewerImaginaryChannelChoice->deactivate();
    viewerColorMode->clear();
    viewerRedChannelChoice->deactivate();
    viewerGreenChannelChoice->deactivate();
    viewerBlueChannelChoice->deactivate();
    }
  oss.str("");
  m_ImageViewer->Update();

  bAddClass->activate();
  bRemoveClass->activate();
  bChangeClassName->activate();
  bChangeClassColor->activate();
  bTrainingSet->activate();
  bValidationSet->activate();
  bLearn->activate();
  bEraseLastROI->activate();
  bClearROIs->activate();
  bEndPolygon->activate();
  bUsePolygon->activate();
  bVisualisationSetup->activate();
  slTrainingSetOpacity->activate();
  bROIFocus->activate();
  bImportVectorData->activate();
  //bExportVectorData->activate();
  bExportAllVectorData->activate();
  bImportROIsImage->activate();
  bRandomGeneration->value(1);
  bRandomGeneration->do_callback();

  Fl::check();
  this->Update();
  Fl::check();

  this->UpdateImageInfo();
  this->ResetClassification();
  this->TrainingSetSelected();
}

/** File selection Callbacks */
/**
*
*/
void
SupervisedClassificationAppli
::ImportROIsImage()
{
  const char * cfname = flu_file_chooser("Pick an image file", "*.*", m_LastPath.c_str());
  Fl::check();
  if (cfname == NULL || strlen(cfname) < 1)
    {
    return;
    }

  m_ROIsImageFileName = std::string(cfname);
  m_LastPath = itksys::SystemTools::GetFilenamePath(m_ROIsImageFileName);
  this->LoadROIsImage();
}

/**
*
*/
void
SupervisedClassificationAppli
::ImportVectorData()
{
  const char * cfname = flu_file_chooser("Vector data file:", "*.shp\t*.kml", m_LastPath.c_str());
  if (cfname == NULL || strlen(cfname) < 1)
    {
    return;
    }
  std::string filename = std::string(cfname);

  VectorDataFileReaderPointerType reader = VectorDataFileReaderType::New();
  reader->SetFileName(filename.c_str());
  reader->Update();

  typedef otb::VectorDataIntoImageProjectionFilter<VectorDataType, ImageType>
    VectorDataReprojectionType;
  VectorDataReprojectionType::Pointer reproj = VectorDataReprojectionType::New();
  reproj->SetInputImage(m_InputImage);
  reproj->SetInputVectorData(reader->GetOutput());
  // We want to transform into image physical coordinates
  reproj->SetUseOutputSpacingAndOriginFromImage(true);
  // TODO : add DEM management for reprojection on sensor model geometry rasters
  reproj->Update();

  if (m_ClassesMap.empty())
    {
    this->AddClass();
    }


  TreeIteratorType it(reproj->GetOutput()->GetDataTree());
  it.GoToBegin();

  while (!it.IsAtEnd())
    {
    if (it.Get()->IsPolygonFeature())
      {
      m_ImageViewer->GetPolygonROIList()->PushBack(it.Get()->GetPolygonExteriorRing());
      m_ImageViewer->GetPolygonROIList()->Back()->SetValue(m_ImageViewer->GetNextROILabel());
      }
    ++it;
    }
  m_ImageViewer->GetPolygonROIList()->PushBack(PolygonType::New());
  m_ImageViewer->GetPolygonROIList()->Back()->SetValue(m_ImageViewer->GetNextROILabel());

  this->Update();
  this->ResetClassification();
}

/**
*
*/
void
SupervisedClassificationAppli
::ExportAllVectorData()
{
  const char* dirname = flu_dir_chooser("Select directory:", m_LastPath.c_str());
  if (dirname == NULL || strlen(dirname) < 1)
    {
    return;
    }

  // If the specified directory doesn't exists, create it.
  bool isFile = false;
  if (!itksys::SystemTools::FileExists(dirname, isFile))
    {
    itksys::SystemTools::MakeDirectory(dirname);
    }
  else
    {
    // the the soecify file is not a directory -> error message
    if (isFile)
      {
      std::ostringstream oss;
      oss.str("");
      oss << dirname << " already exists as a file.";
      oss << "Please select a valid directory or set a new one name that will be created." << std::endl;
      MsgReporter::GetInstance()->SendError(oss.str().c_str());
      return;
      }
    }

  std::vector<DataTreePointerType>   dataTreeVector;
  std::vector<DataNodePointerType>   dataNodeVector;
  std::vector<VectorDataPointerType> vectorDataVector;

  for (ClassesMapType::iterator it = m_ClassesMap.begin();
       it != m_ClassesMap.end(); ++it)
    {
    VectorDataPointerType vectorData = VectorDataType::New();

    DataNodePointerType document = DataNodeType::New();
    document->SetNodeType(DOCUMENT);
    document->SetNodeId("polygon");
    DataNodePointerType folder = DataNodeType::New();
    folder->SetNodeType(FOLDER);

    DataTreePointerType tree = vectorData->GetDataTree();
    DataNodePointerType root = tree->GetRoot()->Get();

    tree->Add(document, root);
    tree->Add(folder, document);

    dataTreeVector.push_back(tree);
    dataNodeVector.push_back(folder);

    vectorDataVector.push_back(vectorData);
    }
  PolygonListType::Pointer list = m_ImageViewer->GetPolygonROIList();

  for (PolygonListType::Iterator it = list->Begin(); it != list->End(); ++it)
    {
    int                      classIndex = 0;
    bool                     found = false;
    ClassesMapType::iterator cit = m_ClassesMap.begin();
    while (cit != m_ClassesMap.end() && !found)
      {
      if ((*cit)->GetId() == it.Get()->GetValue())
        {
        found = true;
        }
      ++classIndex;
      ++cit;
      }
    if (found)
      {
      DataNodePointerType newPolygon = DataNodeType::New();
      newPolygon->SetPolygonExteriorRing(it.Get());
      dataTreeVector[classIndex - 1]->Add(newPolygon, dataNodeVector[classIndex - 1]);
      }
    }

  std::vector<VectorDataPointerType>::iterator vectorDataVectorIterator =  vectorDataVector.begin();
  ClassesMapType::iterator                     cit = m_ClassesMap.begin();

  std::ostringstream oss;

  while (cit != m_ClassesMap.end() && vectorDataVectorIterator !=  vectorDataVector.end())
    {
    std::string filename = (*cit)->GetName();
    std::replace(filename.begin(), filename.end(), ' ', '_');

    oss << dirname << "\\" << filename << ".shp";

    // Add the projection to the vectordata
    typedef otb::VectorDataProjectionFilter<VectorDataType,
                                            VectorDataType>    ProjectionFilterType;
    ProjectionFilterType::Pointer vectorDataProjection = ProjectionFilterType::New();
    vectorDataProjection->SetInput((*vectorDataVectorIterator));

    VectorDataType::DataNodeType::PointType lNewOrigin;
    lNewOrigin[0] = m_InputImage->GetOrigin()[0];
    lNewOrigin[1] = m_InputImage->GetOrigin()[1];

    vectorDataProjection->SetInputOrigin(lNewOrigin);
    vectorDataProjection->SetInputSpacing(m_InputImage->GetSpacing());

    std::string projectionRef;
    itk::ExposeMetaData<std::string>(m_InputImage->GetMetaDataDictionary(),
                                     MetaDataKey::ProjectionRefKey, projectionRef );
    vectorDataProjection->SetInputProjectionRef(projectionRef);
    vectorDataProjection->SetInputKeywordList(m_InputImage->GetImageKeywordlist());

    vectorDataProjection->Update();

    VectorDataFileWriterPointerType writer = VectorDataFileWriterType::New();
    writer->SetInput(vectorDataProjection->GetOutput());
    writer->SetFileName(oss.str());
    oss.str("");
    try
      {
      writer->Update();
      }
    catch (itk::ExceptionObject& err)
      {

      oss.str("");
      oss << "Error while writing data file: " << err << std::endl;
      MsgReporter::GetInstance()->SendError(oss.str().c_str());
      }
    ++cit;
    ++vectorDataVectorIterator;
    }
  m_LastPath = itksys::SystemTools::GetFilenamePath(dirname);
}

/**
 *
 */
void
SupervisedClassificationAppli
::SaveClassifAsVectorData()
{
  m_OutputVector.clear();
  LabeledPixelType label;
  // for each label, extrat 2 vector data. For this, threshold the classif to have a binary image
  // then connectedcomponent -> relabelcomponent -> persistentvectorization
  for (ClassesMapType::iterator it = m_ClassesMap.begin(); it != m_ClassesMap.end(); ++it)
    {
    label = (*it)->GetId();

    ThresholderPointerType binner = ThresholderType::New();
    binner->SetInput(m_ClassificationFilter->GetOutput());
    binner->SetUpperThreshold(label);
    binner->SetLowerThreshold(label);
    binner->SetInsideValue(1);
    binner->SetOutsideValue(0);

    ConnectedFilterPointerType       connecter = ConnectedFilterType::New();
    RelabelFilterPointerType         relabelFilter = RelabelFilterType::New();
    VectorizationFilterType::Pointer vectorizer = VectorizationFilterType::New();
    connecter->SetInput(binner->GetOutput());
    relabelFilter->SetInput(connecter->GetOutput());
    relabelFilter->SetMinimumObjectSize(10);
    vectorizer->SetInput(relabelFilter->GetOutput());

    vectorizer->Update();

    OutputVectorDataType::Pointer             vectorData = OutputVectorDataType::New();
    OutputVectorDataType::DataNodePointerType document = OutputVectorDataType::DataNodeType::New();
    OutputVectorDataType::DataNodePointerType root = vectorData->GetDataTree()->GetRoot()->Get();
    document->SetNodeType(otb::DOCUMENT);
    document->SetNodeId("DOCUMENT");
    vectorData->GetDataTree()->Add(document, root);

    OutputVectorDataType::DataNodePointerType folder = OutputVectorDataType::DataNodeType::New();
    folder->SetNodeType(otb::FOLDER);
    vectorData->GetDataTree()->Add(folder, document);

    // We do not clear the lists in order to add the new results
    OutputPolygonListType::Pointer lTempPolygonsList = vectorizer->GetPathList();
    unsigned int                   i = 1;
    for (OutputPolygonListType::Iterator it = lTempPolygonsList->Begin(); it != lTempPolygonsList->End(); ++it)
      {
      OutputVectorDataType::DataNodePointerType poly = OutputVectorDataType::DataNodeType::New();
      poly->SetNodeType(otb::FEATURE_POLYGON);
      std::ostringstream oss;
      oss.str("");
      oss << i + 1;
      poly->SetNodeId(oss.str().c_str());
      poly->SetPolygonExteriorRing(it.Get());
      vectorData->GetDataTree()->Add(poly, folder);
      i++;
      }

    typedef otb::VectorDataProjectionFilter<OutputVectorDataType, OutputVectorDataType> ProjectionFilterType;
    ProjectionFilterType::Pointer vectorDataProjection = ProjectionFilterType::New();
    vectorDataProjection->SetInput(vectorData);

    ImageType::PointType lNewOrigin;
    // polygons are recorded with a 0.5 shift...
    lNewOrigin[0] = m_InputImage->GetOrigin()[0] + 0.5;
    lNewOrigin[1] = m_InputImage->GetOrigin()[1] + 0.5;

    vectorDataProjection->SetInputOrigin(lNewOrigin);
    vectorDataProjection->SetInputSpacing(m_InputImage->GetSpacing());

    std::string projectionRef;
    itk::ExposeMetaData<std::string>(
      m_InputImage->GetMetaDataDictionary(), MetaDataKey::ProjectionRefKey, projectionRef);
    vectorDataProjection->SetInputProjectionRef(projectionRef);
    vectorDataProjection->SetInputKeywordList(m_InputImage->GetImageKeywordlist());
    vectorDataProjection->Update();

    m_OutputVector.push_back(vectorDataProjection->GetOutput());
    }

  m_HasOutputVector = true;
  this->NotifyAll();
  m_HasOutputVector = false;

}

/**
*
*/
void
SupervisedClassificationAppli
::OpenSVMModel()
{
  const char * cfname = flu_file_chooser("SVM model file:", "*.svm", m_LastPath.c_str());
  if (cfname == NULL || strlen(cfname) < 1)
    {
    return;
    }
  m_ModelFileName = std::string(cfname);

  m_LastPath = itksys::SystemTools::GetFilenamePath(m_ModelFileName);
  this->LoadSVMModel();
}

/**
*
*/
void
SupervisedClassificationAppli
::LoadSVMModel()
{
  if (m_ClassesMap.size() != 0) // reset class list
    {
    unsigned int nbClass = m_ClassesMap.size();
    for (unsigned int classIndex = 0; classIndex < nbClass; classIndex++)
      {
      dClassList->value(1);
      this->RemoveClass();
      }
    }
  m_Model = ModelType::New();
  m_Model->LoadModel(m_ModelFileName.c_str());
  m_ClassesMap.clear();
  m_CurrentLabel = 1;

  for (unsigned int i = 0; i < m_Model->GetNumberOfClasses(); ++i)
    {
    this->AddClass();
    }

  this->m_UpToDateResult = false;
  bLearn->set();
  bSaveClassifAsVectorData->activate();
  bSaveSVMModel->activate();
  bDisplay->activate();
  bValidate->activate();
}

/**
*
*/
void
SupervisedClassificationAppli
::SaveSVMModel()
{
  const char * cfname = flu_file_chooser("SVM model file:", "*.svm", m_LastPath.c_str());
  if (cfname == NULL || strlen(cfname) < 1)
    {
    return;
    }

  std::string filename = std::string(cfname);
  m_Estimator->SaveModel(filename.c_str());
  m_LastPath = itksys::SystemTools::GetFilenamePath(filename);
}

/** Setup Callbacks */

/**
*
*/
void
SupervisedClassificationAppli
::SVMSetup()
{
  this->LoadSVMParams();
  guiSVMSetup->show();
}

/**
*
*/
void
SupervisedClassificationAppli
::SVMSetupOk()
{
  m_Estimator = EstimatorType::New();
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

  this->ResetClassification();
}

/**
*
*/
void
SupervisedClassificationAppli
::ResetClassification()
{
  bLearn->clear();
  if (static_cast<int>(bDisplay->value()) == 1)
    {
    this->ShowImage();
    }
  bDisplay->clear();
  bDisplay->deactivate();
  bValidate->deactivate();
  guiSVMSetup->hide();
  bSaveClassifAsVectorData->deactivate();
  bSaveSVMModel->deactivate();
}

/**
*
*/
void
SupervisedClassificationAppli
::SVMSetupCancel()
{
  guiSVMSetup->hide();
}

/**
*
*/
void
SupervisedClassificationAppli
::VisualisationSetup()
{
  std::ostringstream oss;
  for (unsigned int i = 0; i < m_InputImage->GetNumberOfComponentsPerPixel(); ++i)
    {
    oss.str("");
    oss << i + 1;
    viewerGrayscaleChannelChoice->add(oss.str().c_str());
    viewerRedChannelChoice->add(oss.str().c_str());
    viewerGreenChannelChoice->add(oss.str().c_str());
    viewerBlueChannelChoice->add(oss.str().c_str());
    viewerGrayscaleChannelChoice->add(oss.str().c_str());
    viewerRealChannelChoice->add(oss.str().c_str());
    viewerImaginaryChannelChoice->add(oss.str().c_str());
    }

  if (m_ImageViewer->GetViewModel() == ImageViewerType::ScrollWidgetType::RGB)
    {
    RGBSet();
    }
  else if (m_ImageViewer->GetViewModel() == ImageViewerType::ScrollWidgetType::GRAYSCALE)
    {
    GrayscaleSet();
    }
  else
    {
    ComplexSet();
    }
  guiVisualisationSetup->show();
}

/**
*
*/
void
SupervisedClassificationAppli
::VisualisationSetupOk()
{

  if (m_ResultViewer.IsNotNull())
    {
    if (viewerColorMode->value())
      {
      m_ResultViewer->SetRedChannelIndex(atoi(viewerRedChannelChoice->value()) - 1);
      m_ResultViewer->SetGreenChannelIndex(atoi(viewerGreenChannelChoice->value()) - 1);
      m_ResultViewer->SetBlueChannelIndex(atoi(viewerBlueChannelChoice->value()) - 1);
      m_ResultViewer->SetViewModel(ImageViewerType::ScrollWidgetType::RGB);
      }
    else if (viewerGrayscaleMode->value())
      {
      m_ResultViewer->SetRedChannelIndex(atoi(viewerGrayscaleChannelChoice->value()) - 1);
      m_ResultViewer->SetViewModel(ImageViewerType::ScrollWidgetType::GRAYSCALE);
      }
    else if (viewerComplexMode->value())
      {
      m_ResultViewer->SetRedChannelIndex(atoi(viewerRealChannelChoice->value()) - 1);
      m_ResultViewer->SetGreenChannelIndex(atoi(viewerImaginaryChannelChoice->value()) - 1);
      if (viewerModulus->value())
        {
        m_ResultViewer->SetViewModel(ImageViewerType::ScrollWidgetType::COMPLEX_MODULUS);
        }
      else
        {
        m_ResultViewer->SetViewModel(ImageViewerType::ScrollWidgetType::COMPLEX_PHASE);
        }
      }

    m_ResultViewer->Reset();
    m_ResultViewer->Update();
    }

  if (viewerColorMode->value())
    {
    m_ImageViewer->SetRedChannelIndex(atoi(viewerRedChannelChoice->value()) - 1);
    m_ImageViewer->SetGreenChannelIndex(atoi(viewerGreenChannelChoice->value()) - 1);
    m_ImageViewer->SetBlueChannelIndex(atoi(viewerBlueChannelChoice->value()) - 1);
    m_ImageViewer->SetViewModel(ImageViewerType::ScrollWidgetType::RGB);
    }
  else if (viewerGrayscaleMode->value())
    {
    m_ImageViewer->SetRedChannelIndex(atoi(viewerGrayscaleChannelChoice->value()) - 1);
    m_ImageViewer->SetViewModel(ImageViewerType::ScrollWidgetType::GRAYSCALE);
    }
  else if (viewerComplexMode->value())
    {
    m_ImageViewer->SetRedChannelIndex(atoi(viewerRealChannelChoice->value()) - 1);
    m_ImageViewer->SetGreenChannelIndex(atoi(viewerImaginaryChannelChoice->value()) - 1);
    if (viewerModulus->value())
      {
      m_ImageViewer->SetViewModel(ImageViewerType::ScrollWidgetType::COMPLEX_MODULUS);
      }
    else
      {
      m_ImageViewer->SetViewModel(ImageViewerType::ScrollWidgetType::COMPLEX_PHASE);
      }
    }

  m_ImageViewer->Reset();
  m_ImageViewer->Update();
  this->UpdateImageInfo();
  guiVisualisationSetup->hide();

}

/**
*
*/
void
SupervisedClassificationAppli
::VisualisationSetupCancel()
{
  guiVisualisationSetup->hide();
}

/**
*
*/
void
SupervisedClassificationAppli
::GrayscaleSet()
{
  viewerRedChannelChoice->deactivate();
  viewerGreenChannelChoice->deactivate();
  viewerBlueChannelChoice->deactivate();
  viewerRealChannelChoice->deactivate();
  viewerImaginaryChannelChoice->deactivate();
  viewerModulus->deactivate();
  viewerPhase->deactivate();
  viewerGrayscaleChannelChoice->activate();
  viewerColorMode->clear();
  viewerComplexMode->clear();
  viewerGrayscaleMode->set();
  viewerGrayscaleChannelChoice->value(m_ImageViewer->GetRedChannelIndex());
}

/**
*
*/
void
SupervisedClassificationAppli
::RGBSet()
{
  viewerColorMode->set();
  viewerComplexMode->clear();
  viewerGrayscaleMode->clear();
  viewerGrayscaleChannelChoice->deactivate();
  viewerRealChannelChoice->deactivate();
  viewerImaginaryChannelChoice->deactivate();
  viewerModulus->deactivate();
  viewerPhase->deactivate();
  viewerRedChannelChoice->activate();
  viewerGreenChannelChoice->activate();
  viewerBlueChannelChoice->activate();

  viewerRedChannelChoice->value(m_ImageViewer->GetRedChannelIndex());
  viewerGreenChannelChoice->value(m_ImageViewer->GetGreenChannelIndex());
  viewerBlueChannelChoice->value(m_ImageViewer->GetBlueChannelIndex());
}

/**
*
*/
void
SupervisedClassificationAppli
::ComplexSet()
{
  viewerComplexMode->set();
  viewerColorMode->clear();
  viewerGrayscaleMode->clear();
  viewerGrayscaleChannelChoice->deactivate();
  viewerRedChannelChoice->deactivate();
  viewerGreenChannelChoice->deactivate();
  viewerBlueChannelChoice->deactivate();
  viewerRealChannelChoice->activate();
  viewerImaginaryChannelChoice->activate();
  viewerModulus->activate();
  viewerPhase->activate();
  viewerRealChannelChoice->value(m_ImageViewer->GetRedChannelIndex());
  viewerImaginaryChannelChoice->value(m_ImageViewer->GetGreenChannelIndex());
}

/** Class edition callbacks */

/**
*
*/
void
SupervisedClassificationAppli
::ClassSelected()
{
  ClassType * theClass = GetSelectedClass();

  if (theClass == NULL)
    {
    return;
    }

  this->UpdateClassInfo();
  this->EndPolygon();
  m_ImageViewer->GetPolygonROIList()->Back()->SetValue(theClass->GetId());
  m_ImageViewer->SetNextROILabel(theClass->GetId());
}

/**
*
*/
void
SupervisedClassificationAppli
::AddClass()
{
  std::ostringstream oss;

  ClassPointerType newClass = ClassType::New();
  oss << "Class " << m_CurrentLabel;
  newClass->SetName(oss.str());
  newClass->SetId(m_CurrentLabel);
  ColorType color;
  //FIXME the color selection should be done with a proper lookup table class.
  srand(m_CurrentLabel + 123456);
  color[0] = rand() / (RAND_MAX + 1.0);
  color[1] = rand() / (RAND_MAX + 1.0);
  color[2] = rand() / (RAND_MAX + 1.0);
  color[3] = (float) this->slTrainingSetOpacity->value();

  Fl_Color flColor = fl_color_cube(static_cast<int>((FL_NUM_RED - 1) * color[0]),
                                   static_cast<int>((FL_NUM_GREEN - 1) * color[1]),
                                   static_cast<int>((FL_NUM_BLUE - 1) * color[2]));

  dClassList->selection_color(flColor);

  newClass->SetColor(color);
  m_ClassesMap.push_back(newClass);
  dClassList->add(oss.str().c_str());
  dClassList->select(dClassList->size());
  m_ImageViewer->AddROIColorMapEntry(m_CurrentLabel, color);
  ++m_CurrentLabel;

  bEraseLastPoint->activate();

  this->ResetClassification();
  this->ClassSelected();
  this->UsePolygon();

}

/**
*
*/
void
SupervisedClassificationAppli
::RemoveClass()
{
  unsigned int selectedItem = dClassList->value();

  if (selectedItem == 0)
    {
    fl_alert("No class selected.");
    return;
    }

  if (m_ClassesMap.size() == 1)
    {
    m_ImageViewer->SetPolygonalROISelectionMode(false);
    m_ImageViewer->SetRectangularROISelectionMode(false);
    }

  ClassType * theClass = this->GetSelectedClass();

  if (theClass == NULL)
    {
      return;
    }
  
  std::vector<PolygonListType::Iterator> toRemove;
  for (PolygonListType::Iterator pit = m_TrainingSet->Begin();
       pit != m_TrainingSet->End(); ++pit)
    {
    if (pit.Get()->GetValue() == theClass->GetId())
      {
      toRemove.push_back(pit);
      }
    }
  for (std::vector<PolygonListType::Iterator>::reverse_iterator it = toRemove.rbegin(); it != toRemove.rend(); ++it)
    {
    m_TrainingSet->Erase(*it);
    }
  toRemove.clear();
  for (PolygonListType::Iterator pit = m_ValidationSet->Begin();
       pit != m_ValidationSet->End(); ++pit)
    {
    if (pit.Get()->GetValue() == theClass->GetId())
      {
      toRemove.push_back(pit);
      }
    }
  for (std::vector<PolygonListType::Iterator>::reverse_iterator it = toRemove.rbegin(); it != toRemove.rend(); ++it)
    {
    m_ValidationSet->Erase(*it);
    }

  
  m_ClassesMap.erase(m_ClassesMap.begin() + selectedItem - 1);
  dClassList->remove(selectedItem);

  this->Update();
  this->ResetClassification();
}

/**
*
*/
void
SupervisedClassificationAppli
::ChangeClassName()
{
  ClassType * theClass = this->GetSelectedClass();
  if (theClass == NULL)
    {
    return;
    }

  tNewClassName->value(theClass->GetName().c_str());
  guiChangeClassName->show();
}

/**
*
*/
void
SupervisedClassificationAppli
::ChangeClassNameOk()
{
  guiChangeClassName->hide();

  unsigned int selectedItem = dClassList->value();

  if (selectedItem == 0)
    {
    return;
    }
  ClassType * theClass = m_ClassesMap[selectedItem - 1];
  theClass->SetName(tNewClassName->value());

  this->Update();
}

/**
*
*/
SupervisedClassificationAppli
::ClassPointerType
SupervisedClassificationAppli
::GetSelectedClass()
{
  ClassPointerType resp = NULL;

  unsigned int selectedItem = dClassList->value();
  if (selectedItem > 0)
    {
    resp = m_ClassesMap[selectedItem - 1];
    }
  return resp;
}

/**
*
*/
int
SupervisedClassificationAppli
::GetSelectedROI()
{
  ClassType * theClass = this->GetSelectedClass();
  if (theClass == NULL)
    {
    return -1;
    }

  PolygonListPointerType polygonList;
  if (bTrainingSet->value())
    {
    polygonList = m_TrainingSet;
    }
  else
    {
    polygonList = m_ValidationSet;
    }

  int                       ClassROICounter = 0;
  unsigned int              ROICounter = 0;
  PolygonListType::Iterator it = polygonList->Begin();

  while (it != polygonList->End() && ClassROICounter < dROIList->value())
    {
    if (it.Get()->GetValue() == theClass->GetId())
      {
      ClassROICounter++;
      }
    ++it;
    ++ROICounter;
    }

  if (ClassROICounter != dROIList->value())
    {
    return -1;
    }

  return (ROICounter - 1);
}

/**
*
*/
void
SupervisedClassificationAppli
::UpdateClassInfo()
{

  ClassType * theClass = this->GetSelectedClass();
  if (theClass == NULL)
    {
    return;
    }

  Fl_Color flColor = fl_color_cube(static_cast<int>((FL_NUM_RED - 1) * theClass->GetColor()[0]),
                                   static_cast<int>((FL_NUM_GREEN - 1) * theClass->GetColor()[1]),
                                   static_cast<int>((FL_NUM_BLUE - 1) * theClass->GetColor()[2]));

  dClassList->selection_color(flColor);
  dClassList->text(dClassList->value(), theClass->GetName().c_str());
  dClassList->redraw();

  dClassColor->color(flColor);
  dClassColor->redraw();

  dClassInfo->buffer()->remove(0, dClassInfo->buffer()->length());

  std::ostringstream oss;
  oss << "Class name: " << theClass->GetName() << " (Id: " << theClass->GetId() << ")";
  dClassInfo->insert(oss.str().c_str());
  oss.str("");

  unsigned int selectedROI = dROIList->value();

  dROIList->clear();

  unsigned ROICounter = 0;

  PolygonListPointerType polygonList;
  if (bTrainingSet->value())
    {
    polygonList = m_TrainingSet;
    }
  else
    {
    polygonList = m_ValidationSet;
    }

  for (PolygonListType::Iterator it = polygonList->Begin(); it != polygonList->End(); ++it)
    {
    if (it.Get()->GetValue() == theClass->GetId())
      {
      oss.str("");
      oss << "ROI " << ROICounter << ", " << it.Get()->GetArea() << " pixels";
      dROIList->add(oss.str().c_str());
      ++ROICounter;
      }
    }

  dROIList->select(selectedROI);
}

/**
*
*/
void
SupervisedClassificationAppli
::UpdateImageInfo()
{
  std::ostringstream oss;
  oss.str("");
  // Clear the info buffer
  dImageInfo->buffer()->remove(0, dImageInfo->buffer()->length());
  oss << "Filename: " << m_ImageFileName << std::endl;
  dImageInfo->insert(oss.str().c_str());
  oss.str("");
  oss << "Number of bands: " << m_InputImage->GetNumberOfComponentsPerPixel();
  oss << " - Size: " << m_InputImage->GetLargestPossibleRegion().GetSize() << std::endl;
  dImageInfo->insert(oss.str().c_str());
  oss.str("");
  if (m_ImageViewer->GetUseScroll())
    {
    oss << "Scroll window activated" << std::endl;
    dImageInfo->insert(oss.str().c_str());
    oss.str("");
    }

  oss.str("");
  switch (m_ImageViewer->GetViewModel())
    {
    case ImageViewerType::ScrollWidgetType::RGB:
      {
      oss << "RGB Composition: ";
      oss << " Band 1: " << m_ImageViewer->GetRedChannelIndex();
      oss << " Band 2: " << m_ImageViewer->GetGreenChannelIndex();
      oss << " Band 3: " << m_ImageViewer->GetBlueChannelIndex() << std::endl;
      break;
      }
    case ImageViewerType::ScrollWidgetType::GRAYSCALE:
      {
      oss << "Grayscale: ";
      oss << " Band: " << m_ImageViewer->GetRedChannelIndex() << std::endl;
      break;
      }
    case ImageViewerType::ScrollWidgetType::COMPLEX_MODULUS:
      {
      oss << "Complex modulus: ";
      oss << " Real part: "     << m_ImageViewer->GetRedChannelIndex();
      oss << " Imaginary part: " << m_ImageViewer->GetGreenChannelIndex();
      break;
      }
    case ImageViewerType::ScrollWidgetType::COMPLEX_PHASE:
      {
      oss << "Complex phase: ";
      oss << " Real part: "     << m_ImageViewer->GetRedChannelIndex();
      oss << " Imaginary part: " << m_ImageViewer->GetGreenChannelIndex();
      break;
      }
    }
  dImageInfo->insert(oss.str().c_str());
  oss.str("");
}

/**
*
*/
void
SupervisedClassificationAppli
::ChangeClassColor()
{

  ClassType * theClass = this->GetSelectedClass();

  if (theClass == NULL)
    {
    return;
    }

  double r = (double) theClass->GetColor()[0];
  double g = (double) theClass->GetColor()[1];
  double b = (double) theClass->GetColor()[2];

  int ok = fl_color_chooser("Changed class color:", r, g, b);

  if (ok)
    {
    ColorType newColor;
    newColor[0] = r;
    newColor[1] = g;
    newColor[2] = b;
    newColor[3] = 0.75;
    theClass->SetColor(newColor);
    m_ImageViewer->AddROIColorMapEntry(theClass->GetId(), newColor);

    OverlayImageType::PixelType color(3);
    color[0] = static_cast<unsigned char>(theClass->GetColor()[0] * 255);
    color[1] = static_cast<unsigned char>(theClass->GetColor()[1] * 255);
    color[2] = static_cast<unsigned char>(theClass->GetColor()[2] * 255);
    m_ChangeLabelFilter->SetChange(theClass->GetId(), color);

    if (static_cast<int>(bDisplay->value()) == 1)
      {
      m_UpToDateResult = false;
      this->DisplayResults();
      }

    this->Update();
    }
}

/**
*
*/
void
SupervisedClassificationAppli
::TrainingSetSelected()
{
  m_ImageViewer->SetPolygonROIList(m_TrainingSet);
  if (m_ImageViewer->GetPolygonROIList()->Size() == 0 ||
      m_ImageViewer->GetPolygonROIList()->Back()->GetVertexList()->Size() > 0)
    {
    m_ImageViewer->GetPolygonROIList()->PushBack(PolygonType::New());
    }
  m_ImageViewer->GetPolygonROIList()->Back()->SetValue(m_ImageViewer->GetNextROILabel());
  bTrainingSet->set();
  bValidationSet->clear();
  this->Update();
}

/**
*
*/
void
SupervisedClassificationAppli
::ValidationSetSelected()
{
  m_ImageViewer->SetPolygonROIList(m_ValidationSet);
  if (m_ImageViewer->GetPolygonROIList()->Size() == 0 ||
      m_ImageViewer->GetPolygonROIList()->Back()->GetVertexList()->Size() > 0)
    {
    m_ImageViewer->GetPolygonROIList()->PushBack(PolygonType::New());
    }
  m_ImageViewer->GetPolygonROIList()->Back()->SetValue(m_ImageViewer->GetNextROILabel());
  bTrainingSet->clear();
  bValidationSet->set();
  this->Update();
}

/** ROI edition callbacks */

/**
*
*/
void
SupervisedClassificationAppli
::EndPolygon()
{
  // Case no polygon yet
  int sizeList = m_ImageViewer->GetPolygonROIList()->Size();
  if (m_ImageViewer->GetPolygonROIList()->Size() == 0)
    {
    m_ImageViewer->GetPolygonROIList()->PushBack(PolygonType::New());

    }
  // if before last is empty
  if (sizeList > 1)
    {
    if (m_ImageViewer->GetPolygonROIList()->GetNthElement(sizeList - 2)->GetVertexList()->Size() == 0)
      {
      m_ImageViewer->GetPolygonROIList()->SetNthElement(sizeList - 2, m_ImageViewer->GetPolygonROIList()->Back());
      m_ImageViewer->GetPolygonROIList()->Erase(sizeList - 1);
      }
    }

  if (m_ImageViewer->GetPolygonROIList()->Size() == 0 ||
      m_ImageViewer->GetPolygonROIList()->Back()->GetVertexList()->Size() > 0)
    {
    m_ImageViewer->GetPolygonROIList()->PushBack(PolygonType::New());

    }

  //FIXME this is a dangerous way to proceed
  m_ImageViewer->GetPolygonROIList()->Back()->SetValue(m_ImageViewer->GetNextROILabel());
  this->Update();
  this->ResetClassification();

}

/**
*
*/
void
SupervisedClassificationAppli
::ClearROIs()
{
  ClassType * theClass = this->GetSelectedClass();

  if (theClass == NULL)
    {
    return;
    }

  std::vector<PolygonListType::Iterator> toRemove;
  for (PolygonListType::Iterator pit = m_ImageViewer->GetPolygonROIList()->Begin();
       pit != m_ImageViewer->GetPolygonROIList()->End(); ++pit)
    {
    if (pit.Get()->GetValue() == theClass->GetId())
      {
      toRemove.push_back(pit);
      }
    }
  for (std::vector<PolygonListType::Iterator>::reverse_iterator it = toRemove.rbegin(); it != toRemove.rend(); ++it)
    {
    m_ImageViewer->GetPolygonROIList()->Erase(*it);
    }

  m_ImageViewer->GetPolygonROIList()->PushBack(PolygonType::New());
  m_ImageViewer->GetPolygonROIList()->Back()->SetValue(m_ImageViewer->GetNextROILabel());
  this->Update();
  this->ResetClassification();
}

/**
*
*/
void
SupervisedClassificationAppli
::EraseLastPoint()
{
  ROIFocus();

  if (static_cast<int>(m_ImageViewer->GetPolygonROIList()->Size()) > 0) //index)
    {
    int          index = m_ImageViewer->GetPolygonROIList()->Size() - 1;
    unsigned int sizeOfThePolygon = m_ImageViewer->GetPolygonROIList()->GetNthElement(index)->GetVertexList()->Size();
    if (sizeOfThePolygon > 0)
      {
      typedef  PolygonType::VertexListType VertexListType;
      typedef  VertexListType::Pointer     VertexListPointerType;

      // itk::PolylineParametricPath does not provide a RemoveVertex() method, and the access to the vertex list is const, so we have no other choice to remove a vertex.
      VertexListPointerType list =
        const_cast<VertexListType *>(m_ImageViewer->GetPolygonROIList()->GetNthElement(index)->GetVertexList());
      list->pop_back();
      }
    }
  else return;

  m_ImageViewer->Update();
}

/**
*
*/
void
SupervisedClassificationAppli
::EraseLastROI()
{
  ROIFocus();
  int index = GetSelectedROI();

  if (index < 0)
    {
    return;
    }

  if (static_cast<int>(m_ImageViewer->GetPolygonROIList()->Size()) > index)
    {
    m_ImageViewer->GetPolygonROIList()->Erase(index);
    }
  this->Update();
  this->ResetClassification();
}

/**
*
*/
void
SupervisedClassificationAppli
::UsePolygon()
{
  if (m_ClassesMap.size() == 0)
    {
    return;
    }
  PolygonType::Pointer pol = PolygonType::New();
  pol->SetValue(m_ImageViewer->GetNextROILabel());
  m_ImageViewer->GetPolygonROIList()->PushBack(pol);

  if (bUsePolygon->value())
    {
    m_ImageViewer->SetPolygonalROISelectionMode(true);
    m_ImageViewer->SetRectangularROISelectionMode(false);
    bEndPolygon->activate();
    bEraseLastPoint->activate();
    }
  else
    {
    m_ImageViewer->SetPolygonalROISelectionMode(false);
    m_ImageViewer->SetRectangularROISelectionMode(true);
    bEndPolygon->deactivate();
    bEraseLastPoint->deactivate();
    }
}

/**
*
*/
void
SupervisedClassificationAppli
::ChangeTrainingSetOpacity()
{
  for (ClassesMapType::iterator it = m_ClassesMap.begin(); it != m_ClassesMap.end(); ++it)
    {
    ColorType color = (*it)->GetColor();
    color[3] = (float) slTrainingSetOpacity->value();
    (*it)->SetColor(color);
    m_ImageViewer->AddROIColorMapEntry((*it)->GetId(), color);
    }
  if (static_cast<int>(bDisplay->value()) == 1)
    {
    m_ResultViewer->SetImageOverlayOpacity(static_cast<unsigned char>(slTrainingSetOpacity->value() * 255));
    }
  this->Update();
}

/**
*
*/
void
SupervisedClassificationAppli
::ROIFocus()
{

  ClassType * theClass = GetSelectedClass();

  if (theClass == NULL)
    {
    return;
    }

  PolygonListPointerType polygonList;
  if (bTrainingSet->value())
    {
    polygonList = m_TrainingSet;
    }
  else
    {
    polygonList = m_ValidationSet;
    }

  int index = GetSelectedROI();

  if (index < 0)
    {
    return;
    }

  if (polygonList->GetNthElement(index)->GetVertexList()->Size() == 0)
    {
    return;
    }

  RegionType boundingRegion = polygonList->GetNthElement(index)->GetBoundingRegion().GetImageRegion();
  IndexType  upperLeft;
  upperLeft[0] = boundingRegion.GetIndex()[0] + boundingRegion.GetSize()[0] / 2;
  upperLeft[1] = boundingRegion.GetIndex()[1] + boundingRegion.GetSize()[1] / 2;

  m_ImageViewer->ChangeFullViewedRegion(upperLeft);
  m_ImageViewer->Update();
}

/**
*
*/
void
SupervisedClassificationAppli
::GenerateTrainingSamplesFromROIs()
{
  m_TrainingListSample->Clear();
  m_TrainingListLabelSample->Clear();

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  ImagePointerType image = m_InputImage;
  unsigned int     sampleSize = image->GetNumberOfComponentsPerPixel();

  for (PolygonListType::Iterator polygIt = m_TrainingSet->Begin();
       polygIt != m_TrainingSet->End(); ++polygIt)
    {
    image->SetRequestedRegion(polygIt.Get()->GetBoundingRegion().GetImageRegion());
    image->PropagateRequestedRegion();
    image->UpdateOutputData();

    IteratorType imageIt(image, polygIt.Get()->GetBoundingRegion().GetImageRegion());
    imageIt.GoToBegin();

    while (!imageIt.IsAtEnd())
      {
      PolygonType::VertexType point;
      point[0] = imageIt.GetIndex()[0];
      point[1] = imageIt.GetIndex()[1];

      if (polygIt.Get()->IsInside(point))
        {

        SampleType         newSample(sampleSize);
        TrainingSampleType newTrainingSample;

        newSample.Fill(0);

        for (unsigned int i = 0; i < sampleSize; ++i)
          {
          newSample[i] = imageIt.Get()[i];
          }

        newTrainingSample[0] = polygIt.Get()->GetValue();

        m_TrainingListSample->PushBack(newSample);
        m_TrainingListLabelSample->PushBack(newTrainingSample);
        }
      ++imageIt;
      }
    }
}

/**
*
*/
void
SupervisedClassificationAppli
::JointlyGenerateTrainingAndValidationSamplesFromROIs()
{

  ImagePointerType image = m_InputImage;
  unsigned int     sampleSize = image->GetNumberOfComponentsPerPixel();

  m_TrainingListSample->Clear();
  m_TrainingListLabelSample->Clear();
  m_ValidationListSample->Clear();
  m_ValidationListLabelSample->Clear();
  m_TrainingListSample->SetMeasurementVectorSize(sampleSize);
  m_ValidationListSample->SetMeasurementVectorSize(sampleSize);  

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  for (PolygonListType::Iterator polygIt = m_TrainingSet->Begin();
       polygIt != m_TrainingSet->End(); ++polygIt)
    {
    image->SetRequestedRegion(polygIt.Get()->GetBoundingRegion().GetImageRegion());
    image->PropagateRequestedRegion();
    image->UpdateOutputData();

    IteratorType imageIt(image, polygIt.Get()->GetBoundingRegion().GetImageRegion());
    imageIt.GoToBegin();

    while (!imageIt.IsAtEnd())
      {
      PolygonType::VertexType point;
      point[0] = imageIt.GetIndex()[0];
      point[1] = imageIt.GetIndex()[1];

      if (polygIt.Get()->IsInside(point))
        {

        SampleType         newSample(sampleSize);
        TrainingSampleType newTrainingSample;

        newSample.Fill(0);

        for (unsigned int i = 0; i < sampleSize; ++i)
          {
          newSample[i] = imageIt.Get()[i];
          }

        newTrainingSample[0] = polygIt.Get()->GetValue();

        if (rand() / (RAND_MAX + 1.0) <= slRandomProbability->value())
          {
          m_TrainingListSample->PushBack(newSample);
          m_TrainingListLabelSample->PushBack(newTrainingSample);
          }
        else
          {
          m_ValidationListSample->PushBack(newSample);
          m_ValidationListLabelSample->PushBack(newTrainingSample);
          }
        }
      ++imageIt;
      }
    }
}

/**
*
*/
void
SupervisedClassificationAppli
::Learn()
{
  bLearn->clear();
  
  if (m_ClassesMap.size() <= 1)
    {
      fl_alert("At least two classes are required to do classification!");
      return;
    }
  
  if (bRandomGeneration->value())
    {
      this->JointlyGenerateTrainingAndValidationSamplesFromROIs();
    }
  else
    {
      this->GenerateTrainingSamplesFromROIs();
    }
  
  // Link map between vector index and existing label
  // classical case          : label (1, 2, 3, 4) => index (0, 1, 2, 3)
  // class 3 supressed       : label (1, 2, 4)    => index (0, 1, 2)
  // class 1 and 3 supressed : label (2, 4)    => index (0, 1)
  
  ClassesMapType::iterator      classesIt = m_ClassesMap.begin();
  unsigned int nbOfLabel = m_ClassesMap.size();
  
  // Get and Sort the label list
  std::vector<unsigned int> labelList;
  while ( classesIt != m_ClassesMap.end() )
    {
      labelList.push_back( (*classesIt)->GetId() );
      ++classesIt;
    }
  std::sort(labelList.begin(), labelList.end());
  
  // Construct the link map (label, index)
  unsigned int count = 0;
  std::map<unsigned int, unsigned int> linkMap;
  for(unsigned int j=0; j<=labelList[nbOfLabel-1]; j++)
    {
      for( unsigned k=0; k<nbOfLabel; k++)
        {
          if( j == labelList[k] )
            {
              linkMap[labelList[k]] = count;
              count++;
            }
        }
    }
  
  TrainingListSampleType::ConstIterator sampleValIter = m_TrainingListLabelSample->Begin();
  TrainingListSampleType::ConstIterator sampleValEnd  = m_TrainingListLabelSample->End();
  
  std::vector<double> sampleCount(m_ClassesMap.size(), 0.);
  
  while (sampleValIter != sampleValEnd)
    {
      sampleCount[ linkMap[sampleValIter.GetMeasurementVector()[0]] ] += 1;
      ++sampleValIter;
    }
  
  classesIt = m_ClassesMap.begin();
  std::vector<double>::iterator countIt = sampleCount.begin();
  
  while (countIt != sampleCount.end() && classesIt != m_ClassesMap.end() )
    {
      if (*countIt == 0)
        {
          std::ostringstream oss;
          oss << "Class " << (*classesIt)->GetName() << " has no training sample, cannot do SVM estimation." << std::endl;
          MsgReporter::GetInstance()->SendError(oss.str().c_str());
          return;
        }
      
      ++countIt;
      ++classesIt;
    }
  
  m_Estimator = EstimatorType::New();
  this->SVMSetupOk();
  m_Estimator->SetInputSampleList(m_TrainingListSample);
  m_Estimator->SetTrainingSampleList(m_TrainingListLabelSample);
  
  try
    {
      m_Estimator->Update();
    }
  catch( itk::ExceptionObject& exc )
    {
      fl_alert("%s", exc.what());
      return;
    }
  
  m_Model = m_Estimator->GetModel();
  
  bLearn->set();
  bSaveClassifAsVectorData->activate();
  bSaveSVMModel->activate();
  bDisplay->activate();
  bValidate->activate();

  // Launch the classification
  this->SetupClassification();

  m_UpToDateResult = false;
}

/**
 *
 */
void
SupervisedClassificationAppli
::SetupClassification()
{
  m_ClassificationFilter->SetModel(m_Model);
  m_ClassificationFilter->SetInput(m_InputImage);
  m_ChangeLabelFilter->ClearChangeMap();
  m_ChangeLabelFilter->SetInput(m_ClassificationFilter->GetOutput());
  m_ChangeLabelFilter->SetNumberOfComponentsPerPixel(3);

  for (ClassesMapType::iterator it = m_ClassesMap.begin(); it != m_ClassesMap.end(); ++it)
    {
    OverlayImageType::PixelType color(3);
    color[0] = static_cast<unsigned char>((*it)->GetColor()[0] * 255);
    color[1] = static_cast<unsigned char>((*it)->GetColor()[1] * 255);
    color[2] = static_cast<unsigned char>((*it)->GetColor()[2] * 255);

    m_ChangeLabelFilter->SetChange((*it)->GetId(), color);
    }

  m_Output = m_ChangeLabelFilter->GetOutput();
  m_HasOutput = true;
  this->NotifyAll();
}

/**
*
*/
void
SupervisedClassificationAppli
::QuitCallback()
{
  m_HasCloseModule = true;
  this->NotifyAll();
  m_HasCloseModule = false;
  m_HasOutput = false;

  bClassesBrowser->hide();
  guiScrollWindow->hide();
  guiFullWindow->hide();
  guiSVMSetup->hide();
  guiVisualisationSetup->hide();
  guiValidationWindow->hide();
  guiChangeClassName->hide();

}

void
SupervisedClassificationAppli
::QuitScrollCallback()
{
  ScrollWidgetPointerType scroll;

  // image viewer shown or result viewer
  if (!m_ResultShown)
    {
    scroll = m_ImageViewer->GetScrollWidget();
    }
  else scroll = m_ResultViewer->GetScrollWidget();

  guiScrollWindow->remove(scroll);
  guiScrollWindow->hide();
}

void
SupervisedClassificationAppli
::QuitFullCallback()
{
  FullWidgetPointerType full;

  // image viewer shown or result viewer
  if (!m_ResultShown)
    {
    full = m_ImageViewer->GetFullWidget();
    }
  else full = m_ResultViewer->GetFullWidget();

  guiFullWindow->remove(full);
  guiFullWindow->hide();
}

/**
*
*/
void
SupervisedClassificationAppli
::ShowImage()
{
   m_ResultViewer->Hide();

   if (m_ResultViewer.IsNotNull())
   {
      FullWidgetPointerType full = m_ResultViewer->GetFullWidget();
      guiFullWindow->remove(full);
      guiFullWindow->redraw();
   
      if (m_ResultViewer->GetUseScroll())
      {
         ScrollWidgetPointerType scroll = m_ResultViewer->GetScrollWidget();
         guiScrollWindow->remove(scroll);
         guiScrollWindow->redraw();
      }
   }


  FullWidgetPointerType full = m_ImageViewer->GetFullWidget();
  full = m_ImageViewer->GetFullWidget();
  full->resize(0, 0, guiFullWindow->w(), guiFullWindow->h());
  guiFullWindow->add(full);
  guiFullWindow->resizable(full);
  guiFullWindow->show();
  full->show();

  if (m_ImageViewer->GetUseScroll())
    {
      ScrollWidgetPointerType scroll = m_ImageViewer->GetScrollWidget();
      guiScrollWindow->add(scroll);
      guiScrollWindow->resizable(scroll);
      guiScrollWindow->show();
      scroll->show();
    }

   this->Update();
   m_ResultShown = false;
}

/**
*
*/
void
SupervisedClassificationAppli
::ShowResults()
{
  if (m_UpToDateResult == false)
  {
      m_ChangeLabelFilter->UpdateOutputInformation();
      m_ChangeLabelFilter->GetOutput()->UpdateOutputInformation();

     // Cast for result viewer display
      m_CastFilter = CastFilterType::New();
      m_CastFilter->SetInput(m_ChangeLabelFilter->GetOutput());
      m_CastFilter->UpdateOutputInformation();

      m_ResultViewer = ImageViewerType::New();
      m_ResultViewer->SetImage(m_InputImage);
      m_ResultViewer->SetImageOverlay(m_CastFilter->GetOutput());
      m_ResultViewer->SetLabeledImage(m_ClassificationFilter->GetOutput());
      m_ResultViewer->SetShowClass(true);
      m_ResultViewer->SetClassesMap(m_ClassesMap);
      m_ResultViewer->SetUseImageOverlay(true);
      m_ResultViewer->SetShowHistograms(false);
      m_ResultViewer->SetPixLocOutput(oPixLocValue);
      m_ResultViewer->SetShowZoomWidget(false);
      m_ResultViewer->SetLabel("Result image");
      m_ResultViewer->Build();
      m_ResultViewer->SetImageOverlayOpacity(static_cast<unsigned char>(slTrainingSetOpacity->value() * 255));
      m_UpToDateResult = true;
  }

  this->VisualisationSetupOk();

  if (m_ImageViewer.IsNotNull())
    {
  FullWidgetPointerType full = m_ImageViewer->GetFullWidget();
  guiFullWindow->remove(full);
  guiFullWindow->redraw();

  if (m_ImageViewer->GetUseScroll())
    {
    ScrollWidgetPointerType scroll = m_ImageViewer->GetScrollWidget();
    guiScrollWindow->remove(scroll);
    guiScrollWindow->redraw();
    }
  }

  FullWidgetPointerType full = m_ResultViewer->GetFullWidget();
  full->resize(0, 0, guiFullWindow->w(), guiFullWindow->h());
  guiFullWindow->add(full);
  guiFullWindow->resizable(full);
  guiFullWindow->show();
  full->show();

  if (m_ResultViewer->GetUseScroll())
    {
    ScrollWidgetPointerType scroll = m_ResultViewer->GetScrollWidget();
    scroll->resize(0, 0, guiScrollWindow->w(), guiScrollWindow->h());
    guiScrollWindow->add(scroll);
    guiScrollWindow->resizable(scroll);
    guiScrollWindow->show();
    scroll->show();
    }
  m_ImageViewer->Hide();
  m_ResultViewer->Show();

  m_ResultShown = true;

}

void
SupervisedClassificationAppli
::DisplayResults()
{
  bDisplay->deactivate();
  if (static_cast<int> (bDisplay->value()) == 0)
    {
    this->ShowImage();
    }
  else
    {
    this->SetupClassification();
    this->ShowResults();
    }
  bDisplay->activate();
}


/**
*
*/
void
SupervisedClassificationAppli
::GenerateValidationSamplesFromROIs()
{

  m_ValidationListSample->Clear();
  m_ValidationListLabelSample->Clear();

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  ImagePointerType image = m_InputImage;
  unsigned int     sampleSize = image->GetNumberOfComponentsPerPixel();

  std::vector<unsigned int> sampleCount(m_ClassesMap.size(), 0);

  for (PolygonListType::Iterator polygIt = m_ValidationSet->Begin();
       polygIt != m_ValidationSet->End(); ++polygIt)
    {
    image->SetRequestedRegion(polygIt.Get()->GetBoundingRegion().GetImageRegion());
    image->PropagateRequestedRegion();
    image->UpdateOutputData();

    IteratorType imageIt(image, polygIt.Get()->GetBoundingRegion().GetImageRegion());
    imageIt.GoToBegin();

    while (!imageIt.IsAtEnd())
      {
      PolygonType::VertexType point;
      point[0] = imageIt.GetIndex()[0];
      point[1] = imageIt.GetIndex()[1];

      if (polygIt.Get()->IsInside(point))
        {
        SampleType         newSample(sampleSize);
        TrainingSampleType newValidationSample;

        newSample.Fill(0);

        for (unsigned int i = 0; i < sampleSize; ++i)
          {
          newSample[i] = imageIt.Get()[i];
          }

        newValidationSample[0] = polygIt.Get()->GetValue();
        sampleCount[polygIt.Get()->GetValue() - 1] += 1;
        m_ValidationListSample->PushBack(newSample);
        m_ValidationListLabelSample->PushBack(newValidationSample);
        }
      ++imageIt;
      }
    }
}

/**
*
*/
void
SupervisedClassificationAppli
::Validate()
{
  if (static_cast<int>(bValidate->value()) == 0)
    {
    guiValidationWindow->hide();
    return;
    }
  if (m_ValidationListSample->Size() == 0)
    {
    std::ostringstream oss;
    oss << "No Validation samples selected..." << std::endl;
    MsgReporter::GetInstance()->SendError(oss.str().c_str());
    return;
    }
  if (!bRandomGeneration->value())
    {
    this->GenerateValidationSamplesFromROIs();
    }

  ClassifierType::Pointer validationClassifier = ClassifierType::New();
  validationClassifier->SetInput(m_ValidationListSample);
  validationClassifier->SetNumberOfClasses(m_ClassesMap.size());
  validationClassifier->SetModel(m_Model);
  validationClassifier->Update();

  ClassifierType::OutputType::Pointer       membershipSample = validationClassifier->GetOutput();
  ClassifierType::OutputType::ConstIterator sampleIter = membershipSample->Begin();
  ClassifierType::OutputType::ConstIterator sampleLast = membershipSample->End();

  TrainingListSampleType::ConstIterator sampleValIter = m_ValidationListLabelSample->Begin();
  TrainingListSampleType::ConstIterator sampleValEnd  = m_ValidationListLabelSample->End();

  ConfusionMatrixType confusion(m_ClassesMap.size(), m_ClassesMap.size());
  confusion.Fill(0);

  std::vector<double> sampleCount(m_ClassesMap.size(), 0.);

  while (sampleIter != sampleLast
         && sampleValIter != sampleValEnd)
    {
    confusion(sampleValIter.GetMeasurementVector()[0] - 1, sampleIter.GetClassLabel() - 1) += 1;
    sampleCount[sampleValIter.GetMeasurementVector()[0] - 1] += 1;
    ++sampleIter;
    ++sampleValIter;
    }

  double overallAccuracy  = 0.;

  for (unsigned int i = 0; i < m_ClassesMap.size(); ++i)
    {
    overallAccuracy += confusion(i, i);
    }

  //Visual does not recognize the accumulate method
  double totalSample = 0.;
  for (std::vector<double>::iterator it = sampleCount.begin(); it != sampleCount.end(); ++it)
    {
    totalSample += (*it);
    }

  overallAccuracy /= totalSample;

  double luckyRate = 0.;
  for (unsigned int i = 0; i < m_ClassesMap.size(); ++i)
    {
    double sum_ij = 0.;
    double sum_ji = 0.;
    for (unsigned int j = 0; j < m_ClassesMap.size(); ++j)
      {
      sum_ij += confusion(i, j);
      sum_ji += confusion(j, i);
      }
    luckyRate += sum_ij * sum_ji;
    }

  luckyRate /= vcl_pow(totalSample, 2.0);

  double kappa = (overallAccuracy - luckyRate) / (1 - luckyRate);

  std::ostringstream oss;
  oss << std::setprecision(2);
  oss << std::fixed;
  std::vector<double> columnSum(m_ClassesMap.size(), 0);

  unsigned int w = 100;
  unsigned int h = 25;
  unsigned int x0 = 50;
  unsigned int y0 = 50;

  guiValidationWindow->resize(guiValidationWindow->x() - 1, guiValidationWindow->y() - 1, 2 * x0 + w *
                              (m_ClassesMap.size() + 2) + 2, 2 * y0 + h * (m_ClassesMap.size() + 8) + 2);
  gMatrix->resizable(NULL);
  gMatrix->resize(x0, y0, (m_ClassesMap.size() + 2) * w, (m_ClassesMap.size() + 2) * h);
  gMatrix->align(FL_ALIGN_TOP);
  guiValidationWindow->add(gMatrix);

  Fl_Output * piece;

  for (unsigned int j = 0; j < m_ClassesMap.size(); ++j)
    {
    oss << m_ClassesMap[j]->GetName();
    Fl_Output * piece = new Fl_Output(x0 + (j + 1) * w, y0, w, h);
    piece->value(oss.str().c_str());

    Fl_Color flColor = fl_color_cube(static_cast<int>((FL_NUM_RED - 1) * m_ClassesMap[j]->GetColor()[0]),
                                     static_cast<int>((FL_NUM_GREEN - 1) * m_ClassesMap[j]->GetColor()[1]),
                                     static_cast<int>((FL_NUM_BLUE - 1) * m_ClassesMap[j]->GetColor()[2]));

    piece->textcolor(flColor);
    piece->textfont(FL_HELVETICA_BOLD);
    gMatrix->add(piece);
    oss.str("");
    }

  piece = new Fl_Output(x0 + (m_ClassesMap.size() + 1) * w, y0, w, h);
  piece->value("Sum");
  piece->textfont(FL_HELVETICA_BOLD);
  gMatrix->add(piece);

  for (unsigned int i = 0; i < m_ClassesMap.size(); ++i)
    {
    double sum = 0;
    oss << m_ClassesMap[i]->GetName();
    piece = new Fl_Output(x0, y0 + (i + 1) * h, w, h);

    Fl_Color flColor = fl_color_cube(static_cast<int>((FL_NUM_RED - 1) * m_ClassesMap[i]->GetColor()[0]),
                                     static_cast<int>((FL_NUM_GREEN - 1) * m_ClassesMap[i]->GetColor()[1]),
                                     static_cast<int>((FL_NUM_BLUE - 1) * m_ClassesMap[i]->GetColor()[2]));

    piece->textcolor(flColor);
    piece->textfont(FL_HELVETICA_BOLD);
    piece->value(oss.str().c_str());

    gMatrix->add(piece);
    oss.str("");

    for (unsigned int j = 0; j < m_ClassesMap.size(); ++j)
      {
      sum += confusion(i, j);
      columnSum[j] += confusion(i, j);
      oss << static_cast<unsigned int>(confusion(i, j));
      if (sampleCount[j] > 0)
        {
        oss << " (" << confusion(i, j) * 100 / sampleCount[j] << "%)";
        }
      piece = new Fl_Output(x0 + (j + 1) * w, y0 + (i + 1) * h, w, h);
      piece->value(oss.str().c_str());
      piece->textcolor(FL_BLACK);
      if (i == j)
        {
        piece->textfont(FL_HELVETICA_BOLD);
        }
      else
        {
        piece->textfont(FL_HELVETICA);
        }
      gMatrix->add(piece);
      oss.str("");
      }
    oss << static_cast<unsigned int>(sum);
    piece = new Fl_Output(x0 + (m_ClassesMap.size() + 1) * w, y0 + (i + 1) * h, w, h);
    piece->value(oss.str().c_str());
    gMatrix->add(piece);
    oss.str("");
    }
  piece = new Fl_Output(x0, y0 + (m_ClassesMap.size() + 1) * h, w, h);
  piece->value("Sum");
  piece->textfont(FL_HELVETICA_BOLD);
  gMatrix->add(piece);

  for (unsigned int i = 0; i < m_ClassesMap.size(); ++i)
    {
    oss << static_cast<unsigned int>(columnSum[i]);
    Fl_Output * piece = new Fl_Output(x0 + (i + 1) * w, y0 + (m_ClassesMap.size() + 1) * h, w, h);
    piece->value(oss.str().c_str());
    gMatrix->add(piece);
    oss.str("");
    }

  gAccuracy->resizable(NULL);
  gAccuracy->resize(x0, y0 + (m_ClassesMap.size() + 3) * h, (m_ClassesMap.size() + 2) * w,
                    (m_ClassesMap.size() + 2) * h);
  gAccuracy->align(FL_ALIGN_TOP);
  guiValidationWindow->add(gAccuracy);

  oss << std::setprecision(4);

  oss << kappa;
  piece = new Fl_Output(x0 + 2 * w, y0 + (m_ClassesMap.size() + 4) * h, w, h, "Kappa");
  piece->value(oss.str().c_str());
  piece->align(FL_ALIGN_LEFT);
  piece->labelfont(FL_HELVETICA_BOLD);
  piece->labelsize(12);
  piece->labelcolor(gAccuracy->labelcolor());
  gAccuracy->add(piece);
  oss.str("");
  oss << overallAccuracy;
  piece = new Fl_Output(x0 + 2 * w, y0 + (m_ClassesMap.size() + 5) * h, w, h, "Overall Accuracy");
  piece->value(oss.str().c_str());
  piece->align(FL_ALIGN_LEFT);
  piece->labelfont(FL_HELVETICA_BOLD);
  piece->labelsize(12);
  piece->labelcolor(gAccuracy->labelcolor());
  gAccuracy->add(piece);
  oss.str("");

  bCloseValidationWindow->position(x0 + 2 * w - bCloseValidationWindow->w() / 2, y0 + (m_ClassesMap.size() + 8) * h);

  guiValidationWindow->show();
}

/**
*
*/
void
SupervisedClassificationAppli
::LoadROIsImage()
{
  typedef otb::ImageFileReader<LabeledImageType>                                 LabeledReaderType;
  typedef otb::ExtractROI<LabeledPixelType, LabeledPixelType>                    ExtractROIFilterType;
  typedef itk::MinimumMaximumImageCalculator<LabeledImageType>                   MinMaxCalculatorType;
  typedef itk::BinaryThresholdImageFilter<LabeledImageType, LabeledImageType>    ThresholdFilterType;
  typedef itk::ConnectedComponentImageFilter<LabeledImageType, LabeledImageType> ConnectedComponentsFilterType;
  typedef itk::RelabelComponentImageFilter<LabeledImageType, LabeledImageType>   RelabelComponentsFilterType;
  typedef otb::ImageToEdgePathFilter<LabeledImageType, PolygonType>              EdgeExtractionFilterType;
  typedef otb::SimplifyPathListFilter<PolygonType>                               SimplifyPathListFilterType;
  typedef otb::StreamingTraits<LabeledImageType>                                 StreamingTraitsType;
  typedef itk::ImageRegionSplitter<2>                                            SplitterType;
  typedef  ImageType::RegionType                                                 RegionType;

  LabeledReaderType::Pointer labeledReader = LabeledReaderType::New();
  labeledReader->SetFileName(m_ROIsImageFileName);

  try
    {
    labeledReader->GenerateOutputInformation();
    }
  catch (itk::ExceptionObject& err)
    {
    std::ostringstream oss;
    oss << "Error while reading labeled image: "  << err << std::endl;
    MsgReporter::GetInstance()->SendError(oss.str().c_str());
    return;
    }

  RegionType largestRegion = labeledReader->GetOutput()->GetLargestPossibleRegion();

  // Setting up local streaming capabilities
  SplitterType::Pointer splitter = SplitterType::New();
  unsigned int          numberOfStreamDivisions =
    StreamingTraitsType::CalculateNumberOfStreamDivisions(labeledReader->GetOutput(),
                                                          largestRegion,
                                                          splitter,
                                                          otb::SET_TILING_WITH_SET_AUTOMATIC_NUMBER_OF_STREAM_DIVISIONS,
                                                          0UL, 0UL, 0UL);
  RegionType                                   streamingRegion;
  std::map<LabeledPixelType, LabeledPixelType> labelTranslationMap;

  for (unsigned int i  = 0; i < m_ClassesMap.size(); ++i)
    {
    labelTranslationMap[m_ClassesMap[i]->GetId()] = m_ClassesMap[i]->GetId();
    }

  PolygonListPointerType polygonList;
  if (bTrainingSet->value())
    {
    polygonList = m_TrainingSet;
    }
  else
    {
    polygonList = m_ValidationSet;
    }

  for (unsigned int piece = 0; piece < numberOfStreamDivisions; ++piece)
    {
    streamingRegion = splitter->GetSplit(piece, numberOfStreamDivisions, largestRegion);

    ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
    extract->SetInput(labeledReader->GetOutput());
    extract->SetExtractionRegion(streamingRegion);
    extract->Update();

    MinMaxCalculatorType::Pointer minMax = MinMaxCalculatorType::New();
    minMax->SetImage(extract->GetOutput());
    minMax->Compute();

    for (LabeledPixelType label = 1; label <= minMax->GetMaximum(); ++label)
      {

      ThresholdFilterType::Pointer labelThreshold = ThresholdFilterType::New();
      labelThreshold->SetInput(extract->GetOutput());
      labelThreshold->SetLowerThreshold(label);
      labelThreshold->SetUpperThreshold(label);
      labelThreshold->SetInsideValue(1);
      labelThreshold->SetOutsideValue(0);
      labelThreshold->Update();

      ConnectedComponentsFilterType::Pointer cc = ConnectedComponentsFilterType::New();
      cc->SetInput(labelThreshold->GetOutput());

      RelabelComponentsFilterType::Pointer relabelcc = RelabelComponentsFilterType::New();
      relabelcc->SetInput(cc->GetOutput());
      relabelcc->Update();

      for (LabeledPixelType cindex = 1; cindex <= relabelcc->GetNumberOfObjects(); ++cindex)
        {
        EdgeExtractionFilterType::Pointer edgeExtractor = EdgeExtractionFilterType::New();
        edgeExtractor->SetInput(relabelcc->GetOutput());
        edgeExtractor->SetForegroundValue(cindex);
        edgeExtractor->Update();

        PolygonListPointerType singleList = PolygonListType::New();
        singleList->PushBack(edgeExtractor->GetOutput());
        SimplifyPathListFilterType::Pointer simplifier = SimplifyPathListFilterType::New();
        simplifier->SetInput(singleList);
        simplifier->Update();

        LabeledPixelType newLabel = label;

        if (labelTranslationMap.find(label) != labelTranslationMap.end())
          {
          newLabel = labelTranslationMap[label];
          }
        else
          {
          this->AddClass();
          newLabel = m_ClassesMap.size();
          labelTranslationMap[label] = newLabel;
          }

        polygonList->PushBack(simplifier->GetOutput()->Back());
        polygonList->Back()->SetValue(newLabel);
        }
      }
    }
  this->Update();
}
} // end namespace otb
