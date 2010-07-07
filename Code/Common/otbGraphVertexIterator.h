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
#ifndef __otbGraphVertexIterator_h
#define __otbGraphVertexIterator_h

#include "itkObject.h"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace otb
{
/**
 * \class VertexIterator
 * \brief Iterates on the graph vertices.
 */
template <class TGraph>
class ITK_EXPORT GraphVertexIterator
{
public:
  /** self typedef */
  typedef GraphVertexIterator Self;

  /** Graph typedef */
  typedef TGraph                                   GraphType;
  typedef typename GraphType::GraphContainerType   GraphContainerType;
  typedef typename GraphType::VertexType           VertexType;
  typedef typename GraphType::VertexDescriptorType VertexDescriptorType;
  typedef typename GraphType::Pointer              GraphPointerType;

  /** typedef of the internal iterator */
  typedef typename boost::graph_traits<GraphContainerType>::vertex_iterator InternalIteratorType;

  /** Constructor */
  GraphVertexIterator()
  {
    m_Graph = GraphType::New();
    tie(m_Iter, m_End) = vertices(*(m_Graph->m_GraphContainer));
  }

  /** Copy constructor */
  GraphVertexIterator(const Self& iter)
  {
    m_Iter = iter.m_Iter;
    m_Graph = iter.m_Graph;
    m_End = iter.m_End;
  }

  /**  Constructor with input graph */
  GraphVertexIterator(TGraph * graph)
  {
    m_Graph = graph;
    boost::tie(m_Iter, m_End) = boost::vertices(m_Graph->m_GraphContainer);
  }
  /**
   * Get the current vertex.
   * \return The current vertex pointed by the iterator.
   */
  const VertexType& Get(void) const
  {
    return m_Graph->m_GraphContainer[*m_Iter];
  }

  /**
   * Set the current vertex.
   * \param vertex The vertex to set.
   */
  void Set(const VertexType& vertex)
  {
    m_Graph->m_GraphContainer[*m_Iter] = vertex;
  }
  /**
   * Get the current vertex index.
   * \return The current vertex index.
   */
  VertexDescriptorType GetVertexDescriptor(void) const
  {
    return *m_Iter;
  }
  /**
   * Return true if the iterator is at the end.
   * \return True if the iterator is at the end.
   */
  bool IsAtEnd(void)
  {
    return (m_Iter == m_End);
  }
  /**
   * Go to the beginning.
   */
  void GoToBegin(void)
  {
    boost::tie(m_Iter, m_End) = boost::vertices(m_Graph->m_GraphContainer);
  }
  /**
   * Increment.
   */
  Self& operator ++()
  {
    ++m_Iter;
    return *this;
  }
  /**
   * Decrement.
   */
  Self& operator --()
  {
    --m_Iter;
    return *this;
  }
  /**
   * Add
   */
  Self& operator +(int i)
  {
    m_Iter = m_Iter + i;
    return *this;
  }
  /**
   * Remove
   */
  Self& operator -(int i)
  {
    m_Iter = m_Iter - i;
    return *this;
  }
  /**
   * Difference comparison operator.
   */
  bool operator !=(const Self& iter)
  {
    return (m_Iter != iter.m_Iter);
  }
  /**
   * Equality comparison operator.
   */
  bool operator ==(const Self& iter)
  {
    return (m_Iter == iter.m_Iter);
  }
  /**
   * Instantiation operator.
   */
  Self& operator =(const Self& iter)
  {
    m_Iter = iter.m_Iter;
    m_End  = iter.m_End;
    m_Graph = iter.m_Graph;
    return *this;
  }
private:
  // End
  InternalIteratorType m_End;
  // Internal iterator.
  InternalIteratorType m_Iter;
  // Input graph pointer
  GraphPointerType m_Graph;
};
} // End namespace otb

#endif
