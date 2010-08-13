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
#ifndef __otbVectorizationController_h
#define __otbVectorizationController_h

#include "otbVectorizationControllerInterface.h"
#include "otbVectorizationView.h"
#include "otbImageWidgetController.h"

// Handlers list
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbChangeScaledExtractRegionActionHandler.h"
#include "otbChangeScaleActionHandler.h"
#include "otbVectorDataActionHandler.h"

namespace otb
{
class ITK_EXPORT VectorizationController
  : public VectorizationControllerInterface
{
public:
  /** Standard class typedefs */
  typedef VectorizationController          Self;
  typedef VectorizationControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Standard type macros */
  itkTypeMacro(VectorizationController, Superclass);
  itkNewMacro(Self);

  /** Widgets controller and action handlers */
  typedef VectorizationView
  ::ImageViewType ImageViewType;
  typedef VectorizationModel
  ::VisualizationModelType VisualizationModelType;
  typedef VectorizationModel
  ::VectorDataModelType VectorDataModelType;
  typedef VectorizationModel
  ::VectorImageType VectorImageType;

  /** Handlers */
  typedef ImageWidgetController WidgetControllerType;
  typedef WidgetResizingActionHandler
  <VisualizationModelType, ImageViewType> ResizingHandlerType;
  typedef ChangeExtractRegionActionHandler
  <VisualizationModelType, ImageViewType> ChangeRegionHandlerType;
  typedef ChangeScaledExtractRegionActionHandler
  <VisualizationModelType, ImageViewType> ChangeScaledRegionHandlerType;
  typedef ChangeScaleActionHandler
  <VisualizationModelType, ImageViewType> ChangeScaleHandlerType;
  typedef VectorDataActionHandler
  <VectorDataModelType,
      ImageViewType>                        VectorDataActionHandlerType;

  void SetModel(ModelType* model);

  /** Set the pointer to the view */
  void SetView(VectorizationView * view);

  /** Get the widgets controller */
  itkGetObjectMacro(WidgetController, WidgetControllerType);

  virtual void SetInputImage(VectorizationModel::VectorImageType* image)
  {
    m_Model->SetImage(image);
  }

  virtual void RemoveDataNode(DataNodeType * node);
  virtual void SetDataNodeFieldAsInt(DataNodeType * node, const std::string& name, int value);
  virtual void SetDataNodeFieldAsFloat(DataNodeType * node, const std::string& name, float value);
  virtual void SetDataNodeFieldAsString(DataNodeType* node, const std::string& name, const std::string& value);
  virtual void RemoveFieldFromDataNode(DataNodeType * node, const std::string& name);
  virtual void RemovePointFromDataNode(DataNodeType * node,
                                       const unsigned long& index,
                                       bool interiorRing,
                                       const unsigned int& interiorRingIndex = 0);
  virtual void UpdatePointFromDataNode(DataNodeType * node,
                                       const unsigned long& index,
                                       const PointType& point,
                                       bool interiorRing,
                                       const unsigned int& interiorRingIndex = 0);
  virtual void ChangeNavigationMode();

protected:
  /** Constructor */
  VectorizationController();
  /** Destructor */
  virtual ~VectorizationController();

private:
  VectorizationController(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Pointer to the view */
  VectorizationView * m_View;

  /** The instance of the model */
  ModelType * m_Model;

  /** Widgets controller */
  WidgetControllerType::Pointer m_WidgetController;

  /** Action handlers */
  ResizingHandlerType::Pointer           m_ResizingHandler;
  ChangeRegionHandlerType::Pointer       m_ChangeRegionHandler;
  ChangeScaledRegionHandlerType::Pointer m_ChangeScaledRegionHandler;
  ChangeScaleHandlerType::Pointer        m_ChangeScaleHandler;
  VectorDataActionHandlerType::Pointer   m_VectorDataActionHandler;
};
} //end namespace otb

#endif
