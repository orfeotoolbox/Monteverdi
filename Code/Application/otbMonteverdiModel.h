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
#ifndef __otbMonteverdiModel_h
#define __otbMonteverdiModel_h

#include "itkObject.h"
#include "otbModule.h"

// There are function prototype conflits under cygwin between standard w32 API
// and standard C ones
#ifndef CALLBACK
#if defined(_WINDOWS) || defined(__CYGWIN__)
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#endif


namespace otb
{

class ITK_EXPORT MonteverdiModel
  : public itk::Object
{
 public:
  /** Standard class typedefs */
  typedef MonteverdiModel                                 Self;
  typedef itk::Object                                     Superclass;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;

  /** Standard type macro */
  itkTypeMacro(MonteverdiModel, itk::Object);
  
  /** Standard new macro */
  itkNewMacro(Self);
  
  /** typedef of the Module constructors function */
  typedef otb::Module::Pointer (CALLBACK * ConstructorPointerType)();
  typedef std::map<std::string,ConstructorPointerType> ModuleConstructorMapType;
  
  /** Register a new module with its associated name */
  template <class T> void RegisterModule(const std::string & name, const std::string menupath = "Module/")
  {
    if(m_ModuleConstructorMap.count(name)>0)
      {
      itkExceptionMacro(<<"A module named "<<name<<" is already registered"); 
      }
    m_ModuleConstructorMap[name]=(ConstructorPointerType)T::New;
  }

  /** Create a module according to its name. If the name is not a
  registered module, throws an exception */
  otb::Module::Pointer CreateModuleByName(const std::string & name) const;

 protected:
  /** Constructor */
  MonteverdiModel();

  /** Destructor */
  virtual ~MonteverdiModel();

 private:
  MonteverdiModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Constructors map */
  ModuleConstructorMapType m_ModuleConstructorMap;

};

}
#endif


