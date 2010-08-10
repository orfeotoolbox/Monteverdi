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
#ifndef __otbObjectLabelingModule_h
#define __otbObjectLabelingModule_h

// include the base class
#include "otbModule.h"

#include "otbObjectLabelingApplicationModel.h"
#include "otbObjectLabelingApplicationView.h"
#include "otbObjectLabelingApplicationController.h"
#include "otbEventsListener.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"
#include "itkCastImageFilter.h"

namespace otb
{
/** \class ObjectLabelingModule
 *
 *  \brief This is the ObjectLabeling module, which allows to perform
 *  ObjectLabeling filtering, segmentation and clustering.
 *
 *
 */

class ITK_EXPORT ObjectLabelingModule
  : public Module, public EventsListener<std::string>
{
public:
  /** Standard class typedefs */
  typedef ObjectLabelingModule          Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ObjectLabelingModule,Module);

  /** MVC typedefs */
  typedef otb::ObjectLabelingApplicationController ControllerType;
  typedef otb::ObjectLabelingApplicationModel      ModelType;
  typedef otb::ObjectLabelingApplicationView       ViewType;

  /** Data types */
  typedef ModelType::ImageType             ImageType;
  typedef ModelType::VectorImageType       VectorImageType;
  typedef TypeManager::Labeled_Short_Image LabeledImageType;

  /** Cast filter type */
  typedef itk::CastImageFilter<LabeledImageType, ImageType> CasterFilterType;

  /** Accessor to the view*/
  itkGetObjectMacro(View, ViewType);
  
  /** Accessor to the Controller : needed to test the right clicks */
  itkGetObjectMacro(Model, ModelType);
  
protected:
  /** Constructor */
  ObjectLabelingModule();

  /** Destructor */
  virtual ~ObjectLabelingModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify(const std::string & event);
  
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();
  
private:
  ObjectLabelingModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  ControllerType::Pointer m_Controller;
  ViewType::Pointer m_View;
  ModelType::Pointer m_Model;

  CasterFilterType::Pointer m_Caster;
};


} // End namespace otb

#endif
