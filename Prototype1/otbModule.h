
#include "itkObject.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"

namespace otb
{

struct Invalid{};

class ITK_EXPORT ParameterBase
{
public:
  ParameterBase(){};
  virtual ~ParameterBase(){};
};

template<class TType=Invalid>
class ITK_EXPORT Parameter : public ParameterBase
{
public:
  Parameter(TType value)
  {
    m_Value = value;
  }
  ~Parameter(){};

  Parameter(const Parameter<TType>& param)
  {
    m_Value = param.GetValue();
  }

  void operator=(const Parameter<TType>& param)
  {
    m_Value = param.GetValue();
  }

  TType GetValue() const
  {
    return m_Value;
  }

private:
  TType m_Value;
};

class DataDescriptor
{
public:
  DataDescriptor() : m_DataType("Unknown"), m_DataKey("Unknown"), m_DataDescription("Unknown") {}
  virtual ~DataDescriptor() {}

  /// The key to identify data
  std::string m_DataType;

  /// Data name
  std::string m_DataKey;

  /// A description of the data
  std::string m_DataDescription;
};

class InputDataDescriptor
  : public DataDescriptor
{
public:
  InputDataDescriptor() : m_Optional(false), m_Multiple(false) {}
  virtual ~InputDataDescriptor() {}
  
  bool m_Optional;
  
  bool m_Multiple;
};

class OutputDataDescriptor
  : public DataDescriptor
{
public:
  OutputDataDescriptor() : m_NumberOfData(0) {}
  virtual ~OutputDataDescriptor() {}
  
  unsigned int m_NumberOfData;
};


/**
* The module base class: used at the application level, it contains the
* generic interface
*/

class ITK_EXPORT ModuleBase : public itk::Object
{
public:
  /** Standard typedefs */
  typedef ModuleBase            Self;
  typedef itk::Object Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  /** Data Description interface */
  typedef std::map<std::string,InputDataDescriptor>  InputDataDescriptorMapType;
  typedef std::map<std::string,OutputDataDescriptor> OutputDataDescriptorMapType;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ModuleBase, itk::Object);

  virtual void SetParameters(std::string key, const ParameterBase* value)
  {
    itkExceptionMacro(<<"Subclass must overload this method");
  }

  /// Input Data management
  virtual void AddInputData(const std::string & key, itk::DataObject * data)
  {
    // We may be missing type checking here
    itkExceptionMacro(<<"Subclass must overload this method");
  }
 
  /// Output data management
  virtual itk::DataObject * GetOutputByKey(const std::string & key,unsigned int idx)
  {
    itkExceptionMacro(<<"Subclass must overload this method");
  }

  // Output data management


  const InputDataDescriptorMapType & GetInputDataDescriptorsMap()
  {
    return m_InputDataDescriptorsMap;
  }

  const OutputDataDescriptorMapType & GetOutputDataDescriptorsMap()
  {
    return m_OutputDataDescriptorsMap;
  }

  // Default is doing nothing (pipeline execution)
  virtual void Update(){}

protected:
  /** Constructor */
  ModuleBase() {};
  /** Destructor */
  virtual ~ModuleBase() {};


  // Map could be replaced by a simple std::vector
  InputDataDescriptorMapType m_InputDataDescriptorsMap;

  OutputDataDescriptorMapType m_OutputDataDescriptorsMap;

private:
  ModuleBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

/**
* Example of module defined for the reader
*/
class ITK_EXPORT ModuleReader : public ModuleBase
{
public:
  /** Standard typedefs */
  typedef  ModuleReader           Self;
  typedef ModuleBase Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ModuleReader, ModuleBase);

  typedef otb::Image<double, 2> ImageType;
  typedef otb::ImageFileReader<ImageType> ProcessType;

  virtual void SetParameters(std::string key, const ParameterBase* value)
  {
    if (key == "FileName")
    {
      //TODO check the dynamic_cast output
      m_Process->SetFileName(dynamic_cast<const Parameter<std::string>*>(value)->GetValue());
    }
  }

  virtual itk::DataObject * GetOutputByKey(const std::string & key,unsigned int idx)
  {
    if(key == "OutputImage")
      {
      return m_Process->GetOutput();
      }
    else
      {
      itkExceptionMacro(<<"No output corresponding to "<<key<<" (idx = "<<idx<<")");
      }
  }

protected:
  /** Constructor */
  ModuleReader()
  {
    m_Process = ProcessType::New();
    
    // Describe outputs
    OutputDataDescriptor outputDescriptor;
    outputDescriptor.m_DataType = "Floating_Point_Image";
    outputDescriptor.m_DataKey = "OutputImage";
    outputDescriptor.m_DataDescription = "Image read from the disk";
    outputDescriptor.m_NumberOfData = 1;
    m_OutputDataDescriptorsMap[outputDescriptor.m_DataKey]=outputDescriptor;
  }
  /** Destructor */
  virtual ~ModuleReader() {};

private:
  ModuleReader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  ProcessType::Pointer m_Process;
};


