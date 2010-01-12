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
#ifndef __otbOpticCalibration_h
#define __otbOpticCalibration_h

#include "otbEventsListener.h"

// include the base class
#include "otbModule.h"

// the MVC classes
#include "otbOpticCalibrationController.h"
#include "otbOpticCalibrationModel.h"
#include "otbOpticCalibrationModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"

namespace otb
{
/** \class OpticCalibrationModule
 *
 *  \brief This is the OpticCalibration module, which allows to perform
 *  OpticCalibration (Luminance, TOA, TOC) for Spot4, Spot5, and QuickBird.
 *
 *
 */

class ITK_EXPORT OpticCalibrationModule
  : public Module, public EventsListener<std::string>
{
public:
  /** Standard class typedefs */
  typedef OpticCalibrationModule        Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(OpticCalibrationModule,Module);

  /** Data typedefs */
  /// Dataset
  typedef VectorImage<double,2>         FloatingVectorImageType;
  typedef Image<double,2>               FloatingImageType;
  typedef Image<unsigned short,2>       LabelImageType;

  /** MVC typedefs */
  typedef otb::OpticCalibrationController ControllerType;
  typedef otb::OpticCalibrationModel      ModelType;
  typedef otb::OpticCalibrationView       ViewType;

  itkGetObjectMacro(View, ViewType);
  itkGetObjectMacro(Model, ModelType);
  itkGetObjectMacro(Controller, ControllerType);

protected:
  /** Constructor */
  OpticCalibrationModule();

  /** Destructor */
  virtual ~OpticCalibrationModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify(const std::string & event);
  
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();
  
    /** Show the Module GUI */
  virtual bool CanShow(){return true;};

  /** Show the Module GUI */
  virtual void Show()
    {
      m_View->Show();
    };

  /** Hide the Module GUI */
  virtual void Hide()
    {
      m_View->HideAll();
    };

private:
  OpticCalibrationModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  ControllerType::Pointer m_Controller;
  ViewType::Pointer m_View;
  ModelType::Pointer m_Model;
};


} // End namespace otb

#endif
