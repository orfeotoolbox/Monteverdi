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

#include "otbExtractROIModule.h"
#include "otbMsgReporter.h"
#include "itkImageRegion.h"

#include "otbForwardSensorModel.h"
#include "otbFltkFilterWatcher.h"

namespace otb
{
/** Constructor */
ExtractROIModule::ExtractROIModule()
{
  // Describe inputs
  this->AddInputDescriptor<FloatingImageType>("InputImage", otbGetTextMacro("Image to read"));
  this->AddTypeToInputDescriptor<FloatingVectorImageType>("InputImage");
  m_VectorImageExtractROIFilter = VectorImageExtractROIFilterType::New();
  m_ImageExtractROIFilter       = ImageExtractROIFilterType::New();
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
ExtractROIModule::~ExtractROIModule()
{
  this->Cancel();
  vView->remove(m_View->GetScrollWidget());
}

/** PrintSelf method */
void ExtractROIModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void ExtractROIModule::Run()
{
  FloatingImageType::Pointer       image              = this->GetInputData<FloatingImageType>("InputImage");
  FloatingVectorImageType::Pointer vectorImage  = this->GetInputData<FloatingVectorImageType>("InputImage");

  itk::ImageRegion<2> imageRegion;

  typedef ForwardSensorModel<double> ForwardSensorType;

  vLong1->activate();
  vLatitude1->activate();
  vLong2->activate();
  vLatitude2->activate();

  if (!image.IsNull() && vectorImage.IsNull())
    {
    image->UpdateOutputInformation();
    imageRegion = image->GetLargestPossibleRegion();

    m_Generator = LayerGeneratorType::New();
    m_Generator->SetImage(image);

    FltkFilterWatcher qlwatcher(m_Generator->GetProgressSource(), 0, 0, 200, 20,
                                otbGetTextMacro("Generating QuickLook ..."));
    m_Generator->GenerateLayer();
    m_Model->AddLayer(m_Generator->GetLayer());

    try
      {
      const itk::MetaDataDictionary& inputDict = image->GetMetaDataDictionary();
      m_Transform->SetInputDictionary(inputDict);
      m_Transform->SetInputProjectionRef(image->GetProjectionRef());
//      m_Transform->SetInputOrigin(image->GetOrigin());
//      m_Transform->SetInputSpacing(image->GetSpacing());
      m_Transform->InstanciateTransform();

      isNotAProjection = (m_Transform->GetTransformAccuracy() == Projection::UNKNOWN);
      if (isNotAProjection)
        {
        vLong1->deactivate();
        vLatitude1->deactivate();
        vLong2->deactivate();
        vLatitude2->deactivate();
        bUpdate->deactivate();
        bUpdate->deactivate();
        }
      }
    catch (itk::ExceptionObject&  e)
      {
      vLong1->deactivate();
      vLatitude1->deactivate();
      vLong2->deactivate();
      vLatitude2->deactivate();
      bUpdate->deactivate();
      bUpdate->deactivate();
         MsgReporter::GetInstance()->SendError(e.GetDescription());
      }
    }
  else if (image.IsNull() && !vectorImage.IsNull())
    {
    vectorImage->UpdateOutputInformation();
    imageRegion = vectorImage->GetLargestPossibleRegion();

    /** Add view */
    m_VectorGenerator = VectorLayerGeneratorType::New();
    m_VectorGenerator->SetImage(vectorImage);

    FltkFilterWatcher qlwatcher(m_VectorGenerator->GetProgressSource(), 0, 0, 200, 20,
                                otbGetTextMacro("Generating QuickLook ..."));
    m_VectorGenerator->GenerateLayer();
    m_Model->AddLayer(m_VectorGenerator->GetLayer());

    try
      {
      const itk::MetaDataDictionary& inputDict = vectorImage->GetMetaDataDictionary();
      m_Transform->SetInputDictionary(inputDict);
      m_Transform->SetInputProjectionRef(vectorImage->GetProjectionRef());
//      m_Transform->SetInputOrigin(vectorImage->GetOrigin());
//      m_Transform->SetInputSpacing(vectorImage->GetSpacing());
      m_Transform->InstanciateTransform();

      isNotAProjection = (m_Transform->GetTransformAccuracy() == Projection::UNKNOWN);

      if (isNotAProjection)
        {
        vLong1->deactivate();
        vLatitude1->deactivate();
        vLong2->deactivate();
        vLatitude2->deactivate();
        bUpdate->deactivate();
        }
      }
    catch (itk::ExceptionObject& e)
      {
      vLong1->deactivate();
      vLatitude1->deactivate();
      vLong2->deactivate();
      vLatitude2->deactivate();
      bUpdate->deactivate();
         MsgReporter::GetInstance()->SendError(e.GetDescription());
      }
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

  /** Update Long / Lat for selected area */
  if (!isNotAProjection)
    {
    IndexType       lIndexStart, lIndexEnd;
    OutputPointType pto1, pto2, pt1, pt2;

    lIndexStart[0]  = static_cast<long int>(vStartX->value());
    lIndexStart[1]  = static_cast<long int>(vStartY->value());
    lIndexEnd[0]    = static_cast<long int>(lIndexStart[0] + vSizeX->value());
    lIndexEnd[1]    = static_cast<long int>(lIndexStart[1] + vSizeY->value());

    if (!image.IsNull() && vectorImage.IsNull())
      {
      image->TransformIndexToPhysicalPoint(lIndexStart, pto1);
      image->TransformIndexToPhysicalPoint(lIndexEnd, pto2);
      }
    else if (image.IsNull() && !vectorImage.IsNull())
      {
      vectorImage->TransformIndexToPhysicalPoint(lIndexStart, pto1);
      vectorImage->TransformIndexToPhysicalPoint(lIndexEnd, pto2);
      }
    else
      {
      itkExceptionMacro(<< "Input image is NULL.");
      }
    pt1 = m_Transform->TransformPoint(pto1);
    pt2 = m_Transform->TransformPoint(pto2);

    // Test if values are NAN
    if ((pt1[0] != pt1[0]) || (pt1[1] != pt1[1]) || (pt2[0] != pt2[0]) || (pt2[1] != pt2[1]))
      {
      // If nan then there is no transform
      isNotAProjection = true;
      vLong1->deactivate();
      vLatitude1->deactivate();
      vLong2->deactivate();
      vLatitude2->deactivate();
      bUpdate->deactivate();
      }
    else
      {
      vLong1->value(pt1[0]);
      vLatitude1->value(pt1[1]);
      vLong2->value(pt2[0]);
      vLatitude2->value(pt2[1]);
      }
    }

  wExtractROIWindow->show();
  m_View->GetScrollWidget()->show();
  m_Model->Update();
}

void ExtractROIModule::UpdateRegion()
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

  /** Update Long / Lat for selected area */
  if (!isNotAProjection)
    {
    FloatingImageType::Pointer       image = this->GetInputData<FloatingImageType>("InputImage");
    FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");

    IndexType       lIndexStart, lIndexEnd;
    OutputPointType pto1, pto2, pt1, pt2;

    lIndexStart[0]  = static_cast<long int>(vStartX->value());
    lIndexStart[1]  = static_cast<long int>(vStartY->value());
    lIndexEnd[0]    = static_cast<long int>(lIndexStart[0] + vSizeX->value());
    lIndexEnd[1]    = static_cast<long int>(lIndexStart[1] + vSizeY->value());

    if (!image.IsNull() && vectorImage.IsNull())
      {
      image->TransformIndexToPhysicalPoint(lIndexStart, pto1);
      image->TransformIndexToPhysicalPoint(lIndexEnd, pto2);
      }
    else if (image.IsNull() && !vectorImage.IsNull())
      {
      vectorImage->TransformIndexToPhysicalPoint(lIndexStart, pto1);
      vectorImage->TransformIndexToPhysicalPoint(lIndexEnd, pto2);
      }

    pt1 = m_Transform->TransformPoint(pto1);
    pt2 = m_Transform->TransformPoint(pto2);

    vLong1->value(pt1[0]);
    vLatitude1->value(pt1[1]);
    vLong2->value(pt2[0]);
    vLatitude2->value(pt2[1]);
    }

  m_SelectAreaHandler->UpdateGlRegion();
}

void ExtractROIModule::UpdateRegionWithLatLong()
{
  FloatingImageType::Pointer       image = this->GetInputData<FloatingImageType>("InputImage");
  FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");

  IndexType       lIndex1, lIndex2;
  OutputPointType pt1, pt2, pto1, pto2;

  pt1[0] = static_cast<InternalPixelType>(vLong1->value());
  pt1[1] = static_cast<InternalPixelType>(vLatitude1->value());
  pt2[0] = static_cast<InternalPixelType>(vLong2->value());
  pt2[1] = static_cast<InternalPixelType>(vLatitude2->value());

  /** Check if value are in img */
  m_Transform->GetInverse(m_InverseTransform);
  pto1 = m_InverseTransform->TransformPoint(pt1);
  pto2 = m_InverseTransform->TransformPoint(pt2);

  if (!image.IsNull() && vectorImage.IsNull())
    {
    image->TransformPhysicalPointToIndex(pto1, lIndex1);
    image->TransformPhysicalPointToIndex(pto2, lIndex2);
    }
  else if (image.IsNull() && !vectorImage.IsNull())
    {
    vectorImage->TransformPhysicalPointToIndex(pto1, lIndex1);
    vectorImage->TransformPhysicalPointToIndex(pto2, lIndex2);
    }
  else
    {
    itkExceptionMacro(<< "Input image is NULL.");
    }

  vStartX->value(lIndex1[0]);
  vStartY->value(lIndex1[1]);
  vSizeX->value(vcl_abs(lIndex2[0] - lIndex1[0]));
  vSizeY->value(vcl_abs(lIndex2[1] - lIndex1[1]));

  this->UpdateRegion();
}

/** Cancel */
void ExtractROIModule::Cancel()
{
  wExtractROIWindow->hide();
}
/** Ok */
void ExtractROIModule::Ok()
{
  try
    {
    FloatingImageType::Pointer       image = this->GetInputData<FloatingImageType>("InputImage");
    FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");

    IndexType  idxInit;
    OffsetType offSize;
    if (!image.IsNull() && vectorImage.IsNull())
      {
      // Get input image
      FloatingImageType::Pointer image = this->GetInputData<FloatingImageType>("InputImage");

      idxInit[0] = static_cast<unsigned long>(vStartX->value());
      idxInit[1] = static_cast<unsigned long>(vStartY->value());

      offSize[0] = static_cast<unsigned long>(vSizeX->value());
      offSize[1] = static_cast<unsigned long>(vSizeY->value());

      m_ImageExtractROIFilter->SetStartX(idxInit[0]);
      m_ImageExtractROIFilter->SetStartY(idxInit[1]);
      m_ImageExtractROIFilter->SetSizeX(offSize[0]);
      m_ImageExtractROIFilter->SetSizeY(offSize[1]);
      m_ImageExtractROIFilter->SetInput(image);

      this->ClearOutputDescriptors();
      this->AddOutputDescriptor(m_ImageExtractROIFilter->GetOutput(), "OutputImage", otbGetTextMacro("Image extracted"));

      /*ImageRegionType newRequestedRegion;
      newRequestedRegion.SetSize(0, 0);
      newRequestedRegion.SetIndex(static_cast<unsigned long>(vStartX->value()), static_cast<unsigned long>(vStartY->value()));
      m_ImageExtractROIFilter->GetOutput()->SetRequestedRegion(newRequestedRegion); */

      this->NotifyOutputsChange();
      }
    else if (image.IsNull() && !vectorImage.IsNull())
      {
      // Get Input Vector Image
      FloatingVectorImageType::Pointer vectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");

      idxInit[0] = static_cast<unsigned long>(vStartX->value());
      idxInit[1] = static_cast<unsigned long>(vStartY->value());

      offSize[0] = static_cast<unsigned long>(vSizeX->value());
      offSize[1] = static_cast<unsigned long>(vSizeY->value());

      m_VectorImageExtractROIFilter->SetStartX(idxInit[0]);
      m_VectorImageExtractROIFilter->SetStartY(idxInit[1]);
      m_VectorImageExtractROIFilter->SetSizeX(offSize[0]);
      m_VectorImageExtractROIFilter->SetSizeY(offSize[1]);
      m_VectorImageExtractROIFilter->SetInput(vectorImage);

      this->ClearOutputDescriptors();
      this->AddOutputDescriptor(m_VectorImageExtractROIFilter->GetOutput(), "OutputImage",
                                otbGetTextMacro("Image extracted"));

     /* VectorImageRegionType newRequestedRegion;
      newRequestedRegion.SetSize(0, 0);
      newRequestedRegion.SetIndex(static_cast<unsigned long>(vStartX->value()), static_cast<unsigned long>(vStartY->value()));
      m_VectorImageExtractROIFilter->GetOutput()->SetRequestedRegion(newRequestedRegion); */

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
