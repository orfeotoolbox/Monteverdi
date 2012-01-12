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

#include "otbUncompressJpeg2000Module.h"
#include "otbMsgReporter.h"
#include "itkImageRegion.h"

#include "otbFltkFilterWatcher.h"

#include <FLU/Flu_File_Chooser.h>
#include <FL/Fl.H>
#include <FL/fl_ask.H>

namespace otb
{
/** Constructor */
UncompressJpeg2000Module::UncompressJpeg2000Module()
{
  this->NeedsPipelineLockingOn();
  
  // Describe inputs
  this->AddInputDescriptor<FloatImageWithQuicklook>("InputImage", otbGetTextMacro("Image to read"));

  m_VectorImageExtractROIFilter = VectorImageExtractROIFilterType::New();
  
  m_Model                       = ModelType::New();
  m_View                        = ViewType::New();
  m_Controller                  = ControllerType::New();
  m_ResizingHandler             = ResizingHandlerType::New();
  m_SelectAreaHandler           = SelectAreaHandlerType::New();
  m_RegionGl                    = RegionGlComponentType::New();
  m_TileRegionGl                = RegionGlComponentType::New();

  m_ResolutionFactor = 0;

  m_View->SetModel(m_Model);
  m_View->SetController(m_Controller);

  // RegionGlComponent for select area
  ColorType color;
  color[0] = 1;
  color[1] = 0;
  color[2] = 0;
  color[3] = 1;
  m_RegionGl->SetColor(color);
  m_View->GetScrollWidget()->AddGlComponent(m_RegionGl);

  ColorType tileColor;
  tileColor[0] = 0;
  tileColor[1] = 1;
  tileColor[2] = 0;
  tileColor[3] = 1;
  m_TileRegionGl->SetColor(tileColor);
  m_View->GetScrollWidget()->AddGlComponent(m_TileRegionGl);

  // Add the select area handler
  m_SelectAreaHandler->SetModel(m_Model);
  m_SelectAreaHandler->SetWidget(m_View->GetScrollWidget());
  m_SelectAreaHandler->SetRegionGlComponent(m_RegionGl);
  m_SelectAreaHandler->SetTileRegionGlComponent(m_TileRegionGl);
  m_Controller->AddActionHandler(m_SelectAreaHandler);

  // Add the resizing handler
  m_ResizingHandler->SetModel(m_Model);
  m_ResizingHandler->SetView(m_View);
  m_Controller->AddActionHandler(m_ResizingHandler);

  this->BuildGUI();

  m_SelectAreaHandler->SetStartX(vStartX);
  m_SelectAreaHandler->SetStartY(vStartY);
  m_SelectAreaHandler->SetSizeX(vSizeX);
  m_SelectAreaHandler->SetSizeY(vSizeY);
  m_SelectAreaHandler->SetNbTile(vNbTileExt);

  vView->add(m_View->GetScrollWidget());
  vView->resizable(m_View->GetScrollWidget());
  m_View->GetScrollWidget()->resize(vView->x(), vView->y(), vView->w(), vView->h());
  
  m_CheckFileExistance = true;
  m_Filename = "";
}

/** Destructor */
UncompressJpeg2000Module::~UncompressJpeg2000Module()
{
  this->Cancel();
  vView->remove(m_View->GetScrollWidget());
}

/** PrintSelf method */
void UncompressJpeg2000Module::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void UncompressJpeg2000Module::Run()
{
  FloatImageWithQuicklook::Pointer vectorImageQL = this->GetInputData<FloatImageWithQuicklook>("InputImage");
  FloatingVectorImageType::Pointer vectorImage = FloatingVectorImageType::New();

  itk::ImageRegion<2> imageRegion;

  if (vectorImageQL.IsNull())
    {
    itkExceptionMacro("The image pointer is not initialized!!");
    }

  vectorImage = vectorImageQL->GetImage();
  vectorImage->UpdateOutputInformation();
  imageRegion = vectorImage->GetLargestPossibleRegion();
  
  m_NbBands = vectorImage->GetNumberOfComponentsPerPixel();
  
  /** Add view */
  m_VectorGenerator = VectorLayerGeneratorType::New();
  m_VectorGenerator->SetImage(vectorImage);

  m_VectorGenerator->GenerateQuicklookOff();
  m_VectorGenerator->SetQuicklook(vectorImageQL->GetQuicklook());
  m_VectorGenerator->SetSubsamplingRate(vectorImageQL->GetShrinkFactor());

  m_VectorGenerator->GenerateLayer();
  m_Model->AddLayer(m_VectorGenerator->GetLayer());
    
  itk::ExposeMetaData<unsigned int>(vectorImage->GetMetaDataDictionary(),
                                    MetaDataKey::ResolutionFactor,
                                    m_ResolutionFactor);
  vRes->value(m_ResolutionFactor);

  itk::ExposeMetaData<unsigned int>(vectorImage->GetMetaDataDictionary(),
                                    MetaDataKey::TileHintX,
                                    m_TileHintX);

  itk::ExposeMetaData<unsigned int>(vectorImage->GetMetaDataDictionary(),
                                    MetaDataKey::TileHintY,
                                    m_TileHintY);

  vTileSizeX->value(m_TileHintX);
  vTileSizeY->value(m_TileHintY);
  
  m_SelectAreaHandler->SetTileHintX(m_TileHintX);
  m_SelectAreaHandler->SetTileHintY(m_TileHintY);

  // Compute the total number of tiles
  unsigned int nbTiles = ( vcl_ceil( static_cast<double>(imageRegion.GetSize()[0])/static_cast<double>(m_TileHintX) ) )
    * ( vcl_ceil( static_cast<double>(imageRegion.GetSize()[1])/static_cast<double>(m_TileHintY) ) );
  
  vNbTiles->value( nbTiles );
  
  vTileSize->value(static_cast<double>(m_TileHintX) * 
                   static_cast<double>(m_TileHintY) * 
                   static_cast<double>(4 * m_NbBands) / 1048576.0);

  m_SelectAreaHandler->SetLargestRegion(imageRegion);

  m_RegionGl->SetRegion(imageRegion);
  m_TileRegionGl->SetRegion(imageRegion);

  vInputImageSizeX->value(imageRegion.GetSize()[0]);
  vInputImageSizeY->value(imageRegion.GetSize()[1]);

  /** Update Start and Size for selected area */
  vStartX->minimum(static_cast<double>(imageRegion.GetIndex()[0]));
  vStartX->maximum(static_cast<double>(imageRegion.GetSize()[0]));
  vStartX->value(vStartX->minimum());
  vStartY->minimum(static_cast<double>(imageRegion.GetIndex()[1]));
  vStartY->maximum(static_cast<double>(imageRegion.GetSize()[1]));
  vStartY->value(vStartY->minimum());

  if (vStartX->minimum() <= 0) vSizeX->minimum(1);
  else vSizeX->minimum(vStartX->minimum());

  vSizeX->maximum(vStartX->maximum());
  vSizeX->value(vSizeX->maximum());

  if (vStartY->minimum() <= 0) vSizeY->minimum(1);
  else vSizeY->minimum(vStartX->minimum());

  vSizeY->maximum(vStartY->maximum());
  vSizeY->value(vSizeY->maximum());
  
  /** update size on disk */
  vMemSize->value(vSizeX->value() * vSizeY->value() * static_cast<double>(4 * m_NbBands) / 1048576.0);
  
  vNbTileExt->value(nbTiles);
  
  pBar->minimum(0);
  pBar->maximum(1);
  
  wExtractROIWindow->show();
  //pBar->copy_label("0%");
  m_View->GetScrollWidget()->show();
  m_Model->Update();
}

/** UpdateRegion */
void UncompressJpeg2000Module::UpdateRegion()
{
  unsigned long startx, starty, sizex, sizey;
  startx = static_cast<unsigned long>(vStartX->value());
  starty = static_cast<unsigned long>(vStartY->value());
  sizex = static_cast<unsigned long>(vSizeX->value());
  sizey = static_cast<unsigned long>(vSizeY->value());

  if (startx < vStartX->minimum()) vStartX->value(vStartX->minimum());
  if (startx >= vStartX->maximum()) vStartX->value(vStartX->maximum() - 1);

  if (starty < vStartY->minimum()) vStartY->value(vStartY->minimum());
  if (starty >= vStartY->maximum()) vStartY->value(vStartY->maximum() - 1);

  if (sizex < vSizeX->minimum()) vSizeX->value(vSizeX->minimum());
  if ((startx + sizex) > vSizeX->maximum()) vSizeX->value(vSizeX->maximum() - startx);

  if (sizey < vSizeY->minimum()) vSizeY->value(vSizeY->minimum());
  if ((starty + sizey) > vSizeY->maximum()) vSizeY->value(vSizeY->maximum() - starty);

  m_SelectAreaHandler->UpdateGlRegion();
  
  IndexType tileRegIndex = m_TileRegionGl->GetRegion().GetIndex();
  SizeType tileRegSize = m_TileRegionGl->GetRegion().GetSize();
  
  /** update size on disk */
  vMemSize->value( static_cast<double>(tileRegSize[0]) *
                   static_cast<double>(tileRegSize[1]) *
                   static_cast<double>(4 * m_NbBands) / 1048576.0);
}

/** Cancel */
void UncompressJpeg2000Module::Cancel()
{
  wExtractROIWindow->hide();
}

/** Ok */
void UncompressJpeg2000Module::Ok()
{
  FloatImageWithQuicklook::Pointer vectorImageQL = this->GetInputData<FloatImageWithQuicklook>("InputImage");
  FloatingVectorImageType::Pointer vectorImage = FloatingVectorImageType::New();

  IndexType  idxInit;
  SizeType   regSize;
  
  if (vectorImageQL.IsNotNull())
    {
    idxInit = m_TileRegionGl->GetRegion().GetIndex();
    regSize = m_TileRegionGl->GetRegion().GetSize();

    m_VectorImageExtractROIFilter->SetStartX(idxInit[0]);
    m_VectorImageExtractROIFilter->SetStartY(idxInit[1]);
    m_VectorImageExtractROIFilter->SetSizeX(regSize[0]);
    m_VectorImageExtractROIFilter->SetSizeY(regSize[1]);
    m_VectorImageExtractROIFilter->SetInput(vectorImageQL->GetImage());
    
    if (m_Filename.length() == 0)
      {
      const char * filename = NULL;
      filename = flu_file_chooser(otbGetTextMacro("Choose the image file..."), "*.tif", "");
      if (filename == NULL)
        {
        otbMsgDebugMacro(<< "Empty file name!");
        return;
        }
      m_Filename = filename;
      }
    
    std::ifstream isFileNameExist( m_Filename.c_str() );
    bool isProcessing = true;

    if(isFileNameExist && m_CheckFileExistance==true)
      {
        isFileNameExist.close();
        isProcessing = ::fl_choice("File already exist, do you want to overwrite this file?", "cancel", "OK", NULL );
      }

    if(isProcessing)
      {
      this->StartProcess2();
      this->StartProcess1();
      }
    }
}

/** UpdateProgress */
void UncompressJpeg2000Module::UpdateProgress()
{
  double progress = m_ProcessObject->GetProgress();

  itk::OStringStream oss1, oss2;
  oss1.str("");
  oss1 << otbGetTextMacro("Writing image") << "  (" << std::floor(100 * progress) << "%)";
  oss2.str("");
  oss2 << std::floor(100 * progress);
  oss2 << "%";
  pBar->value(progress);
  wExtractROIWindow->copy_label(oss1.str().c_str());
  pBar->copy_label(oss2.str().c_str());
}

/** UpdateProgressCallback */
void UncompressJpeg2000Module::UpdateProgressCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if (writer.IsNotNull())
    {
    writer->UpdateProgress();
    }
}

