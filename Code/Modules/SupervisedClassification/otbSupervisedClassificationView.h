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
#ifndef __otbSupervisedClassificationView_h
#define __otbSupervisedClassificationView_h

#include "otbSupervisedClassificationGUI.h"

#include "otbListenerBase.h"
#include "otbSupervisedClassificationModel.h"
#include "otbSupervisedClassificationControllerInterface.h"

namespace otb
{
/** \class SupervisedClassificationView
 *
 */
class ITK_EXPORT SupervisedClassificationView
      : public ListenerBase,
        public SupervisedClassificationGUI,
        public itk::Object
{
public:
  /** Standard class typedefs */
  typedef SupervisedClassificationView         Self;
  typedef itk::Object                          Superclass;
  typedef itk::SmartPointer<Self>              Pointer;
  typedef itk::SmartPointer<const Self>        ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(SupervisedClassificationView,Object);


  /** Event from the model */
  virtual void Notify();

  /** Fluid call backs*/
  virtual void Quit();

  /** Set the controller */
  itkSetObjectMacro(Controller,SupervisedClassificationControllerInterface);
  itkGetObjectMacro(Controller,SupervisedClassificationControllerInterface);

  /** Build the interface */
  virtual void BuildInterface();

  virtual void SVMSetup();
  virtual void SVMSetupOk();
  virtual void SVMSetupCancel();
  virtual void Train();
  virtual void Validate();
  virtual void HideAll();
  virtual void Show();
  virtual void Ok();
  virtual void Cancel();


protected:
  /** Constructor */
  SupervisedClassificationView() {};

  /** Destructor */
  virtual ~SupervisedClassificationView() {};

  /** Clean up before leaving*/
  virtual void CleanUp();

private:
  SupervisedClassificationView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the controller */
  SupervisedClassificationControllerInterface::Pointer  m_Controller;

};
} //end namespace otb

#endif
