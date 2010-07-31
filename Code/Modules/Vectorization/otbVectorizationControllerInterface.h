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
#ifndef __otbVectorizationControllerInterface_h
#define __otbVectorizationControllerInterface_h

#include "itkObject.h"
#include "otbVectorizationModel.h"

namespace otb
{
class ITK_EXPORT VectorizationControllerInterface
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef VectorizationControllerInterface Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef VectorizationModel      ModelType;
  typedef ModelType::DataNodeType DataNodeType;
  typedef ModelType::PointType    PointType;

  /** Standard type macros */
  itkTypeMacro(VectorizationControllerInterface, Superclass);

  /** Users actions */
  virtual void RemoveDataNode(DataNodeType * node) = 0;
  virtual void SetDataNodeFieldAsInt(DataNodeType * node, const std::string& name, int value) = 0;
  virtual void SetDataNodeFieldAsFloat(DataNodeType * node, const std::string& name, float value) = 0;
  virtual void SetDataNodeFieldAsString(DataNodeType* node, const std::string& name, const std::string& value) = 0;
  virtual void RemoveFieldFromDataNode(DataNodeType * node, const std::string& name) = 0;
  virtual void RemovePointFromDataNode(DataNodeType * node,
                                       const unsigned long& index,
                                       bool interiorRing,
                                       const unsigned int& interiorRingIndex = 0) = 0;
  virtual void UpdatePointFromDataNode(DataNodeType * node,
                                       const unsigned long& index,
                                       const PointType& point,
                                       bool interiorRing,
                                       const unsigned int& interiorRingIndex = 0) = 0;

protected:
  /** Constructor */
  VectorizationControllerInterface() {}
  /** Destructor */
  ~VectorizationControllerInterface() {}

private:
  VectorizationControllerInterface(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
