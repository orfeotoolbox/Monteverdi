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
#ifndef __otbSpeckleFilteringModule_h
#define __otbSpeckleFilteringModule_h

#include "otbModule.h"

// MVC from Speckle filtering application
#include "otbSpeckleFilteringView.h"
#include "otbSpeckleFilteringModel.h"
#include "otbSpeckleFilteringController.h"

namespace otb
{
/** \class SpeckleFilteringModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_ABI_EXPORT SpeckleFilteringModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef SpeckleFilteringModule        Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SpeckleFilteringModule, Module);

  itkGetObjectMacro(View, SpeckleFilteringView);

  // Input typedef
  typedef SpeckleFilteringModel::InputImageType InputImageType;

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    this->Run();
  }
protected:
  /** Constructor */
  SpeckleFilteringModule();
  /** Destructor */
  virtual ~SpeckleFilteringModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify();

private:
  SpeckleFilteringModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // The view
  SpeckleFilteringView::Pointer m_View;

  // The controller
  SpeckleFilteringController::Pointer m_Controller;

  // The model
  SpeckleFilteringModel::Pointer m_Model;
};

} // End namespace otb

#endif
