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

#ifndef __otbGCPToSensorModelView_h
#define __otbGCPToSensorModelView_h

#include "otbGCPToSensorModelViewGroup.h"

#include "otbListenerBase.h"
#include "otbGCPToSensorModelModel.h"
#include "otbGCPToSensorModelControllerInterface.h"

#include "otbCrossGlComponent.h"
#include "otbCircleGlComponent.h"
#include "otbImageView.h"

namespace otb
{
/** \class GCPToSensorModelView
 *
 */
class ITK_EXPORT GCPToSensorModelView
      : public ListenerBase, public GCPToSensorModelViewGroup, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef GCPToSensorModelView  Self;
  typedef itk::Object                          Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(GCPToSensorModelView,Object);


  typedef GCPToSensorModelModel::VisualizationModelType     VisualizationModelType;
  typedef GCPToSensorModelModel::RGBPixelType               PixelType;
  typedef GCPToSensorModelModel::IndexType                  IndexType;
  typedef GCPToSensorModelModel::ContinuousIndexType        ContinuousIndexType;
  typedef GCPToSensorModelModel::GCPType                    GCPType;
  typedef GCPToSensorModelModel::GCPsContainerType          GCPsContainerType;
  typedef GCPToSensorModelModel::ErrorsContainerType        ErrorsContainerType;
  typedef GCPToSensorModelModel::CrossIndexesContainerType  CrossIndexesContainerType;
  typedef GCPToSensorModelModel::Point2DType                Point2DType;
  typedef GCPToSensorModelModel::Point3DType                Point3DType;  
  typedef ImageView<VisualizationModelType>                 ImageViewType;
  typedef CrossGlComponent::ColorType                       ColorType;
  typedef std::vector<ColorType>                            ColorListType;

  /** Event from the model */
  virtual void Notify();

  /** Fluid call backs*/
  virtual void Quit();

  /** Set the controller */
  itkSetObjectMacro(Controller,GCPToSensorModelControllerInterface);
  itkGetObjectMacro(Controller,GCPToSensorModelControllerInterface);

  /** Set the widgets controller */
  itkSetObjectMacro(WidgetController,ImageWidgetController);
  itkSetObjectMacro(MapWidgetController,ImageWidgetController);

  /** Build the interface */
  virtual void BuildInterface();
  virtual void AddPoints();
  virtual void ClearPointList();
  virtual void DeletePoint();
  virtual void Focus(unsigned int i);
  virtual void ClearTransformationInfo();
  virtual void SelectDEM();
  virtual void ChangeDEM();
  virtual void HideAll();
  virtual void ReloadGCPs();
  virtual void SetProjectionType();
  virtual void ImportGcpsFromXmlFile();
  virtual void ExportGcpsToXmlFile();

  /** Street Map window */
  virtual void UpdatePlaceName();
  virtual void UpdateLonLat();
  virtual void DisplayMap();
  
  void UpdateListSelectionColor();
  void ChangePointValue(ContinuousIndexType index );//, unsigned int viewId );
  void AddPointsToList( GCPType gcp, double error, IndexType index );
  void RedrawWidgets();
  void Show();
  
  /** Get a pointer to the view parts for the controller */
  itkGetObjectMacro(ImageView,ImageViewType);
  itkGetObjectMacro(MapView,ImageViewType);

  void SetModel(GCPToSensorModelModel* model);
  
protected:
  /** Constructor */
  GCPToSensorModelView();

  /** Destructor */
  virtual ~GCPToSensorModelView();


private:
  GCPToSensorModelView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  void UpdateGCPView();

  /** Pointer to the controller */
  GCPToSensorModelControllerInterface::Pointer  m_Controller;
  /** Pointer to the widgets controller */
  ImageWidgetController::Pointer                m_WidgetController;
  /** Pointer to the widgets controller */
  ImageWidgetController::Pointer                m_MapWidgetController;
  /** Pointer to the model */
  GCPToSensorModelModel::Pointer                m_Model;
  /** Image view */
  ImageViewType::Pointer                        m_ImageView;
  ImageViewType::Pointer                        m_MapView;
  /** Cross Gl Component */
  CrossGlComponent::Pointer                     m_CrossGlComponent;
  /** Circle Gl Component */
  CircleGlComponent::Pointer                    m_CircleGlComponent;
  /** Point color */
  ColorListType                                 m_ColorList;
  /** Green color */
  ColorType                                     m_Green;
  
  /** Draw map */
  void DrawMap();
};
}//end namespace otb

#endif
