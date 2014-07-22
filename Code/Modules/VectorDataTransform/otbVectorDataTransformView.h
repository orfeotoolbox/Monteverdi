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
#ifndef __otbVectorDataTransformView_h
#define __otbVectorDataTransformView_h

#include "otbVectorDataTransformGUI.h"

#include "otbEventsListener.h"
#include "otbVectorDataTransformModel.h"
#include "otbVectorDataTransformControllerInterface.h"

#include "otbImageView.h"
#include "otbPixelDescriptionView.h"
#include "otbVectorDataGlComponent.h"

#include <FL/Fl_Color_Chooser.H>

namespace otb
{
/** \class VectorDataTransformView
 *
 */
class ITK_ABI_EXPORT VectorDataTransformView
  :  public EventsListener<std::string>,
     public VectorDataTransformGUI,
     public itk::Object
{
public:
  /** Standard class typedefs */
  typedef VectorDataTransformView  Self;
  typedef itk::Object                    Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  /** Standard macros */
  itkNewMacro(Self);
  itkTypeMacro(VectorDataTransformView, Object);

  /** Visualization typedefs */
  typedef VectorDataTransformModel::RGBPixelType              RGBPixelType;
  typedef VectorDataTransformModel::VisualizationModelType    VisualizationModelType;
  typedef VectorDataTransformModel::PixelDescriptionModelType PixelDescriptionModelType;

  typedef ImageView<VisualizationModelType>                   ImageViewType;
  typedef PixelDescriptionView<PixelDescriptionModelType>     PixelViewType;
  
  typedef VectorDataTransformModel::VectorDataType            VectorDataType;
  typedef VectorDataGlComponent<VectorDataType>               VectorDataGlComponentType;
  typedef std::vector<VectorDataGlComponentType * >           VectorDataGlComponentVectorType;

  /** VectorData Color type  */
  typedef itk::FixedArray<double, 4>                           ColorType;

  /** Event from the model */
  virtual void Notify(const std::string & event);

  /** Callbacks */
  
  /** Exit*/
  virtual void Exit();
  /** */
  
  /** Save learning params to an xml file */
  void SaveClassificationParametersToXMLFile();

  /** Set the application controller */
  void SetController(VectorDataTransformControllerInterface * controller);

  /** Set the widgets controller */
  void SetWidgetsController(ImageWidgetController * controller);

  /** Set the model */
  void SetModel(VectorDataTransformModel * model)
  {
    m_Model = model;
    m_Model->RegisterListener(this);
  }

  /** Build the interface */
  virtual void Build();

  /** Dispay the OSM VectorData computed in the model */
  virtual void DisplayVectorData();
  
  /** Hide the displayed OSM Vector Data */
  virtual void HideVectorData();

  /** Apply transform to VectorData */
  virtual void ApplyTransformToVectorData();

  /** Hide all the windows */
  virtual void Hide();

  /** Show the Gui */
  virtual void Show();
  
  /** Get a pointer to the view parts for the controller */
  itkGetObjectMacro(ImageView, ImageViewType);
  itkGetObjectMacro(PixelView, PixelViewType);
  
protected:
  /** Constructor */
  VectorDataTransformView();
  
  /** Destructor */
  ~VectorDataTransformView();

  /** Gui Initialization with default values */
  void InitializeGui();
  
private:
  VectorDataTransformView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  /** Pointer to the controller */
  VectorDataTransformControllerInterface *    m_Controller;
  
  /** Pointer to the widgets controller */
  ImageWidgetController *                     m_WidgetsController;

  /** Pointer to the model */
  VectorDataTransformModel *                  m_Model;

  /** Image view */
  ImageViewType::Pointer                      m_ImageView;
  
  /** Pixel view */
  PixelViewType::Pointer                      m_PixelView;

  /** Green Color Type */
  ColorType                                   m_Green;

};
}//end namespace otb

#endif
