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

#include <FL/Fl_File_Chooser.H>
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

  m_ImageViewer               = ImageViewerType::New();
  m_OutputVector              = VectorDataType::New();
  m_Output                    = OverlayImageType::New();
  m_Estimator                 = EstimatorType::New();
  m_ClassificationFilter      = ClassificationFilterType::New();
  m_ChangeLabelFilter         = ChangeLabelFilterType::New();
  m_VectorizationFilter       = VectorizationFilterType::New();
  m_TrainingSet               = PolygonListType::New();
  m_ValidationSet             = PolygonListType::New();
  m_TrainingListSample        = ListSampleType::New();
  m_ValidationListSample      = ListSampleType::New();
  m_TrainingListLabelSample   = TrainingListSampleType::New();
  m_ValidationListLabelSample = TrainingListSampleType::New();
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
}

/**
*
*/
void
SupervisedClassificationAppli
::Update()
{
  m_ImageViewer->Update();

  if (bDisplay->value())
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
  m_ImageViewer->Build();

  FullWidgetPointerType full = m_ImageViewer->GetFullWidget();
  full->resize(0,0,guiFullWindow->h(),guiFullWindow->w());
  guiFullWindow->resize(guiFullWindow->x(),guiFullWindow->y(),full->w(),full->h());
  guiFullWindow->add(full);
  guiFullWindow->resizable(full);
  guiFullWindow->show();
  full->show();

  if (m_ImageViewer->GetUseScroll())
  {
    ScrollWidgetPointerType scroll = m_ImageViewer->GetScrollWidget();
    guiScrollWindow->resize(guiScrollWindow->x(),guiScrollWindow->y(),scroll->w(),scroll->h());
    guiScrollWindow->add(scroll);
    guiScrollWindow->resizable(scroll);
    guiScrollWindow->show();
    scroll->show();
  }

  /// Add the choice for color composition
  itk::OStringStream oss;
  for (unsigned int i = 0; i<m_InputImage->GetNumberOfComponentsPerPixel();++i)
  {
    oss<<i+1;
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
  bEraseLastPoint->activate();
  bEraseLastROI->activate();
  bClearROIs->activate();
  bEndPolygon->activate();
  bUsePolygon->activate();
  bVisualisationSetup->activate();
  slTrainingSetOpacity->activate();
  bROIFocus->activate();
  bImportVectorData->activate();
  bExportVectorData->activate();
  bExportAllVectorData->activate();
  bImportROIsImage->activate();

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
  const char * cfname = fl_file_chooser("Pick an image file", "*.*",m_LastPath.c_str());
  Fl::check();
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }

  m_ROIsImageFileName = std::string(cfname);
  ossimFilename fname(m_ROIsImageFileName.c_str());
  m_LastPath = fname.path();
  this->LoadROIsImage();
}

/**
*
*/
void
SupervisedClassificationAppli
::ImportVectorData()
{
  const char * cfname = fl_file_chooser("Vector data file :", "*.shp\t*.kml",m_LastPath.c_str());
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  std::string filename = std::string(cfname);
  if (m_ClassesMap.empty())
  {
    this->AddClass();
  }

  VectorDataFileReaderPointerType reader = VectorDataFileReaderType::New();
  reader->SetFileName(filename.c_str());
  reader->Update();

  TreeIteratorType it(reader->GetOutput()->GetDataTree());

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
  const char* dirname = fl_dir_chooser("Location:",m_LastPath.c_str());
  if (dirname == NULL || strlen(dirname)<1)
  {
    return ;
  }

  std::vector<DataTreePointerType> dataTreeVector;
  std::vector<DataNodePointerType> dataNodeVector;
  std::vector<VectorDataPointerType> vectorDataVector;

  for ( ClassesMapType::iterator it = m_ClassesMap.begin();
       it!=m_ClassesMap.end();++it)
  {
    VectorDataPointerType vectorData = VectorDataType::New();

    DataNodePointerType document = DataNodeType::New();
    document->SetNodeType(DOCUMENT);
    document->SetNodeId("polygon");
    DataNodePointerType folder = DataNodeType::New();
    folder->SetNodeType(FOLDER);

    DataTreePointerType tree = vectorData->GetDataTree();
    DataNodePointerType root = tree->GetRoot()->Get();

    tree->Add(document,root);
    tree->Add(folder,document);

    dataTreeVector.push_back(tree);
    dataNodeVector.push_back(folder);
    vectorDataVector.push_back(vectorData);
  }
   PolygonListType::Pointer list = m_ImageViewer->GetPolygonROIList();

  for ( PolygonListType::Iterator it = list->Begin();it!=list->End();++it)
  {
    int classIndex = 0;
    bool found = false;
     ClassesMapType::iterator cit = m_ClassesMap.begin();
    while (cit!=m_ClassesMap.end()&&!found)
    {
      if ((*cit)->GetId()==it.Get()->GetValue())
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
      dataTreeVector[classIndex-1]->Add(newPolygon,dataNodeVector[classIndex-1]);
    }
  }

   std::vector<VectorDataPointerType>::iterator vectorDataVectorIterator =  vectorDataVector.begin();
   ClassesMapType::iterator cit = m_ClassesMap.begin();

  itk::OStringStream oss;

  while ( cit != m_ClassesMap.end() &&vectorDataVectorIterator !=  vectorDataVector.end() )
  {
    std::string filename = (*cit)->GetName();
    std::replace(filename.begin(),filename.end(),' ','_');

    oss<<dirname<<"\\"<<filename<<".shp";

    VectorDataFileWriterPointerType writer = VectorDataFileWriterType::New();
    writer->SetInput(*vectorDataVectorIterator);
    writer->SetFileName(oss.str());
    oss.str("");
    try
    {
      writer->Update();
    }
    catch ( itk::ExceptionObject & err )
    {

      oss.str("");
      oss<<"Error while writing data file: "<<err<<std::endl;
      fl_alert(oss.str().c_str());
    }
    ++cit;
    ++vectorDataVectorIterator;
  }
  ossimFilename fname(dirname);
  m_LastPath = fname.path();
}

