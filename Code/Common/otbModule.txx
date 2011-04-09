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
#ifndef __otbModule_txx
#define __otbModule_txx

#include "otbModule.h"

// Filters fir partial specialization
#include "otbMultiToMonoChannelExtractROI.h"
#include "otbVectorImageToAmplitudeImageFilter.h"
#include "itkComplexToRealImageFilter.h"
#include "itkComplexToImaginaryImageFilter.h"
#include "itkComplexToPhaseImageFilter.h"
#include "itkComplexToModulusImageFilter.h"

namespace otb
{

/** Add a new input descriptor */
template <typename T> void Module::AddInputDescriptor(const std::string& key,
                                                      const std::string& description,
                                                      bool optional,
                                                      bool multiple)
{
  // Check if the key already exists
  if (m_InputsMap.count(key) > 0)
    {
    itkExceptionMacro(<< "An input with key " << key << " already exists !");
    }

  // Else build a new descriptor
  InputDataDescriptor desc(TypeManager::GetInstance()->GetTypeName<T>(), key, description);
  desc.SetOptional(optional);
  desc.SetMultiple(multiple);

  // Insert it into the map
  m_InputsMap[key] = desc;
}

/** Add additional supported types for a given input descriptors */
template <typename T> void Module::AddTypeToInputDescriptor(const std::string& key)
{
  // Check if the key already exists
  if (m_InputsMap.count(key) <= 0)
    {
    itkExceptionMacro(<< "No input with key " << key);
    }
  m_InputsMap[key].AddSupportedType(TypeManager::GetInstance()->GetTypeName<T>());
}

/** Add a new output descriptor */
template <typename T> void Module::AddOutputDescriptor(T* data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  // Check if the key already exists
  if (IsOutputKeyPresent(key))
//  if (m_OutputsMap.count(key) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << key << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapper = DataObjectWrapper::Create(data);
  wrapper.SetSourceInstanceId(m_InstanceId);
  wrapper.SetSourceOutputKey(key);
  wrapper.SetDescription(description);

  // Else build a new descriptor
  OutputDataDescriptor desc(wrapper, key, description, cached);

  // Insert it into the map
  m_OutputsMap.push_back(desc);
}

/** Add a new output descritpor (SmartPointer version) */
template <typename T> void Module::AddOutputDescriptor(itk::SmartPointer<T> data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  this->AddOutputDescriptor(data.GetPointer(), key, description, cached);
}

/** Partial specialization for the vector image case */
template <typename T> void Module::AddOutputDescriptor(otb::VectorImage<T> * data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  // Stringstream used to enhance descriptions
  itk::OStringStream oss, ossId;

  // Update output information
  data->UpdateOutputInformation();

  // First, add the whole dataset

  // Description
  oss.str("");
  oss << description << " (" << otbGetTextMacro("Whole dataset") << ")";
  // Output ID
  ossId.str("");
  ossId << key;

  // Check if the key already exists
  if (IsOutputKeyPresent(ossId.str()))
//  if (m_OutputsMap.count(ossId.str()) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapperWhole = DataObjectWrapper::Create(data);
  wrapperWhole.SetSourceInstanceId(m_InstanceId);
  wrapperWhole.SetSourceOutputKey(ossId.str());
  wrapperWhole.SetDescription(oss.str());

  // Build a new descriptor
  OutputDataDescriptor descWhole(wrapperWhole, ossId.str(), oss.str(), cached);

  // Insert it into the map
  //m_OutputsMap[ossId.str()] = descWhole;
  m_OutputsMap.push_back(descWhole);

  // Then, add each band

  // Typedef of the extract ROI filter
  typedef otb::MultiToMonoChannelExtractROI<T, T> ExtractROIImageFilterType;

  // Add sub-bands
  for (unsigned int band = 0; band < data->GetNumberOfComponentsPerPixel(); ++band)
    {
    // Extract band
    typename ExtractROIImageFilterType::Pointer extract = ExtractROIImageFilterType::New();
    extract->SetInput(data);
    extract->SetChannel(band + 1);
    m_ProcessObjects.push_back(extract.GetPointer());

    // Description
    oss.str("");
    oss << description << "(" << otbGetTextMacro("Band") << " " << band + 1 << ")";
    // Output ID
    ossId.str("");
    ossId << key << " (" << otbGetTextMacro("band") << " " << band + 1 << ")";

    // Check if the key already exists
    if (IsOutputKeyPresent(ossId.str()))
  //    if (m_OutputsMap.count(ossId.str()) > 0)
      {
      itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
      }

    // Create a DataObjectWrapper
    DataObjectWrapper wrapper = DataObjectWrapper::Create(extract->GetOutput());
    wrapper.SetSourceInstanceId(m_InstanceId);
    wrapper.SetSourceOutputKey(ossId.str());
    wrapper.SetDescription(oss.str());

    // Build a new descriptor
    OutputDataDescriptor desc(wrapper, ossId.str(), oss.str(), cached);

    // Insert it into the map
    //m_OutputsMap[ossId.str()] = desc;
    m_OutputsMap.push_back(desc);
    }

  // Then, if needed, add the amplitude

  typedef otb::VectorImageToAmplitudeImageFilter<otb::VectorImage<T>, otb::Image<T> > AmplitudeFilterType;

  if (data->GetNumberOfComponentsPerPixel() > 1)
    {
    typename AmplitudeFilterType::Pointer amplitudeFilter = AmplitudeFilterType::New();
    amplitudeFilter->SetInput(data);
    m_ProcessObjects.push_back(amplitudeFilter.GetPointer());

    oss.str("");
    oss << description << " (" << otbGetTextMacro("Amplitude") << ")";
    ossId.str("");
    ossId << key << " (" << otbGetTextMacro("amplitude") << ")";

    // Check if the key already exists
    if (IsOutputKeyPresent(ossId.str()))
  //    if (m_OutputsMap.count(ossId.str()) > 0)
      {
      itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
      }

    // Create a DataObjectWrapper
    DataObjectWrapper wrapper = DataObjectWrapper::Create(amplitudeFilter->GetOutput());
    wrapper.SetSourceInstanceId(m_InstanceId);
    wrapper.SetSourceOutputKey(ossId.str());
    wrapper.SetDescription(oss.str());

    // Build a new descriptor
    OutputDataDescriptor desc(wrapper, ossId.str(), oss.str(), cached);

    // Insert it into the map
    //m_OutputsMap[ossId.str()] = desc;
    m_OutputsMap.push_back(desc);
    }
}

/** Partial specialization for the vector image case (SmartPointer version) */
template <typename T> void Module::AddOutputDescriptor(typename otb::VectorImage<T>::Pointer data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  this->AddOutputDescriptor(data.GetPointer(), key, description, cached);
}

/** Partial specialization for the vector complex image case */
template <typename T> void Module::AddOutputDescriptor(otb::VectorImage<std::complex<T> > * data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  // Stringstream used to enhance descriptions
  itk::OStringStream oss, ossId;

  // First, add the whole dataset

  // Description
  oss.str("");
  oss << description << " (" << otbGetTextMacro("Whole dataset") << ")";
  // Output ID
  ossId.str("");
  ossId << key;

  // Check if the key already exists
  if ( IsOutputKeyPresent(ossId.str()) )
  //if (m_OutputsMap.count(ossId.str()) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapperWhole = DataObjectWrapper::Create(data);
  wrapperWhole.SetSourceInstanceId(m_InstanceId);
  wrapperWhole.SetSourceOutputKey(ossId.str());
  wrapperWhole.SetDescription(oss.str());

  // Build a new descriptor
  OutputDataDescriptor descWhole(wrapperWhole, ossId.str(), oss.str(), cached);

  // Insert it into the map
  //m_OutputsMap[ossId.str()] = descWhole;
  m_OutputsMap.push_back(descWhole);

  // Then, add real, imaginary, module and phase parts from each band

  // Typedef of the extract ROI filter
  typedef otb::MultiToMonoChannelExtractROI<std::complex<T>, std::complex<T> > ExtractROIImageFilterType;

  // First, some typedefs
  typedef otb::Image<std::complex<T> >                                    ComplexImageType;
  typedef otb::Image<T>                                                   ImageType;
  typedef itk::ComplexToRealImageFilter<ComplexImageType, ImageType>      RealFilterType;
  typedef itk::ComplexToImaginaryImageFilter<ComplexImageType, ImageType> ImaginaryFilterType;
  typedef itk::ComplexToModulusImageFilter<ComplexImageType, ImageType>   ModulusFilterType;
  typedef itk::ComplexToPhaseImageFilter<ComplexImageType, ImageType>     PhaseFilterType;

  for (unsigned int band = 0; band < data->GetNumberOfComponentsPerPixel(); ++band)
    {
    // Extract band
    typename ExtractROIImageFilterType::Pointer extract = ExtractROIImageFilterType::New();
    extract->SetInput(data);
    extract->SetChannel(band + 1);
    m_ProcessObjects.push_back(extract.GetPointer());

    // Description
    oss.str("");
    oss << description << "(" << otbGetTextMacro("Band") << " " << band + 1 << ")";
    // Output ID
    ossId.str("");
    ossId << key << " (" << otbGetTextMacro("band") << " " << band + 1 << ")";

    // Check if the key already exists
    if ( IsOutputKeyPresent(ossId.str()) )
    //if (m_OutputsMap.count(ossId.str()) > 0)
      {
      itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
      }

    // Create a DataObjectWrapper
    DataObjectWrapper wrapper = DataObjectWrapper::Create(extract->GetOutput());
    wrapper.SetSourceInstanceId(m_InstanceId);
    wrapper.SetSourceOutputKey(ossId.str());
    wrapper.SetDescription(oss.str());

    // Build a new descriptor
    OutputDataDescriptor desc(wrapper, ossId.str(), oss.str(), cached);

    // Insert it into the map
    //m_OutputsMap[ossId.str()] = desc;
    m_OutputsMap.push_back(desc);

    // Real part
    typename RealFilterType::Pointer realFilter = RealFilterType::New();
    realFilter->SetInput(extract->GetOutput());
    m_ProcessObjects.push_back(realFilter.GetPointer());
    oss.str("");
    oss << description << " (" << otbGetTextMacro("Real part, Band") << " "<< band + 1  <<")";
    ossId.str("");
    ossId << key << " (" << otbGetTextMacro("Real part, band") << " " << band + 1 << ")";

    // Check if the key already exists
    if ( IsOutputKeyPresent(ossId.str()) )
    //if (m_OutputsMap.count(ossId.str()) > 0)
      {
      itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
      }

    // Create a DataObjectWrapper
    DataObjectWrapper wrapperReal = DataObjectWrapper::Create(realFilter->GetOutput());
    wrapperReal.SetSourceInstanceId(m_InstanceId);
    wrapperReal.SetSourceOutputKey(ossId.str());
    wrapperReal.SetDescription(oss.str());

    // Build a new descriptor
    OutputDataDescriptor descReal(wrapperReal, ossId.str(), oss.str(), cached);

    // Insert it into the map
    //m_OutputsMap[ossId.str()] = descReal;
    m_OutputsMap.push_back(descReal);

    // Imaginary part
    typename ImaginaryFilterType::Pointer imaginaryFilter = ImaginaryFilterType::New();
    imaginaryFilter->SetInput(extract->GetOutput());
    m_ProcessObjects.push_back(imaginaryFilter.GetPointer());
    oss.str("");
    oss << description << " (" << otbGetTextMacro("Imaginary part, Band") << " " << band + 1 << ")";
    ossId.str("");
    ossId << key << " (" << otbGetTextMacro("Imaginary part, band") << " " << band + 1 <<")";

      // Check if the key already exists
    if ( IsOutputKeyPresent(ossId.str()) )
    //if (m_OutputsMap.count(ossId.str()) > 0)
      {
      itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
      }

    // Create a DataObjectWrapper
    DataObjectWrapper wrapperImaginary = DataObjectWrapper::Create(imaginaryFilter->GetOutput());
    wrapperImaginary.SetSourceInstanceId(m_InstanceId);
    wrapperImaginary.SetSourceOutputKey(ossId.str());
    wrapperImaginary.SetDescription(oss.str());

    // Build a new descriptor
    OutputDataDescriptor descImaginary(wrapperImaginary, ossId.str(), oss.str(), cached);

    // Insert it into the map
    //m_OutputsMap[ossId.str()] = descImaginary;
    m_OutputsMap.push_back(descImaginary);

    // Modulus part
      typename ModulusFilterType::Pointer modulusFilter = ModulusFilterType::New();
      modulusFilter->SetInput(extract->GetOutput());
      m_ProcessObjects.push_back(modulusFilter.GetPointer());
      oss.str("");
      oss << description << " (" << otbGetTextMacro("Modulus part, Band") << " "<< band +1 << ")";
      ossId.str("");
      ossId << key << " (" << otbGetTextMacro("Modulus part, band") << " "<< band +1 << ")";

      // Check if the key already exists
      if ( IsOutputKeyPresent(ossId.str()) )
      //if (m_OutputsMap.count(ossId.str()) > 0)
        {
        itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
        }

      // Create a DataObjectWrapper
      DataObjectWrapper wrapperModulus = DataObjectWrapper::Create(modulusFilter->GetOutput());
      wrapperModulus.SetSourceInstanceId(m_InstanceId);
      wrapperModulus.SetSourceOutputKey(ossId.str());
      wrapperModulus.SetDescription(oss.str());

      // Build a new descriptor
      OutputDataDescriptor descModulus(wrapperModulus, ossId.str(), oss.str(), cached);

      // Insert it into the map
      //m_OutputsMap[ossId.str()] = descModulus;
      m_OutputsMap.push_back(descModulus);

      // Phase part
      typename PhaseFilterType::Pointer PhaseFilter = PhaseFilterType::New();
      PhaseFilter->SetInput(extract->GetOutput());
      m_ProcessObjects.push_back(PhaseFilter.GetPointer());
      oss.str("");
      oss << description << " (" << otbGetTextMacro("Phase part, Band") << " "<< band +1 << ")";
      ossId.str("");
      ossId << key << " (" << otbGetTextMacro("Phase part, band") << " "<< band +1 << ")";

      // Check if the key already exists
      if ( IsOutputKeyPresent(ossId.str()) )
      //if (m_OutputsMap.count(ossId.str()) > 0)
        {
        itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
        }

      // Create a DataObjectWrapper
      DataObjectWrapper wrapperPhase = DataObjectWrapper::Create(PhaseFilter->GetOutput());
      wrapperPhase.SetSourceInstanceId(m_InstanceId);
      wrapperPhase.SetSourceOutputKey(ossId.str());
      wrapperPhase.SetDescription(oss.str());

      // Build a new descriptor
      OutputDataDescriptor descPhase(wrapperPhase, ossId.str(), oss.str(), cached);

      // Insert it into the map
      //m_OutputsMap[ossId.str()] = descPhase;
      m_OutputsMap.push_back(descPhase);
    }


}

/** Partial specialization for the vector complex image case (SmartPointer version) */
template <typename T> void Module::AddOutputDescriptor(typename otb::VectorImage<std::complex<T> >::Pointer data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  this->AddOutputDescriptor(data.GetPointer(), key, description, cached);
}

/** Partial specialization for the complex image case */
template <typename T> void Module::AddOutputDescriptor(otb::Image<std::complex<T> > * data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  // Stringstream used to enhance descriptions
  itk::OStringStream oss, ossId;

  // Update output information
  data->UpdateOutputInformation();

  // First, add the whole dataset

  // Description
  oss.str("");
  oss << description << " (" << otbGetTextMacro("Whole dataset") << ")";
  // Output ID
  ossId.str("");
  ossId << key;

  // Check if the key already exists
  if (IsOutputKeyPresent(ossId.str()))
//  if (m_OutputsMap.count(ossId.str()) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapperWhole = DataObjectWrapper::Create(data);
  wrapperWhole.SetSourceInstanceId(m_InstanceId);
  wrapperWhole.SetSourceOutputKey(ossId.str());
  wrapperWhole.SetDescription(oss.str());

  // Build a new descriptor
  OutputDataDescriptor descWhole(wrapperWhole, ossId.str(), oss.str(), cached);

  // Insert it into the map
  //m_OutputsMap[ossId.str()] = descWhole;
  m_OutputsMap.push_back(descWhole);

  // Then, add real, imaginary, module and phase parts

  // First, some typedefs
  typedef otb::Image<std::complex<T> >                                    ComplexImageType;
  typedef otb::Image<T>                                                   ImageType;
  typedef itk::ComplexToRealImageFilter<ComplexImageType, ImageType>      RealFilterType;
  typedef itk::ComplexToImaginaryImageFilter<ComplexImageType, ImageType> ImaginaryFilterType;
  typedef itk::ComplexToModulusImageFilter<ComplexImageType, ImageType>   ModulusFilterType;
  typedef itk::ComplexToPhaseImageFilter<ComplexImageType, ImageType>     PhaseFilterType;

  // Real part
  typename RealFilterType::Pointer realFilter = RealFilterType::New();
  realFilter->SetInput(data);
  m_ProcessObjects.push_back(realFilter.GetPointer());
  oss.str("");
  oss << description << " (" << otbGetTextMacro("Real part") << ")";
  ossId.str("");
  ossId << key << " (" << otbGetTextMacro("Real part") << ")";

  // Check if the key already exists
  if (IsOutputKeyPresent(ossId.str()))
//  if (m_OutputsMap.count(ossId.str()) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapperReal = DataObjectWrapper::Create(realFilter->GetOutput());
  wrapperReal.SetSourceInstanceId(m_InstanceId);
  wrapperReal.SetSourceOutputKey(ossId.str());
  wrapperReal.SetDescription(oss.str());

  // Build a new descriptor
  OutputDataDescriptor descReal(wrapperReal, ossId.str(), oss.str(), cached);

  // Insert it into the map
  //m_OutputsMap[ossId.str()] = descReal;
  m_OutputsMap.push_back(descReal);

  // Imaginary part
  typename ImaginaryFilterType::Pointer imaginaryFilter = ImaginaryFilterType::New();
  imaginaryFilter->SetInput(data);
  m_ProcessObjects.push_back(imaginaryFilter.GetPointer());
  oss.str("");
  oss << description << " (" << otbGetTextMacro("Imaginary part") << ")";
  ossId.str("");
  ossId << key << " (" << otbGetTextMacro("Imaginary part") << ")";

  // Check if the key already exists
  if (IsOutputKeyPresent(ossId.str()))
//  if (m_OutputsMap.count(ossId.str()) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapperImaginary = DataObjectWrapper::Create(imaginaryFilter->GetOutput());
  wrapperImaginary.SetSourceInstanceId(m_InstanceId);
  wrapperImaginary.SetSourceOutputKey(ossId.str());
  wrapperImaginary.SetDescription(oss.str());

  // Build a new descriptor
  OutputDataDescriptor descImaginary(wrapperImaginary, ossId.str(), oss.str(), cached);

  // Insert it into the map
  //m_OutputsMap[ossId.str()] = descImaginary;
  m_OutputsMap.push_back(descImaginary);

  // Modulus part
  typename ModulusFilterType::Pointer modulusFilter = ModulusFilterType::New();
  modulusFilter->SetInput(data);
  m_ProcessObjects.push_back(modulusFilter.GetPointer());
  oss.str("");
  oss << description << " (" << otbGetTextMacro("Modulus part") << ")";
  ossId.str("");
  ossId << key << " (" << otbGetTextMacro("Modulus part") << ")";

  // Check if the key already exists
  if (IsOutputKeyPresent(ossId.str()))
//  if (m_OutputsMap.count(ossId.str()) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapperModulus = DataObjectWrapper::Create(modulusFilter->GetOutput());
  wrapperModulus.SetSourceInstanceId(m_InstanceId);
  wrapperModulus.SetSourceOutputKey(ossId.str());
  wrapperModulus.SetDescription(oss.str());

  // Build a new descriptor
  OutputDataDescriptor descModulus(wrapperModulus, ossId.str(), oss.str(), cached);

  // Insert it into the map
  //m_OutputsMap[ossId.str()] = descModulus;
  m_OutputsMap.push_back(descModulus);

  // Phase part
  typename PhaseFilterType::Pointer PhaseFilter = PhaseFilterType::New();
  PhaseFilter->SetInput(data);
  m_ProcessObjects.push_back(PhaseFilter.GetPointer());
  oss.str("");
  oss << description << " (" << otbGetTextMacro("Phase part") << ")";
  ossId.str("");
  ossId << key << " (" << otbGetTextMacro("Phase part") << ")";

  // Check if the key already exists
  if (IsOutputKeyPresent(ossId.str()))
//  if (m_OutputsMap.count(ossId.str()) > 0)
    {
    itkExceptionMacro(<< "An Output with key " << ossId.str() << " already exists !");
    }

  // Create a DataObjectWrapper
  DataObjectWrapper wrapperPhase = DataObjectWrapper::Create(PhaseFilter->GetOutput());
  wrapperPhase.SetSourceInstanceId(m_InstanceId);
  wrapperPhase.SetSourceOutputKey(ossId.str());
  wrapperPhase.SetDescription(oss.str());

  // Build a new descriptor
  OutputDataDescriptor descPhase(wrapperPhase, ossId.str(), oss.str(), cached);

  // Insert it into the map
  //m_OutputsMap[ossId.str()] = descPhase;
  m_OutputsMap.push_back(descPhase);
}

/** Partial specialization for the complex image case (SmartPointer version) */
template <typename T> void Module::AddOutputDescriptor(typename otb::Image<std::complex<T> >::Pointer data,
                                                       const std::string& key,
                                                       const std::string& description,
                                                       bool cached)
{
  this->AddOutputDescriptor(data.GetPointer(), key, description, cached);
}

/** Retrieve the actual data from the map (returns NULL if wrong DataType */
template <typename T> T * Module::GetInputData(const std::string& key, unsigned int idx) const
{
  // Search for the key in the input map
  InputDataDescriptorMapType::const_iterator it = m_InputsMap.find(key);

  // If the key can not be found, throw an exception
  if (it == m_InputsMap.end())
    {
    itkExceptionMacro(<< "Module has no input with key " << key);
    }

  // Check if type are compatible
  if (!it->second.IsTypeCompatible(TypeManager::GetInstance()->GetTypeName<T>()))
    {
    itkExceptionMacro(
      << "Type " << TypeManager::GetInstance()->GetTypeName<T>() <<
      " incompatible with available types for this input: " << it->second.GetDataType());
    }

  // If type is compatible, try to convert
  if(it->second.GetNumberOfData() > 0)
    {
      return dynamic_cast<T*>(it->second.GetNthData(idx).GetDataObject());
    }
  else
    {
      return NULL;
    }
}

/** Retrieve the actual data description from the map */
template <typename T> std::string Module::GetInputDataDescription(const std::string& key, unsigned int idx) const
{
  /*
  T * data = this->GetInputData<T>(key, idx);
  DataObjectWrapper wrapper = DataObjectWrapper::Create(data);
  */

  // Search for the key in the input map
  InputDataDescriptorMapType::const_iterator it = m_InputsMap.find(key);

  // If the key can not be found, throw an exception
  if (it == m_InputsMap.end())
    {
    itkExceptionMacro(<< "Module has no input with key " << key);
    }

  // Check if type are compatible
  if (!it->second.IsTypeCompatible(TypeManager::GetInstance()->GetTypeName<T>()))
    {
    itkExceptionMacro(
      << "Type " << TypeManager::GetInstance()->GetTypeName<T>() <<
      " incompatible with available types for this input: " << it->second.GetDataType());
    }

  return it->second.GetNthData(idx).GetDescription();
}

} // end namespace otb

#endif
