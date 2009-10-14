#ifndef __otbMeanShiftModuleControllerInterface_h
#define __otbMeanShiftModuleControllerInterface_h

#include "itkObject.h"
#include "otbMeanShiftModuleModel.h"

namespace otb
{
class ITK_EXPORT MeanShiftModuleControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef MeanShiftModuleControllerInterface Self;
  typedef itk::Object                      Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  typedef MeanShiftModuleModel           ModelType;


  /** Standard type macros */
  itkTypeMacro(MeanShiftModuleControllerInterface,Superclass);

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

  virtual bool GenerateFiltered() = 0;
  virtual bool GenerateClustered() = 0;
  virtual bool GenerateLabeled() = 0;
  virtual void GenerateFiltered(bool t) = 0;
  virtual void GenerateClustered(bool t) = 0;
  virtual void GenerateLabeled(bool t) = 0;

  
  virtual bool IsModelUpdating() = 0;
  
  virtual void Quit() = 0;
  
protected:
  /** Constructor */
  MeanShiftModuleControllerInterface()
  {
  }
  /** Destructor */
  ~MeanShiftModuleControllerInterface() {};


private:
  MeanShiftModuleControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
