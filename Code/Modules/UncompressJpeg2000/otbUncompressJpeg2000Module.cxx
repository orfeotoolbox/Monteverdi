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

#include "otbForwardSensorModel.h"
#include "otbFltkFilterWatcher.h"

namespace otb
{
/** Constructor */
UncompressJpeg2000Module::UncompressJpeg2000Module()
{
  // Describe inputs
  this->AddInputDescriptor<FloatImageWithQuicklook>("InputImage", otbGetTextMacro("Image to read"));

  m_VectorImageExtractROIFilter = VectorImageExtractROIFilterType::New();
  m_Transform                   = TransformType::New();
  m_InverseTransform            = TransformType::New();

  m_Model                       = ModelType::New();
  m_View                        = ViewType::New();
  m_Controller                  = ControllerType::New();
  m_ResizingHandler             = ResizingHandlerType::New();
  m_SelectAreaHandler           = SelectAreaHandlerType::New();
  m_regionGl                    = RegionGlComponentType::New();

  m_View->SetModel(m_Model);
  m_View->SetController(m_Controller);

  // RegionGlComponent for select area
  ColorType color;
  color[0] = 0;
  color[1] = 1;
  color[2] = 0;
  color[3] = 1;
  m_regionGl->SetColor(color);
  m_View->GetScrollWidget()->AddGlComponent(m_regionGl);

  // Add the select area handler
  m_SelectAreaHandler->SetModel(m_Model);
  m_SelectAreaHandler->SetWidget(m_View->GetScrollWidget());
  m_SelectAreaHandler->SetRegionGlComponent(m_regionGl);
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

  vView->add(m_View->GetScrollWidget());
  vView->resizable(m_View->GetScrollWidget());
  m_View->GetScrollWidget()->resize(vView->x(), vView->y(), vView->w(), vView->h());
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

  typedef ForwardSensorModel<double> ForwardSensorType;

  if (vectorImageQL.IsNotNull())
    {
    vectorImage = vectorImageQL->GetImage();
    vectorImage->UpdateOutputInformation();
    imageRegion = vectorImage->GetLargestPossibleRegion();

    /** Add view */
    m_VectorGenerator = VectorLayerGeneratorType::New();
    m_VectorGenerator->SetImage(vectorImage);

    m_VectorGenerator->GenerateQuicklookOff();
    m_VectorGenerator->SetQuicklook(vectorImageQL->GetQuicklook());
    m_VectorGenerator->SetSubsamplingRate(vectorImageQL->GetShrinkFactor());

    m_VectorGenerator->GenerateLayer();
    m_Model->AddLayer(m_VectorGenerator->GetLayer());
    }
  else
    {
    itkExceptionMacro("The image pointer is not initialized!!");
    }

  m_SelectAreaHandler->SetLargestRegion(imageRegion);

  m_regionGl->SetRegion(imageRegion);

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

  wExtractROIWindow->show();
  m_View->GetScrollWidget()->show();
  m_Model->Update();
}

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
}


/** Cancel */
void UncompressJpeg2000Module::Cancel()
{
  wExtractROIWindow->hide();
}
/** Ok */
void UncompressJpeg2000Module::Ok()
{
  try
    {
    FloatImageWithQuicklook::Pointer vectorImageQL = this->GetInputData<FloatImageWithQuicklook>("InputImage");
    FloatingVectorImageType::Pointer vectorImage = FloatingVectorImageType::New();

    IndexType  idxInit;
    OffsetType offSize;
    if (vectorImageQL.IsNotNull())
      {
      idxInit[0] = static_cast<unsigned long>(vStartX->value());
      idxInit[1] = static_cast<unsigned long>(vStartY->value());

      offSize[0] = static_cast<unsigned long>(vSizeX->value());
      offSize[1] = static_cast<unsigned long>(vSizeY->value());

      m_VectorImageExtractROIFilter->SetStartX(idxInit[0]);
      m_VectorImageExtractROIFilter->SetStartY(idxInit[1]);
      m_VectorImageExtractROIFilter->SetSizeX(offSize[0]);
      m_VectorImageExtractROIFilter->SetSizeY(offSize[1]);
      m_VectorImageExtractROIFilter->SetInput(vectorImageQL->GetImage());

      this->ClearOutputDescriptors();
      this->AddOutputDescriptor(m_VectorImageExtractROIFilter->GetOutput(), "OutputImage",
                                otbGetTextMacro("Image extracted"));

      this->NotifyOutputsChange();

      }
    wExtractROIWindow->hide();
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

} // End namespace otb
