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
#ifndef __otbWriterViewGUI_h
#define __otbWriterViewGUI_h


// #include "otbListenerBase.h"
#include "otbEventsListener.h"

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbWriterViewGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <FL/Fl_Group.H>
#include <FL/Fl.H>

#include "otbWriterModel.h"
#include "otbWriterControllerInterface.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbImageViewerBase.h"
#include "otbObjectList.h"
#include "otbImageView.h"

namespace otb
{
/** \class WriterViewGUI
 *
 */
class ITK_EXPORT WriterViewGUI
  : public EventsListener<std::string>, public WriterViewGroup, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef WriterViewGUI          Self;
  typedef itk::Object   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(WriterViewGUI,itk::Object);


  // Pixel type
  typedef double PixelType;

  typedef WriterModel                        WriterModelType;
  typedef WriterModel::InputImagePixelType   ModelInputPixelType;
  typedef WriterModelType::VisuModelType     VisuModelType;
  typedef ImageView<VisuModelType>                      VisuViewType;
  typedef WriterControllerInterface::Pointer WriterControllerInterfacePointerType;
  typedef WriterModelType::SingleImageType   ModelSingleImageType;
  typedef VisuViewType::ImageType                       ImageType;
  typedef VisuViewType::Pointer                         VisuViewPointerType;
  typedef ImageType::RegionType                         RegionType;
  typedef RegionType::SizeType                          SizeType;
  typedef RegionType::IndexType                         IndexType;
  typedef ImageType::Pointer                            ImagePointerType;

  typedef std::vector<std::string>                      StringVectorType;


  /** progress tmp */
  itkSetMacro(Progress, double);
  itkGetMacro(Progress, double);
  
  /** Set the controller */
  itkGetObjectMacro(WriterController,WriterControllerInterface);

  void SetWriterController(WriterControllerInterface *pController)
  {
    m_WriterController = pController;
   // Set the model
    m_VisuView->SetController(m_WriterController->GetVisuController());
    m_ResultVisuView->SetController(m_WriterController->GetResultVisuController());
  }

  /** Set the model and register the view to the model */
  void SetWriterModel(WriterModel *pModel)
  {
    m_WriterModel = pModel;
    m_WriterModel->RegisterListener(this);
  }
  
  itkGetMacro(VisuView,VisuViewPointerType);
  itkGetMacro(ResultVisuView,VisuViewPointerType);

  void InitWidgets();
  void InitVisu();

  /** Standard Running macro */
  otbRunningMacro();

  /** Event from the model */
  virtual void Notify(const std::string & event);
  virtual void UpdateFeaturePreview();
  virtual void UpdateFeaturePreviewFromOutputList();
  virtual void UpdateParameterArea(unsigned int groupId);
  void Show();

  virtual void ClearFeature();
  virtual void UpdateChannels();

  /** Constructor */
  WriterViewGUI();

  /** InputOutputFeatureLink accessors */
  std::vector<int> GetInputOutputFeatureLink()
  {
    return m_InputOutputFeatureLink;
  };
  void AddToInputOutputFeatureLink(int i)
  {
    m_InputOutputFeatureLink.push_back(i);
  };

  void UpdateSelectedPixel(const IndexType & index);

  ModelInputPixelType GetSelectedPixel() const
  {
    return m_SelectedPixel;
  };

  virtual void SetPixelType(PixelType i);
  virtual void ClearImage();
  
  virtual void InitFeatureOutputList();
  
  virtual void ManageActivationWindowButtons(bool withAwake);
  
  virtual void AwakeProgressFields (double progress);
protected:

  /** Destructor */
  virtual ~WriterViewGUI();

  /** Center the widgets */
  virtual void OpenImage();
  virtual void AddOutputChannel();
  virtual void RemoveOutputChannel();
  virtual void UpOutputChannel();
  virtual void DownOutputChannel();
  virtual void Quit();
//   virtual void AddFeature();
  virtual void OK();
  virtual void Browse();

  void InitParameterGroupList();

  // Update the progress bar
  void UpdateProgress();

private:
  
  // Callback to update the window label
  static void UpdateProgressCallback(void * data);

  // Callback to hide window
  static void QuitCallback(void * data);
  
  WriterViewGUI(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  void UpdateInformation();
//   void UpdateChannelSelection();

  /** Pointer to the model */
  WriterModel::Pointer m_WriterModel;
  /** Pointer to the controller */
  WriterControllerInterface::Pointer m_WriterController;

  StringVectorType         m_FileNameList;
  StringVectorType         m_ViewerNameList;
  std::string              m_LastPath;
  std::string              m_TemplateViewerName;
  StringVectorType         m_DisplayedLabelList;
  StringVectorType         m_UndisplayedLabelList;
  std::vector<bool>        m_DisplayStatusList;
  PixelType                m_PixelType;
  /** output/input feature list link. */
  std::vector<int> m_InputOutputFeatureLink;
  int m_FeatureExtractPreviewParentBrowser;
  /** Value of the selected Pixel*/
  ModelInputPixelType m_SelectedPixel;
  /** Store a list avor all parameter selection group*/
  std::vector<Fl_Group*> m_ParameterGroupList;

  VisuViewPointerType                 m_VisuView;
  VisuViewPointerType                 m_ResultVisuView;
  
  /** progress tmp*/
  double m_Progress;
};
}//end namespace otb

#endif
