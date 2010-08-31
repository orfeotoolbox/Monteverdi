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
#ifndef __otbSpectrumModuleController_h
#define __otbSpectrumModuleController_h

#include "otbSpectrumModuleControllerInterface.h"
#include "otbSpectrumModuleView.h"

namespace otb {

class SpectrumModuleController
: public otb::SpectrumModuleControllerInterface
{
public:
  /** Standard class typedefs */
  typedef SpectrumModuleController          Self;
  typedef SpectrumModuleControllerInterface Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  typedef Superclass::SpectrumModuleModelType SpectrumModuleModelType;
  typedef SpectrumModuleModelType::Pointer    SpectrumModuleModelPointerType;
  // model subtype
  typedef SpectrumModuleModelType::ImageModelType ImageModelType;

  /** relative to MVC */
  typedef otb::SpectrumModuleView         SpectrumModuleViewType;
  typedef SpectrumModuleViewType::Pointer SpectrumModuleViewPointerType;
  //view subtype
  typedef SpectrumModuleViewType::ImageViewType ImageViewType;

  //controller subtype
  typedef Superclass::PixelSpectrumControllerType PixelSpectrumControllerType;

  /** Handlers */
  typedef otb::WidgetResizingActionHandler
  <ImageModelType,
   ImageViewType>
  ResizingHandlerType;

  typedef otb::ChangeScaledExtractRegionActionHandler
  <ImageModelType,
   ImageViewType>
  ChangeScaledRegionHandlerType;

  typedef otb::ChangeExtractRegionActionHandler
  <ImageModelType,
   ImageViewType>
  ChangeRegionHandlerType;

  typedef otb::ChangeScaleActionHandler
  <ImageModelType,
   ImageViewType>
  ChangeScaleHandlerType;

  typedef otb::PixelSpectrumActionHandler
  <PixelSpectrumControllerType,
   ImageViewType>                                                           PixelSpectrumActionHandlerType;

  void SetSpectrumModuleView(SpectrumModuleViewPointerType s);

  SpectrumModuleViewPointerType GetSpectrumModuleView()
  {
    return m_SpectrumModuleView;
  }

  /** New macro */
  itkNewMacro(Self);

  /** Methods */
  //itkSetObjectMacro(SpectrumModuleView, SpectrumModuleViewType);
  //itkGetObjectMacro(SpectrumModuleView, SpectrumModuleViewType);

  virtual void SetSpectrumModuleModel(SpectrumModuleModelType * m);
  virtual void UpdateViewerDisplay();
  virtual void GenerateSpectralAngle(int Id);
  virtual void SetSpectralLayerOpacity(float f);
  virtual void WriteSpreadsheetFile(int id, const char *path);
  virtual void ForceSpectralContrast(float c);
  virtual void UseColorMap(bool b);
  virtual void SaveAndQuit();
  virtual void Quit();

protected:
  SpectrumModuleController();
  ~SpectrumModuleController();

private:

  SpectrumModuleViewPointerType m_SpectrumModuleView;

  /** relative to Handlers */
  ResizingHandlerType::Pointer            m_ResizingHandler;
  ChangeScaledRegionHandlerType::Pointer  m_ChangeScaledRegionHandler;
  ChangeRegionHandlerType::Pointer        m_ChangeHandler;
  ChangeScaleHandlerType::Pointer         m_ChangeScaleHandler;
  PixelSpectrumActionHandlerType::Pointer m_PixelActionHandler;

};
}

#endif
