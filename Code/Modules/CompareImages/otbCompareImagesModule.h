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
#ifndef __otbCompareImagesModule_h
#define __otbCompareImagesModule_h

#include "otbEventsListener.h"

// include the base class
#include "otbModule.h"

// the MVC classes
#include "otbCompareImagesController.h"
#include "otbCompareImagesModel.h"
#include "otbCompareImagesView.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"

namespace otb
{
/** \class CompareImagesModule
 *
 *  \brief This module computes PSNR RMSE and Max error between two images
 *
 *
 *
 */

class ITK_EXPORT CompareImagesModule
  : public Module, public EventsListener<std::string>
{
public:
  /** Standard class typedefs */
  typedef CompareImagesModule           Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(CompareImagesModule, Module);

  /** MVC typedefs */
  typedef otb::CompareImagesController ControllerType;
  typedef otb::CompareImagesModel      ModelType;
  typedef otb::CompareImagesView       ViewType;

  /** Data typedefs */
  typedef ModelType::InputImageType        InputImageType;

  itkGetObjectMacro(View, CompareImagesView);


  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    this->Run();
  }
protected:
  /** Constructor */
  CompareImagesModule();

  /** Destructor */
  virtual ~CompareImagesModule();

  /** Notify Monteverdi application that CompareImages has a result */
  void Notify(const std::string& event);

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();



private:
  CompareImagesModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ControllerType::Pointer m_Controller;
  ViewType::Pointer       m_View;
  ModelType::Pointer      m_Model;
};

} // End namespace otb

#endif
