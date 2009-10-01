#ifndef __otbMeanShiftModuleController_h
#define __otbMeanShiftModuleController_h

#include "otbMeanShiftModuleControllerInterface.h"
#include "otbMeanShiftModuleView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbMouseClickActionHandler.h"
#include "itkMultiThreader.h"

namespace otb
{
class ITK_EXPORT MeanShiftModuleController
      : public MeanShiftModuleControllerInterface
{
public:
  /** Standard class typedefs */
  typedef MeanShiftModuleController          Self;
  typedef MeanShiftModuleControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Standard type macros */
  itkTypeMacro(MeanShiftModuleController,Superclass);
  itkNewMacro(Self);


  /** Widgets controller and action handlers */
  typedef MeanShiftModuleView::ImageViewType              ImageViewType;
  typedef MeanShiftModuleModel::VisualizationModelType    VisualizationModelType;

  typedef ImageWidgetController                                     WidgetsControllerType;
  typedef WidgetResizingActionHandler
    <VisualizationModelType,ImageViewType>                          ResizingHandlerType;
  typedef ChangeExtractRegionActionHandler
      <VisualizationModelType,ImageViewType>                        ChangeRegionHandlerType;

  /** Set the pointer to the view */
  void SetView(MeanShiftModuleView * view);

  /** Get the widgets controller */
  itkGetObjectMacro(WidgetsController,WidgetsControllerType);


  /** User action */
  virtual void OpenImage( const char * filename );
  virtual void SaveLabelImage( const char * filename );
  virtual void SaveClusterImage( const char * filename );
  virtual void RunSegmentation();
  virtual void SwitchClusters(bool sc);
  virtual void SwitchBoundaries(bool sb);
  virtual void SetSpatialRadius(unsigned int sr);
  virtual void SetSpectralRadius(unsigned int sr);
  virtual void SetMinRegionSize(unsigned int mr);

  virtual bool GenerateFiltered();
  virtual bool GenerateClustered();
  virtual bool GenerateLabeled();
  virtual void GenerateFiltered(bool t);
  virtual void GenerateClustered(bool t);
  virtual void GenerateLabeled(bool t);




  bool IsModelUpdating()
  {
    return m_Model->IsUpdating();
  }
protected:
  /** Constructor */
  MeanShiftModuleController();
  /** Destructor */
  virtual ~MeanShiftModuleController();

  static ITK_THREAD_RETURN_TYPE ThreadFunction(void*);

private:
  MeanShiftModuleController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  MeanShiftModuleView * m_View;

  /** Widgets controller */
  WidgetsControllerType::Pointer             m_WidgetsController;

  /** Action handlers */
  ResizingHandlerType::Pointer               m_ResizingHandler;
  ChangeRegionHandlerType::Pointer           m_ChangeRegionHandler;

  itk::MultiThreader::Pointer m_Threader;

  bool m_ImageReady;

};
} //end namespace otb

#endif
