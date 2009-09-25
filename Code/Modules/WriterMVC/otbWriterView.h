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
#ifndef __otbWriterView_h
#define __otbWriterView_h

#include "otbListenerBase.h"
#include <FL/Fl_Group.H>

// include the GUI
#include "otbWriterMVCModuleGUI.h"

// include the OTB elements
#include "otbWriterControllerInterface.h"
#include "otbWriterModel.h"
#include "itkObject.h"

#include "itkMacro.h"

namespace otb
{
/** \class WriterView
 *  \brief 
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT WriterView
  : public ListenerBase, public WriterModuleGUI, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef WriterView                 Self;
  typedef itk::Object                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standards macros */
  itkNewMacro(Self);
  itkTypeMacro(WriterView,itk::Object);

 typedef double PixelType;
 typedef WriterModel   WriterModelType;
  
 /** Constructor */
 WriterView();
 
 /** Standard Running macro */
 otbRunningMacro();
 /** Set the controller */
 itkGetObjectMacro(WriterController,WriterControllerInterface);

 void SetWriterController(WriterControllerInterface *pController)
 {
   m_WriterController = pController;
   // Set the model
//    m_VisuView->SetController(m_WriterController->GetVisuController());
//    m_ResultVisuView->SetController(m_WriterController->GetResultVisuController());
 }
 
 /** Event from the model */
 virtual void Notify();
 
 virtual void Show();

protected:
  
  /** Destructor */
  virtual ~WriterView();

  /** Callbacks */
  virtual void OpenDataSet();
  virtual void Browse();
  virtual void Cancel();

private:
  WriterView(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the model */
  WriterModel::Pointer m_WriterModel;
  /** Pointer to the controller */
  WriterControllerInterface::Pointer m_WriterController;
};
} // End namespace otb

#endif