/**
* Example of module defined for the threshold
*/
class ITK_EXPORT ModuleThreshold : public ModuleBase
{
public:
  /** Standard typedefs */
  typedef  ModuleThreshold           Self;
  typedef ModuleBase Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ModuleThreshold, ModuleBase);

  typedef otb::Image<double, 2> ImageType;
  typedef itk::BinaryThresholdImageFilter<ImageType, ImageType>  ProcessType;

  virtual void SetParameters(std::string key, const ParameterBase* value)
  {
    if (key == "UpperThreshold")
    {
      //TODO check the result of the dynamic_cast
      m_Process->SetUpperThreshold(
          dynamic_cast<const Parameter<double>*>(value)->GetValue());
    }
  }
  
  virtual void AddInputData(const std::string & key, itk::DataObject * data)
  {
    if(key == "InputImage")
      {
      // Possible dynamic cast here
      m_Process->GetInputs()[0] = data;
      }
    else
      {
      itkExceptionMacro(<<"No input corresponding to "<<key);
      }
  }

  virtual itk::DataObject * GetOutputByKey(const std::string & key,unsigned int idx)
  {
    if(key == "OutputImage")
      {
      return m_Process->GetOutput();
      }
    else
      {
      itkExceptionMacro(<<"No output corresponding to "<<key<<" (idx = "<<idx<<")");
      }
  }

protected:
  /** Constructor */
  ModuleThreshold()
  {
    m_Process = ProcessType::New();

    // Describe outputs
    OutputDataDescriptor outputDescriptor;
    outputDescriptor.m_DataType = "Floating_Point_Image";
    outputDescriptor.m_DataKey = "OutputImage";
    outputDescriptor.m_DataDescription = "Thresholded image";
    outputDescriptor.m_NumberOfData = 1;
    m_OutputDataDescriptorsMap[outputDescriptor.m_DataKey]=outputDescriptor;

    // Describe inputs
    InputDataDescriptor inputDescriptor;
    inputDescriptor.m_DataType = "Floating_Point_Image";
    inputDescriptor.m_DataKey = "InputImage";
    inputDescriptor.m_DataDescription = "Image to apply threshold on";
    m_InputDataDescriptorsMap[inputDescriptor.m_DataKey]=inputDescriptor;

  }
  /** Destructor */
  virtual ~ModuleThreshold() {};

private:
  ModuleThreshold(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ProcessType::Pointer m_Process;
};




/**
* Example of module defined for the writer
*/
class ITK_EXPORT ModuleWriter : public ModuleBase
{
public:
  /** Standard typedefs */
  typedef  ModuleWriter           Self;
  typedef ModuleBase Superclass;
  typedef itk::SmartPointer<Self>           Pointer;
  typedef itk::SmartPointer<const Self>     ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ModuleWriter, ModuleBase);

  typedef otb::Image<double, 2>           ImageType;
  typedef otb::ImageFileWriter<ImageType> ProcessType;

  virtual void SetParameters(std::string key, const ParameterBase* value)
  {
    if (key == "FileName")
    {
      //TODO check the result of the dynamic_cast
      m_Process->SetFileName(
          dynamic_cast<const Parameter<string>*>(value)->GetValue());
    }
  }
  
  virtual void AddInputData(const std::string & key, itk::DataObject * data)
  {
    if(key == "InputImage")
      {
      // Possible dynamic cast here
      m_Process->SetInput( dynamic_cast<ImageType *>(data));
      }
    else
      {
      itkExceptionMacro(<<"No input corresponding to "<<key);
      }
  }

protected:
  /** Constructor */
  ModuleWriter()
  {
    m_Process = ProcessType::New();

     // Describe inputs
    InputDataDescriptor inputDescriptor;
    inputDescriptor.m_DataType = "Floating_Point_Image";
    inputDescriptor.m_DataKey = "InputImage";
    inputDescriptor.m_DataDescription = "Image to write";
    m_InputDataDescriptorsMap[inputDescriptor.m_DataKey]=inputDescriptor;

  }
  /** Destructor */
  virtual ~ModuleWriter() {};

  virtual void Update()
  {
    m_Process->Update();
  }

private:
  ModuleWriter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ProcessType::Pointer m_Process;

};


}