/**
*
*/
void
SupervisedClassificationAppli
::ExportVectorData()
{
  const char * cfname = fl_file_chooser("Vector data file :", "*.shp\t*.kml",m_LastPath.c_str());
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  std::string filename = std::string(cfname);


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

  tree->Add(document,root);
  tree->Add(folder,document);
  tree->Add(multiPolygon,folder);

   PolygonListType::Pointer list = m_ImageViewer->GetPolygonROIList();

  for ( PolygonListType::Iterator it = list->Begin();it!=list->End();++it)
  {
    /* Check if vertexes are presents in the polygon to write */
    if (it.Get()->GetVertexList()->size() != 0)
    {
      if (it.Get()->GetValue()==m_ImageViewer->GetNextROILabel())
      {
        DataNodePointerType newPolygon = DataNodeType::New();
        newPolygon->SetNodeType(otb::FEATURE_POLYGON);
        newPolygon->SetPolygonExteriorRing(it.Get());
        tree->Add(newPolygon,multiPolygon);
      }
    }
  }

  VectorDataFileWriterPointerType writer = VectorDataFileWriterType::New();
  writer->SetInput(vectorData);
  writer->SetFileName(filename.c_str());

  try
  {
    writer->Update();
  }
  catch ( itk::ExceptionObject & err )
  {
    itk::OStringStream oss;
    oss<<"Error while writing data file: "<<err<<std::endl;
    fl_alert(oss.str().c_str());
  }

  ossimFilename fname(filename.c_str());
  m_LastPath = fname.path();
}


/**
 *
 */
void
SupervisedClassificationAppli
::SaveClassifAsVectorData()
{
  this->SetupClassification();

  m_VectorizationFilter->Update();

  typedef otb::VectorData<double, 2, unsigned short> VectorDataType;
  VectorDataType::Pointer vectorData = VectorDataType::New();
  VectorDataType::DataTreePointerType tree = vectorData->GetDataTree();
  VectorDataType::DataNodePointerType root = VectorDataType::DataNodeType::New();
  root->SetNodeType(ROOT);
  tree->SetRoot(root);
  VectorDataType::DataNodePointerType document = VectorDataType::DataNodeType::New();
  document->SetNodeType(DOCUMENT);
  tree->Add(document,root);
  VectorDataType::DataNodePointerType folder = VectorDataType::DataNodeType::New();
  folder->SetNodeType(FOLDER);
  tree->Add(folder,document);

  for ( PolygonListType::Iterator it = m_VectorizationFilter->GetPathList()->Begin();
       it != m_VectorizationFilter->GetPathList()->End();
       ++it)
  {
    VectorDataType::DataNodePointerType newDataNode = VectorDataType::DataNodeType::New();
    newDataNode->SetNodeType(FEATURE_POLYGON);
    newDataNode->SetPolygonExteriorRing(it.Get());//Vertex list ?
    tree->Add(newDataNode,folder);
  }


  typedef otb::VectorDataProjectionFilter<VectorDataType,VectorDataType> ProjectionFilterType;
  ProjectionFilterType::Pointer vectorDataProjection = ProjectionFilterType::New();
  vectorDataProjection->SetInput(vectorData);
  vectorDataProjection->SetInputOrigin(m_InputImage->GetOrigin());
  vectorDataProjection->SetInputSpacing(m_InputImage->GetSpacing());

  std::string projectionRef;
  itk::ExposeMetaData<std::string>(m_InputImage->GetMetaDataDictionary(),
                                   MetaDataKey::ProjectionRefKey, projectionRef );
  vectorDataProjection->SetInputProjectionRef(projectionRef);

  vectorDataProjection->SetInputKeywordList(m_InputImage->GetImageKeywordlist());

   m_OutputVector = vectorDataProjection->GetOutput();

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
  const char * cfname = fl_file_chooser("SVM model file :", "*.svm",m_LastPath.c_str());
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }
  m_ModelFileName = std::string(cfname);
  
  ossimFilename fname(m_ModelFileName.c_str());
  m_LastPath = fname.path();
  this->LoadSVMModel();
}

/**
*
*/
void
SupervisedClassificationAppli
::LoadSVMModel()
{
  m_Model = ModelType::New();
  m_Model->LoadModel(m_ModelFileName.c_str());

  m_ClassesMap.clear();
  m_CurrentLabel = 1;

  for (unsigned int i = 0;i<m_Model->GetNumberOfClasses();++i)
  {
    this->AddClass();
  }

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
  const char * cfname = fl_file_chooser("SVM model file :", "*.svm",m_LastPath.c_str());
  if (cfname == NULL || strlen(cfname)<1)
  {
    return ;
  }

  std::string filename = std::string(cfname);
  m_Estimator->SaveModel(filename.c_str());
  ossimFilename fname(filename.c_str());
  m_LastPath = fname.path();
}

/** Setup Callbacks */

/**
*
*/
void
SupervisedClassificationAppli
:: SVMSetup()
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
  guiSVMSetup->show();
}

/**
*
*/
void
SupervisedClassificationAppli
:: SVMSetupOk()
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
  if (bDisplay->value())
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
:: SVMSetupCancel()
{
  guiSVMSetup->hide();
}

