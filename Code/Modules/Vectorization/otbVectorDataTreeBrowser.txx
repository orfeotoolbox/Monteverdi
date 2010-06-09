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

#include "otbVectorDataTreeBrowser.h"
#include <map>

namespace otb
{
/** Constructor */
template <class TVectorData>
VectorDataTreeBrowser<TVectorData>
::VectorDataTreeBrowser()
 : Flu_Tree_Browser(0,0,0,0)
{
  this->box( FL_DOWN_BOX );
  this->auto_branches( true );
  this->label( otbGetTextMacro("VectorData browser") );

  // allow callback with the tree
  this->box( FL_DOWN_BOX );
  this->allow_dnd( true );
  this->always_open(true);
  this->allow_leaf_duplication(false);

  // this->when( FL_WHEN_RELEASE );
  // this->callback( TreeCallback );

  // animate the tree
  this->animate( true );
  this->collapse_time( 0.02 );
  this->frame_rate(500);

  //Flu_Tree_Browser::Node* root = this->first();
  typename Flu_Tree_Browser::Node* root = this->first();
  root->open(true);
  root->label(otbGetTextMacro("VectorData"));

  this->show_root(false);
  this->insertion_mode(FLU_INSERT_BACK);
}

/** Destructor */
template <class TVectorData>
VectorDataTreeBrowser<TVectorData>
::~VectorDataTreeBrowser()
{}

/** Update */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::Update()
{
   std::cout<<"Entering update method"<<std::endl;
   // If the vector data is not null
  if(m_VectorData.IsNotNull())
    {
    // First, clear existing display
    //this->Clear();

    // Get root node
    typename Flu_Tree_Browser::Node* root = this->first();
    typename Flu_Tree_Browser::Node* parent = root;

    // Refresh display
    typename VectorDataType::DataTreeType
      ::PreOrderIteratorType it(m_VectorData->GetDataTree(),
                                m_VectorData->GetDataTree()->GetRoot());
    it.GoToBegin();

    // This map is used in order to number items by type
    std::map<std::string,unsigned int> numberingMap;
    numberingMap["Document"]=0;
    numberingMap["Folder"]=0;
    numberingMap["Point"]=0;
    numberingMap["Line"]=0;
    numberingMap["Polygon"]=0;

    // StringStream used to build strings
    itk::OStringStream oss;
    oss.str("");

    while(!it.IsAtEnd())
      {
      // Get the data node
      DataNodePointerType node = it.GetNode()->Get();

      std::cout<<"Next node type: "<<node->GetNodeTypeAsString()<<std::endl;

      switch(node->GetNodeType())
      {
      case ROOT:
        {
        parent = root;
        break;
        }
      case DOCUMENT:
        {
        oss.str("");
        oss<<"Document"<<numberingMap["Document"];
        Node * new_node = this->FindExistingOrCreateNew(parent,oss.str().c_str());
        if(new_node)
          {
            parent = new_node;
          }
        numberingMap["Document"]+=1;

        break;
        }
      case FOLDER:
        {
        oss.str("");
        oss<<"Folder"<<numberingMap["Folder"];
        Node * new_node =this->FindExistingOrCreateNew(parent,oss.str().c_str());
        if(new_node)
          {
          parent = new_node;
          }
        numberingMap["Folder"]+=1;

        break;
        }
      case FEATURE_POINT:
        {
        oss.str("");
        oss<<"Point"<<numberingMap["Point"];
        Node* new_node = this->FindExistingOrCreateNew(parent,oss.str().c_str());
        if(new_node)
          {
          this->ReportPoint(new_node,node);
          }
        numberingMap["Point"]+=1;
        break;
        }
      case FEATURE_LINE:
        {
        oss.str("");
        oss<<"Line"<<numberingMap["Line"];
        Node* new_node = this->FindExistingOrCreateNew(parent,oss.str().c_str());
        if(new_node)
          {
          this->ReportLine(new_node,node);
          }
        numberingMap["Line"]+=1;

        break;
        }
      case FEATURE_POLYGON:
        {
        oss.str("");
        oss<<"Polygon"<<numberingMap["Polygon"];
        std::cout<<oss.str()<<std::endl;
        Node* new_node = this->FindExistingOrCreateNew(parent,oss.str().c_str());
        if(new_node)
          {
          this->ReportPolygon(new_node,node);
          }
          numberingMap["Polygon"]+=1;

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
    }

  this->redraw();
}

/** Display node attributes */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportAttributes(FluNodeType * fluNode, DataNodeType * dataNode)
{


}

/** Display points information */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportPoint(FluNodeType * fluNode, DataNodeType * dataNode)
{
  FluNodeType * geoDataNode = this->FindExistingOrCreateNew(fluNode,"Geometric Data");
  this->UpdateOrCreatePointField(geoDataNode,"p=",dataNode->GetPoint());
}

/** Display line information */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportLine(FluNodeType * fluNode, DataNodeType * dataNode)
{
  // Udpate point list
  FluNodeType * geoDataNode = this->FindExistingOrCreateNew(fluNode,"Geometric Data");

    typename LineType::VertexListType::ConstIterator it
      = dataNode->GetLine()->GetVertexList()->Begin();

    int count = 0;
    itk::OStringStream oss;

    while(it!=dataNode->GetLine()->GetVertexList()->End())
      {
      oss.str("");
      oss<<"p"<<count<<"=";
      this->UpdateOrCreatePointField(geoDataNode,oss.str().c_str(),it.Value());
      ++it;
      ++count;
      }

    // Remove trailing nodes if any
    while(geoDataNode->children() > count)
      {
      geoDataNode->remove(geoDataNode->children()-1);
      }

}

/** Display polygon information */
template <class TVectorData>
void
VectorDataTreeBrowser<TVectorData>
::ReportPolygon(FluNodeType * fluNode, DataNodeType * dataNode)
{
  FluNodeType * geoDataNode = this->FindExistingOrCreateNew(fluNode,"Geometric Data");
  FluNodeType * extRingNode = this->FindExistingOrCreateNew(geoDataNode,"Exterior Ring");

  // Report exterior ring
  typename PolygonType::VertexListType::ConstIterator it
    = dataNode->GetPolygonExteriorRing()->GetVertexList()->Begin();

  int count = 0;
  itk::OStringStream oss;

  while(it!=dataNode->GetPolygonExteriorRing()->GetVertexList()->End())
    {
    oss.str("");
    oss<<"p"<<count<<"=";
    this->UpdateOrCreatePointField(extRingNode,oss.str().c_str(),it.Value());
    ++it;
    ++count;
    }

  // Remove trailing nodes if any
  while(geoDataNode->children() > count)
    {
    geoDataNode->remove(geoDataNode->children()-1);
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
	Superclass::PrintSelf(os,indent);
}

/** Find or create new node */
template <class TVectorData>
typename VectorDataTreeBrowser<TVectorData>
::FluNodeType *
VectorDataTreeBrowser<TVectorData>
::FindExistingOrCreateNew(FluNodeType * parent,const char * label)
{
  itk::OStringStream oss;
  oss<<parent->find_path()<<label;
  std::cout<<"Looking for node "<<oss.str()<<std::endl;
  FluNodeType * node = this->find(oss.str().c_str());

  if(!node)
    {
    node = parent->add(label);
    std::cout<<"Node not found, creating."<<std::endl;
    }
  else
    std::cout<<"Node found"<<std::endl;

  return node;
}

template <class TVectorData>
typename VectorDataTreeBrowser<TVectorData>
::FluNodeType *
VectorDataTreeBrowser<TVectorData>
::UpdateOrCreatePointField(FluNodeType * parent, const char * label,const PointType & point)
 {
  itk::OStringStream oss;
  oss<<parent->find_path()<<label;
  FluNodeType * node = this->find(oss.str().c_str());

  if(!node)
    {
    Fl_Group *g = new Fl_Group( 0, 0, 150, 20 );
    Fl_Value_Input * ix = new Fl_Value_Input(0,0,75,20);
    Fl_Value_Input * iy = new Fl_Value_Input(75,0,75,20);
    ix->value(point[0]);
    iy->value(point[1]);
    g->add(ix);
    g->add(iy);
    node = parent->add(label,g);
    }
  else
    {
    Fl_Group * g = dynamic_cast<Fl_Group *>(node->widget());
    Fl_Value_Input * ix = dynamic_cast<Fl_Value_Input *>(g->child(0));
    Fl_Value_Input * iy = dynamic_cast<Fl_Value_Input *>(g->child(1));
    ix->value(point[0]);
    iy->value(point[1]);
    }

    return node;
 }
} // End namespace otb

#endif
