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
#include "otbInputDataDescriptor.h"
#include "otbOutputDataDescriptor.h"

namespace otb
{
/** \class Module
 *  \brief Base class for modules
 * 
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
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
  typedef std::map<std::string, InputDataDescriptor>  InputDataDescriptorMapType;

  /** Output data description map */
  typedef std::map<std::string, OutputDataDescriptor> OutputDataDescriptorMapType;

  /** Add an input data by its key. This method performs key and type
   * checking before invoking the protected method
   * AssignInputByKey(). */
  void AddInputByKey(const std::string & key, const DataObjectWrapper & data);

  /** Get an output by its key. This method performs key and type
   * checking before invoking the protected method
   * AssignInputByKey(). */
  const DataObjectWrapper GetOutputByKey(const std::string & key, unsigned int idx = 0) const;

  /** Get the input data descriptors map */
  const InputDataDescriptorMapType  & GetInputsMap() const;

  /** Get the output data descriptors map */
  const OutputDataDescriptorMapType & GetOutputsMap() const;

protected:
  /** Constructor */
  Module();
  /** Destructor */
  virtual ~Module();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Add a new input descriptor */
  void AddInputDescriptor(const std::string & type, const std::string & key, const std::string & description, bool optional = false, bool multiple = false);

  /** Add a new output descriptor */
  void AddOutputDescriptor(const std::string & type, const std::string & key, const std::string & description, unsigned int nb = 1);

  /** Assign input by key. This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual void AssignInputByKey(const std::string & key, const DataObjectWrapper & data);

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual const DataObjectWrapper RetrieveOutputByKey(const std::string & key) const;
  
private:
  Module(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Input descriptor map */
  InputDataDescriptorMapType m_InputsMap;

  /** Output descriptor map */
  OutputDataDescriptorMapType m_OutputsMap;
};


} // End namespace otb

#endif