/**
*
*/
void
SupervisedClassificationAppli
:: VisualisationSetup()
{
  itk::OStringStream oss;
  for (unsigned int i = 0;i<m_InputImage->GetNumberOfComponentsPerPixel();++i)
  {
    oss.str("");
    oss<<i+1;
    viewerGrayscaleChannelChoice->add(oss.str().c_str());
    viewerRedChannelChoice->add(oss.str().c_str());
    viewerGreenChannelChoice->add(oss.str().c_str());
    viewerBlueChannelChoice->add(oss.str().c_str());
    viewerGrayscaleChannelChoice->add(oss.str().c_str());
    viewerRealChannelChoice->add(oss.str().c_str());
    viewerImaginaryChannelChoice->add(oss.str().c_str());
  }

  if (m_ImageViewer->GetViewModel()==ImageViewerType::ScrollWidgetType::RGB)
  {
    RGBSet();
  }
  else if (m_ImageViewer->GetViewModel()==ImageViewerType::ScrollWidgetType::GRAYSCALE)
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
:: VisualisationSetupOk()
{

  if (m_ResultViewer.IsNotNull())
  {
    if (viewerColorMode->value())
    {
      m_ResultViewer->SetRedChannelIndex(atoi(viewerRedChannelChoice->value())-1);
      m_ResultViewer->SetGreenChannelIndex(atoi(viewerGreenChannelChoice->value())-1);
      m_ResultViewer->SetBlueChannelIndex(atoi(viewerBlueChannelChoice->value())-1);
      m_ResultViewer->SetViewModel(ImageViewerType::ScrollWidgetType::RGB);
    }
    else if (viewerGrayscaleMode->value())
    {
      m_ResultViewer->SetRedChannelIndex(atoi(viewerGrayscaleChannelChoice->value())-1);
      m_ResultViewer->SetViewModel(ImageViewerType::ScrollWidgetType::GRAYSCALE);
    }
    else if (viewerComplexMode->value())
    {
      m_ResultViewer->SetRedChannelIndex(atoi(viewerRealChannelChoice->value())-1);
      m_ResultViewer->SetGreenChannelIndex(atoi(viewerImaginaryChannelChoice->value())-1);
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
    m_ImageViewer->SetRedChannelIndex(atoi(viewerRedChannelChoice->value())-1);
    m_ImageViewer->SetGreenChannelIndex(atoi(viewerGreenChannelChoice->value())-1);
    m_ImageViewer->SetBlueChannelIndex(atoi(viewerBlueChannelChoice->value())-1);
    m_ImageViewer->SetViewModel(ImageViewerType::ScrollWidgetType::RGB);
  }
  else if (viewerGrayscaleMode->value())
  {
    m_ImageViewer->SetRedChannelIndex(atoi(viewerGrayscaleChannelChoice->value())-1);
    m_ImageViewer->SetViewModel(ImageViewerType::ScrollWidgetType::GRAYSCALE);
  }
  else if (viewerComplexMode->value())
  {
    m_ImageViewer->SetRedChannelIndex(atoi(viewerRealChannelChoice->value())-1);
    m_ImageViewer->SetGreenChannelIndex(atoi(viewerImaginaryChannelChoice->value())-1);
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
:: VisualisationSetupCancel()
{
  guiVisualisationSetup->hide();
}

/**
*
*/
void
SupervisedClassificationAppli
:: GrayscaleSet()
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
:: RGBSet()
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
:: ComplexSet()
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
:: ClassSelected()
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
:: AddClass()
{
  itk::OStringStream oss;

  ClassPointerType newClass = ClassType::New();
  oss<<"Class "<<m_CurrentLabel;
  newClass->SetName(oss.str());
  newClass->SetId(m_CurrentLabel);
  ColorType color;
  //FIXME the color selection should be done with a proper lookup table class.
  srand(m_CurrentLabel+123456);
  color[0]=rand()/(RAND_MAX+1.0);
  color[1]=rand()/(RAND_MAX+1.0);
  color[2]=rand()/(RAND_MAX+1.0);
  color[3]=(float)this->slTrainingSetOpacity->value();

  fl_color(static_cast<unsigned char>(255*color[0]),
           static_cast<unsigned char>(255*color[1]),
           static_cast<unsigned char>(255*color[2]));

  dClassList->selection_color(fl_color());

  newClass->SetColor(color);
  m_ClassesMap.push_back(newClass);
  dClassList->add(oss.str().c_str());
  dClassList->select(dClassList->size());
  m_ImageViewer->AddROIColorMapEntry(m_CurrentLabel,color);
  ++m_CurrentLabel;

  this->ResetClassification();
  this->ClassSelected();
  this->UsePolygon();
}

/**
*
*/
void
SupervisedClassificationAppli
:: RemoveClass()
{
  unsigned int selectedItem = dClassList->value();

  if (selectedItem == 0)
  {
    fl_alert("No class selected.");
    return;
  }

  if (m_ClassesMap.size()==1)
  {
    m_ImageViewer->SetPolygonalROISelectionMode(false);
    m_ImageViewer->SetRectangularROISelectionMode(false);
  }


  ClassType * theClass = this->GetSelectedClass();

  if (theClass == NULL)
  {
    return;
  }

  std::vector< PolygonListType::Iterator> toRemove;
  for ( PolygonListType::Iterator pit = m_TrainingSet->Begin();
       pit!=m_TrainingSet->End();++pit)
  {
    if (pit.Get()->GetValue()==theClass->GetId())
    {
      toRemove.push_back(pit);
    }
  }
  for ( std::vector< PolygonListType::Iterator>::reverse_iterator it = toRemove.rbegin();it!=toRemove.rend();++it)
  {
    m_TrainingSet->Erase(*it);
  }
  toRemove.clear();
  for ( PolygonListType::Iterator pit = m_ValidationSet->Begin();
       pit!=m_ValidationSet->End();++pit)
  {
    if (pit.Get()->GetValue()==theClass->GetId())
    {
      toRemove.push_back(pit);
    }
  }
  for ( std::vector< PolygonListType::Iterator>::reverse_iterator it = toRemove.rbegin();it!=toRemove.rend();++it)
  {
    m_ValidationSet->Erase(*it);
  }
  m_ClassesMap.erase(m_ClassesMap.begin()+selectedItem-1);
  dClassList->remove(selectedItem);

  this->Update();
  this->ResetClassification();
}

/**
*
*/
void
SupervisedClassificationAppli
:: ChangeClassName()
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
:: ChangeClassNameOk()
{
  guiChangeClassName->hide();

  unsigned int selectedItem = dClassList->value();

  if (selectedItem == 0)
  {
    return;
  }
  ClassType * theClass = m_ClassesMap[selectedItem-1];
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
    resp = m_ClassesMap[selectedItem-1];
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

  int ClassROICounter = 0;
  unsigned int ROICounter = 0;
   PolygonListType::Iterator it = polygonList->Begin();

  while (it!=polygonList->End() && ClassROICounter < dROIList->value())
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
  return (ROICounter-1);
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

  fl_color(static_cast<unsigned char>(255*theClass->GetColor()[0]),
           static_cast<unsigned char>(255*theClass->GetColor()[1]),
           static_cast<unsigned char>(255*theClass->GetColor()[2]));

  dClassList->selection_color(fl_color());
  dClassList->text(dClassList->value(),theClass->GetName().c_str());
  dClassList->redraw();

  dClassColor->color(fl_color());
  dClassColor->redraw();

  dClassInfo->buffer()->remove(0,dClassInfo->buffer()->length());

  itk::OStringStream oss;
  oss<<"Class name: "<<theClass->GetName()<<" (Id: "<<theClass->GetId() <<")";
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


  for ( PolygonListType::Iterator it = polygonList->Begin();it!=polygonList->End();++it)
  {
    if (it.Get()->GetValue()==theClass->GetId())
    {
      oss.str("");
      oss<<"ROI "<<ROICounter<<", "<<it.Get()->GetArea()<<" pixels";
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
  itk::OStringStream oss;
  oss.str("");
  // Clear the info buffer
  dImageInfo->buffer()->remove(0,dImageInfo->buffer()->length());
  oss<<"Filename: "<<m_ImageFileName<<std::endl;
  dImageInfo->insert(oss.str().c_str());
  oss.str("");
  oss<<"Number of bands: "<<m_InputImage->GetNumberOfComponentsPerPixel();
  oss<<" - Size: "<<m_InputImage->GetLargestPossibleRegion().GetSize()<<std::endl;
  dImageInfo->insert(oss.str().c_str());
  oss.str("");
  if (m_ImageViewer->GetUseScroll())
  {
    oss<<"Scroll window activated"<<std::endl;
    dImageInfo->insert(oss.str().c_str());
    oss.str("");
  }

  oss.str("");
  switch (m_ImageViewer->GetViewModel())
  {
  case ImageViewerType::ScrollWidgetType::RGB:
  {
    oss<<"RGB Composition: ";
    oss<<" Band 1: "<<m_ImageViewer->GetRedChannelIndex();
    oss<<" Band 2: "<<m_ImageViewer->GetGreenChannelIndex();
    oss<<" Band 3: "<<m_ImageViewer->GetBlueChannelIndex()<<std::endl;
    break;
  }
  case ImageViewerType::ScrollWidgetType::GRAYSCALE:
  {
    oss<<"Grayscale: ";
    oss<<" Band: "<<m_ImageViewer->GetRedChannelIndex()<<std::endl;
    break;
  }
  case ImageViewerType::ScrollWidgetType::COMPLEX_MODULUS:
  {
    oss<<"Complex modulus: ";
    oss<<" Real part: "     <<m_ImageViewer->GetRedChannelIndex();
    oss<<" Imaginary part: "<<m_ImageViewer->GetGreenChannelIndex();
    break;
  }
  case ImageViewerType::ScrollWidgetType::COMPLEX_PHASE:
  {
    oss<<"Complex phase: ";
    oss<<" Real part: "     <<m_ImageViewer->GetRedChannelIndex();
    oss<<" Imaginary part: "<<m_ImageViewer->GetGreenChannelIndex();
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
:: ChangeClassColor()
{


  ClassType * theClass = this->GetSelectedClass();

  if (theClass == NULL)
  {
    return;
  }

  double r = (double)theClass->GetColor()[0];
  double g = (double)theClass->GetColor()[1];
  double b = (double)theClass->GetColor()[2];

  int ok = fl_color_chooser("Changed class color:",r,g,b);

  if (ok)
  {
    ColorType newColor;
    newColor[0]=r;
    newColor[1]=g;
    newColor[2]=b;
    newColor[3]=0.75;
    theClass->SetColor(newColor);
    m_ImageViewer->AddROIColorMapEntry(theClass->GetId(),newColor);

     OverlayImageType::PixelType color(3);
    color[0]=static_cast<unsigned char>(theClass->GetColor()[0]*255);
    color[1]=static_cast<unsigned char>(theClass->GetColor()[1]*255);
    color[2]=static_cast<unsigned char>(theClass->GetColor()[2]*255);
    m_ChangeLabelFilter->SetChange(theClass->GetId(),color);

    if (bDisplay->value())
    {
      m_ResultViewer->Reset();
    }

    this->Update();
  }
}

/**
*
*/
void
SupervisedClassificationAppli
:: TrainingSetSelected()
{
  m_ImageViewer->SetPolygonROIList(m_TrainingSet);
  if (m_ImageViewer->GetPolygonROIList()->Size() == 0 || m_ImageViewer->GetPolygonROIList()->Back()->GetVertexList()->Size()>0)
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
:: ValidationSetSelected()
{
  m_ImageViewer->SetPolygonROIList(m_ValidationSet);
  if (m_ImageViewer->GetPolygonROIList()->Size() == 0 || m_ImageViewer->GetPolygonROIList()->Back()->GetVertexList()->Size()>0)
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
:: EndPolygon()
{
  // Case no polygon yet
  int sizeList = m_ImageViewer->GetPolygonROIList()->Size();
  if (m_ImageViewer->GetPolygonROIList()->Size() == 0 )
  {
    m_ImageViewer->GetPolygonROIList()->PushBack(PolygonType::New());

  }
  // if before last is empty
  if (sizeList>1)
  {
    if ( m_ImageViewer->GetPolygonROIList()->GetNthElement(sizeList-2)->GetVertexList()->Size() == 0 )
    {
      m_ImageViewer->GetPolygonROIList()->SetNthElement(sizeList-2, m_ImageViewer->GetPolygonROIList()->Back());
      m_ImageViewer->GetPolygonROIList()->Erase(sizeList-1);
    }
  }

  if (m_ImageViewer->GetPolygonROIList()->Size() == 0 || m_ImageViewer->GetPolygonROIList()->Back()->GetVertexList()->Size()>0)
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
:: ClearROIs()
{
  ClassType * theClass = this->GetSelectedClass();

  if (theClass == NULL)
  {
    return;
  }

  std::vector< PolygonListType::Iterator> toRemove;
  for ( PolygonListType::Iterator pit = m_ImageViewer->GetPolygonROIList()->Begin();
       pit!=m_ImageViewer->GetPolygonROIList()->End();++pit)
  {
    if (pit.Get()->GetValue()==theClass->GetId())
    {
      toRemove.push_back(pit);
    }
  }
  for ( std::vector< PolygonListType::Iterator>::reverse_iterator it = toRemove.rbegin();it!=toRemove.rend();++it)
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
:: EraseLastPoint()
{
  ROIFocus();
  int index = GetSelectedROI();

  if (index<0)
  {
    return;
  }

  if (static_cast<int>(m_ImageViewer->GetPolygonROIList()->Size())>index)
  {
    unsigned int sizeOfThePolygon = m_ImageViewer->GetPolygonROIList()->GetNthElement(index)->GetVertexList()->Size();
    if (sizeOfThePolygon>0)
    {
      typedef  PolygonType::VertexListType VertexListType;
      typedef  VertexListType::Pointer VertexListPointerType;

      // itk::PolylineParametricPath does not provide a RemoveVertex() method, and the access to the vertex list is const, so we have no other choice to remove a vertex.
      VertexListPointerType list = const_cast<VertexListType *>(m_ImageViewer->GetPolygonROIList()->GetNthElement(index)->GetVertexList());
      list->pop_back();
    }
  }
  this->Update();
  this->ResetClassification();
}

/**
*
*/
void
SupervisedClassificationAppli
:: EraseLastROI()
{
  ROIFocus();
  int index = GetSelectedROI();

  if (index<0)
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
:: UsePolygon()
{
  if (m_ClassesMap.size()==0)
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
  for ( ClassesMapType::iterator it = m_ClassesMap.begin(); it!=m_ClassesMap.end();++it)
  {
    ColorType color = (*it)->GetColor();
    color[3]=(float)slTrainingSetOpacity->value();
    (*it)->SetColor(color);
    m_ImageViewer->AddROIColorMapEntry((*it)->GetId(),color);
  }
  if (bDisplay->value())
  {
    m_ResultViewer->SetImageOverlayOpacity(static_cast<unsigned char>(slTrainingSetOpacity->value()*255));
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

  if (index<0)
  {
    return;
  }

  if (polygonList->GetNthElement(index)->GetVertexList()->Size()==0)
  {
    return;
  }

  RegionType boundingRegion = polygonList->GetNthElement(index)->GetBoundingRegion().GetImageRegion();
  IndexType upperLeft;
  upperLeft[0] = boundingRegion.GetIndex()[0] + boundingRegion.GetSize()[0]/2;
  upperLeft[1] = boundingRegion.GetIndex()[1] + boundingRegion.GetSize()[1]/2;

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
  unsigned int sampleSize = image->GetNumberOfComponentsPerPixel();

  for ( PolygonListType::Iterator polygIt = m_TrainingSet->Begin();
       polygIt!=m_TrainingSet->End();++polygIt)
    {
    image->SetRequestedRegion(polygIt.Get()->GetBoundingRegion().GetImageRegion());
    image->PropagateRequestedRegion();
    image->UpdateOutputData();

    IteratorType imageIt(image,polygIt.Get()->GetBoundingRegion().GetImageRegion());
    imageIt.GoToBegin();

    while (!imageIt.IsAtEnd())
    {
       PolygonType::VertexType point;
      point[0]=imageIt.GetIndex()[0];
      point[1]=imageIt.GetIndex()[1];

      if (polygIt.Get()->IsInside(point))
      {

        SampleType newSample(sampleSize);
        TrainingSampleType newTrainingSample;

        newSample.Fill(0);

        for (unsigned int i = 0;i<sampleSize;++i)
        {
          newSample[i]=imageIt.Get()[i];
        }

        newTrainingSample[0]=polygIt.Get()->GetValue();

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

  m_TrainingListSample->Clear();
  m_TrainingListLabelSample->Clear();
  m_ValidationListSample->Clear();
  m_ValidationListLabelSample->Clear();

  typedef itk::ImageRegionIteratorWithIndex<ImageType> IteratorType;

  ImagePointerType image = m_InputImage;
  unsigned int sampleSize = image->GetNumberOfComponentsPerPixel();

  for ( PolygonListType::Iterator polygIt = m_TrainingSet->Begin();
       polygIt!=m_TrainingSet->End();++polygIt)
  {
    image->SetRequestedRegion(polygIt.Get()->GetBoundingRegion().GetImageRegion());
    image->PropagateRequestedRegion();
    image->UpdateOutputData();

    IteratorType imageIt(image,polygIt.Get()->GetBoundingRegion().GetImageRegion());
    imageIt.GoToBegin();

    while (!imageIt.IsAtEnd())
    {
       PolygonType::VertexType point;
      point[0]=imageIt.GetIndex()[0];
      point[1]=imageIt.GetIndex()[1];

      if (polygIt.Get()->IsInside(point))
      {

      SampleType newSample(sampleSize);
        TrainingSampleType newTrainingSample;

        newSample.Fill(0);

        for (unsigned int i = 0;i<sampleSize;++i)
        {
          newSample[i]=imageIt.Get()[i];
        }

        newTrainingSample[0]=polygIt.Get()->GetValue();

        if (rand()/(RAND_MAX+1.0)<=slRandomProbability->value())
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

  if (m_ClassesMap.size()<=1)
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

   TrainingListSampleType::ConstIterator sampleValIter = m_TrainingListLabelSample->Begin();
   TrainingListSampleType::ConstIterator sampleValEnd  = m_TrainingListLabelSample->End();

  std::vector<double> sampleCount(m_ClassesMap.size(),0.);

  while (sampleValIter != sampleValEnd)
  {
    sampleCount[sampleValIter.GetMeasurementVector()[0]-1]+=1;
    ++sampleValIter;
  }

   ClassesMapType::iterator classesIt = m_ClassesMap.begin();
  std::vector<double>::iterator countIt = sampleCount.begin();

  while (countIt!=sampleCount.end()&&classesIt!=m_ClassesMap.end())
  {
    if (*countIt == 0)
    {
      itk::OStringStream oss;
      oss<<"Class "<<(*classesIt)->GetName()<<" has no training sample, cannot do SVM estimation."<<std::endl;
      fl_alert(oss.str().c_str());
      return;
    }
    ++countIt;
    ++classesIt;
  }

  m_Estimator->SetInputSampleList(m_TrainingListSample);
  m_Estimator->SetTrainingSampleList(m_TrainingListLabelSample);
  m_Estimator->SetNumberOfClasses(m_ClassesMap.size());
  m_Estimator->Update();

  m_Model = m_Estimator->GetModel();


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
::SetupClassification()
{
  m_ClassificationFilter = ClassificationFilterType::New();
  m_ClassificationFilter->SetModel(m_Model);
  m_ClassificationFilter->SetInput(m_InputImage);
  m_ChangeLabelFilter->SetInput(m_ClassificationFilter->GetOutput());
  m_ChangeLabelFilter->SetNumberOfComponentsPerPixel(3);

  for ( ClassesMapType::iterator it = m_ClassesMap.begin();it!=m_ClassesMap.end();++it)
  {
     OverlayImageType::PixelType color(3);

    color[0]=static_cast<unsigned char>((*it)->GetColor()[0]*255);
    color[1]=static_cast<unsigned char>((*it)->GetColor()[1]*255);
    color[2]=static_cast<unsigned char>((*it)->GetColor()[2]*255);
    m_ChangeLabelFilter->SetChange((*it)->GetId(),color);
  }

  m_VectorizationFilter->SetInput(m_ClassificationFilter->GetOutput());

  m_Output = m_ChangeLabelFilter->GetOutput();
  m_HasOutput = true;
}

/**
*
*/
void
SupervisedClassificationAppli
::QuitCallback()
{
  this->NotifyAll();
  m_HasOutput = false;

  bClassesBrowser->hide();
  guiScrollWindow->hide();
  guiFullWindow->hide();
  guiSVMSetup->hide();
  guiVisualisationSetup->hide();
  guiValidationWindow->hide();
  guiChangeClassName->hide();

}

/**
*
*/
void
SupervisedClassificationAppli
::ShowImage()
{
  FullWidgetPointerType full = m_ResultViewer->GetFullWidget();
  guiFullWindow->remove(full);

  if (m_ImageViewer->GetUseScroll())
  {
    ScrollWidgetPointerType scroll = m_ResultViewer->GetScrollWidget();
    guiScrollWindow->remove(scroll);
  }

  full = m_ImageViewer->GetFullWidget();
  full->resize(0,0,guiFullWindow->w(),guiFullWindow->h());
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
  m_ImageViewer->Update();
  this->Update();
}

/**
*
*/
void
SupervisedClassificationAppli
::ShowResults()
{
  m_ChangeLabelFilter->GetOutput()->UpdateOutputInformation();

  m_ResultViewer = ImageViewerType::New();
  m_ResultViewer->SetImage(m_InputImage);
  m_ResultViewer->SetImageOverlay(m_ChangeLabelFilter->GetOutput());
  m_ResultViewer->SetLabeledImage(m_ClassificationFilter->GetOutput());
  m_ResultViewer->SetShowClass(true);
  m_ResultViewer->SetClassesMap(m_ClassesMap);
  m_ResultViewer->SetUseImageOverlay(true);
  m_ResultViewer->SetShowHistograms(false);
  m_ResultViewer->SetPixLocOutput(oPixLocValue);
  m_ResultViewer->SetShowZoomWidget(false);
  m_ResultViewer->SetLabel("Result image");
  m_ResultViewer->Build();
  m_ResultViewer->SetImageOverlayOpacity(static_cast<unsigned char>(slTrainingSetOpacity->value()*255));
  this->VisualisationSetupOk();

  FullWidgetPointerType full = m_ImageViewer->GetFullWidget();
  guiFullWindow->remove(full);
  guiFullWindow->redraw();

  if (m_ImageViewer->GetUseScroll())
  {
    ScrollWidgetPointerType scroll = m_ImageViewer->GetScrollWidget();
    guiScrollWindow->remove(scroll);
    guiScrollWindow->redraw();
  }

  full = m_ResultViewer->GetFullWidget();
  full->resize(0,0,guiFullWindow->w(),guiFullWindow->h());
  guiFullWindow->add(full);
  guiFullWindow->resizable(full);
  guiFullWindow->show();
  full->show();

  if (m_ResultViewer->GetUseScroll())
  {
    ScrollWidgetPointerType scroll = m_ResultViewer->GetScrollWidget();
    scroll->resize(0,0,guiScrollWindow->w(),guiScrollWindow->h());
    guiScrollWindow->add(scroll);
    guiScrollWindow->resizable(scroll);
    guiScrollWindow->show();
    scroll->show();
  }
  m_ImageViewer->Hide();
  m_ResultViewer->Show();

}

void
SupervisedClassificationAppli
::DisplayResults()
{

  if (!bDisplay->value())
  {
    this->ShowImage();
  }
  else
  {
    this->SetupClassification();
    this->ShowResults();
  }
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
  unsigned int sampleSize = image->GetNumberOfComponentsPerPixel();

  std::vector<unsigned int> sampleCount(m_ClassesMap.size(),0);

  for ( PolygonListType::Iterator polygIt = m_ValidationSet->Begin();
       polygIt!=m_ValidationSet->End();++polygIt)
  {
    image->SetRequestedRegion(polygIt.Get()->GetBoundingRegion().GetImageRegion());
    image->PropagateRequestedRegion();
    image->UpdateOutputData();

    IteratorType imageIt(image,polygIt.Get()->GetBoundingRegion().GetImageRegion());
    imageIt.GoToBegin();

    while (!imageIt.IsAtEnd())
    {
       PolygonType::VertexType point;
      point[0]=imageIt.GetIndex()[0];
      point[1]=imageIt.GetIndex()[1];

      if (polygIt.Get()->IsInside(point))
      {
        SampleType newSample(sampleSize);
        TrainingSampleType newValidationSample;

        newSample.Fill(0);

        for (unsigned int i = 0;i<sampleSize;++i)
        {
          newSample[i]=imageIt.Get()[i];
        }

        newValidationSample[0]=polygIt.Get()->GetValue();
        sampleCount[polygIt.Get()->GetValue()-1]+=1;
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
  if (!bValidate->value())
  {
    guiValidationWindow->hide();
    return;
  }

  if (!bRandomGeneration->value())
  {
    this->GenerateValidationSamplesFromROIs();
  }

   ClassifierType::Pointer validationClassifier = ClassifierType::New();
  validationClassifier->SetSample(m_ValidationListSample);
  validationClassifier->SetNumberOfClasses(m_ClassesMap.size());
  validationClassifier->SetModel(m_Model);
  validationClassifier->Update();

    ClassifierType::OutputType::Pointer membershipSample = validationClassifier->GetOutput();
   ClassifierType::OutputType::ConstIterator sampleIter = membershipSample->Begin();
   ClassifierType::OutputType::ConstIterator sampleLast = membershipSample->End();

   TrainingListSampleType::ConstIterator sampleValIter = m_ValidationListLabelSample->Begin();
   TrainingListSampleType::ConstIterator sampleValEnd  = m_ValidationListLabelSample->End();

  ConfusionMatrixType confusion(m_ClassesMap.size(),m_ClassesMap.size());
  confusion.Fill(0);

  std::vector<double> sampleCount(m_ClassesMap.size(),0.);

  while (sampleIter != sampleLast
         && sampleValIter != sampleValEnd)
  {
    confusion(sampleValIter.GetMeasurementVector()[0]-1,sampleIter.GetClassLabel()-1)+=1;
    sampleCount[sampleValIter.GetMeasurementVector()[0]-1]+=1;
    ++sampleIter;
    ++sampleValIter;
  }

  double overallAccuracy  = 0.;

  for (unsigned int i = 0;i<m_ClassesMap.size();++i)
  {
    overallAccuracy += confusion(i,i);
  }

  //Visual does not recognize the accumulate method
  double totalSample = 0.;
  for (std::vector<double>::iterator it = sampleCount.begin();it!=sampleCount.end();++it)
  {
    totalSample +=(*it);
  }

  overallAccuracy/=totalSample;


  double luckyRate = 0.;
  for (unsigned int i = 0;i<m_ClassesMap.size();++i)
  {
    double sum_ij = 0.;
    double sum_ji = 0.;
    for (unsigned int j = 0;j<m_ClassesMap.size();++j)
    {
      sum_ij +=confusion(i,j);
      sum_ji +=confusion(j,i);
    }
    luckyRate+=sum_ij*sum_ji;
  }


  luckyRate/=vcl_pow(totalSample,2.0);

  double kappa = (overallAccuracy-luckyRate)/(1-luckyRate);

  itk::OStringStream oss;
  oss<<std::setprecision(2);
  oss<<std::fixed;
  std::vector<double> columnSum(m_ClassesMap.size(),0);

  unsigned int w = 100;
  unsigned int h = 25;
  unsigned int x0 = 50;
  unsigned int y0 = 50;

  guiValidationWindow->resize(guiValidationWindow->x()-1,guiValidationWindow->y()-1,2*x0+w*(m_ClassesMap.size()+2)+2,2*y0+h*(m_ClassesMap.size()+8)+2);
  gMatrix->resizable(NULL);
  gMatrix->resize(x0,y0,(m_ClassesMap.size()+2)*w,(m_ClassesMap.size()+2)*h);
  gMatrix->align(FL_ALIGN_TOP);
  guiValidationWindow->add(gMatrix);

  Fl_Output * piece;

  for (unsigned int j = 0;j<m_ClassesMap.size();++j)
  {
    oss<<m_ClassesMap[j]->GetName();
    Fl_Output * piece = new Fl_Output(x0+(j+1)*w,y0,w,h);
    piece->value(oss.str().c_str());
    fl_color(static_cast<unsigned char>(255*m_ClassesMap[j]->GetColor()[0]),
             static_cast<unsigned char>(255*m_ClassesMap[j]->GetColor()[1]),
             static_cast<unsigned char>(255*m_ClassesMap[j]->GetColor()[2]));
    piece->textcolor(fl_color());
    piece->textfont(FL_HELVETICA_BOLD);
    gMatrix->add(piece);
    oss.str("");
  }

  piece = new Fl_Output(x0+(m_ClassesMap.size()+1)*w,y0,w,h);
  piece->value("Sum");
  piece->textfont(FL_HELVETICA_BOLD);
  gMatrix->add(piece);


  for (unsigned int i = 0;i<m_ClassesMap.size();++i)
  {
    double sum = 0;
    oss<<m_ClassesMap[i]->GetName();
    piece = new Fl_Output(x0,y0+(i+1)*h,w,h);
    fl_color(static_cast<unsigned char>(255*m_ClassesMap[i]->GetColor()[0]),
             static_cast<unsigned char>(255*m_ClassesMap[i]->GetColor()[1]),
             static_cast<unsigned char>(255*m_ClassesMap[i]->GetColor()[2]));
    piece->textcolor(fl_color());
    piece->textfont(FL_HELVETICA_BOLD);
    piece->value(oss.str().c_str());

    gMatrix->add(piece);
    oss.str("");

    for (unsigned int j = 0;j<m_ClassesMap.size();++j)
    {
      sum += confusion(i,j);
      columnSum[j]+=confusion(i,j);
      oss<<static_cast<unsigned int>(confusion(i,j));
      if (sampleCount[j]>0)
      {
        oss<<" ("<<confusion(i,j)*100/sampleCount[j]<<"%)";
      }
      piece = new Fl_Output(x0+(j+1)*w,y0+(i+1)*h,w,h);
      piece->value(oss.str().c_str());
      piece->textcolor(FL_BLACK);
      if (i==j)
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
    oss<<static_cast<unsigned int>(sum);
    piece = new Fl_Output(x0+(m_ClassesMap.size()+1)*w,y0+(i+1)*h,w,h);
    piece->value(oss.str().c_str());
    gMatrix->add(piece);
    oss.str("");
  }
  piece = new Fl_Output(x0,y0+(m_ClassesMap.size()+1)*h,w,h);
  piece->value("Sum");
  piece->textfont(FL_HELVETICA_BOLD);
  gMatrix->add(piece);

  for (unsigned int i = 0;i<m_ClassesMap.size();++i)
  {
    oss<<static_cast<unsigned int>(columnSum[i]);
    Fl_Output * piece = new Fl_Output(x0+(i+1)*w,y0+(m_ClassesMap.size()+1)*h,w,h);
    piece->value(oss.str().c_str());
    gMatrix->add(piece);
    oss.str("");
  }

  gAccuracy->resizable(NULL);
  gAccuracy->resize(x0,y0+(m_ClassesMap.size()+3)*h,(m_ClassesMap.size()+2)*w,(m_ClassesMap.size()+2)*h);
  gAccuracy->align(FL_ALIGN_TOP);
  guiValidationWindow->add(gAccuracy);

  oss<<std::setprecision(4);

  oss<<kappa;
  piece = new Fl_Output(x0+2*w,y0+(m_ClassesMap.size()+4)*h ,w,h,"Kappa");
  piece->value(oss.str().c_str());
  piece->align(FL_ALIGN_LEFT);
  piece->labelfont(FL_HELVETICA_BOLD);
  piece->labelsize(12);
  piece->labelcolor(gAccuracy->labelcolor());
  gAccuracy->add(piece);
  oss.str("");
  oss<<overallAccuracy;
  piece = new Fl_Output(x0+2*w,y0+(m_ClassesMap.size()+5)*h ,w,h,"Overall Accuracy");
  piece->value(oss.str().c_str());
  piece->align(FL_ALIGN_LEFT);
  piece->labelfont(FL_HELVETICA_BOLD);
  piece->labelsize(12);
  piece->labelcolor(gAccuracy->labelcolor());
  gAccuracy->add(piece);
  oss.str("");

  bCloseValidationWindow->position(x0+2*w-bCloseValidationWindow->w()/2,y0+(m_ClassesMap.size()+8)*h);

  guiValidationWindow->show();
}


/**
*
*/
void
SupervisedClassificationAppli
::LoadROIsImage()
{
  typedef otb::ImageFileReader<LabeledImageType>                                LabeledReaderType;
  typedef otb::ExtractROI<LabeledPixelType,LabeledPixelType>                    ExtractROIFilterType;
  typedef itk::MinimumMaximumImageCalculator<LabeledImageType>                  MinMaxCalculatorType;
  typedef itk::BinaryThresholdImageFilter<LabeledImageType,LabeledImageType>    ThresholdFilterType;
  typedef itk::ConnectedComponentImageFilter<LabeledImageType,LabeledImageType> ConnectedComponentsFilterType;
  typedef itk::RelabelComponentImageFilter<LabeledImageType,LabeledImageType>   RelabelComponentsFilterType;
  typedef otb::ImageToEdgePathFilter<LabeledImageType,PolygonType>              EdgeExtractionFilterType;
  typedef otb::SimplifyPathListFilter<PolygonType>                              SimplifyPathListFilterType;
  typedef otb::StreamingTraits<LabeledImageType>                                StreamingTraitsType;
  typedef itk::ImageRegionSplitter<2>                                           SplitterType;
  typedef  ImageType::RegionType                                        RegionType;

  LabeledReaderType::Pointer labeledReader = LabeledReaderType::New();
  labeledReader->SetFileName(m_ROIsImageFileName);

  try
  {
    labeledReader->GenerateOutputInformation();
  }
  catch ( itk::ExceptionObject & err )
  {
    itk::OStringStream oss;
    oss << "Error while reading labeled image: "  << err << std::endl;
    fl_alert(oss.str().c_str());
    return;
  }


  RegionType largestRegion = labeledReader->GetOutput()->GetLargestPossibleRegion();

  // Setting up local streaming capabilities
   SplitterType::Pointer splitter = SplitterType::New();
  unsigned int numberOfStreamDivisions =
    StreamingTraitsType::CalculateNumberOfStreamDivisions(labeledReader->GetOutput(),
        largestRegion,
        splitter,
        otb::SET_TILING_WITH_SET_AUTOMATIC_NUMBER_OF_STREAM_DIVISIONS,
        0UL,0UL,0UL);
  RegionType streamingRegion;
  std::map<LabeledPixelType,LabeledPixelType> labelTranslationMap;

  for (unsigned int i  = 0; i<m_ClassesMap.size();++i)
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

  for (unsigned int piece = 0;piece < numberOfStreamDivisions;++piece)
  {
    streamingRegion = splitter->GetSplit(piece,numberOfStreamDivisions,largestRegion);

     ExtractROIFilterType::Pointer extract = ExtractROIFilterType::New();
    extract->SetInput(labeledReader->GetOutput());
    extract->SetExtractionRegion(streamingRegion);
    extract->Update();

     MinMaxCalculatorType::Pointer minMax = MinMaxCalculatorType::New();
    minMax->SetImage(extract->GetOutput());
    minMax->Compute();

    for (LabeledPixelType label = 1;label<=minMax->GetMaximum();++label)
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

      for (LabeledPixelType cindex = 1; cindex <= relabelcc->GetNumberOfObjects();++cindex)
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

        if (labelTranslationMap.find(label)!=labelTranslationMap.end())
        {
          newLabel = labelTranslationMap[label];
        }
        else
        {
          this->AddClass();
          newLabel = m_ClassesMap.size();
          labelTranslationMap[label]=newLabel;
        }

        polygonList->PushBack(simplifier->GetOutput()->Back());
        polygonList->Back()->SetValue(newLabel);
      }
    }
  }
  this->Update();
}
} // end namespace otb


