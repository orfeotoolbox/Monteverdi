#ifndef __otbMeanShiftApplicationController_h
#define __otbMeanShiftApplicationController_h

#include "otbMeanShiftApplicationControllerInterface.h"
#include "otbMeanShiftApplicationView.h"

#include "otbImageWidgetController.h"
#include "otbWidgetResizingActionHandler.h"
#include "otbChangeExtractRegionActionHandler.h"
#include "otbPixelDescriptionActionHandler.h"
#include "otbMouseClickActionHandler.h"
#include "itkMultiThreader.h"

namespace otb
{
class ITK_EXPORT MeanShiftApplicationController
      : public MeanShiftApplicationControllerInterface
{
public:
  /** Standard class typedefs */
  typedef MeanShiftApplicationController          Self;
  typedef MeanShiftApplicationControllerInterface Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Standard type macros */
  itkTypeMacro(MeanShiftApplicationController,Superclass);
  itkNewMacro(Self);


  /** Widgets controller and action handlers */
  typedef MeanShiftApplicationView::ImageViewType              ImageViewType;
  typedef MeanShiftApplicationModel::VisualizationModelType    VisualizationModelType;

  typedef ImageWidgetController                                     WidgetsControllerType;
  typedef WidgetResizingActionHandler
    <VisualizationModelType,ImageViewType>                          ResizingHandlerType;
  typedef ChangeExtractRegionActionHandler
      <VisualizationModelType,ImageViewType>                        ChangeRegionHandlerType;

  /** Set the pointer to the view */
  void SetView(MeanShiftApplicationView * view);

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


  bool IsModelUpdating()
  {
    return m_Model->IsUpdating();
  }
protected:
  /** Constructor */
  MeanShiftApplicationController();
  /** Destructor */
  virtual ~MeanShiftApplicationController();

  static ITK_THREAD_RETURN_TYPE ThreadFunction(void*);

private:
  MeanShiftApplicationController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  MeanShiftApplicationView * m_View;

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
