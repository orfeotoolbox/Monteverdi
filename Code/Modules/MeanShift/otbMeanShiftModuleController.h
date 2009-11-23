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

#ifndef __otbMeanShiftModuleController_h
#define __otbMeanShiftModuleController_h

#include "otbMeanShiftModuleControllerInterface.h"
#include "otbMeanShiftModuleView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbMouseClickActionHandler.h"
#include "itkMultiThreader.h"

namespace otb
{
class ITK_EXPORT MeanShiftModuleController
      : public MeanShiftModuleControllerInterface
{
public:
  /** Standard class typedefs */
  typedef MeanShiftModuleController          Self;
  typedef MeanShiftModuleControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Standard type macros */
  itkTypeMacro(MeanShiftModuleController,Superclass);
  itkNewMacro(Self);


  /** Widgets controller and action handlers */
  typedef MeanShiftModuleView::ImageViewType              ImageViewType;
  typedef MeanShiftModuleModel::VisualizationModelType    VisualizationModelType;

  typedef ImageWidgetController                                                        WidgetsControllerType;
  typedef WidgetResizingActionHandler<VisualizationModelType,ImageViewType>            ResizingHandlerType;
  typedef ChangeExtractRegionActionHandler<VisualizationModelType,ImageViewType>       ChangeRegionHandlerType;
  typedef ChangeScaledExtractRegionActionHandler<VisualizationModelType,ImageViewType> ChangeScaledRegionHandlerType;
  typedef ChangeScaleActionHandler<VisualizationModelType,ImageViewType>               ChangeScaleHandlerType;

  void SetModel(ModelType* model);

  /** Set the pointer to the view */
  void SetView(MeanShiftModuleView * view);

  /** Get the widgets controller */
  itkGetObjectMacro(WidgetsController,WidgetsControllerType);


  /** User action */
  virtual void OpenImage( const char * filename );
  virtual void RunSegmentation();
  virtual void SwitchClusters(bool sc);
  virtual void SwitchBoundaries(bool sb);
  virtual void SetSpatialRadius(unsigned int sr);
  virtual void SetSpectralRadius(unsigned int sr);
  virtual void SetMinRegionSize(unsigned int mr);
  virtual void SetOpacity( double op );


  virtual void SetInputImage(MeanShiftModuleModel::VectorImageType* image)
  {
    m_Model->SetInputImage(image);
  }


  bool IsModelUpdating()
  {
    return m_Model->IsUpdating();
  }
  
  virtual void Quit();
protected:
  /** Constructor */
  MeanShiftModuleController();
  /** Destructor */
  virtual ~MeanShiftModuleController();


private:
  MeanShiftModuleController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  MeanShiftModuleView * m_View;

  /** The instance of the model */
  ModelType * m_Model;


  /** Widgets controller */
  WidgetsControllerType::Pointer             m_WidgetsController;

  /** Action handlers */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;
  ChangeScaledRegionHandlerType::Pointer     m_ChangeScaledRegionHandler;
  ChangeScaleHandlerType::Pointer            m_ChangeScaleHandler;

  itk::MultiThreader::Pointer m_Threader;

  bool m_ImageReady;

};
} //end namespace otb

#endif
