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
#ifndef __otbOutputDataDescriptor_h
#define __otbOutputDataDescriptor_h

#include "otbDataDescriptor.h"

namespace otb
{

/** \class OutputDataDescriptor
 *  \brief This small class specializes DataDescritpor to represent outputs.
 *
 *  The number of data field allow to specify the number of data
 *  associated with the output.
 *
 *  See the DataDescriptor class for more details.
 *
 *  \sa Module, DataDescriptor
 */

class OutputDataDescriptor
  : public DataDescriptor
{
public:
  /** Constructors */
  OutputDataDescriptor();
  OutputDataDescriptor(const DataObjectWrapper& data,
                       const std::string& key,
                       const std::string& description,
                       bool cached = false);

  /** Destructor */
  virtual ~OutputDataDescriptor();

  void CacheNthData(unsigned int idx, const DataObjectWrapper& data);

  // Is the output cached
  bool IsCached() const;

private:
  // Is Data cached ?
  bool m_Cached;
};

/** Overloading the << operator */
std::ostream& operator <<(std::ostream& ostr, const OutputDataDescriptor& descriptor);

} // End namespace otb

#endif
