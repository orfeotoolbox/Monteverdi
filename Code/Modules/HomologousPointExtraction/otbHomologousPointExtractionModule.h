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
#ifndef __otbHomologousPointExtractionModule_h
#define __otbHomologousPointExtractionModule_h

// include the base class
#include "otbModule.h"

// the MVC classes
#include "otbHomologousPointExtractionModuleController.h"
#include "otbHomologousPointExtractionModuleModel.h"
#include "otbHomologousPointExtractionModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"

namespace otb
{
/** \class HomologousPointExtractionModule
 *
 *  \brief This is the HomologousPointExtraction module, which allows to perform
 *  HomologousPointExtraction filtering, segmentation and clustering.
 *
 *
 */

class ITK_EXPORT HomologousPointExtractionModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef HomologousPointExtractionModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(HomologousPointExtractionModule,Module);

  /** Data typedefs */
  /// Dataset
  typedef VectorImage<double,2>         FloatingVectorImageType;

  /** MVC typedefs */
  typedef otb::HomologousPointExtractionModuleController ControllerType;
  typedef otb::HomologousPointExtractionModuleModel      ModelType;
  typedef otb::HomologousPointExtractionModuleView       ViewType;

 itkGetObjectMacro(View,HomologousPointExtractionModuleView);

protected:
  /** Constructor */
  HomologousPointExtractionModule();

  /** Destructor */
  virtual ~HomologousPointExtractionModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify();
  
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

  /** Show the Module GUI */
  virtual void Hide()
  {
    m_View->HideAll();
  };

private:
  HomologousPointExtractionModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  ControllerType::Pointer m_Controller;
  ViewType::Pointer m_View;
  ModelType::Pointer m_Model;
};


} // End namespace otb

#endif
