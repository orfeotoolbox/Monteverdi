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
#include "otbCachingModule.h"
#include "otbGraph.h"
#include "otbWriterModule.h"
#include "otbViewerModule.h"

namespace otb
{

class ITK_EXPORT MonteverdiModel
  : public itk::Object, public EventsSender<MonteverdiEvent>, public EventsListener<MonteverdiEvent>
{
 public:
  /** Standard class typedefs */
  typedef MonteverdiModel                                Self;
  typedef MVCModel<ListenerBase>                         Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;


  /** Standard type macro */
  itkTypeMacro(MonteverdiModel, MVCModel);


 /** typedef of the Module constructors function */
  typedef otb::Module::Pointer (* ConstructorPointerType)();

  /** Inner class to represent a registered module */
  class RegisteredModuleDescriptor
  {
    friend class MonteverdiModel;
  public:
    // key to identify the module
    std::string m_Key;
    // Path in the application menu
    std::string m_MenuPath;
    // Registration order
    unsigned int m_RegistrationOrder;
  protected:
    // pointer to the constructor of the module
    ConstructorPointerType m_Constructor;
  };

  /** Map of open modules */
  typedef std::map<std::string,Module::Pointer>            ModuleMapType;

  // Map of registered modules
  typedef std::map<std::string,RegisteredModuleDescriptor> ModuleDescriptorMapType;
  typedef std::map<std::string,unsigned int>               InstancesCountMapType;
  
  // Map of caching modules
  typedef std::map<std::string,CachingModule::Pointer>     CachingModuleMapType;

  // Descriptors
  typedef otb::Module::OutputDataDescriptorMapType         OutputDataDescriptorMapType;
  typedef otb::Module::InputDataDescriptorMapType          InputDataDescriptorMapType;

  // Connection graph
  typedef std::string                                      CGraphVertexType;
  typedef std::pair<std::string, std::string>              CGraphEdgeType;
  typedef otb::Graph<CGraphVertexType, CGraphEdgeType>     ConnectionGraphType;

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
    desc.m_Constructor = (ConstructorPointerType)&T::New;
    desc.m_RegistrationOrder = m_NumberOfRegisteredModules;
    ++m_NumberOfRegisteredModules;
    m_ModuleDescriptorMap[key]=desc;
    m_InstancesCountMap[key]=0;
  }

  itkGetMacro(NumberOfRegisteredModules,unsigned int);

  /** Create a module according to its key. If the key is not a
  registered module, throws an exception */
  void CreateModuleByKey(const std::string & key);

  /** Get available modules map */
  const ModuleDescriptorMapType & GetRegisteredModuleDescriptors() const;

  /** Get available module instances */
  const std::vector<std::string> GetAvailableModuleInstanceIds() const;

  /** Get outputs for a given module instance */
  const OutputDataDescriptorMapType & GetModuleOutputsByInstanceId(const std::string & id) const;

  /** Get required inputs for a given module instance */
  const InputDataDescriptorMapType & GetModuleInputsByInstanceId(const std::string & id) const;

  /** Get the pointer to the module by an instanceId */
  Module * GetModuleByInstanceId(const std::string & instanceId) const;

  /** Add a new connection between modules */
  void AddModuleConnection(const std::string& sourceModuleId,const std::string& outputKey, const std::string& targetModuleId, const std::string& inputKey);

  /** Start the given module */
  void StartModuleByInstanceId(const std::string & id);

  /** Change module InstanceId (ie. tree label) */
  void ChangeInstanceId( const std::string & oldInstanceId, const std::string & newIntanceId );

  /** Change module InstanceId (ie. tree label) output data key from oldKey to newKey */
  void ChangeOutputDataKey(const std::string & instanceId, const std::string & oldKey, const std::string & newKey);

  /** Does the output key from the module instanceId supports caching ? */
  bool SupportsCaching(const std::string & instanceId, const std::string & outputKey) const;

  /** Start caching the given data */
  void StartCaching(const std::string & instanceId, const std::string & outputKey, bool watch = false,unsigned int idx = 0);

  /** Does the output key from the module instanceId supports writing ? */
  bool SupportsWriting(const std::string & instanceId, const std::string & outputKey) const;

  /** Start writing the given data */
  void StartWriting(const std::string & instanceId, const std::string & outputKey, unsigned int idx = 0);

  /** Does the output key from the module instanceId supports viewing ? */
  bool SupportsViewing(const std::string & instanceId, const std::string & outputKey) const;

  /** Start writing the given data */
  void StartViewing(const std::string & instanceId, const std::string & outputKey, unsigned int idx = 0);

  /** Get the caching progress for the given data */
  double GetCachingProgress(const std::string & instanceId, const std::string & outputKey, unsigned int idx = 0) const;

  /** Is data cached ? */
  bool IsCached(const std::string & instanceId, const std::string & outputKey, unsigned int idx = 0) const;

  /** Is module locked */
  bool IsModuleLocked(const std::string & instanceId, std::string & lockingInstanceId) const;

  // Temporary notify stub
  virtual void Notify(const MonteverdiEvent & event);

  ModuleMapType GetModuleMap() const
  {
    return m_ModuleMap;
  }
  virtual void Close();

protected:

  /** This is protected for the singleton. Use GetInstance() instead. */
  itkNewMacro(Self);

  /** Constructor */
  MonteverdiModel();

  /** Destructor */
  virtual ~MonteverdiModel();

  /** Build caching module id */
  static std::string BuildCachingModuleId(const std::string & instanceId, const std::string & outputKey, unsigned int idx = 0);

  /** Split the caching module id */
  static bool SplitCachingModuleId(const std::string & instanceId, std::string & sourceId, std::string & key, unsigned int & idx);

  /** End the caching process by updating the output id */
  void EndCaching(const std::string & cachingModuleId);

  /** Add a graph connection */
  void AddGraphConnection(const std::string& sourceModuleId,const std::string& outputKey, const std::string& targetModuleId, const std::string& inputKey);

 private:
  MonteverdiModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The instance singleton */
  static Pointer Instance;

  /** Constructors map */
  ModuleDescriptorMapType m_ModuleDescriptorMap;

  /** Vector of open modules */
  ModuleMapType         m_ModuleMap;

  /** Instances count map */
  InstancesCountMapType m_InstancesCountMap;

  /** The caching modules map */
  CachingModuleMapType m_CachingModuleMap;

  /** The connections graph */
  ConnectionGraphType::Pointer m_ConnectionGraph;

  /** Number of registered module */
  unsigned int m_NumberOfRegisteredModules;
};

}
#endif


