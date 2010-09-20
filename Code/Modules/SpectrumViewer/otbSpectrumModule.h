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
#ifndef __otbSpectrumModule_h
#define __otbSpectrumModule_h

#include "otbEventsListener.h"

// include the base class
#include "otbModule.h"

// the MVC classes
#include "otbSpectrumModuleController.h"
#include "otbSpectrumModuleModel.h"
//#include "otbSpectrumModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"

namespace otb {

class SpectrumModule
: public Module, public EventsListener<std::string>
{
public:
  /** Standard class typedefs */
  typedef SpectrumModule                Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** General types */
  typedef TypeManager::Floating_Point_VectorImage ImageType;
  typedef TypeManager::Labeled_Short_VectorImage  LabelImageType;

  //typedef double                               PixelType;
  typedef itk::RGBPixel<unsigned char> RGBPixelType;
  typedef otb::Image<RGBPixelType, 2>  OutputImageType;
  //typedef itk::VariableLengthVector<PixelType> PixelVectorType;

  typedef otb::ImageLayer <ImageType, OutputImageType> LayerType;
  typedef otb::ImageLayerGenerator<LayerType> LayerGeneratorType;

  /** Relative to MVC - Model*/
  typedef otb::SpectrumModuleModel SpectrumModuleModelType;
  /** Subtypes */
  typedef SpectrumModuleModelType::ImageModelType         ImageModelType;
  typedef SpectrumModuleModelType::PixelSpectrumModelType PixelSpectrumModelType;

  /** Relative to MVC - Controller*/
  typedef otb::SpectrumModuleController SpectrumModuleControllerType;
  /** Subtypes */
  typedef SpectrumModuleControllerType::ImageWidgetControllerType   ImageWidgetControllerType;
  typedef SpectrumModuleControllerType::PixelSpectrumControllerType PixelSpectrumControllerType;

  /** Relative to MVC - View*/
  typedef otb::SpectrumModuleView         SpectrumModuleViewType;
  typedef SpectrumModuleViewType::Pointer SpectrumModuleViewPointerType;

  /** Subtypes */
  typedef SpectrumModuleViewType::ImageViewType         ImageViewType;
  typedef SpectrumModuleViewType::PixelSpectrumViewType PixelSpectrumViewType;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SpectrumModule, Module);

  /** Methods */
  itkGetObjectMacro(SpectrumModuleController, SpectrumModuleControllerType);
  itkSetObjectMacro(SpectrumModuleController, SpectrumModuleControllerType);

  itkGetObjectMacro(SpectrumModuleModel, SpectrumModuleModelType);
  itkSetObjectMacro(SpectrumModuleModel, SpectrumModuleModelType);

  //itkGetObjectMacro(SpectrumModuleView, SpectrumModuleViewType);
  //itkSetObjectMacro(SpectrumModuleView, SpectrumModuleViewType);

  SpectrumModuleViewPointerType GetSpectrumModuleView()
  {
    return m_SpectrumModuleView;
  }

  void SetSpectrumModuleView(SpectrumModuleViewPointerType v)
  {
    m_SpectrumModuleView = v;
  }

protected:
  SpectrumModule();
  virtual ~SpectrumModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify(const std::string& event);

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_SpectrumModuleView->Show();
  }

  /** Hide the Module GUI */
  virtual void Hide()
  {
    m_SpectrumModuleView->Hide();
  }

private:
  /** Relative to global MVC */
  SpectrumModuleControllerType::Pointer m_SpectrumModuleController;
  SpectrumModuleModelType::Pointer      m_SpectrumModuleModel;
  SpectrumModuleViewType::Pointer       m_SpectrumModuleView;

};
}

#endif
