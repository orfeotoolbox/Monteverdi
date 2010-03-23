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
#ifndef __otbMonteverdiModel_cxx
#define __otbMonteverdiModel_cxx

#include "otbMonteverdiModel.h"
#include "otbGraphVertexIterator.h"
#include "otbGraphOutEdgeIterator.h"

// For pipeline locking mechanism
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graphviz.hpp>
// #include <boost/graph/iteration_macros.hpp>

namespace otb
{
/** Initialize the singleton */
MonteverdiModel::Pointer MonteverdiModel::Instance = NULL;


/**
 * Constructor
 */
MonteverdiModel::MonteverdiModel() : m_ModuleDescriptorMap(), m_ModuleMap(), m_InstancesCountMap(), m_CachingModuleMap(), m_ConnectionGraph(), m_NumberOfRegisteredModules(0), m_EraseCaching(false)
{
  m_ConnectionGraph = ConnectionGraphType::New();
}

MonteverdiModel::~MonteverdiModel()
{
  this->Close();
}

void
MonteverdiModel::Close()
{
  m_ModuleDescriptorMap.clear();
  m_ModuleMap.clear();
  m_InstancesCountMap.clear();
  
  //Remove Caching directory
  if ( m_EraseCaching )
  {
        // Look for the caching module instance id
        CachingModuleMapType::const_iterator it = m_CachingModuleMap.begin();

          // Report errors
          while (it != m_CachingModuleMap.end())
            {
            it->second->RemoveCachingDirectory();
            ++it;
            }     
  }
  m_CachingModuleMap.clear();
}

/** Create a module according to its name. If the name is not a
  registered module, throws an exception */
void MonteverdiModel::CreateModuleByKey(const std::string & key)
{
  ModuleDescriptorMapType::const_iterator mcIt = m_ModuleDescriptorMap.find(key);

  if(mcIt!=m_ModuleDescriptorMap.end())
    {
    // Create a new module instance
    Module::Pointer module = mcIt->second.m_Constructor();

    // Build a unique key
    itk::OStringStream oss;
    oss<<key<<m_InstancesCountMap[key];

    // Register module instance
    module->SetInstanceId(oss.str());
    m_ModuleMap[oss.str()] = module;
    m_ConnectionGraph->AddVertex(oss.str());

    // Register the main model to receive events from the new module
    module->RegisterListener(this);

    // Notify the view that a new module needs inputs
    this->NotifyAll(MonteverdiEvent("InstanceCreated",oss.str()));

    // Update instances count
    m_InstancesCountMap[key]++;
    }
  else
    {
    itkExceptionMacro(<<"No module with key "<<key<<" has been registered.");
    }
}


void MonteverdiModel::StartModuleByInstanceId(const std::string & id)
{
  this->GetModuleByInstanceId(id)->Start();
}

/** Get available modules map */
const MonteverdiModel::ModuleDescriptorMapType & MonteverdiModel::GetRegisteredModuleDescriptors() const
{
  return m_ModuleDescriptorMap;
}


/** Get available module instances */
const std::vector<std::string> MonteverdiModel::GetAvailableModuleInstanceIds() const
{
  std::vector<std::string> availableModulesInstances;
  ModuleMapType::const_iterator mcIt;
  for( mcIt=m_ModuleMap.begin(); mcIt!=m_ModuleMap.end(); mcIt++ )
    {
    availableModulesInstances.push_back(mcIt->first);
    }
  return availableModulesInstances;
}

/** Get outputs for a given module instance */
const MonteverdiModel::OutputDataDescriptorMapType & MonteverdiModel::GetModuleOutputsByInstanceId(const std::string & id) const
{
    return this->GetModuleByInstanceId(id)->GetOutputsMap();
}

/** Get inputs for a given module instance */
const MonteverdiModel::InputDataDescriptorMapType & MonteverdiModel::GetModuleInputsByInstanceId(const std::string & id) const
{
  return this->GetModuleByInstanceId(id)->GetInputsMap();
}

/** Get the pointer to the module by an instanceId */
Module * MonteverdiModel::GetModuleByInstanceId(const std::string & instanceId) const
{
  ModuleMapType::const_iterator it_in;

  ModuleMapType::const_iterator mcIt = m_ModuleMap.find(instanceId);
  if(mcIt!=m_ModuleMap.end())
    {
    return mcIt->second;
    }
  else
    {
    itkExceptionMacro(<<"No module instance with id "<<instanceId<<" has been registered.");
    }
}

/** Add a new connection between modules */
void MonteverdiModel::AddModuleConnection(const std::string& sourceModuleId,const std::string& outputKey, const std::string& targetModuleId, const std::string& inputKey)
{
  // Find the source module
  Module::Pointer source = this->GetModuleByInstanceId(sourceModuleId);

  // Find the target module
  Module::Pointer target = this->GetModuleByInstanceId(targetModuleId);

  // Retrieve the output data wrapper from the source module
  DataObjectWrapper outputWrapper = source->GetOutputByKey(outputKey);

  // Add the given data wrapper as an input to target module
  target->AddInputByKey(inputKey,outputWrapper);
  
  // Add the graph connection
  this->AddGraphConnection(sourceModuleId,outputKey,targetModuleId,inputKey);
}

void MonteverdiModel::AddGraphConnection(const std::string& sourceModuleId,const std::string& outputKey, const std::string& targetModuleId, const std::string& inputKey)
{
  // Create an edge to add in the graph
  CGraphEdgeType cEdge(outputKey,inputKey);
  
  
  // Look for source and target in the graph
  otb::GraphVertexIterator<ConnectionGraphType> sourceIt(m_ConnectionGraph), targetIt(m_ConnectionGraph);

  // Look for the source vertex in the connection graph
  while(!sourceIt.IsAtEnd() && sourceIt.Get() != sourceModuleId)
    {
    ++sourceIt;
    }

  if(sourceIt.IsAtEnd())
    {
    itkExceptionMacro(<<"No module found with instanceId "<<sourceModuleId<<" in the connections graph");
    }

  // Look for the target vertex in the connection graph
  while(!targetIt.IsAtEnd() && targetIt.Get() != targetModuleId)
    {
    ++targetIt;
    }

  if(targetIt.IsAtEnd())
    {
    itkExceptionMacro(<<"No module found with instanceId "<<targetModuleId<<" in the connections graph");
    }

  // Add connection
  m_ConnectionGraph->AddEdge(sourceIt.GetVertexDescriptor(),targetIt.GetVertexDescriptor(),cEdge);
}

void MonteverdiModel::ChangeInstanceId( const std::string & oldInstanceId,  const std::string & newInstanceId )
{
  // Look for the old instance id
  ModuleMapType::const_iterator mcIt = m_ModuleMap.find(oldInstanceId);

  if(mcIt == m_ModuleMap.end())
    {
      itkExceptionMacro(<<"No module found with instanceId "<<oldInstanceId<<".");
    }

  // Look for the old instance id in the graph
  otb::GraphVertexIterator<ConnectionGraphType> vertexIt(m_ConnectionGraph);

  while(!vertexIt.IsAtEnd() && vertexIt.Get() != oldInstanceId)
    {
    ++vertexIt;
    }

  if(!vertexIt.IsAtEnd())
    {
    vertexIt.Set(newInstanceId);
    }
  else
    {
    itkExceptionMacro(<<"No module found with instanceId "<<oldInstanceId<<" in the connections graph");
    }

  // Check if the new instanceId is in use
  if(m_ModuleMap.count(newInstanceId)>0)
    {
    itkExceptionMacro(<<"InstanceId "<<newInstanceId<<" already exists");
    }

  // Retrieve the module pointer
  Module::Pointer module = mcIt->second;

  // Register module instance
  module->SetInstanceId(newInstanceId);
  m_ModuleMap[newInstanceId] = module;
  m_ModuleMap.erase( oldInstanceId );
}

void MonteverdiModel::ChangeOutputDataKey(const std::string & instanceId, const std::string & oldKey, const std::string & newKey)
{
  // Look for the old instance id
  ModuleMapType::const_iterator mcIt = m_ModuleMap.find(instanceId);
  
  if(mcIt == m_ModuleMap.end())
    {
      itkExceptionMacro(<<"No module found with instanceId "<<instanceId<<".");
    }
  
  mcIt->second->ChangeOutputKey(oldKey, newKey);

  // Update connection graph as well

   // Look for the instance id in the graph
  otb::GraphVertexIterator<ConnectionGraphType> vertexIt(m_ConnectionGraph);

  while(!vertexIt.IsAtEnd() && vertexIt.Get() != instanceId)
    {
    ++vertexIt;
    }

  // If found
  if(!vertexIt.IsAtEnd())
    {
    otb::GraphOutEdgeIterator<ConnectionGraphType> outEdgeIt(m_ConnectionGraph,vertexIt);

    while(!outEdgeIt.IsAtEnd())
      {
      CGraphEdgeType edge = outEdgeIt.Get();

      if(edge.first == oldKey)
       {
       edge.first = newKey;
       outEdgeIt.Set(edge);
       }
      ++outEdgeIt;
      }
    }
}
  
bool MonteverdiModel::SupportsCaching(const std::string & instanceId, const std::string & outputKey) const
{
  // First retrieve the data descritpor
  Module::Pointer module = GetModuleByInstanceId(instanceId);

  // Then, retrieve the ouptut
  DataObjectWrapper output = module->GetOutputByKey(outputKey);

  // Create a local instance of the caching module
  CachingModule::Pointer cache = CachingModule::New();
  
  // Check type compatibility
  InputDataDescriptor inputDD = cache->GetInputDataDescriptorByKey("InputDataSet");
  return inputDD.IsTypeCompatible(output.GetDataType());
}

bool MonteverdiModel::SupportsWriting(const std::string & instanceId, const std::string & outputKey) const
{
  // First retrieve the data descritpor
  Module::Pointer module = GetModuleByInstanceId(instanceId);

  // Then, retrieve the ouptut
  DataObjectWrapper output = module->GetOutputByKey(outputKey);

  // Create a local instance of the caching module
  WriterModule::Pointer writer = WriterModule::New();
  
  // Check type compatibility
  InputDataDescriptor inputDD = writer->GetInputDataDescriptorByKey("InputDataSet");
  return inputDD.IsTypeCompatible(output.GetDataType());
}


bool MonteverdiModel::SupportsViewing(const std::string & instanceId, const std::string & outputKey) const
{
  // First retrieve the data descritpor
  Module::Pointer module = GetModuleByInstanceId(instanceId);

  // Then, retrieve the ouptut
  DataObjectWrapper output = module->GetOutputByKey(outputKey);

  // Create a local instance of the caching module
  ViewerModule::Pointer viewer = ViewerModule::New();
  
  // Check type compatibility
  InputDataDescriptor inputDD = viewer->GetInputDataDescriptorByKey("InputImage");
  return inputDD.IsTypeCompatible(output.GetDataType());
}


/** Is data cached ? */
bool MonteverdiModel::IsCached(const std::string & instanceId, const std::string & outputKey, unsigned int idx) const
{
  // Retrieve the output map
  OutputDataDescriptorMapType outputs = GetModuleOutputsByInstanceId(instanceId);
  
  // Retrieve the output data descriptor
  OutputDataDescriptorMapType::const_iterator outIt = outputs.find(outputKey);
  if(outIt == outputs.end())
    {
    itkExceptionMacro(<<"Module "<<instanceId<<" has no output with key "<<outputKey);
    }

  return outIt->second.IsCached();
}

/** Start caching the given data */
void MonteverdiModel::StartWriting(const std::string & instanceId, const std::string & outputKey, unsigned int idx)
{
  // First retrieve the data descritpor
  Module::Pointer module = GetModuleByInstanceId(instanceId);

  // Then, retrieve the ouptut
  DataObjectWrapper output = module->GetOutputByKey(outputKey,idx);

  // Now, create a new instance of the caching module
  WriterModule::Pointer writer = WriterModule::New();

  // Pass data to the module
  writer->AddInputByKey("InputDataSet",output);

  // Build a unique key
  itk::OStringStream oss;
  oss<<"Writer";

  if(m_InstancesCountMap.count("Writer")>0)
    {
    oss<<m_InstancesCountMap["Writer"];
    // Update instances count
    m_InstancesCountMap["Writer"]++;
    }
  
    // Register module instance
    writer->SetInstanceId(oss.str());
    m_ModuleMap[oss.str()] = writer;
    m_ConnectionGraph->AddVertex(oss.str());
    
    // Add the graph connection
    this->AddGraphConnection(instanceId,outputKey,oss.str(),"InputDataSet");

    // Register the main model to receive events from the new module
    writer->RegisterListener(this);

    // Start module
    writer->Start();
}

/** Start caching the given data */
void MonteverdiModel::StartViewing(const std::string & instanceId, const std::string & outputKey, unsigned int idx)
{
  // First retrieve the data descritpor
  Module::Pointer module = GetModuleByInstanceId(instanceId);

  // Then, retrieve the ouptut
  DataObjectWrapper output = module->GetOutputByKey(outputKey,idx);

  // Now, create a new instance of the caching module
  ViewerModule::Pointer viewer = ViewerModule::New();

  // Pass data to the module
  viewer->AddInputByKey("InputImage",output);

  // Build a unique key
  itk::OStringStream oss;
  oss<<"Viewer";

  if(m_InstancesCountMap.count("Viewer")>0)
    {
    oss<<m_InstancesCountMap["Viewer"];
    // Update instances count
    m_InstancesCountMap["Viewer"]++;
    }
  
    // Register module instance
    viewer->SetInstanceId(oss.str());
    m_ModuleMap[oss.str()] = viewer;
    m_ConnectionGraph->AddVertex(oss.str());

    // Add the graph connection
    this->AddGraphConnection(instanceId,outputKey,oss.str(),"InputImage");

    // Register the main model to receive events from the new module
    viewer->RegisterListener(this);
    
    // Start module
    viewer->Start();
}

/** Start caching the given data */
void MonteverdiModel::StartCaching(const std::string & instanceId, const std::string & outputKey, bool watch, unsigned int idx)
{
  // First retrieve the data descritpor
  Module::Pointer module = GetModuleByInstanceId(instanceId);

  // Then, retrieve the ouptut
  DataObjectWrapper output = module->GetOutputByKey(outputKey,idx);

  // Now, create a new instance of the caching module
  CachingModule::Pointer cache = CachingModule::New();

  // Pass data to the module
  cache->AddInputByKey("InputDataSet",output);

  // Register to receive events
  cache->RegisterListener(this);

  if(!watch)
    {
    // Disable individual reporting
    cache->WatchProgressOff();
    }
    
  // Create the instance id
  std::string id = BuildCachingModuleId(instanceId,outputKey,idx);
  
  // Store the module in the map
  cache->SetInstanceId(id);
  m_CachingModuleMap[id] = cache;
  
  // Finally starts the module
  cache->Start();
}

/** End the caching process by updating the output id */
void MonteverdiModel::EndCaching(const std::string & cachingModuleId)
{
  // Retrieve the parameters from the caching module id
    std::string id,key;
    unsigned int idx;

    // Try to split back the string
    bool ok = SplitCachingModuleId(cachingModuleId,id,key,idx);

    if(!ok)
      {
      itkExceptionMacro("Failed to split the caching module instance id");
      }

    // Retrieve the caching module
    CachingModule::Pointer cache = m_CachingModuleMap[cachingModuleId];
    
    // First retrieve the data descritpor
    Module::Pointer module = GetModuleByInstanceId(id);

    // Retrieve the cached data
    DataObjectWrapper cachedData = cache->GetOutputByKey("CachedData");
  
    // Load cached data to the module
    module->LoadCachedData(cachedData,key,idx);
}

/** Get the caching progress for the given data */
double MonteverdiModel::GetCachingProgress(const std::string & instanceId, const std::string & outputKey, unsigned int idx) const
{
  // Create the instance id
  std::string id = BuildCachingModuleId(instanceId,outputKey,idx);

  // Look for the caching module instance id
  CachingModuleMapType::const_iterator it = m_CachingModuleMap.find(id);

  // Report errors
  if(it == m_CachingModuleMap.end())
    {
    itkExceptionMacro(<<"No caching module found with instanceId "<<id);
    }
  
  // Return progress
  return it->second->GetProgress();
}

std::string MonteverdiModel::BuildCachingModuleId(const std::string & instanceId, const std::string & outputKey, unsigned int idx)
{
 // Create the instance id
  itk::OStringStream oss;
  oss<<"[Cache] "<<instanceId<<":"<<outputKey<<":"<<idx;
  return oss.str();
}

bool MonteverdiModel::SplitCachingModuleId(const std::string & instanceId, std::string & sourceId, std::string & key, unsigned int & idx)
{
  // Eliminate prefix
  std::string temp = instanceId.substr(8,std::string::npos);

  // Get source id
  size_t idPos = temp.find_first_of(":");

  if(idPos==std::string::npos)
    {
    return false;
    }
  sourceId = temp.substr(0,idPos);

  // Remove the sourceId part
  temp = temp.substr(idPos+1);

  // Get outputKey
  size_t keyPos = temp.find_first_of(":");

  
  if(keyPos==std::string::npos)
    {
    return false;
    }
  key = temp.substr(0,keyPos);
  
  // Remove the outputKey part
  temp = temp.substr(keyPos+1);
  
  // compute the idx from the remaining
  idx = atoi(temp.c_str());

  return true;
}

bool MonteverdiModel::IsModuleLocked(const std::string & instanceId, std::string & lockingInstanceId) const
{
  // First, extract the connected components of the connections graph
  std::vector<int> component(num_vertices(m_ConnectionGraph->GetGraphContainer()));
  connected_components(m_ConnectionGraph->GetGraphContainer(), &component[0]);

  // Then, find the component to which instanceId belongs
  otb::GraphVertexIterator<ConnectionGraphType> vertexIt(m_ConnectionGraph);
  vertexIt.GoToBegin();

  while(!vertexIt.IsAtEnd() && vertexIt.Get() != instanceId)
    {
    ++vertexIt;
    }
  if(vertexIt.IsAtEnd())
    {
    itkExceptionMacro(<<"No module found with instanceId "<<instanceId<<" in the connections graph");
    }

  // Store the component id
  int componentId = vertexIt.GetVertexDescriptor();

  // Now, look in the connection graph if another module is locking
  // this connected component
  vertexIt.GoToBegin();

  bool resp = false;

  while(!vertexIt.IsAtEnd() && !resp)
    {
    // If we are in the same component
    if(component[vertexIt.GetVertexDescriptor()] == componentId)
      {
      Module::Pointer currentModule = this->GetModuleByInstanceId(vertexIt.Get());

      if(currentModule->IsLockingPipeline())
       {
       resp = true;
       lockingInstanceId = vertexIt.Get();
       }
      }
    ++vertexIt;
    }
  return resp;
}

void MonteverdiModel::Notify(const MonteverdiEvent & event)
{
  // First, check if the notify event comes from a CachingModule
  CachingModuleMapType::iterator it = m_CachingModuleMap.find(event.GetInstanceId());

  if(it == m_CachingModuleMap.end())
    {
    // The event does not come from a caching module, so forward it
    this->NotifyAll(event);
    }
  else
    {
      // End the caching process
      this->EndCaching(event.GetInstanceId());
    }
}

/** Manage the singleton */
MonteverdiModel::Pointer
MonteverdiModel::GetInstance()
{
  if (!Instance)
  {
    Instance = MonteverdiModel::New();
  }
  return Instance;
}

void MonteverdiModel::GetGraphvizDotFile (const std::string & fname)
{
  // Look for the old instance id in the graph
//   otb::GraphVertexIterator<ConnectionGraphType> vertexIt(m_ConnectionGraph);
//   otb::GraphOutEdgeIterator<ConnectionGraphType> outEdgeIt(m_ConnectionGraph,vertexIt);
  
//   const boost::property_map<ConnectionGraphType, boost::vertex_attribute_t>::type& vertAttr = boost::get(boost::vertex_attribute, m_ConnectionGraph->GetGraphContainer());
//   const boost::property_map<ConnectionGraphType, boost::edge_attribute_t>::type& edgeAttr = boost::get(boost::edge_attribute, m_ConnectionGraph->GetGraphContainer());
//
//   boost::dynamic_properties dp;
//   dp.property("Operation", edgeAttr );
//   dp.property("Module", vertAttr );
  std::ofstream ofs( fname.c_str() );

  boost::write_graphviz(ofs, m_ConnectionGraph->GetGraphContainer()/*, dp*/);
}


}// End namespace

#endif
