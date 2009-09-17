
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
  DataDescriptor() : m_DataType("Unknown"), m_DataKey("Unknown"), m_DataDescription("Unknown"),
  m_Optional(false), m_Multiple(false), m_NumberOfData(0) {}
  virtual ~DataDescriptor() {}

  /// The key to identify data
  std::string m_DataType;

  /// Data name
  std::string m_DataKey;

  /// A description of the data
  std::string m_DataDescription;
  
  bool m_Optional;
  
  bool m_Multiple;
  
  unsigned int m_NumberOfData;

};

class DataWrapper
{
public:
  DataWrapper()  {}
  ~DataWrapper() {}
  
  itk::DataObject * m_DataObject;
  std::string       m_DataType;
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
  typedef std::map<std::string, DataDescriptor>  DataDescriptorMapType;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ModuleBase, itk::Object);

  virtual void SetParameters(std::string key, const ParameterBase* value)
  {
    itkExceptionMacro(<<"Subclasses should override this method");
  }

  /// Input Data management
  virtual void AddDataByKey(const std::string & key, const DataWrapper & data)
  {
    // We may be missing type checking here
    itkExceptionMacro(<<"Subclasses should override this method 1");
  }
 
  /// Output data management
  virtual const DataWrapper GetDataByKey(const std::string & key,unsigned int idx)
  {
    itkExceptionMacro(<<"Subclasses should override this method 2");
  }


  const DataDescriptorMapType & GetDataDescriptorsMap()
  {
    return m_DataDescriptorsMap;
  }

  // Default is doing nothing (pipeline execution)
  virtual void Update(){}

protected:
  /** Constructor */
  ModuleBase() {};
  /** Destructor */
  virtual ~ModuleBase() {};

  // Map could be replaced by a simple std::vector
  DataDescriptorMapType m_DataDescriptorsMap;

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

  virtual const DataWrapper GetDataByKey(const std::string & key, unsigned int idx)
  {
    if (key == "OutputImage")
    {
      DataWrapper dwrap;
      dwrap.m_DataObject = m_Process->GetOutput();
      dwrap.m_DataType = m_DataDescriptorsMap[key].m_DataType;
      return dwrap;
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
    DataDescriptor descriptor;
    descriptor.m_DataType = "Floating_Point_Image";
    descriptor.m_DataKey = "OutputImage";
    descriptor.m_DataDescription = "Image read from the disk";
    descriptor.m_NumberOfData = 1;
    m_DataDescriptorsMap[descriptor.m_DataKey]=descriptor;
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
  typedef otb::Image<unsigned char,2> OutputImageType;
  typedef itk::BinaryThresholdImageFilter<ImageType, OutputImageType>  ProcessType;

  virtual void SetParameters(std::string key, const ParameterBase* value)
  {
    if (key == "UpperThreshold")
    {
      //TODO check the result of the dynamic_cast
      m_Process->SetUpperThreshold(
          dynamic_cast<const Parameter<double>*>(value)->GetValue());
    }
  }
  
  virtual void AddDataByKey(const std::string & key, const DataWrapper & data)
  {
    if (key == "InputImage")
    {
      if (m_DataDescriptorsMap[key].m_DataType == data.m_DataType)
      {
        // Possible dynamic cast here
        m_Process->GetInputs()[0] = data.m_DataObject;
      }
      else
      {
        itkExceptionMacro(<<"Wrong type for input "<<key<<": expected "<<m_DataDescriptorsMap[key].m_DataType<<", received "<<data.m_DataType);
      }
    }
    else
    {
      itkExceptionMacro(<<"No input corresponding to "<<key);
    }
  }

  virtual const DataWrapper GetDataByKey(const std::string & key,unsigned int idx)
  {
    if(key == "OutputImage")
      {
      DataWrapper dwrap;
      dwrap.m_DataObject = m_Process->GetOutput();
      dwrap.m_DataType = m_DataDescriptorsMap[key].m_DataType;
      return dwrap;
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
    DataDescriptor descriptor;
    descriptor.m_DataType = "Labeled_Image";
    descriptor.m_DataKey = "OutputImage";
    descriptor.m_DataDescription = "Thresholded image";
    descriptor.m_NumberOfData = 1;
    m_DataDescriptorsMap[descriptor.m_DataKey]=descriptor;

    // Describe inputs
    descriptor.m_DataType = "Floating_Point_Image";
    descriptor.m_DataKey = "InputImage";
    descriptor.m_DataDescription = "Image to apply threshold on";
    m_DataDescriptorsMap[descriptor.m_DataKey]=descriptor;

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
  
  virtual void AddDataByKey(const std::string & key, const DataWrapper & data)
  {
    if (key == "InputImage")
    {
      if (m_DataDescriptorsMap[key].m_DataType == data.m_DataType)
      {
        m_Process->SetInput(dynamic_cast<ImageType *> (data.m_DataObject));
      }
      else
      {
        itkExceptionMacro(<<"Wrong type for input "<<key<<": expected "<<m_DataDescriptorsMap[key].m_DataType<<", received "<<data.m_DataType);
      }
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
    DataDescriptor descriptor;
    descriptor.m_DataType = "Floating_Point_Image";
    descriptor.m_DataKey = "InputImage";
    descriptor.m_DataDescription = "Image to write";
    m_DataDescriptorsMap[descriptor.m_DataKey]=descriptor;

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
