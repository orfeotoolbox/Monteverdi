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
#ifndef __otbVectorDataTreeBrowser_h
#define __otbVectorDataTreeBrowser_h

#include <FLU/Flu_Tree_Browser.h>
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "otbFieldEditorGUI.h"
#include "otbVectorizationControllerInterface.h"

namespace otb
{

/** \class VectorDataTreeBrowser
 *  \brief Simple tree browser for VectorData
 */
template <class TVectorData>
class ITK_ABI_EXPORT VectorDataTreeBrowser
  : public Flu_Tree_Browser, public itk::Object
{
public:
/** Standard class typedefs */
  typedef VectorDataTreeBrowser         Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

/** Creation through object factory */
  itkNewMacro(Self);

/** VectorData typedef */
  typedef TVectorData VectorDataType;
  typedef typename VectorDataType
  ::Pointer VectorDataPointerType;
  typedef typename VectorDataType
  ::DataNodeType DataNodeType;
  typedef typename DataNodeType
  ::Pointer DataNodePointerType;
  typedef typename Flu_Tree_Browser
  ::Node FluNodeType;
  typedef typename DataNodeType
  ::PointType PointType;
  typedef typename DataNodeType
  ::PolygonType PolygonType;
  typedef typename DataNodeType
  ::LineType LineType;

  typedef std::map<DataNodeType *,
      FluNodeType *>                      DataToFluNodeMapType;

  typedef std::map<NodeType,
      unsigned int>                       NumberingMapType;

  typedef std::vector<FluNodeType *> FluNodeVectorType;

  typedef typename VectorDataKeywordlist
  ::FieldType FieldType;

  typedef FieldEditorGUI FieldEditorType;
  typedef typename FieldEditorType
  ::Pointer FieldEditorPointerType;

  typedef typename PolygonType::RegionType                 RegionType;
  typedef typename RegionType::SizeType                    SizeType;
  typedef typename RegionType::IndexType                    IndexType;

/** RTT type info */
  itkTypeMacro(VectorDataTreeBrowser, itk::Object);

/** Update representation */
  void Update();

/** Clear browser */
  void Clear();

  itkSetObjectMacro(VectorData, VectorDataType);
  itkGetObjectMacro(VectorData, VectorDataType);

  void SetController(VectorizationControllerInterface * controller);
  itkGetObjectMacro(Controller, VectorizationControllerInterface);

protected:
/** Constructor */
  VectorDataTreeBrowser();

/** Destructor */
  ~VectorDataTreeBrowser();

/** PrintSelf method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

/** Display node attributes */
  void ReportAttributes(FluNodeType * fluNode, DataNodeType * dataNode);

/** Display points information */
  void ReportPoint(FluNodeType * fluNode, DataNodeType * dataNode);

/** Display line information */
  void ReportLine(FluNodeType * fluNode, DataNodeType * dataNode);

/** Display polygon information */
  void ReportPolygon(FluNodeType * fluNode, DataNodeType * dataNode);

private:
  VectorDataTreeBrowser(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

/** Find or create new node */
  FluNodeType * FetchNode(FluNodeType * parent, DataNodeType * dataNode);

  FluNodeType * FetchSubNode(FluNodeType * parent, const std::string& label);

/** Create an x/y point field */
  FluNodeType * UpdateOrCreatePointField(FluNodeType * parent, const char * label, const PointType& point);

/** Create an attribute field */
/*FluNodeType **/ void UpdateOrCreateAttributeField(FluNodeType * parent, const FieldType& field);

/** Clear unused nodes */
  void RemoveUnusedNodes(const FluNodeVectorType& nodes);

/** Main callbacks */
  static void TreeCallback(Fl_Widget* w, void*);

  static void DeleteGeometryCallback(Fl_Widget* w, void* data);

  void DeleteSelectedGeometry();

  static void DeletePointCallback(Fl_Widget* w, void* data);

  void DeleteSelectedPoint();

  static void AddFieldCallback(Fl_Widget* w, void* data);

  void AddFieldToSelectedGeometry();

  static void DeleteFieldCallback(Fl_Widget*w, void *data);

  void DeleteSelectedField();

  static void FocusCallback(Fl_Widget* w, void* data);

  void FocusOnSelectedGeometry();

  void PopupMenu(FluNodeType * node);

  int handle(int event);

/** VectorData pointer */
  VectorDataPointerType m_VectorData;

/** Correspondance between data node and flu node */
  DataToFluNodeMapType m_NodeMap;

/** The numbering map */
  NumberingMapType m_NumberingMap;

  std::string                     m_GeometricDataKey;
  std::string                     m_AttributesKey;
  std::string                     m_ExteriorRingKey;
  std::string                     m_InteriorRingsKey;
  std::string                     m_InteriorRingKey;
  std::map<NodeType, std::string> m_NodeNameMap;

  FieldEditorPointerType m_FieldEditor;

  typename VectorizationControllerInterface::Pointer m_Controller;

}; // End class VectorDataTreeBrowser
} // End namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbVectorDataTreeBrowser.txx"
#endif

#endif
