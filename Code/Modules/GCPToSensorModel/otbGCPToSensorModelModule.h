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
#ifndef __otbGCPToSensorModelModule_h
#define __otbGCPToSensorModelModule_h

// include the base class
#include "otbModule.h"

// the MVC classes
#include "otbGCPToSensorModelController.h"
#include "otbGCPToSensorModelModel.h"
#include "otbGCPToSensorModelModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"

namespace otb
{
/** \class GCPToSensorModelModule
 *
 *  \brief This is the GCPToSensorModel module, which allows to perform
 *  GCPToSensorModel filtering, segmentation and clustering.
 *
 *
 */

class ITK_EXPORT GCPToSensorModelModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef GCPToSensorModelModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(GCPToSensorModelModule,Module);

  /** Data typedefs */
  /// Dataset
  typedef VectorImage<double,2>         FloatingVectorImageType;

  /** MVC typedefs */
  typedef otb::GCPToSensorModelController ControllerType;
  typedef otb::GCPToSensorModelModel      ModelType;
  typedef otb::GCPToSensorModelView       ViewType;

 itkGetObjectMacro(View,GCPToSensorModelView);

protected:
  /** Constructor */
  GCPToSensorModelModule();

  /** Destructor */
  virtual ~GCPToSensorModelModule();

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
  GCPToSensorModelModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  ControllerType::Pointer m_Controller;
  ViewType::Pointer m_View;
  ModelType::Pointer m_Model;
};


} // End namespace otb

#endif
