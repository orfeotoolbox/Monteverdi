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
#ifndef __otbVectorDataTransformModule_h
#define __otbVectorDataTransformModule_h

// include the base class
#include "otbModule.h"

#include "otbVectorDataTransformModel.h"
#include "otbVectorDataTransformView.h"
#include "otbVectorDataTransformController.h"
#include "otbImageToVectorImageCastFilter.h"

namespace otb
{
/** \class VectorDataTransformModule
 *
 *  \brief
 *
 */

class ITK_EXPORT VectorDataTransformModule
  : public Module, public EventsListener<std::string>
{
public:
  /** Standard class typedefs */
  typedef VectorDataTransformModule     Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(VectorDataTransformModule, Module);

  /** MVC typedefs */
  typedef otb::VectorDataTransformController ControllerType;
  typedef otb::VectorDataTransformModel      ModelType;
  typedef otb::VectorDataTransformView       ViewType;

  /** Data types */
  typedef ModelType::VectorImageType         VectorImageType;
  typedef ModelType::ImageType               ImageType;
  typedef ModelType::VectorDataType          VectorDataType;
  
  /** Cast Image Filter */
  typedef ImageToVectorImageCastFilter<ImageType,
                                       VectorImageType>  CastImageFilter;

  /** Get the view */
  itkGetObjectMacro(View, ViewType);
  
protected:
  /** Constructor */
  VectorDataTransformModule();

  /** Destructor */
  virtual ~VectorDataTransformModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify(const std::string & event);
  
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){ return true; }
  
  /** Show the Module GUI */
  void Show()
  {
    m_View->Show();
  }

private:
  VectorDataTransformModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ControllerType::Pointer  m_Controller;
  ViewType::Pointer        m_View;
  ModelType::Pointer       m_Model;

  CastImageFilter::Pointer m_CastImageFilter;
};


} // End namespace otb

#endif
