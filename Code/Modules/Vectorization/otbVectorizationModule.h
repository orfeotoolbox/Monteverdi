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
#ifndef __otbVectorizationModule_h
#define __otbVectorizationModule_h

// include the base class
#include "otbModule.h"
#include "otbListenerBase.h"

// the MVC classes
#include "otbVectorizationModel.h"
#include "otbVectorizationView.h"
#include "otbVectorizationController.h"

// include the OTB/ITK elements

namespace otb
{
/** \class VectorizationModule
 *
 *  \brief This is the Vectorization module. It allows to perform generic vectorization
 *    of points, polylines and polygons over an image.
 */

class ITK_ABI_EXPORT VectorizationModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef VectorizationModule           Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(VectorizationModule, Module);

  /** Data typedefs */
  typedef TypeManager
  ::Floating_Point_VectorImage FloatingVectorImageType;
  typedef TypeManager
  ::Vector_Data VectorDataType;
  typedef VectorDataType::Pointer      VectorDataPointerType;

  /** MVC typedefs */
  typedef otb::VectorizationController ControllerType;
  typedef otb::VectorizationModel      ModelType;
  typedef otb::VectorizationView       ViewType;

  itkGetObjectMacro(View, ViewType);
  itkGetObjectMacro(Controller, ControllerType);
  itkGetObjectMacro(Model, ModelType);

protected:
  /** Constructor */
  VectorizationModule();

  /** Destructor */
  virtual ~VectorizationModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_View->Show();
  }

  /** Show the Module GUI */
  virtual void Hide()
  {
    m_View->HideAll();
  }


private:
  VectorizationModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // MVC members
  ControllerType::Pointer m_Controller;
  ViewType::Pointer       m_View;
  ModelType::Pointer      m_Model;
};

} // End namespace otb

#endif
