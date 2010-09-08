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
#ifndef __otbotbVectorDataTreeBrowser_txx
#define __otbotbVectorDataTreeBrowser_txx

#include "otbMonteverdiEnum.h"

#include "otbVectorDataTreeBrowser.h"
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Item.H>

#include <map>

namespace otb
{
/** Constructor */
template <class TVectorData>
VectorDataTreeBrowser<TVectorData>
::VectorDataTreeBrowser()
  : Flu_Tree_Browser(0, 0, 0, 0)
{
  this->box(FL_DOWN_BOX);
  this->auto_branches(true);
  this->label(otbGetTextMacro("VectorData browser"));

  // allow callback with the tree
  this->box(FL_DOWN_BOX);
  this->allow_dnd(true);
  this->always_open(true);
  this->allow_leaf_duplication(false);

  // this->when( FL_WHEN_RELEASE );
  // this->callback( TreeCallback );

  // animate the tree
  this->animate(true);
  this->collapse_time(0.02);
  this->frame_rate(500);

  //Flu_Tree_Browser::Node* root = this->first();
  typename Flu_Tree_Browser::Node* root = this->first();
  root->open(true);
  root->label(otbGetTextMacro("VectorData"));

  this->show_root(false);
  this->insertion_mode(FLU_INSERT_BACK);

  // Initialize numbering map
  m_NumberingMap[FOLDER] = 0;
  m_NumberingMap[DOCUMENT] = 0;
  m_NumberingMap[FEATURE_POINT] = 0;
  m_NumberingMap[FEATURE_LINE] = 0;
  m_NumberingMap[FEATURE_POLYGON] = 0;

  // TODO: Add GetTexMacro here.
  m_GeometricDataKey = "Geometric Data";
  m_AttributesKey = "Attributes";
  m_ExteriorRingKey = "Exterior ring";
  m_InteriorRingsKey = "Interior rings";
  m_InteriorRingKey = "Interior ring";

  m_NodeNameMap[FOLDER] = "Folder";
  m_NodeNameMap[DOCUMENT] = "Document";
  m_NodeNameMap[FEATURE_POINT] = "Point";
  m_NodeNameMap[FEATURE_POLYGON] = "Polygon";
  m_NodeNameMap[FEATURE_LINE] = "Line";

  this->callback(TreeCallback);

  m_FieldEditor = FieldEditorType::New();
}

/** Destructor */
template <class TVectorData>
VectorDataTreeBrowser<TVectorData>
::~VectorDataTreeBrowser()
{}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::SetController(VectorizationControllerInterface * controller)
{
  m_Controller = controller;
  m_FieldEditor->SetController(controller);
  this->Modified();
}

/** Update */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::Update()
{
  // If the vector data is not null
  if (m_VectorData.IsNotNull())
    {
    // First, clear existing display
    // this->Clear();

    // Get root node
    typename Flu_Tree_Browser::Node* root = this->first();
    typename Flu_Tree_Browser::Node* parent = root;

    // Refresh display
    typename VectorDataType::DataTreeType
    ::PreOrderIteratorType it(m_VectorData->GetDataTree(),
                              m_VectorData->GetDataTree()->GetRoot());
    it.GoToBegin();

    // StringStream used to build strings
    itk::OStringStream oss;
    oss.str("");

    FluNodeVectorType updatedFluNodes;

    while (!it.IsAtEnd())
      {
      // Get the data node
      DataNodePointerType node = it.GetNode()->Get();
      switch (node->GetNodeType())
        {
        case ROOT:
          {
          parent = root;
          break;
          }
        case DOCUMENT:
          {
          FluNodeType * fnode = this->FetchNode(parent, node);
          this->ReportAttributes(fnode, node);
          if (node)
            {
            parent = fnode;
            updatedFluNodes.push_back(fnode);
            }
          break;
          }
        case FOLDER:
          {
          FluNodeType * fnode = this->FetchNode(parent, node);
          this->ReportAttributes(fnode, node);

          if (node)
            {
            parent = fnode;
            updatedFluNodes.push_back(fnode);
            }
          break;
          }
        case FEATURE_POINT:
          {
          FluNodeType * fnode = this->FetchNode(parent, node);
          this->ReportAttributes(fnode, node);

          if (node)
            {
            this->ReportPoint(fnode, node);
            updatedFluNodes.push_back(fnode);
            }
          break;
          }
        case FEATURE_LINE:
          {
          FluNodeType * fnode = this->FetchNode(parent, node);
          this->ReportAttributes(fnode, node);

          if (node)
            {
            this->ReportLine(fnode, node);
            updatedFluNodes.push_back(fnode);
            }
          break;
          }
        case FEATURE_POLYGON:
          {
          FluNodeType * fnode = this->FetchNode(parent, node);
          this->ReportAttributes(fnode, node);

          if (node)
            {
            this->ReportPolygon(fnode, node);
            updatedFluNodes.push_back(fnode);
            }
          break;
          }
        case FEATURE_MULTIPOINT:
          {
          break;
          }
        case FEATURE_MULTILINE:
          {
          break;
          }
        case FEATURE_MULTIPOLYGON:
          {
          break;
          }
        default:
          {
          break;
          }
        }

      ++it;
      }
    this->RemoveUnusedNodes(updatedFluNodes);
    this->redraw();
    }
}

/** Display node attributes */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportAttributes(FluNodeType * fluNode, DataNodeType * dataNode)
{
  if (dataNode->GetMetaDataDictionary().HasKey(MetaDataKey::VectorDataKeywordlistKey))
    {
    VectorDataKeywordlist kwl;
    itk::ExposeMetaData<VectorDataKeywordlist>(
      dataNode->GetMetaDataDictionary(), MetaDataKey::VectorDataKeywordlistKey, kwl);

    FluNodeType * attNode = this->FetchSubNode(fluNode, m_AttributesKey);

    for (unsigned int i = 0; i < kwl.GetNumberOfFields(); ++i)
      {
      typename VectorDataKeywordlist::FieldType field = kwl.GetNthField(i);
      this->UpdateOrCreateAttributeField(attNode, field);
      }
    }
}

/** Display points information */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportPoint(FluNodeType * fluNode, DataNodeType * dataNode)
{
  FluNodeType * geoDataNode = this->FetchSubNode(fluNode, m_GeometricDataKey);
  this->UpdateOrCreatePointField(geoDataNode, "p=", dataNode->GetPoint());
}

/** Display line information */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportLine(FluNodeType * fluNode, DataNodeType * dataNode)
{
  // Udpate point list
  FluNodeType * geoDataNode = this->FetchSubNode(fluNode, m_GeometricDataKey);

  typename LineType::VertexListType::ConstIterator it
    = dataNode->GetLine()->GetVertexList()->Begin();

  int                count = 0;
  itk::OStringStream oss;

  while (it != dataNode->GetLine()->GetVertexList()->End())
    {
    oss.str("");
    oss << count;
    this->UpdateOrCreatePointField(geoDataNode, oss.str().c_str(), it.Value());
    ++it;
    ++count;
    }

  // Remove trailing nodes if any
  while (geoDataNode->children() > count)
    {
    geoDataNode->remove(geoDataNode->children() - 1);
    }
}

/** Display polygon information */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportPolygon(FluNodeType * fluNode, DataNodeType * dataNode)
{
  FluNodeType * geoDataNode = this->FetchSubNode(fluNode, m_GeometricDataKey);
  FluNodeType * extRingNode = this->FetchSubNode(geoDataNode, m_ExteriorRingKey);

  // Report exterior ring
  typename PolygonType::VertexListType::ConstIterator it
    = dataNode->GetPolygonExteriorRing()->GetVertexList()->Begin();

  int                count = 0;
  itk::OStringStream oss;

  while (it != dataNode->GetPolygonExteriorRing()->GetVertexList()->End())
    {
    oss.str("");
    oss << count;
    this->UpdateOrCreatePointField(extRingNode, oss.str().c_str(), it.Value());
    ++it;
    ++count;
    }

  // Remove trailing nodes if any
  while (extRingNode->children() > count)
    {
    extRingNode->remove(extRingNode->children() - 1);
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::Clear()
{
  this->clear();
}

/** Destructor */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call the superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** Find or create new node */
template <class TVectorData>
typename VectorDataTreeBrowser<TVectorData>
::FluNodeType *
VectorDataTreeBrowser<TVectorData>
::FetchNode(FluNodeType * parent, DataNodeType * dataNode)
{
  FluNodeType * resp;

  // Look for node in node map
  typename DataToFluNodeMapType::iterator it = m_NodeMap.find(dataNode);
  if (it != m_NodeMap.end())
    {
    resp = it->second;
    }
  else
    {
    itk::OStringStream oss;
    oss.str("");
    oss << m_NodeNameMap[dataNode->GetNodeType()] << m_NumberingMap[dataNode->GetNodeType()];
    resp = parent->add(oss.str().c_str());
    if (resp)
      {
      m_NodeMap[dataNode] = resp;
      m_NumberingMap[dataNode->GetNodeType()] += 1;
      }
    }
  return resp;
}

template <class TVectorData>
typename VectorDataTreeBrowser<TVectorData>
::FluNodeType *
VectorDataTreeBrowser<TVectorData>
::FetchSubNode(FluNodeType * parent, const std::string& label)
{
  itk::OStringStream oss;
  oss << parent->find_path() << "/" << label;
  FluNodeType * node = this->find(oss.str().c_str());

  if (!node)
    {
    node = parent->add(label.c_str());
    node->open(true);
    }

  return node;
}

template <class TVectorData>
typename VectorDataTreeBrowser<TVectorData>
::FluNodeType *
VectorDataTreeBrowser<TVectorData>
::UpdateOrCreatePointField(FluNodeType * parent, const char * label, const PointType& point)
{
  itk::OStringStream oss;
  oss << parent->find_path() << "/" << label;

  FluNodeType * node = this->find(oss.str().c_str());

  if (!node)
    {
    Fl_Group::current(NULL);
    Fl_Group *g = new Fl_Group(0, 0, 150, 20);
    g->resizable(NULL);
    Fl_Value_Output * ix = new Fl_Value_Output(0, 0, 75, 20);
    Fl_Value_Output * iy = new Fl_Value_Output(75, 0, 75, 20);
    ix->value(point[0]);
    iy->value(point[1]);
    g->add(ix);
    g->add(iy);
    node = parent->add(label, g, false);
    }
  else
    {
    Fl_Group *       g = dynamic_cast<Fl_Group *>(node->widget());
    Fl_Value_Output * ix = dynamic_cast<Fl_Value_Output *>(g->child(0));
    Fl_Value_Output * iy = dynamic_cast<Fl_Value_Output *>(g->child(1));
    ix->value(point[0]);
    iy->value(point[1]);
    }

  return node;
}

template <class TVectorData>
// typename VectorDataTreeBrowser<TVectorData>
// ::FluNodeType *
void
VectorDataTreeBrowser<TVectorData>
::UpdateOrCreateAttributeField(FluNodeType * parent, const FieldType& field)
{
  std::string label = field.first->GetNameRef();

  itk::OStringStream oss;
  oss << parent->find_path() << "/" << label;

  FluNodeType * node = this->find(oss.str().c_str());

  if (!node)
    {
    switch (field.first->GetType())
      {
      case OFTString:
        {
        Fl_Input * value = new Fl_Input(0, 0, 75, 20);
        value->value(field.second.String);
        parent->add(label.c_str(), value);
        break;
        }
      case OFTInteger:
        {
        oss.str("");
        oss << field.second.Integer;
        Fl_Int_Input * value = new Fl_Int_Input(0, 0, 75, 20);
        value->value(oss.str().c_str());
        parent->add(label.c_str(), value);
        break;
        }
      case OFTReal:
        {
        Fl_Value_Input * value = new Fl_Value_Input(0, 0, 75, 20);
        value->value(field.second.Real);
        parent->add(label.c_str(), value);
        break;
        }
      default:
        {
        // Other types not supported yet
        break;
        }
      }
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::RemoveUnusedNodes(const FluNodeVectorType& nodes)
{
  // Build the set of flu nodes to remove
  typename DataToFluNodeMapType::iterator it = m_NodeMap.begin();
  FluNodeVectorType                       toRemoveFlu;
  std::vector<DataNodeType *>             toRemoveData;

  while (it != m_NodeMap.end())
    {
    if (std::find(nodes.begin(), nodes.end(), it->second) == nodes.end())
      {
      toRemoveFlu.push_back(it->second);
      toRemoveData.push_back(it->first);
      }
    ++it;
    }

  // Remove nodes from tree
  typename FluNodeVectorType::const_iterator fit = toRemoveFlu.begin();
  while (fit != toRemoveFlu.end())
    {
    (*fit)->clear();
    this->remove((*fit));
    ++fit;
    }

  // Remove nodes from map
  typename std::vector<DataNodeType *>::const_iterator dit = toRemoveData.begin();

  while (dit != toRemoveData.end())
    {
    m_NodeMap.erase(*dit);
    ++dit;
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::TreeCallback(Fl_Widget* w, void* v)
{
  Self *       t = (Self*) w;
  int          reason = t->callback_reason();
  FluNodeType *n = t->callback_node();

  switch (reason)
    {
    case OTB_FLU_RIGHT_MOUSE_PUSHED:
      t->PopupMenu(n);
      break;
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::DeleteGeometryCallback(Fl_Widget* w, void* data)
{
  Self * pthis = static_cast<Self *>(data);
  if (pthis) pthis->DeleteSelectedGeometry();
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::DeleteSelectedGeometry()
{
  // Retrieve selected flu node
  FluNodeType * selectedFluNode = this->get_hilighted();

  // Find the corresponding node from VectorData
  typename DataToFluNodeMapType::iterator it = m_NodeMap.begin();

  while (it != m_NodeMap.end() && it->second != selectedFluNode)
    {
    ++it;
    }

  if (it != m_NodeMap.end())
    {
    m_Controller->RemoveDataNode(it->first);
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::DeletePointCallback(Fl_Widget* w, void* data)
{
  Self * pthis = static_cast<Self *>(data);
  if (pthis) pthis->DeleteSelectedPoint();
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::DeleteSelectedPoint()
{
  // Retrieve selected flu node
  FluNodeType * selectedFluNode = this->get_hilighted();
  FluNodeType * parent = selectedFluNode->parent();

  unsigned int  index = selectedFluNode->index();
  bool          interiorRing = false;
  unsigned int  interiorRingIndex = 0;
  FluNodeType * geoFeatureFluNode = NULL;

  if (parent->label() == m_GeometricDataKey)
    {
    geoFeatureFluNode = parent->parent();
    }
  else if (parent->label() == m_ExteriorRingKey)
    {
    geoFeatureFluNode = parent->parent()->parent();
    }
  else if (parent->parent()->label() == m_InteriorRingKey)
    {
    geoFeatureFluNode = parent->parent()->parent();
    interiorRing = true;
    interiorRingIndex = parent->index();
    }

  if (geoFeatureFluNode)
    {
    // Find the corresponding data node
    typename DataToFluNodeMapType::iterator it = m_NodeMap.begin();

    while (it != m_NodeMap.end() && it->second != geoFeatureFluNode)
      {
      ++it;
      }

    if (it != m_NodeMap.end())
      {
      m_Controller->RemovePointFromDataNode(it->first, index, interiorRing, interiorRingIndex);
      }
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::AddFieldCallback(Fl_Widget* w, void* data)
{
  Self * pthis = static_cast<Self *>(data);
  if (pthis) pthis->AddFieldToSelectedGeometry();
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::AddFieldToSelectedGeometry()
{
  // Retrieve selected flu node
  FluNodeType * selectedFluNode = this->get_hilighted();

  // Find the corresponding node from VectorData
  typename DataToFluNodeMapType::iterator it = m_NodeMap.begin();

  while (it != m_NodeMap.end() && it->second != selectedFluNode)
    {
    ++it;
    }

  if (it != m_NodeMap.end())
    {
    m_FieldEditor->SetDataNode(it->first);
    m_FieldEditor->wFieldEditor->show();
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::FocusCallback(Fl_Widget* w, void* data)
{
  Self * pthis = static_cast<Self *>(data);
  if (pthis) pthis->FocusOnSelectedGeometry();
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::FocusOnSelectedGeometry()
{
  // Retrieve selected flu node
  FluNodeType * selectedFluNode = this->get_hilighted();

  // Find the corresponding node from VectorData
  typename DataToFluNodeMapType::iterator it = m_NodeMap.begin();

  while (it != m_NodeMap.end() && it->second != selectedFluNode)
    {
    ++it;
    }

  if (it != m_NodeMap.end())
    {
    // Set the Focus to the bounding box
    NodeType nT;
    PointType p;
    RegionType r;
    SizeType s;
    IndexType i, indexFocus;

    nT = it->first->GetNodeType();
    
    switch (nT)
      {
      case FEATURE_POINT:
        p = it->first->GetPoint();
        indexFocus[0] = p[0];
        indexFocus[1] = p[1];
        m_Controller->FocusOnDataNode(indexFocus);
        break;
        
      case FEATURE_LINE:
        r = it->first->GetLine()->GetBoundingRegion();
        s = r.GetSize();
        i = r.GetIndex();
        indexFocus[0] = static_cast<int>(s[0] / 2 + i[0]);
        indexFocus[1] = static_cast<int>(s[1] / 2 + i[1]);
        m_Controller->FocusOnDataNode(indexFocus);
        break;
        
      case FEATURE_POLYGON:
        r = it->first->GetPolygonExteriorRing()->GetBoundingRegion();
        s = r.GetSize();
        i = r.GetIndex();
        indexFocus[0] = static_cast<int>(s[0] / 2 + i[0]);
        indexFocus[1] = static_cast<int>(s[1] / 2 + i[1]);
        m_Controller->FocusOnDataNode(indexFocus);
        break;
        
      default:
        break;
      }
    }
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::DeleteFieldCallback(Fl_Widget*w, void *data)
{
  Self * pthis = static_cast<Self *>(data);
  if (pthis) pthis->DeleteSelectedField();
}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::DeleteSelectedField()
{

}

template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::PopupMenu(FluNodeType * node)
{
  // Ignore fancy rendering nodes
  if (node->label() == m_AttributesKey
      || node->label() == m_GeometricDataKey
      || node->label() == m_ExteriorRingKey
      || node->label() == m_InteriorRingsKey
      || node->label() == m_InteriorRingKey)
    {
    return;
    }

  // Look up this node in the node table
  typename DataToFluNodeMapType::iterator it = m_NodeMap.begin();
  bool                                    found = false;

  while (!found && it != m_NodeMap.end())
    {
    found = (it->second == node);
    ++it;
    }

  // If we are on a data node
  if (found)
    {
    Fl_Menu_Item popup_menu[4] = {
        {"Delete geometry",  0, DeleteGeometryCallback, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK },
        { "Add field",  0, AddFieldCallback, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK },
        {"Focus", 0, FocusCallback, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK }, 
        {NULL}};
    
    int x, y;
    x = Fl::event_x();
    y = Fl::event_y();
    const Fl_Menu_Item *m = popup_menu->popup(x, y, 0, 0, NULL);
    if (m) m->do_callback(this, m->user_data());
    }
  // Else if we are on a key node
  else if (node->parent()->label() == m_AttributesKey)
    {
    Fl_Menu_Item popup_menu[2] = {
                {"Delete field",  0, DeleteFieldCallback, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK }, {NULL}
      };

    int x, y;
    x = Fl::event_x();
    y = Fl::event_y();
    const Fl_Menu_Item *m = popup_menu->popup(x, y, 0, 0, NULL);
    if (m) m->do_callback(this, m->user_data());

    }
  else if (node->parent()->label() == m_GeometricDataKey
           || node->parent()->label() == m_ExteriorRingKey
           || node->parent()->parent()->label() == m_InteriorRingsKey)
    {
    Fl_Menu_Item popup_menu[2] = {
                  {"Delete point",  0, DeletePointCallback, this, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12,
                  FL_BLACK }, {NULL}
      };

    int x, y;
    x = Fl::event_x();
    y = Fl::event_y();
    const Fl_Menu_Item *m = popup_menu->popup(x, y, 0, 0, NULL);
    if (m) m->do_callback(this, m->user_data());
    }

  return;
}

template <class TVectorData>
int
VectorDataTreeBrowser<TVectorData>
::handle(int event)
{
#ifdef USE_FLU_DND
  if (dnd_handle(event)) return 1;
#endif

  if (event == FL_NO_EVENT) //|| event == FL_MOVE )
    return 0;

  if (event == FL_FOCUS) //&& rdata.lastHilighted )
    {
    this->redraw();
    return 1;
    }

  if (event == FL_UNFOCUS)
    {
    Fl_Group::handle(event);
    this->redraw();
    return 1;
    }

  if (!rdata.dragging && !(event == FL_MOVE && rdata.selectUnderMouse))
    {
    if (!(event == FL_MOVE || event == FL_ENTER || event == FL_LEAVE)) _box->redraw();

    if (Fl_Group::handle(event))
      {
      return 1;
      }
    }

  if (event == FL_RELEASE)
    {
    this->rdata.dragging = false;
    this->rdata.grabbed = 0;
    this->rdata.dragNode = 0;
    }

  int dx = Fl::box_dx(box()), dy = Fl::box_dy(box());

  // set some initial values for the recursive data structure
  // account for the scrollbar positions
  this->rdata.x = this->x() + dx; this->rdata.y = this->y() + dy;
  if (this->scrollH->visible()) this->rdata.x -= this->scrollH->value();
  if (scrollV->visible()) this->rdata.y -= this->scrollV->value();

  this->rdata.previous = NULL;
  this->rdata.delta = 0;
  this->rdata.visibilityChanged = false;

  // catch cursor keys for moving the hilighted entry or selecting all entries
  if (event == FL_KEYDOWN)
    {
    // move hilighted entry up
    if (Fl::event_key() == FL_Up)
      {
      this->rdata.delta = -1;
      Fl::focus(this);
      this->redraw();
      }

    // move hilighted entry down
    else if (Fl::event_key() == FL_Down)
      {
      this->rdata.delta = 1;
      Fl::focus(this);
      this->redraw();
      }

    // select all
    else if (Fl::event_state(FL_CTRL) && Fl::event_key() == 'a')
      {
      select_all();
      Fl::focus(this);
      this->redraw();
      return 1;
      }

    // check for the Home key
    else if (Fl::event_key() == FL_Home)
      {
      // set the hilighted entry to be the first entry
      if (this->rdata.showRoot || (this->rdata.root->children() == 0)) this->set_hilighted(rdata.root);
      else if (this->rdata.root->children() > 0) this->set_hilighted(this->rdata.root->child(0));
      this->redraw();
      }

    // check for the End key
    else if (Fl::event_key() == FL_End)
      {
      // set the hilighted entry to be the last visible entry
      if (this->rdata.showRoot && (this->rdata.root->children() == 0)) this->set_hilighted(this->rdata.root);
      else
        {
        // find the last node by repeatedly looking for the last child until there are no more branches
        Node *n = &root;
        while (n->children() && n->open())
          n = n->child(n->children() - 1);
        set_hilighted(n);
        }
      this->redraw();
      }
    }

  // pass the event down the tree
  int val = this->root.recurse(rdata, FluNodeType::HANDLE, event);
  if (val)
    {
    //redraw();
    if (this->rdata.visibilityChanged)
      {
      this->root.determineVisibility();
      }
    if (val == 1)
      {
      if (event == FL_PUSH && Fl::event_button() == FL_RIGHT_MOUSE)
        {
        this->get_hilighted()->do_callback(OTB_FLU_RIGHT_MOUSE_PUSHED);
        }
      return 1;
      }
    }
  // special case: if multi-select or single-select and user clicks on no items, unselect all items
  else if ((this->rdata.selectionMode != FLU_NO_SELECT) && (event == FL_PUSH) && (!Fl::event_state(FL_CTRL)))
    {
    this->unselect_all();
    this->set_hilighted(NULL);
    this->rdata.forceResize = true;
    this->redraw();

    return 1;
    }

  if (event == FL_SHOW || event == FL_HIDE) this->root.determineVisibility();

  return Fl_Group::handle(event);

}

} // End namespace otb

#endif
