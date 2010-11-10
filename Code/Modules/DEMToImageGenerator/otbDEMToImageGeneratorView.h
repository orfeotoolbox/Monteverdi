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
#ifndef __otbDEMToImageGeneratorView_h
#define __otbDEMToImageGeneratorView_h

#include "otbListenerBase.h"

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbDEMToImageGeneratorGUI.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include  "itkObject.h"
#include  "itkObjectFactory.h"

#include "otbObjectList.h"
#include "otbVectorImage.h"
#include <FL/Fl_File_Chooser.H>

#include "otbDEMToImageGenerator.h"
#include "otbDEMToImageGeneratorControllerInterface.h"
#include "otbDEMToImageGeneratorModel.h"

// Remote Sensing Transformation
#include "otbGenericRSTransform.h"

// include the base class
#include "otbModule.h"

#include "otbProjectionEnum.h"

namespace otb
{
/** \class DEMToImageGeneratorView
 *  \brief
 *
 * \ingroup
 * \ingroup
 */
class ITK_EXPORT DEMToImageGeneratorView
  : public ListenerBase, public DEMToImageGeneratorGUI, public itk::Object
{
public:
  /** Standard typedefs */
  typedef DEMToImageGeneratorView                Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(DEMToImageGeneratorView, itk::Object);

  /** typedef projection model*/
  typedef DEMToImageGeneratorModel   ModelType;
  typedef ModelType::InputImageType  ImageType;
  typedef ModelType::TransformType   TransformType;
  typedef ModelType::OutputPointType PointType;
  typedef ModelType::IndexType       IndexType;
  typedef ModelType::SingleImageType SingleImageType;

  // Initialize transform and show the GUI
  virtual void Show();
  //  Shpw the GUI
  virtual void ShowGUI();

  // Method to set the controller
  itkGetObjectMacro(Controller, DEMToImageGeneratorControllerInterface);
  itkSetObjectMacro(Controller, DEMToImageGeneratorControllerInterface);

  // Set the controller
  void SetDEMToImageGeneratorController(DEMToImageGeneratorControllerInterface::Pointer pController)
  {
    m_Controller = pController;
  }

  // Hide the GUI
  virtual void Hide();

protected:

  /** Browser SRTM DEM */
  virtual void BrowseSRTMDEM();

  /** Callback for Launching the DEMToImageGenerator & Quit the application */
  virtual void SaveAndQuit();

  /** Callback for the cancel button (Quit the application) */
  virtual void Cancel();

  /** Update the projection Ref. text zone */
  virtual void UpdateProjectionRef();

  virtual void UpdateOutputParameters();
  
  /** Callback for showing the Azimut and elevation angle  area*/
  virtual void UpdateHillShading();

  /** Model Notificatin catcher*/
  virtual void Notify();


  /** Constructor */
  DEMToImageGeneratorView();
  /** Destructor */
  virtual ~DEMToImageGeneratorView()
  {
    delete m_projectionRefBuffer;
  }

private:
  DEMToImageGeneratorView(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Flag to determine if there is an output
  bool m_HasOutput;

  // Flag to determine if the input DEMToImageGenerator Map is unknowm
  bool m_InputImageUnknown;

  // Controller instance
  DEMToImageGeneratorControllerInterface::Pointer m_Controller;

  /** Projection reference buffer for the Text Display widget*/
  Fl_Text_Buffer * m_projectionRefBuffer;
};

} // End namespace otb

#endif
