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
#ifndef __otbDataObjectWrapper_h
#define __otbDataObjectWrapper_h

#include "itkDataObject.h"

namespace otb
{

/** \class DataObjectWrapper
 *  \brief Wrap a DataObject with usefull type information
 *  This class is used to manipulate data object in the Monteverdi
 *  application while keeping track of the real data type undergoing.
 *  
 *  It is used to perform dynamic type checking via the m_DataType
 *  key.
 *
 * Since this class is a struct-like c++ class, it offers public
 * access to the members.
 *
 * This class could be later handled by SmartPointer if necessary.
 *
 * \sa DataObject
 */

class DataObjectWrapper
{
public:
  /** Constructors */
  DataObjectWrapper();
  DataObjectWrapper(const std::string & type, itk::DataObject * data);

  /** Destructor */
  virtual ~DataObjectWrapper();

  /** Set the data object mandatory types */
  void Set(const std::string & type, itk::DataObject * data);

  /** Get the DataObject */
  itk::DataObject * GetDataObject() const;

  /** Get the DataType */
  const std::string & GetDataType() const;

private:
  /** The key to identify the data type */
  std::string m_DataType;

  /** A pointer to the DataObject */
  itk::DataObject::Pointer m_DataObject;
};

} // End namespace otb

#endif
