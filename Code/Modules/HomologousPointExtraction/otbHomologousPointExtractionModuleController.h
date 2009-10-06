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

#ifndef __otbHomologousPointExtractionModuleController_h
#define __otbHomologousPointExtractionModuleController_h

#include "otbHomologousPointExtractionModuleControllerInterface.h"
#include "otbHomologousPointExtractionModuleView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbMouseClickActionHandler.h"
#include "itkMultiThreader.h"

namespace otb
{
class ITK_EXPORT HomologousPointExtractionModuleController
      : public HomologousPointExtractionModuleControllerInterface
{
public:
  /** Standard class typedefs */
  typedef HomologousPointExtractionModuleController          Self;
  typedef HomologousPointExtractionModuleControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Standard type macros */
  itkTypeMacro(HomologousPointExtractionModuleController,Superclass);
  itkNewMacro(Self);


  /** Widgets controller and action handlers */
  typedef HomologousPointExtractionModuleView::ImageViewType              ImageViewType;
  typedef HomologousPointExtractionModuleModel::VisualizationModelType    VisualizationModelType;

  typedef ImageWidgetController                                     WidgetsControllerType;
  typedef WidgetResizingActionHandler
    <VisualizationModelType,ImageViewType>                          ResizingHandlerType;
  typedef ChangeExtractRegionActionHandler
      <VisualizationModelType,ImageViewType>                        ChangeRegionHandlerType;

  void SetModel(ModelType* model);

  /** Set the pointer to the view */
  void SetView(HomologousPointExtractionModuleView * view);

  /** Get the widgets controller */
  itkGetObjectMacro(WidgetsController,WidgetsControllerType);


  virtual void SetFirstInputImage(HomologousPointExtractionModuleModel::VectorImageType* image)
  {
    m_Model->SetFirstInputImage(image);
  }
  virtual void SetSecondInputImage(HomologousPointExtractionModuleModel::VectorImageType* image)
  {
    m_Model->SetSecondInputImage(image);
  }
 

protected:
  /** Constructor */
  HomologousPointExtractionModuleController();
  /** Destructor */
  virtual ~HomologousPointExtractionModuleController();


private:
  HomologousPointExtractionModuleController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  HomologousPointExtractionModuleView * m_View;

  /** The instance of the model */
  ModelType * m_Model;


  /** Widgets controller */
  WidgetsControllerType::Pointer             m_WidgetsController;

  /** Action handlers */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;

  bool m_ImageReady;

};
} //end namespace otb

#endif
