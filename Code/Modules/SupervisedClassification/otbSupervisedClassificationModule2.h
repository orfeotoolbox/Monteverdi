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
#ifndef __otbSupervisedClassificationModule2_h
#define __otbSupervisedClassificationModule2_h

// include the base class
#include "otbModule.h"

// the MVC classes
#include "otbSupervisedClassificationController.h"
#include "otbSupervisedClassificationModel.h"
#include "otbSupervisedClassificationModule2.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"

namespace otb
{
/** \class SupervisedClassificationModule2
 *
 *  \brief This is the SupervisedClassification module, which allows to perform
 *  SupervisedClassification filtering, segmentation and clustering.
 *
 *
 */

class ITK_EXPORT SupervisedClassificationModule2
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef SupervisedClassificationModule2  Self;
  typedef Module                           Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SupervisedClassificationModule2,Module);

  /** Data typedefs */
  /// Dataset
  typedef VectorImage<float,2>         FloatingVectorImageType;
  typedef VectorData<double,2>         VectorDataType;
  typedef VectorImage<float,2>                 LabelImageType;

  /** MVC typedefs */
  typedef otb::SupervisedClassificationController ControllerType;
  typedef otb::SupervisedClassificationModel      ModelType;
  typedef otb::SupervisedClassificationView       ViewType;

 itkGetObjectMacro(View,ViewType);
 itkGetObjectMacro(Controller,ControllerType);

protected:
  /** Constructor */
  SupervisedClassificationModule2();

  /** Destructor */
  virtual ~SupervisedClassificationModule2();

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
  }

  /** Show the Module GUI */
  virtual void Hide()
  {
    m_View->HideAll();
  }

private:
  SupervisedClassificationModule2(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  ControllerType::Pointer m_Controller;
  ViewType::Pointer       m_View;
  ModelType::Pointer      m_Model;
};


} // End namespace otb

#endif
