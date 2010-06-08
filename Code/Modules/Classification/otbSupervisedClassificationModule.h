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
#ifndef __otbSupervisedClassificationModule_h
#define __otbSupervisedClassificationModule_h

#include "otbModule.h"

// SupervisedClassification Application
#include "otbSupervisedClassificationAppli.h"

namespace otb
{
/** \class SupervisedClassificationModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT SupervisedClassificationModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef SupervisedClassificationModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SupervisedClassificationModule,Module);
    

  /** Accessor to the SupervisedClassification Instance */
  itkGetObjectMacro(SupervisedClassification,SupervisedClassificationAppli);
  /** Get the SVM model if any*/
  itkSetMacro(Model,std::string);

  // Input typedef
  typedef SupervisedClassificationAppli::ImageType       InputImageType;
  typedef SupervisedClassificationAppli::OutputImageType OutputImageType;
  typedef SupervisedClassificationAppli::ClassesMapType  ClassesMapType;

protected:
  /** Constructor */
  SupervisedClassificationModule();
  /** Destructor */
  virtual ~SupervisedClassificationModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Notify Monteverdi application that supervised classification has a result */
  void Notify();

private:
  SupervisedClassificationModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The view
  SupervisedClassificationAppli::Pointer        m_SupervisedClassification;
  
  // The SVM Model
  std::string                                   m_Model;
  // store labels
  std::vector< std::string >                    m_LabelsList;

};


} // End namespace otb

#endif
