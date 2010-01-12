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
#ifndef __otbProjectionModule_h
#define __otbProjectionModule_h

#include "otbModule.h"

// MVC from Speckle filtering application
#include "otbProjectionView.h"
#include "otbProjectionModel.h"
#include "otbProjectionController.h"

namespace otb
{
/** \class ProjectionModuleModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT ProjectionModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef ProjectionModule        Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

    /** Type macro */
  itkTypeMacro(ProjectionModule,Module);

  /** New macro */
  itkNewMacro(Self);

  // Get the view
  itkGetObjectMacro(View,ProjectionView);

  // Input typedef
  typedef ProjectionModel::InputImageType InputImageType;

protected:
  /** Constructor */
  ProjectionModule();
  /** Destructor */
  virtual ~ProjectionModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify();
  
  /** Show the Module GUI */
  virtual bool CanShow(){return true;};

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
  ProjectionModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The view
  ProjectionView::Pointer        m_View;

  // The controller
  ProjectionController::Pointer  m_Controller;

  // The model
  ProjectionModel::Pointer       m_Model;
};


} // End namespace otb

#endif
