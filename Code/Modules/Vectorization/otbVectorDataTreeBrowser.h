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

namespace otb
{

/** \class VectorDataTreeBrowser
 *  \brief Simple tree browser for VectorData
 */
template <class TVectorData>
class ITK_EXPORT VectorDataTreeBrowser
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
typedef TVectorData                   VectorDataType;
typedef typename VectorDataType
  ::Pointer                           VectorDataPointerType;
typedef typename VectorDataType
  ::DataNodeType                      DataNodeType;
typedef typename DataNodeType
  ::Pointer                           DataNodePointerType;
typedef typename Flu_Tree_Browser
  ::Node                              FluNodeType;
typedef typename DataNodeType
::PointType                           PointType;
typedef typename DataNodeType
::PolygonType                         PolygonType;
typedef typename DataNodeType
::LineType                            LineType;

/** RTT type info */
itkTypeMacro(VectorDataTreeBrowser,itk::Object);

/** Update representation */
void Update();

/** Clear browser */
void Clear();

itkSetObjectMacro(VectorData,VectorDataType);
itkGetObjectMacro(VectorData,VectorDataType);

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
void operator=(const Self&); //purposely not implemented

/** Find or create new node */
FluNodeType * FindExistingOrCreateNew(FluNodeType * parent,const char * label);

/** Create an x/y point field */
FluNodeType * UpdateOrCreatePointField(FluNodeType * parent, const char * label,const PointType & point);

/** VectorData pointer */
VectorDataPointerType m_VectorData;

}; // End class VectorDataTreeBrowser
} // End namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbVectorDataTreeBrowser.txx"
#endif

#endif
