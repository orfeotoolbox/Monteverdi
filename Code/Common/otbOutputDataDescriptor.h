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
  OutputDataDescriptor(const std::string & type, const std::string & key, const std::string & description);

  /** Destructor */
  virtual ~OutputDataDescriptor();

  /** Get the number of data */
  unsigned int GetNumberOfData() const;

  /** Set the number of data */
  void SetNumberOfData(unsigned int nb);

private:
  /** The number of data */
  unsigned int m_NumberOfData;

};

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const OutputDataDescriptor & descriptor);

} // End namespace otb

#endif
