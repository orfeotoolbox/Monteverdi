#ifndef __otbBasicApplicationView_h
#define __otbBasicApplicationView_h




#include "otbMeanShiftApplicationViewGUI.h"

#include "otbListenerBase.h"
#include "otbMeanShiftApplicationModel.h"
#include "otbMeanShiftApplicationControllerInterface.h"
#include "itkSimpleFastMutexLock.h"

#include "otbImageView.h"

namespace otb
{
/** \class MeanShiftApplicationView
 *
 */
class ITK_EXPORT MeanShiftApplicationView
      : public ListenerBase, public MeanShiftApplicationViewGUI, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef MeanShiftApplicationView          Self;
  typedef MeanShiftApplicationViewGUI   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(MeanShiftApplicationView,Object);


  typedef MeanShiftApplicationModel::VisualizationModelType    VisualizationModelType;
  typedef MeanShiftApplicationModel::RGBPixelType                    PixelType;

  typedef ImageView<VisualizationModelType>                         ImageViewType;


  /** Event from the model */
  virtual void Notify();

  /** Fluid call backs*/
  virtual void OpenImage();
  virtual void SaveLabelImage();
  virtual void SaveClusterImage();
  virtual void Exit();
  virtual void RunSegmentation();
  virtual void SwitchClusters();
  virtual void SwitchBoundaries();
  virtual void SetSpatialRadius(unsigned int sr);
  virtual void SetSpectralRadius(unsigned int sr);
  virtual void SetMinRegionSize(unsigned int mr);

  /** Set the controller */
  itkSetObjectMacro(Controller,MeanShiftApplicationControllerInterface);
  itkGetObjectMacro(Controller,MeanShiftApplicationControllerInterface);

  /** Set the widgets controller */
  itkSetObjectMacro(WidgetsController,ImageWidgetController);

  /** Build the interface */
  virtual void Build();

  /** Get a pointer to the view parts for the controller */
  itkGetObjectMacro(ImageView,ImageViewType);

protected:
  /** Constructor */
  MeanShiftApplicationView();

  /** Destructor */
  virtual ~MeanShiftApplicationView();

  /** Refresh the image widgets */
  void RefreshInterface();
  void RefreshVisualization();


private:
  MeanShiftApplicationView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the controller */
  MeanShiftApplicationControllerInterface::Pointer m_Controller;

  /** Pointer to the widgets controller */
  ImageWidgetController::Pointer                        m_WidgetsController;

  /** Pointer to the model */
  MeanShiftApplicationModel::Pointer               m_Model;

  /** Image view */
  ImageViewType::Pointer                         m_ImageView;

  itk::SimpleFastMutexLock m_Mutex;

};
}//end namespace otb

#endif
