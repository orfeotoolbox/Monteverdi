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
#include "otbEventsSender.h"
#include "otbMonteverdiEvent.h"
#include "otbTypeManager.h"

namespace otb
{
/** \class Module
 *  \brief Base class for modules.
 *
 * The GetInputsMap() and GetOutputsMap() methods can be used to
 * retrieve inputs and outputs descriptors.
 *
 * Data must be passed and retrieve via the AddInputByKey() and
 * GetOutputByKey() methods.
 *
 * These methods performs key and types checking and call the
 * protected methods AssignInputByKey() and RetrieveOutputByKey()
 * which are the methods actually reimplemented by subclasses.
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT Module
  : public itk::Object, public EventsSender<MonteverdiEvent>
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

  /** Check that every mandatory input has been filled and call the
   * protected virtual run method */
  void Start();

  /** Set/Get the instance id */
  itkSetStringMacro(InstanceId);
  itkGetStringMacro(InstanceId);

protected:
  /** Constructor */
  Module();
  /** Destructor */
  virtual ~Module();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Add a new input descriptor */
  template <typename T> void AddInputDescriptor(const std::string & key, const std::string & description, bool optional = false, bool multiple = false)
  {
    // Check if the key already exists
    if(m_InputsMap.count(key) > 0)
      {
      itkExceptionMacro(<<"An input with key "<<key<<" already exists !");
      }

    // Else build a new descriptor
    InputDataDescriptor desc(TypeManager::GetInstance()->GetTypeName<T>(),key,description);
    desc.SetOptional(optional);
    desc.SetMultiple(multiple);

    // Insert it into the map
    m_InputsMap[key]=desc;
  }

  /** Add additional supported types for a given input descriptors */
  template <typename T> void AddTypeToInputDescriptor(const std::string & key)
  {
    // Check if the key already exists
    if(m_InputsMap.count(key) <= 0)
      {
      itkExceptionMacro(<<"No input with key "<<key);
      }
    m_InputsMap[key].AddSupportedType(TypeManager::GetInstance()->GetTypeName<T>());
  }

  /** Add a new output descriptor */
  template <typename T> void AddOutputDescriptor(const std::string & key, const std::string & description, unsigned int nb = 1)
  {
    // Check if the key already exists
    if(m_OutputsMap.count(key) > 0)
      {
      itkExceptionMacro(<<"An Output with key "<<key<<" already exists !");
      }
    
    // Else build a new descriptor
    OutputDataDescriptor desc(TypeManager::GetInstance()->GetTypeName<T>(),key,description);
    desc.SetNumberOfData(nb);
    
    // Insert it into the map
    m_OutputsMap[key]=desc;
  }

  /** Assign input by key. Subclasses should override this methods.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual void AssignInputByKey(const std::string & key, const DataObjectWrapper & data);

  /** Retrieve output by key. Subclasses should override this method.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual const DataObjectWrapper RetrieveOutputByKey(const std::string & key) const;

  /** The custom run command */
  virtual void Run();

  /** Instance id (intentionnaly left protected) */
  std::string m_InstanceId;

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
