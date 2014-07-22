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
#ifndef __otbFeatureExtractionBaseModule_h
#define __otbFeatureExtractionBaseModule_h

#include "otbModule.h"

// MVC from feature extraction application
#include "otbFeatureExtractionBaseViewGUI.h"
#include "otbFeatureExtractionBaseModel.h"
#include "otbFeatureExtractionBaseController.h"

// #include "otbListenerBase.h"
#include "otbEventsListener.h"

namespace otb
{
/** \class FeatureExtractionBaseModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT FeatureExtractionBaseModule
  : public Module, public EventsListener<std::string>
  //   : public Module, public ListenerBase

{
public:
  /** Standard class typedefs */
  typedef FeatureExtractionBaseModule       Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(FeatureExtractionBaseModule, Module);

  itkGetObjectMacro(View, FeatureExtractionBaseViewGUI);

  // Input image typedef
  typedef FeatureExtractionBaseModel::InputImageType InputImageType;

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_View->Show();
  }

protected:
  /** Constructor */
  FeatureExtractionBaseModule();
  /** Destructor */
  virtual ~FeatureExtractionBaseModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify(const std::string& event);
  
  // The view
  FeatureExtractionBaseViewGUI::Pointer m_View;

  // The controller
  FeatureExtractionBaseController::Pointer m_Controller;

  // The model
  FeatureExtractionBaseModel::Pointer m_Model;

private:
  FeatureExtractionBaseModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};

} // End namespace otb

#endif
