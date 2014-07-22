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
#ifndef __otbDEMToImageGeneratorModule_h
#define __otbDEMToImageGeneratorModule_h

#include "otbModule.h"

// MVC from Speckle filtering application
#include "otbDEMToImageGeneratorView.h"
#include "otbDEMToImageGeneratorModel.h"
#include "otbDEMToImageGeneratorController.h"

namespace otb
{
/** \class DEMToImageGeneratorModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_ABI_EXPORT DEMToImageGeneratorModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef DEMToImageGeneratorModule     Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Type macro */
  itkTypeMacro(DEMToImageGeneratorModule, Module);


  /** New macro */
  itkNewMacro(Self);

  // Get the view
  itkGetObjectMacro(View, DEMToImageGeneratorView);

  // Input typedef
  typedef DEMToImageGeneratorModel::InputImageType InputImageType;

protected:
  /** Constructor */
  DEMToImageGeneratorModule();
  /** Destructor */
  virtual ~DEMToImageGeneratorModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify();

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  void Show()
  {
    m_View->ShowGUI();
  }

  /** Hide the Module GUI */
  void Hide()
  {
    m_View->Hide();
  }

private:
  DEMToImageGeneratorModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // The view
  DEMToImageGeneratorView::Pointer m_View;

  // The controller
  DEMToImageGeneratorController::Pointer m_Controller;

  // The model
  DEMToImageGeneratorModel::Pointer m_Model;
};

} // End namespace otb

#endif
