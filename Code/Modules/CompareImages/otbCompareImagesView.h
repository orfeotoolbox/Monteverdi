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

#ifndef __otbCompareImagesView_h
#define __otbCompareImagesView_h

#include "otbListenerBase.h"

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include "otbCompareImagesGUI.h"
#include "otbCompareImagesControllerInterface.h"
#include  "itkObjectFactory.h"
#include  "itkObject.h"

#include "otbCompareImagesModel.h"

#include "otbImageView.h"
#include "otbPixelDescriptionView.h"

//#include "otbImageView.h"
#include <FL/Fl_Group.H>

namespace otb
{
/** \class CompareImagesView
 *
 */
class ITK_ABI_EXPORT CompareImagesView
  : public EventsListener<std::string>, public CompareImagesGUI, public itk::Object
{
public:
  /** Standard typedefs */
  typedef CompareImagesView          Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  typedef CompareImagesModel::VisuModelType VisuModelType;
  typedef CompareImagesModel::RGBPixelType PixelType;
  typedef CompareImagesModel::PixelDescriptionModelType PixelDescriptionModelType;
  typedef ImageView<VisuModelType>  VisuViewType;

  /** Pixel description */
  typedef otb::PixelDescriptionView<PixelDescriptionModelType>                     PixelDescriptionViewType;
  typedef PixelDescriptionViewType::Pointer                                        PixelDescriptionViewPointerType;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(CompareImagesView, itk::Object);

  typedef CompareImagesControllerInterface::Pointer ControllerPointerType;

  // Update the display
  virtual void Notify(const std::string& event);
  // Show Method
  virtual void Show();
  // Quit method
  virtual void Quit();
  // Save/Quit method
  virtual void SaveQuit();
  // Hide Method
  virtual void Hide();
  
  void UpdateStats();
  void InitVisu();
  void Process();
  
  // Method to set the controller
  itkGetObjectMacro(Controller, CompareImagesControllerInterface);
  //itkSetObjectMacro(Controller, CompareImagesControllerInterface);
  void SetController(CompareImagesControllerInterface *pController)
  {
    m_Controller = pController;
    // Set the model
    m_LeftVisuView->SetController(m_Controller->GetLeftVisuController());
    m_RightVisuView->SetController(m_Controller->GetRightVisuController());

    m_Controller->GetModel()->RegisterListener(this);
  }
  
  // Get a pointer to the view parts for the controller
  itkGetObjectMacro(LeftVisuView, VisuViewType);
  itkGetObjectMacro(RightVisuView, VisuViewType);
  

  
protected:
  /** Constructor */
  CompareImagesView();
  /** Destructor */
  virtual ~CompareImagesView(){}
  /** PrintSelf*/
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  CompareImagesView(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
  CompareImagesModel* GetModel();
  
  /** Pointer to the controller */
  ControllerPointerType m_Controller;

  /** Image view */
  VisuViewType::Pointer m_LeftVisuView;
  VisuViewType::Pointer m_RightVisuView;

  /** The pixel description view */
  PixelDescriptionViewPointerType m_LeftPixelDescriptionView;
  PixelDescriptionViewPointerType m_RightPixelDescriptionView;

/*
  // Pointer to the widgets controller
  ImageWidgetController::Pointer m_LeftWidgetsController;
  ImageWidgetController::Pointer m_RightWidgetsController;

  // Pointer to the model
  CompareImagesModel::Pointer m_Model; */

  

};
} //end namespace otb

#endif
