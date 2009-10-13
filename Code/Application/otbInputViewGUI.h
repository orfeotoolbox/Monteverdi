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
#ifndef __otbInputViewGUI_h
#define __otbInputViewGUI_h


// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbInputViewGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif


#include "itkObject.h"
#include "itkObjectFactory.h"
#include <FL/Fl_Choice.H>
#include <FL/Fl_Browser.H>
#include "otbMonteverdiModel.h"
#include "otbMonteverdiControllerInterface.h"
#include "otbInputViewComponent.h"
#include "otbAsynchronousProcessBase.h"

namespace otb
{
/** \class InputViewGUI
 *
 */
class ITK_EXPORT InputViewGUI
  : public InputViewGroup, public AsynchronousProcessBase
{
public:
  /** Standard class typedefs */
  typedef InputViewGUI                  Self;
  typedef AsynchronousProcessBase       Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macros */
  itkNewMacro(Self);
  itkTypeMacro(InputViewGUI,Object);


  typedef Module::OutputDataDescriptorMapType             OutputDataDescriptorMapType;
  typedef Module::InputDataDescriptorMapType              InputDataDescriptorMapType;

  typedef InputViewComponent::StringPairType           StringPairType;
  typedef InputViewComponent::Pointer                  InputViewComponentPointerType;

  // map containing the key and the descriptor
  typedef std::map<std::string,InputViewComponentPointerType> InputViewComponentMapType;

  /** Getters/Setters */
  itkGetObjectMacro(Model,MonteverdiModel);
  itkSetObjectMacro(Model,MonteverdiModel);
  itkGetObjectMacro(Controller,MonteverdiControllerInterface);
  itkSetObjectMacro(Controller,MonteverdiControllerInterface);
  itkGetMacro(ModuleInstanceId,std::string);
  itkSetMacro(ModuleInstanceId,std::string);

  /** Returns the component map */
  const InputViewComponentMapType & GetInputViewComponentMap() const
  {
    return m_InputViewComponentMap;
  }

  void Show();
  void BuildInputInterface();

protected:
  /** Constructor */
  InputViewGUI();
  /** Destructor */
  virtual ~InputViewGUI();

  /** Callbacks */
  virtual void Ok();
  virtual void Cancel();

  /** This is the watching process for the whole set of inputs */
  virtual void RunProcess1(void *v);

private:
  InputViewGUI(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  
  MonteverdiModel::Pointer                m_Model;
  itk::WeakPointer<MonteverdiControllerInterface> m_Controller;
  std::string                             m_ModuleInstanceId;
  InputViewComponentMapType               m_InputViewComponentMap;
  bool                                    m_Alive;

};
}//end namespace otb

#endif
