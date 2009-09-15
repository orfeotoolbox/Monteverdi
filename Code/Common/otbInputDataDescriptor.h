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
#ifndef __otbInputDataDescriptor_h
#define __otbInputDataDescriptor_h

#include "otbDataDescriptor.h"

namespace otb
{

/** \class InputDataDescriptor
 *  \brief This small class specializes DataDescritpor to represent inputs.
 *  
 *  The Optional flag allows to define optional inputs.
 *
 *  The Multiple flag allows to define unbounded multiple inputs.
 *
 *  See the DataDescriptor class for more details.
 * 
 *  \sa Module, DataDescriptor
 */

class InputDataDescriptor
  : public DataDescriptor
{
public:
  /** Constructors */
  InputDataDescriptor();
  InputDataDescriptor(const std::string & type, const std::string & key, const std::string & description);

  /** Destructor */
  virtual ~InputDataDescriptor();

  /** Is the input data optional ? */
  bool IsOptional() const;

  /** Is the input data multiple ? */
  bool IsMultiple() const;

  /** Is the input data used ? */
  bool IsUsed() const;

  /** Set the optional flag */
  void SetOptional(bool flag);

  /** Set the multiple flag */
  void SetMultiple(bool flag);

  /** Set the used flag */
  void SetUsed(bool flag);

private:
  /** The optional flag */
  bool m_Optional;
  
  /** The multiple flag */
  bool m_Multiple;

  /** This flag indicates if the input is set */
  bool m_Used;
};

/** Overloading the << operator */
std::ostream & operator<<(std::ostream & ostr, const InputDataDescriptor & descriptor);

} // End namespace otb

#endif
