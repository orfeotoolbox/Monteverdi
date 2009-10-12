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
#ifndef __otbFeatureExtractionViewGUI_h
#define __otbFeatureExtractionViewGUI_h


#include "otbListenerBase.h"

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbFeatureExtractionViewGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <FL/Fl_Group.H>

#include "otbFeatureExtractionModel.h"
#include "otbFeatureExtractionControllerInterface.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbImageViewerBase.h"
#include "otbObjectList.h"
#include "otbImageView.h"

namespace otb
{
/** \class FeatureExtractionViewGUI
 *
 */
class ITK_EXPORT FeatureExtractionViewGUI
      : public ListenerBase, public FeatureExtractionViewGroup, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef FeatureExtractionViewGUI          Self;
  typedef itk::Object   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(FeatureExtractionViewGUI,itk::Object);


  // Pixel type
  typedef double PixelType;

  typedef FeatureExtractionModel                        FeatureExtractionModelType;
  typedef FeatureExtractionModel::InputImagePixelType   ModelInputPixelType;
  typedef FeatureExtractionModelType::VisuModelType     VisuModelType;
  typedef ImageView<VisuModelType>                      VisuViewType;
  typedef FeatureExtractionControllerInterface::Pointer FeatureExtractionControllerInterfacePointerType;
  typedef FeatureExtractionModelType::SingleImageType   ModelSingleImageType;
  typedef VisuViewType::ImageType                       ImageType;
  typedef VisuViewType::Pointer                         VisuViewPointerType;
  typedef ImageType::RegionType                         RegionType;
  typedef RegionType::SizeType                          SizeType;
  typedef RegionType::IndexType                         IndexType;
  typedef ImageType::Pointer                            ImagePointerType;
  typedef FeatureInfo::FeatureType                      FeatureType;

  typedef std::vector<std::string>                      StringVectorType;


  /** Set the controller */
  itkGetObjectMacro(FeatureExtractionController,FeatureExtractionControllerInterface);

  void SetFeatureExtractionController(FeatureExtractionControllerInterface *pController)
  {
    m_FeatureExtractionController = pController;
   // Set the model
    m_VisuView->SetController(m_FeatureExtractionController->GetVisuController());
    m_ResultVisuView->SetController(m_FeatureExtractionController->GetResultVisuController());
  }

  /** Set the model and register the view to the model */
  void SetFeatureExtractionModel(FeatureExtractionModel *pModel)
  {
    m_FeatureExtractionModel = pModel;
    m_FeatureExtractionModel->RegisterListener(this);
  }

  itkGetMacro(VisuView,VisuViewPointerType);
  itkGetMacro(ResultVisuView,VisuViewPointerType);

  void InitWidgets();
  void InitVisu();

  /** Standard Running macro */
  otbRunningMacro();

  /** Event from the model */
  virtual void Notify();
  virtual void UpdateFeaturePreview();
  virtual void UpdateFeaturePreviewFromOutputList();
  virtual void UpdateFeatureInfo(FeatureType feat);
  virtual void UpdateParameterArea(unsigned int groupId);
  void Show();

  virtual void ClearFeature();
  virtual void UpdateChannels();

  /** Constructor */
  FeatureExtractionViewGUI();

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

  virtual void SetFeatureType(FeatureType i);
  virtual void ClearImage();

protected:

  /** Destructor */
  virtual ~FeatureExtractionViewGUI();

  /** Center the widgets */
  virtual void OpenImage();
  virtual void AddOutputChannel();
  virtual void RemoveOutputChannel();
  virtual void UpOutputChannel();
  virtual void DownOutputChannel();
  virtual void Quit();
  virtual void AddFeature();
  virtual void OK();


  void InitParameterGroupList();


private:
  FeatureExtractionViewGUI(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  void UpdateInformation();
  void UpdateChannelSelection();

  /** Pointer to the model */
  FeatureExtractionModel::Pointer m_FeatureExtractionModel;
  /** Pointer to the controller */
  FeatureExtractionControllerInterface::Pointer m_FeatureExtractionController;

  StringVectorType         m_FileNameList;
  StringVectorType         m_ViewerNameList;
  std::string              m_LastPath;
  std::string              m_TemplateViewerName;
  StringVectorType         m_DisplayedLabelList;
  StringVectorType         m_UndisplayedLabelList;
  std::vector<bool>        m_DisplayStatusList;
  FeatureType              m_FeatureType;
  /** output/input feature list link. */
  std::vector<int> m_InputOutputFeatureLink;
  int m_FeatureExtractPreviewParentBrowser;
  /** Value of the selected Pixel*/
  ModelInputPixelType m_SelectedPixel;
  /** Store a list avor all parameter selection group*/
  std::vector<Fl_Group*> m_ParameterGroupList;

  VisuViewPointerType                 m_VisuView;
  VisuViewPointerType                 m_ResultVisuView;
};
}//end namespace otb

#endif