/** ThreadedWatch */
void UncompressJpeg2000Module::ThreadedWatch()
{
  // Deactivate window buttons
  Fl::lock();
  bCancel->deactivate();
  bOk->deactivate();
  vStartX->deactivate();
  vStartY->deactivate();
  vSizeX->deactivate();
  vSizeY->deactivate();
  Fl::unlock();

  double last = 0;
  double updateThres = 0.01;
  double current = 0;

  while ((m_ProcessObject.IsNull() || this->IsBusy()))
    {
    if (m_ProcessObject.IsNotNull())
      {
      current = m_ProcessObject->GetProgress();
      if (current - last > updateThres)
        {
        // Make the main fltk loop update progress fields
        Fl::awake(&UpdateProgressCallback, this);
        last = current;
        }
      }
    // Sleep for a while
    Sleep(500);
    }
  
  // Update progress one last time
  Fl::awake(&UpdateProgressCallback, this);

  Fl::lock();
  // Reactivate window buttons
  bCancel->activate();
  bOk->activate();
  vStartX->activate();
  vStartY->activate();
  vSizeX->activate();
  vSizeY->activate();
  Fl::unlock();

  Fl::awake(&HideWindowCallback, this);
}

/** ThreadedRun */
void UncompressJpeg2000Module::ThreadedRun()
{
  this->BusyOn();

  try
    {
    FPVWriterType::Pointer FPVWriter = FPVWriterType::New();
    FPVWriter->SetInput(m_VectorImageExtractROIFilter->GetOutput());
    FPVWriter->WriteGeomFileOn();
    FPVWriter->SetFileName(m_Filename.c_str());
    m_ProcessObject = FPVWriter;
    FPVWriter->Update();
    }
  catch (itk::ExceptionObject& err)
    {
    m_ErrorMsg = err.GetDescription();
    Fl::awake(&SendErrorCallback, &m_ErrorMsg);
    }

  this->BusyOff();
}

/** HideWindow */
void UncompressJpeg2000Module::HideWindow()
{
  wExtractROIWindow->hide();
}

/** HideWindowCallback */
void UncompressJpeg2000Module::HideWindowCallback(void * data)
{
  Self::Pointer writer = static_cast<Self *>(data);

  if (writer.IsNotNull())
    {
    writer->HideWindow();
    }
}

/** SendErrorCallback */
void UncompressJpeg2000Module::SendErrorCallback(void * data)
{
  std::string * error = static_cast<std::string *>(data);
  //TODO test if error is null
  if (error == NULL)
    {
    MsgReporter::GetInstance()->SendError("Unknown error during update");
    }
  else
    {
    MsgReporter::GetInstance()->SendError(error->c_str());
    }
}

} // End namespace otb
