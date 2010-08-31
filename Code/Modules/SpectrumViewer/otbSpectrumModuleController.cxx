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
#include "otbSpectrumModuleController.h"

namespace otb {

SpectrumModuleController
::SpectrumModuleController()
{

  m_ImageWidgetController = ImageWidgetControllerType::New();
  m_PixelSpectrumController = PixelSpectrumControllerType::New();

  /** Members construction (Handler) */
  m_ResizingHandler = ResizingHandlerType::New();
  m_ChangeScaledRegionHandler = ChangeScaledRegionHandlerType::New();
  m_ChangeHandler = ChangeRegionHandlerType::New();
  m_ChangeScaleHandler = ChangeScaleHandlerType::New();
  m_PixelActionHandler = PixelSpectrumActionHandlerType::New();

  m_ImageWidgetController->AddActionHandler(m_ResizingHandler);
  m_ImageWidgetController->AddActionHandler(m_ChangeHandler);
  m_ImageWidgetController->AddActionHandler(m_ChangeScaleHandler);
  m_ImageWidgetController->AddActionHandler(m_ChangeScaledRegionHandler);
  m_ImageWidgetController->AddActionHandler(m_PixelActionHandler);
}

SpectrumModuleController
::~SpectrumModuleController()
{
}

void
SpectrumModuleController
::SaveAndQuit()
{
  m_SpectrumModuleModel->SaveAndQuit();
}

void
SpectrumModuleController
::Quit()
{
  m_SpectrumModuleModel->Quit();
}

void
SpectrumModuleController
::SetSpectrumModuleModel(SpectrumModuleModelType * m)
{
  m_SpectrumModuleModel = m;
  m_PixelSpectrumController->SetModel(m_SpectrumModuleModel->GetPixelSpectrumModel());

  m_ResizingHandler->SetModel(m_SpectrumModuleModel->GetImageModel());
  m_ChangeHandler->SetModel(m_SpectrumModuleModel->GetImageModel());
  m_ChangeScaleHandler->SetModel(m_SpectrumModuleModel->GetImageModel());
  m_ChangeScaledRegionHandler->SetModel(m_SpectrumModuleModel->GetImageModel());
  m_PixelActionHandler->SetController(m_PixelSpectrumController);

}

void
SpectrumModuleController
::SetSpectrumModuleView(SpectrumModuleViewPointerType s)
{

  m_SpectrumModuleView = s;
  m_PixelSpectrumController->SetView(m_SpectrumModuleView->GetPixelSpectrumView());

  m_ResizingHandler->SetView(m_SpectrumModuleView->GetImageView());
  m_ChangeHandler->SetView(m_SpectrumModuleView->GetImageView());
  m_ChangeScaleHandler->SetView(m_SpectrumModuleView->GetImageView());
  m_ChangeScaledRegionHandler->SetView(m_SpectrumModuleView->GetImageView());
  m_PixelActionHandler->SetView(m_SpectrumModuleView->GetImageView());

}

void
SpectrumModuleController
::UpdateViewerDisplay()
{
  if (!m_SpectrumModuleModel->GetIsImageReady())
    {
    return;
    }

  std::vector<unsigned int> channels;

  channels.push_back(m_SpectrumModuleView->RedSlider->value());
  channels.push_back(m_SpectrumModuleView->GreenSlider->value());
  channels.push_back(m_SpectrumModuleView->BlueSlider->value());

  m_SpectrumModuleModel->UpdateViewerDisplay(channels);
}

void
SpectrumModuleController
::GenerateSpectralAngle(int Id)
{
  m_SpectrumModuleModel->GenerateSpectralAngle(Id);
}

void
SpectrumModuleController
::SetSpectralLayerOpacity(float f)
{
  m_SpectrumModuleModel->SetSpectralLayerOpacity(f);
}

void
SpectrumModuleController
::WriteSpreadsheetFile(int id, const char *path)
{
  m_SpectrumModuleModel->WriteSpreadsheetFile(id, path);
}

void
SpectrumModuleController
::ForceSpectralContrast(float c)
{
  m_SpectrumModuleModel->ForceSpectralContrast(c);
}

void
SpectrumModuleController
::UseColorMap(bool b)
{
  m_SpectrumModuleModel->SetUseColorMap(b);
}

}
