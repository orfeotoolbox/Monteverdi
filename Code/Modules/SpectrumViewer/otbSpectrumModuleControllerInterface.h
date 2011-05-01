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
#ifndef __otbSpectrumModuleControllerInterface_h
#define __otbSpectrumModuleControllerInterface_h

#include <itkObject.h>

#include "otbSpectrumModuleModel.h"
#include "otbImageWidgetController.h"
#include "otbPixelSpectrumController.h"

#include "otbWidgetResizingActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbPixelSpectrumActionHandler.h"

namespace otb {

class SpectrumModuleControllerInterface
: public itk::Object
  {
public:
  /** Standard class typedefs */
  typedef SpectrumModuleControllerInterface Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  /** Relative to MVC */
  typedef otb::SpectrumModuleModel         SpectrumModuleModelType;
  typedef SpectrumModuleModelType::Pointer SpectrumModuleModelPointerType;
  /** subtypes */
  typedef SpectrumModuleModelType::ImageModelType ImageModelType;

  /** Controllers */
  typedef otb::ImageWidgetController         ImageWidgetControllerType;
  typedef ImageWidgetControllerType::Pointer ImageWidgetControllerPointerType;

  typedef otb::PixelSpectrumController         PixelSpectrumControllerType;
  typedef PixelSpectrumControllerType::Pointer PixelSpectrumControllerPointerType;

  /** Methods */
  //itkSetObjectMacro(SpectrumModuleModel, SpectrumModuleModelType);
  itkGetObjectMacro(SpectrumModuleModel, SpectrumModuleModelType);

  virtual void SetSpectrumModuleModel(SpectrumModuleModelType * m) = 0;

  itkSetObjectMacro(ImageWidgetController, ImageWidgetControllerType);
  itkGetObjectMacro(ImageWidgetController, ImageWidgetControllerType);

  itkSetObjectMacro(PixelSpectrumController, PixelSpectrumControllerType);
  itkGetObjectMacro(PixelSpectrumController, PixelSpectrumControllerType);

  virtual void SaveAndQuit() = 0;
  virtual void Quit() = 0;
  virtual void UpdateViewerDisplay() = 0;
  virtual void GenerateSpectralAngle(int Id) = 0;
  virtual void ClearSpectralAngle() = 0;
  virtual void SetSpectralLayerOpacity(float f) = 0;
  virtual void WriteSpreadsheetFile(int id, const char *path) = 0;
  virtual void ForceSpectralContrast(float c) = 0;
  virtual void UseColorMap(bool b) = 0;

protected:
  SpectrumModuleControllerInterface()
    {
    }
  ~SpectrumModuleControllerInterface(){}

  SpectrumModuleModelPointerType     m_SpectrumModuleModel;
  ImageWidgetControllerPointerType   m_ImageWidgetController;
  PixelSpectrumControllerPointerType m_PixelSpectrumController;

  };
}

#endif
