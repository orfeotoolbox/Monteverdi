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
#ifndef __otbFeatureExtractionModule_h
#define __otbFeatureExtractionModule_h

#include "otbModule.h"

// MVC from feature extraction application
#include "otbFeatureExtractionViewGUI.h"
#include "otbFeatureExtractionModel.h"
#include "otbFeatureExtractionController.h"

// #include "otbListenerBase.h"
#include "otbEventsListener.h"

namespace otb
{
/** \class FeatureExtractionModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT FeatureExtractionModule
  : public Module, public EventsListener<std::string>
    //   : public Module, public ListenerBase
  
{
public:
  /** Standard class typedefs */
  typedef FeatureExtractionModule         Self;
  typedef Module                          Superclass;
  typedef itk::SmartPointer<Self>         Pointer;
  typedef itk::SmartPointer<const Self>   ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(FeatureExtractionModule,Module);

  itkGetObjectMacro(View,FeatureExtractionViewGUI);
  
  // Input image typedef
  typedef FeatureExtractionModel::InputImageType InputImageType;


protected:
  /** Constructor */
  FeatureExtractionModule();
  /** Destructor */
  virtual ~FeatureExtractionModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify(const std::string & event);

private:
  FeatureExtractionModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The view
  FeatureExtractionViewGUI::Pointer        m_View;

  // The controller
  FeatureExtractionController::Pointer  m_Controller;

  // The model
  FeatureExtractionModel::Pointer       m_Model;
};


} // End namespace otb

#endif
