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

#ifndef __otbModule_h
#define __otbModule_h

#include "itkObject.h"
#include "itkObjectFactory.h"

#include "otbDataObjectWrapper.h"
#include "otbDataDescriptor.h"
#include "otbDataDescriptor.h"

namespace otb
{
/** \class Module
 *  \brief Base class for modules.
 *
 * The GetDataMap() and GetOutputsMap() methods can be used to
 * retrieve inputs and outputs descriptors.
 *
 * Data must be passed and retrieve via the AddDataByKey() and
 * GetDataByKey() methods.
 *
 * These methods performs key and types checking and call the
 * protected methods AssignDataByKey() and RetrieveDataByKey()
 * which are the methods actually reimplemented by subclasses.
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT Module
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef Module                        Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(Module,itk::Object);

  /** Input data description map */
  typedef std::map<std::string, DataDescriptor>  DataDescriptorMapType;

  /** Add an input data by its key. This method performs key and type
   * checking before invoking the protected method
   * AssignDataByKey(). */
  void AddDataByKey(const std::string & key, const DataObjectWrapper & data);

  /** Get an output by its key. This method performs key and type
   * checking before invoking the protected method
   * AssignDataByKey(). */
  const DataObjectWrapper GetDataByKey(const std::string & key, unsigned int idx = 0) const;

  /** Get the input data descriptors map */
  const DataDescriptorMapType  & GetDataMap() const;

  /** Get the output data descriptors map */
  const DataDescriptorMapType & GetOutputsMap() const;

  /** Check that every mandatory input has been filled and call the
   * protected virtual run method */
  void Start();

protected:
  /** Constructor */
  Module();
  /** Destructor */
  virtual ~Module();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Add a new input descriptor */
  void AddDataDescriptor(const std::string & type, const std::string & key, const std::string & description, bool optional = false, bool multiple = false);

  /** Assign input by key. Subclasses should override this methods.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual void AssignDataByKey(const std::string & key, const DataObjectWrapper & data);

  /** Retrieve output by key. Subclasses should override this method.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual const DataObjectWrapper RetrieveDataByKey(const std::string & key) const;

  /** The custom run command */
  virtual void Run();

private:
  Module(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Input descriptor map */
  DataDescriptorMapType m_DataMap;

};


} // End namespace otb

#endif
