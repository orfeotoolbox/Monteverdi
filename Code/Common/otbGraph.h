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
#ifndef __otbGraph_h
#define __otbGraph_h

#ifndef BOOST_NO_HASH
#define BOOST_NO_HASH
#endif

#ifndef BOOST_NO_SLIST
#define BOOST_NO_SLIST
#endif

#ifndef BOOST_NO_0X_HDR_INITIALIZER_LIST
#define BOOST_NO_0X_HDR_INITIALIZER_LIST
#endif

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "itkDataObject.h"
#include "itkObjectFactory.h"

#include "otbGraphVertexIterator.h"
#include "otbGraphEdgeIterator.h"
#include "otbGraphOutEdgeIterator.h"

namespace otb
{
/**
 * \class Graph
 *
 * This class implements a generic graph container from boost graph structures.
 */
template <class TVertex, class TEdge, class TDirected = boost::undirectedS>
class ITK_EXPORT Graph
  : public itk::DataObject
{
public:
  /** Standard class typedefs. */
  typedef Graph                         Self;
  typedef itk::DataObject               Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(Graph,itk::DataObject);
  
  // Iterators are friends
  friend class GraphVertexIterator<Self>;
  friend class GraphEdgeIterator<Self>;
  friend class GraphOutEdgeIterator<Self>;

  /** Vertex typedef */
  typedef TVertex VertexType;

  /** Edge typedef */
  typedef TEdge   EdgeType;

  /** Boost graph typedef */
  typedef boost::adjacency_list<boost::vecS,boost::vecS,
                            TDirected,VertexType,EdgeType> GraphContainerType;
  
  /** Vertex descriptor */
  typedef typename GraphContainerType::vertex_descriptor VertexDescriptorType;

  /** Edge descriptor */
  typedef typename GraphContainerType::edge_descriptor   EdgeDescriptorType;

  /** Clear all vertices and edges of the graph */
  void Clear()
  {
    m_GraphContainer.clear();
  }
  
  /** Returns the number of vertices */
  unsigned long int GetNumberOfVertices() const
  {
    return num_vertices(m_GraphContainer);
  }

  /** Returns the number of edges */
  unsigned long int GetNumberOfEdges() const
  {
    return num_edges(m_GraphContainer);
  }
  
  /** Add a new vertex and return its vertex descriptor */
  virtual VertexDescriptorType AddVertex(const VertexType & vertex)
  {
    return add_vertex(vertex,m_GraphContainer);
  }

  /** Add a new edge and return its edge descriptor */
  EdgeDescriptorType AddEdge(VertexDescriptorType source, VertexDescriptorType destination, const EdgeType & edge)
  {
    std::pair<EdgeDescriptorType,bool> resp = add_edge(source,destination,edge,m_GraphContainer);
    return resp.first;
  }

  /** Get the internal graph container */
  const GraphContainerType & GetGraphContainer() const
  {
    return m_GraphContainer;
  }

protected:
  /** Constructor */
  Graph(){};

  /** Destructor */
  ~Graph(){};

  /** PrintSelf method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
    os<<indent<<"Number of vertices: "<<this->GetNumberOfVertices()<<std::endl;
    os<<indent<<"Number of edges   : "<<this->GetNumberOfEdges()<<std::endl;
  }

private:
  /** Internal representation using the boost graph library */
  GraphContainerType m_GraphContainer;
};
} // end namespace otb

#endif
