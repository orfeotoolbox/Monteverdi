#ifndef __otbMeanShiftApplicationControllerInterface_h
#define __otbMeanShiftApplicationControllerInterface_h

#include "itkObject.h"
#include "otbMeanShiftApplicationModel.h"

namespace otb
{
class ITK_EXPORT MeanShiftApplicationControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef MeanShiftApplicationControllerInterface Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef MeanShiftApplicationModel           ModelType;

  /** Standard type macros */
  itkTypeMacro(MeanShiftApplicationControllerInterface,Superclass);

  /** Users actions */
  virtual void OpenImage( const char * filename ) = 0;
  virtual void SaveLabelImage( const char * filename ) = 0;
  virtual void SaveClusterImage( const char * filename ) = 0;
  virtual void RunSegmentation() = 0;
  virtual void SwitchClusters(bool sc) = 0;
  virtual void SwitchBoundaries(bool sb) = 0;
  virtual void SetSpatialRadius(unsigned int sr) = 0;
  virtual void SetSpectralRadius(unsigned int sr) = 0;
  virtual void SetMinRegionSize(unsigned int mr) =0;


  virtual bool IsModelUpdating() = 0;

protected:
  /** Constructor */
  MeanShiftApplicationControllerInterface()
  {
    m_Model = ModelType::GetInstance();
  }
  /** Destructor */
  ~MeanShiftApplicationControllerInterface() {};

  /** The instance of the model (for commodity) */
  ModelType * m_Model;

private:
  MeanShiftApplicationControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
