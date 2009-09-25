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
#include "otbTypeManager.h"

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
  /** DataObjectWrapper must be created by the Create function. */
  template <typename T> static DataObjectWrapper Create(T * data)
  {
    DataObjectWrapper resp;
    resp.Set(data);
    return resp;
  }
  template <typename T> static DataObjectWrapper Create(itk::SmartPointer<T> data)
  {
    DataObjectWrapper resp;
    resp.Set(data);
    return resp;
  }
  

  /** Constructors */
  DataObjectWrapper();

  /** Destructor */
  virtual ~DataObjectWrapper();

  /** Set the data object mandatory types */
  template <typename T> void Set(T * data)
  {
    // Fill the data type and object
  m_DataType = TypeManager::GetInstance()->GetTypeName<T>();
  m_DataObject = data;
  }

  /** Set the data object mandatory types (SmartPointer version) */
  template <typename T> void Set(itk::SmartPointer<T> ptr)
  {
    this->Set(ptr.GetPointer());
  }

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

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const DataObjectWrapper & wrapper);

} // End namespace otb

#endif
