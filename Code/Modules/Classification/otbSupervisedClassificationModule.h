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
  : public Module
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
  
protected:
  /** Constructor */
  SupervisedClassificationModule();
  /** Destructor */
  virtual ~SupervisedClassificationModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Assign input by key. This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual void AssignDataByKey(const std::string & key, const DataObjectWrapper & data);

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual const DataObjectWrapper RetrieveDataByKey(const std::string & key) const;

  /** The custom run command */
  virtual void Run();

private:
  SupervisedClassificationModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The view
  SupervisedClassificationAppli::Pointer        m_SupervisedClassification;

};


} // End namespace otb

#endif
