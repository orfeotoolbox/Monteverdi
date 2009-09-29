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
#ifndef __otbDataDescriptor_h
#define __otbDataDescriptor_h

#include <string>
#include <vector>
#include <iostream>

#include "itkSmartPointer.h"

#include "otbDataObjectWrapper.h"

namespace otb
{

/** \class DataDescriptor
 *  \brief This small class describes the input/outputs of a module.
 *
 *  The DataType field describe the data type and will be used by the
 *  module to perform dynamic type checking.
 *
 *  The DataKey field identify the input/output of the module the
 *  DataDescriptor instance refers to.
 *
 *  The DataDescription field will be used to store and display
 *  useful information about the data.
 *
 *  \sa Module
 */

class DataDescriptor
{
public:
  /** Constructors */
  DataDescriptor();
  DataDescriptor(const std::string & type, const std::string & key, const std::string & description);

  /** Destructor */
  virtual ~DataDescriptor();

  /** Set the data object mandatory types */
  void Set(const std::string & type, const std::string & key, const std::string & description);

  /** Get the DataType */
  const std::string & GetDataType() const;

  /** Get the DataKey */
  const std::string & GetDataKey() const;

  /** Get the DataDescription */
  const std::string & GetDataDescription() const;

 /** Check if the given type is compatible with the input (virtual
  * because behaviour depends on subclasses) */
  virtual bool IsTypeCompatible(const std::string & type) const;
  
  /** Get number of data */
  unsigned int GetNumberOfData() const;

  /** Add data (virtual because behaviour depends 
   *  on subclasses) */
  virtual void AddData(const DataObjectWrapper & wrapper);

  /** Get nth data */
  const DataObjectWrapper & GetNthData(const unsigned int& idx) const;

  /** Clear data */
  void ClearData();

protected:
  /** The data type */
  std::string m_DataType;

  /** The key to identify input/output of a module */
  std::string m_DataKey;

  /** A description of the input/output */
  std::string m_DataDescription;

  /** A vector of DataObject wrapper associated with the descriptor */
  std::vector<DataObjectWrapper> m_Data;
};

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const DataDescriptor & descriptor);

} // End namespace otb

#endif
