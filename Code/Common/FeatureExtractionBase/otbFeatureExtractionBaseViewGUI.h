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
#ifndef __otbFeatureExtractionBaseViewGUI_h
#define __otbFeatureExtractionBaseViewGUI_h

// #include "otbListenerBase.h"
#include "otbEventsListener.h"
// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbFeatureExtractionBaseViewGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <FL/Fl_Group.H>

#include "otbFeatureExtractionBaseModel.h"
#include "otbFeatureExtractionBaseControllerInterface.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbObjectList.h"
#include "otbImageView.h"

namespace otb
{
/** \class FeatureExtractionBaseViewGUI
 *
 */
class ITK_EXPORT FeatureExtractionBaseViewGUI
  : public EventsListener<std::string>, public FeatureExtractionBaseViewGroup, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef FeatureExtractionBaseViewGUI      Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(FeatureExtractionBaseViewGUI, itk::Object);

  // Pixel type
  typedef double PixelType;

  typedef FeatureExtractionBaseModel                        FeatureExtractionBaseModelType;
  typedef FeatureExtractionBaseModel::InputImagePixelType   ModelInputPixelType;
  typedef FeatureExtractionBaseModelType::VisuModelType     VisuModelType;
  typedef ImageView<VisuModelType>                      VisuViewType;
  typedef FeatureExtractionBaseControllerInterface::Pointer FeatureExtractionBaseControllerInterfacePointerType;
  typedef FeatureExtractionBaseModelType::SingleImageType   ModelSingleImageType;
  typedef VisuViewType::ImageType                       ImageType;
  typedef VisuViewType::Pointer                         VisuViewPointerType;
  typedef ImageType::RegionType                         RegionType;
  typedef RegionType::SizeType                          SizeType;
  typedef RegionType::IndexType                         IndexType;
  typedef ImageType::Pointer                            ImagePointerType;
  typedef FeatureInfoBase::FeatureType                  FeatureType;

  typedef std::vector<std::string> StringVectorType;

  /** Set the controller */
  itkGetObjectMacro(FeatureExtractionBaseController, FeatureExtractionBaseControllerInterface);

  void SetFeatureExtractionBaseController(FeatureExtractionBaseControllerInterface *pController)
  {
    m_FeatureExtractionBaseController = pController;
    // Set the model
    m_VisuView->SetController(m_FeatureExtractionBaseController->GetVisuController());
    m_ResultVisuView->SetController(m_FeatureExtractionBaseController->GetResultVisuController());

    m_FeatureExtractionBaseController->GetModel()->RegisterListener(this);
  }

  /** Set the model and register the view to the model */
  /*  void SetFeatureExtractionBaseModel(FeatureExtractionBaseModel *pModel)
  {
    m_FeatureExtractionBaseModel = pModel;
    m_FeatureExtractionBaseModel->RegisterListener(this);
  }
*/
  itkGetMacro(VisuView, VisuViewPointerType);
  itkGetMacro(ResultVisuView, VisuViewPointerType);

  virtual void InitWidgets();
  void InitVisu();

  /** Standard Running macro */
  otbRunningMacro();

  /** Event from the model */
  virtual void Notify(const std::string& event);
  virtual void UpdateFeaturePreview();
  virtual void UpdateFeaturePreviewFromOutputList();
  virtual void UpdateFeatureInfo(FeatureType feat) {};
  virtual void UpdateParameterArea(unsigned int groupId);
  void Show();

  virtual void ClearFeature();
  virtual void UpdateChannels();

  /** Constructor */
  FeatureExtractionBaseViewGUI();

  /** InputOutputFeatureLink accessors */
  std::vector<int> GetInputOutputFeatureLink()
  {
    return m_InputOutputFeatureLink;
  }
  void AddToInputOutputFeatureLink(int i)
  {
    m_InputOutputFeatureLink.push_back(i);
  }

  virtual void UpdateSelectedPixel(const IndexType& index);
  
  virtual void UpdateSelectedPixelGUI(const IndexType& index) {};

  ModelInputPixelType GetSelectedPixel() const
  {
    return m_SelectedPixel;
  }

  virtual void SetFeatureType(FeatureType i);
  virtual void ClearImage();

  itkSetMacro(FeatureExtractPreviewParentBrowser, int);
  
  itkSetMacro(WindowTitle, std::string);

protected:

  /** Destructor */
  virtual ~FeatureExtractionBaseViewGUI();

  /** Center the widgets */
  virtual void AddOutputChannel();
  virtual void RemoveOutputChannel();
  virtual void UpOutputChannel();
  virtual void DownOutputChannel();
  virtual void Quit();
  virtual void Cancel();
  virtual void AddFeature();
  virtual void OK();

  virtual void InitParameterGroupList();
  
  /** Store a list avor all parameter selection group*/
  std::vector<Fl_Group*> m_ParameterGroupList;
  
  FeatureExtractionBaseModel* GetModel();

private:
  FeatureExtractionBaseViewGUI(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  void UpdateInformation();
  void UpdateChannelSelection();

  /** Pointer to the model */
  //FeatureExtractionBaseModel::Pointer               m_FeatureExtractionBaseModel;
  /** Pointer to the controller */
  FeatureExtractionBaseControllerInterface::Pointer m_FeatureExtractionBaseController;

  StringVectorType  m_FileNameList;
  StringVectorType  m_ViewerNameList;
  std::string       m_LastPath;
  std::string       m_TemplateViewerName;
  StringVectorType  m_DisplayedLabelList;
  StringVectorType  m_UndisplayedLabelList;
  std::vector<bool> m_DisplayStatusList;
  FeatureType       m_FeatureType;
  /** output/input feature list link. */
  std::vector<int> m_InputOutputFeatureLink;
  int              m_FeatureExtractPreviewParentBrowser;
  /** Value of the selected Pixel*/
  ModelInputPixelType m_SelectedPixel;
  /** Title of the module window */
  std::string m_WindowTitle;

  VisuViewPointerType m_VisuView;
  VisuViewPointerType m_ResultVisuView;
};
} //end namespace otb

#endif
