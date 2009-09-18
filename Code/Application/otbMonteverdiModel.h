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
#ifndef __otbMonteverdiModel_h
#define __otbMonteverdiModel_h

#include "itkObject.h"
#include "otbModule.h"
#include "otbMVCModel.h"
#include "otbListenerBase.h"

// There are function prototype conflits under cygwin between standard w32 API
// and standard C ones
#ifndef CALLBACK
#if defined(_WINDOWS) || defined(__CYGWIN__)
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#endif


namespace otb
{

class ITK_EXPORT MonteverdiModel
  : public MVCModel<ListenerBase>, public itk::Object
{
 public:
  /** Standard class typedefs */
  typedef MonteverdiModel                                Self;
  typedef MVCModel<ListenerBase>                         Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;


  /** Standard type macro */
  itkTypeMacro(MonteverdiModel, MVCModel);


  /** Vector of open modules */
  typedef std::vector<Module::Pointer>                    ModuleVectorType;

 /** typedef of the Module constructors function */
  typedef otb::Module::Pointer (CALLBACK * ConstructorPointerType)();

  /** Inner class to represent a registered module */
  class RegisteredModuleDescriptor
  {
    friend class MonteverdiModel;
  public:
    // key to identify the module
    std::string m_Key;
    // Path in the application menu
    std::string m_MenuPath;
  protected:
    // pointer to the constructor of the module
    ConstructorPointerType m_Constructor;
  };

  // Map of registered modules
  typedef std::map<std::string,RegisteredModuleDescriptor> ModuleDescriptorMapType;



  /** Get the unique instance of the model */
  static Pointer GetInstance();


  /** Register a new module with its associated name */
  template <class T> void RegisterModule(const std::string & key, const std::string menupath = "Module/")
  {
    if(m_ModuleDescriptorMap.count(key)>0)
      {
      itkExceptionMacro(<<"A module with key "<<key<<" is already registered"); 
      }

    // Create a new descriptor
    RegisteredModuleDescriptor desc;
    desc.m_Key = key;
    desc.m_MenuPath = menupath;
    desc.m_Constructor = (ConstructorPointerType)T::New;
    m_ModuleDescriptorMap[key]=desc;
  }

  /** Create a module according to its key. If the key is not a
  registered module, throws an exception */
  void CreateModuleByKey(const std::string & key);

  /** Get available modules map */
  const ModuleDescriptorMapType & GetRegisteredModuleDescriptors() const;

protected:

  /** This is protected for the singleton. Use GetInstance() instead. */
  itkNewMacro(Self);

  /** Constructor */
  MonteverdiModel();

  /** Destructor */
  virtual ~MonteverdiModel();

 private:
  MonteverdiModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The instance singleton */
  static Pointer Instance;

  /** Constructors map */
  ModuleDescriptorMapType m_ModuleDescriptorMap;

  /** Vector of open modules */
  ModuleVectorType         m_ModuleList;

};

}
#endif


