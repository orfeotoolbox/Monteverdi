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
#ifndef __otbOrthoRectifModule_h
#define __otbOrthoRectifModule_h

#include "otbModule.h"

// OrthoRectif Application 
#include "otbOrthoRectif.h"

namespace otb
{
/** \class OrthoRectifModule
 *  \brief 
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT OrthoRectifModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef OrthoRectifModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(OrthoRectifModule,Module);
  

  /** Accessor to the OrthoRectif Instance */
  itkGetObjectMacro(OrthoRectif,OrthoRectif);
  
protected:
  /** Constructor */
  OrthoRectifModule();
  /** Destructor */
  virtual ~OrthoRectifModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Assign input by key. This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual void AssignInputByKey(const std::string & key, const DataObjectWrapper & data);

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual const DataObjectWrapper RetrieveOutputByKey(const std::string & key) const;

  /** The custom run command */
  virtual void Run();

private:
  OrthoRectifModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The view
  OrthoRectif::Pointer        m_OrthoRectif;

};


} // End namespace otb

#endif
