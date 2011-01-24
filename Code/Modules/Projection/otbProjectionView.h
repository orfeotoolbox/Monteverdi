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
#ifndef __otbProjectionView_h
#define __otbProjectionView_h

#include "otbListenerBase.h"

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbProjectionGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include  "itkObject.h"
#include  "itkObjectFactory.h"

#include "otbObjectList.h"
#include "otbVectorImage.h"
#include <FL/Fl_File_Chooser.H>

#include "otbDEMToImageGenerator.h"
#include "itkInterpolateImageFunction.h"
#include "otbProjectionControllerInterface.h"
#include "otbProjectionModel.h"

// Remote Sensing Transformation
#include "otbGenericRSTransform.h"

// include the base class
#include "otbModule.h"

#include "otbProjectionEnum.h"

// rendering the preview
#include "itkRGBAPixel.h"
#include "otbImageWidget.h"
#include "otbImage.h"
#include "otbImageLayer.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayerRenderingModel.h"

namespace otb
{
/** \class ProjectionView
 *  \brief
 *
 * \ingroup
 * \ingroup
 */
class ITK_EXPORT ProjectionView
  : public ListenerBase, public ProjectionGroup, public itk::Object
{
public:
  /** Standard typedefs */
  typedef ProjectionView                Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ProjectionView, itk::Object);

  /** typedef projection model*/
  typedef ProjectionModel            ModelType;
  typedef ModelType::InputImageType  ImageType;
  typedef ModelType::TransformType   TransformType;
  typedef ModelType::OutputPointType PointType;
  typedef ModelType::IndexType       IndexType;
  typedef ModelType::SingleImageType SingleImageType;

  /** Widget for the preview*/
  typedef ImageWidget<>              ImageWidgetType;
  typedef ImageWidgetType::SizeType  SizeType;
  typedef ImageWidgetType::Pointer   ImageWidgetPointerType;
  
  /** Useful typedefs for the preview rendering */
  typedef Image<itk::RGBAPixel<unsigned char> >        ViewerImageType;
  typedef otb::ImageLayer<ImageType, ViewerImageType>  LayerType;
  typedef otb::ImageLayerGenerator<LayerType>          LayerGeneratorType;
  
  typedef otb::ImageLayerRenderingModel<ViewerImageType>   VisuModelType;


  // Called before building the GUI
  virtual void InitializeAction();
  // Init2ialize transform and show the GUI
  virtual void Show();
  //  Shpw the GUI
  virtual void ShowGUI();

  // Method to set the controller
  itkGetObjectMacro(Controller, ProjectionControllerInterface);
  itkSetObjectMacro(Controller, ProjectionControllerInterface);

  // Set the controller
  void SetProjectionController(ProjectionControllerInterface::Pointer pController)
  {
    m_Controller = pController;
  }

  // Hide the GUI
  virtual void Hide();

  // Tab position handler
  virtual void TabPositionHandler();

protected:

  // Browser DEM
  virtual void Browse();

  // Update the use dem statuts
  void UpdateDEMUse();

  // 
  virtual void RetrieveInputProjection();

  // Launch the orthorectification & Quit the application
  virtual void OK();

  // Set/Get the used map projection
  void SetMapType(ProjectionMapType map);
  ProjectionMapType GetMapType();

  // Set/Get the used interpolator
  void SetInterpolatorType(ProjectionInterpolatorType interp);
  ProjectionInterpolatorType GetInterpolatorType();

  // Change east/north when change Long/Lat values
  virtual void UpdateMapParam();
  // Update parameter for output image
  virtual void UpdateOutputParameters();
  // Update longitude and latitude values
  virtual void UpdateLongLat();
  // Update longitude and latitude values : In case of unknown input map
  virtual void UpdateUnavailableLongLat();
  // Take care that the ortho ref and image ref are different (lower left vs upper left)
  //ForwardSensorInputPointType ChangeOrigin(ForwardSensorInputPointType point);
  // Update interpolator
  virtual int UpdateInterpolator();

  //
  virtual void UpdateUTMTransform();
  //
  virtual void UpdateLambertIITransform();
  //
  virtual void UpdateTMTransform();
  //
  virtual void UpdateWGS84Transform();

  virtual void UpdateInputUTMTransform();
  //
  virtual void UpdateInputLambertIITransform();
  //
  virtual void UpdateInputTMTransform();
  //
  virtual void UpToDateTransform();
  
  // Set the estimation rpc model possible
  virtual void UpdateRpcEstimation();

  /** Model Notificatin catcher*/
  virtual void Notify();

  // Find parameter value in the projection Ref tree
  virtual bool FindParameter(OGRSpatialReference oSRS, const char * inParam, double * paramValue);

  // Display preview widget
  virtual void DisplayPreviewWidget();

  // Method to compute the isotropic zoom
  double UpdatePreviewWidgetIsotropicZoom(SizeType size);

  /** Constructor */
  ProjectionView();
  /** Destructor */
  virtual ~ProjectionView()
  {
  }

private:
  ProjectionView(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ProjectionMapType          m_MapType;
  ProjectionInterpolatorType m_InterpType;

  // Flag to determine if there is an output
  bool m_HasOutput;

  // Flag to determine if the input Projection Map is unknowm
  bool m_InputProjectionUnknown;

  // Controller instance
  ProjectionControllerInterface::Pointer m_Controller;

  /**ImageWidget for my preview*/
  ImageWidgetPointerType                 m_PreviewWidget;
  
  ModelType::ResampleFilterType::Pointer m_Transform;
  
};

} // End namespace otb

#endif
