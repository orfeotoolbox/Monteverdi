
#include "itkObject.h"
#include "otbImageFileReader.h"
#include "itkBinaryThresholdImageFilter.h"

namespace otb
{

struct Invalid{};

template<class TType=Invalid>
class ITK_EXPORT Parameter
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

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ModuleBase, itk::Object);

  //It works like that, but we have to find a way to avoid the manual
  //definition of all the cases...
  //With this situation, the Parameter class is even useless...
  virtual void SetParameters(std::string key, const Parameter<>& value)
  {
    itkExceptionMacro(<<"Subclass must overload this method");
  }

  virtual void SetParameters(std::string key, const Parameter<std::string>& value)
  {
    itkExceptionMacro(<<"Subclass must overload this method");
  }

  virtual void SetParameters(std::string key, const Parameter<double>& value)
  {
    itkExceptionMacro(<<"Subclass must overload this method");
  }

  virtual itk::ProcessObject* GetProcess()
  {
    itkExceptionMacro(<<"Subclass must overload this method");
  }

protected:
  /** Constructor */
  ModuleBase() {};
  /** Destructor */
  virtual ~ModuleBase() {};

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


  virtual void SetParameters(std::string key, const Parameter<std::string>& value)
  {
    if (key == "FileName")
    {
      m_Process->SetFileName(value.GetValue());
    }
  }

  virtual itk::ProcessObject* GetProcess()
  {
    return m_Process;
  }

protected:
  /** Constructor */
  ModuleReader()
  {
    m_Process = ProcessType::New();
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

  virtual void SetParameters(std::string key, const Parameter<double>& value)
  {
    if (key == "UpperThreshold")
    {
      m_Process->SetUpperThreshold(value.GetValue());
    }
  }

  virtual itk::ProcessObject* GetProcess()
  {
    return m_Process;
  }

protected:
  /** Constructor */
  ModuleThreshold()
  {
    m_Process = ProcessType::New();
  }
  /** Destructor */
  virtual ~ModuleThreshold() {};



private:
  ModuleThreshold(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ProcessType::Pointer m_Process;

};




}
