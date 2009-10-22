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
#ifndef __otbGraphOutEdgeIterator_h
#define __otbGraphOutEdgeIterator_h

#include "itkObject.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace otb
{
/**
 * \class EdgeIterator
 * \brief Iterates on the graph edges.
 */
template <class TGraph>
class ITK_EXPORT GraphOutEdgeIterator
{
public:
  /** self typedef */
  typedef GraphOutEdgeIterator Self;

  /** Graph typedef */
  typedef TGraph GraphType;
  typedef typename GraphType::GraphContainerType   GraphContainerType;
  typedef typename GraphType::EdgeType             EdgeType;
  typedef typename GraphType::EdgeDescriptorType   EdgeDescriptorType;
  typedef typename GraphType::Pointer              GraphPointerType;

  /** typedef of the internal iterator */
  typedef typename boost::graph_traits<GraphContainerType>::out_edge_iterator InternalIteratorType;


  
  /** Copy constructor */
  GraphOutEdgeIterator(const Self& iter)
  {
    m_Iter=iter.m_Iter;
    m_Graph=iter.m_Graph;
    m_End=iter.m_End;
    m_Begin = iter.m_Begin;
  }

  /** Constructor with input graph and vertex iterator (for out_edges) */
  GraphOutEdgeIterator(TGraph * graph, const GraphVertexIterator<TGraph> & vertex)
  {
    m_Graph = graph;
    boost::tie(m_Begin,m_End)=boost::out_edges(vertex.GetVertexDescriptor(),m_Graph->m_GraphContainer);
    m_Iter = m_Begin;
  }

  /**
   * Get the current edge.
   * \return The current edge pointed by the iterator.
   */
  const EdgeType & Get(void)
  {
    return m_Graph->m_GraphContainer[*m_Iter];
  }

  /**
   * Set the current edge.
   * \param edge The edge to set.
   */
  void Set(const EdgeType & edge)
  {
    m_Graph->m_GraphContainer[*m_Iter]=edge;
  }
  /**
   * Get the current edge index.
   * \return The current edge index.
   */
  EdgeDescriptorType GetEdgeDescriptor(void)
  {
    return *m_Iter;
  }
  /**
   * Return true if the iterator is at the end.
   * \return True if the iterator is at the end.
   */
  bool IsAtEnd(void)
  {
    return (m_Iter==m_End);
  }
  /**
   * Go to the beginning.
   */
  void GoToBegin(void)
  {
    m_Iter = m_Begin;
  }
  /**
   * Increment.
   */
  Self& operator++()
  {
    ++m_Iter;
    return *this;
  }
  /**
   * Decrement.
   */
  Self& operator--()
  {
    --m_Iter;
    return *this;
  }
  /**
   * Add
   */
  Self& operator+(int i)
  {
    m_Iter = m_Iter+i;
    return *this;
  }
  /**
   * Remove
   */
  Self& operator-(int i)
  {
     m_Iter = m_Iter-i;
     return *this;
  }
  /**
   * Difference comparison operator.
   */
  bool operator!=(const Self& iter)
  {
    return (m_Iter != iter.m_Iter);
  }
  /**
   * Equality comparison operator.
   */
  bool operator==(const Self& iter)
  {
    return (m_Iter == iter.m_Iter);
  }
  /**
   * Instantiation operator.
   */
  Self& operator=(const Self& iter)
  {
    m_Iter = iter.m_Iter;
    m_Begin = iter.m_Iter;
    m_End  = iter.m_End;
    m_Graph = iter.m_Graph;
    return *this;
  }
private:
  // Begin
  InternalIteratorType m_Begin;
  // End
  InternalIteratorType m_End;
  // Internal iterator.
  InternalIteratorType m_Iter;
  // Input graph pointer
  GraphPointerType m_Graph;
};
} // End namespace otb

#endif
