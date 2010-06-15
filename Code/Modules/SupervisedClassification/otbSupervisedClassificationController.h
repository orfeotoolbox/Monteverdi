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

#ifndef __otbSupervisedClassificationController_h
#define __otbSupervisedClassificationController_h

#include "otbSupervisedClassificationControllerInterface.h"
#include "otbSupervisedClassificationView.h"

namespace otb
{
class ITK_EXPORT SupervisedClassificationController
      : public SupervisedClassificationControllerInterface
{
public:
  /** Standard class typedefs */
  typedef SupervisedClassificationController          Self;
  typedef SupervisedClassificationControllerInterface Superclass;
  typedef itk::SmartPointer<Self>                     Pointer;
  typedef itk::SmartPointer<const Self>               ConstPointer;

  /** Standard type macros */
  itkTypeMacro(SupervisedClassificationController,Superclass)
  itkNewMacro(Self)

  void SetModel(ModelType* model)
  {
    m_Model = model;
  }

  const ModelType* GetModel() const
  {
    return this->m_Model;
  }

  /** Set the pointer to the view */
  void SetView(SupervisedClassificationView * view)
  {
    m_View = view;
  }

  /** Users actions */
  virtual void Train();
  virtual void Validate();
  virtual void SVMSetupOk();
  virtual void Quit();
  virtual void Ok()
  {
    m_Model->Ok();
  }
  virtual ConfusionMatrixType GetConfusionMatrix()
  {
    return m_Model->GetConfusionMatrix();
  }
  virtual double GetOverallAccuracy()
  {
    return m_Model->GetOverallAccuracy();
  }
  virtual double GetKappaIndex()
  {
    return m_Model->GetKappaIndex();
  }

protected:
 /** Constructor */
  SupervisedClassificationController() {/*TODO To implement*/};
 /** Destructor */
 virtual ~SupervisedClassificationController() {/*TODO To implement*/};

private:
  SupervisedClassificationController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  SupervisedClassificationView*               m_View;

  /** The instance of the model */
  ModelType *                                 m_Model;

};
} //end namespace otb

#endif
