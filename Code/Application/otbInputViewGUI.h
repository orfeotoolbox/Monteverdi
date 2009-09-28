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
#include "otbInputChoiceDescriptor.h"


namespace otb
{
/** \class InputViewGUI
 *
 */
class ITK_EXPORT InputViewGUI
   : public itk::Object, public InputViewGroup
{
public:
  /** Standard class typedefs */
  typedef InputViewGUI                  Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macros */
  itkNewMacro(Self);
  itkTypeMacro(InputViewGUI,Object);


  typedef Module::OutputDataDescriptorMapType             OutputDataDescriptorMapType;
  typedef Module::InputDataDescriptorMapType              InputDataDescriptorMapType;

  typedef InputChoiceDescriptor::StringPairType           StringPairType;
  typedef InputChoiceDescriptor::Pointer                  InputChoiceDescriptorPointerType;

  /** Getters/Setters */
  itkGetObjectMacro(Model,MonteverdiModel);
  itkSetObjectMacro(Model,MonteverdiModel);
  itkGetObjectMacro(Controller,MonteverdiControllerInterface);
  itkSetObjectMacro(Controller,MonteverdiControllerInterface);
  itkGetMacro(ModuleInstanceId,std::string);
  itkSetMacro(ModuleInstanceId,std::string);


  void Show();
  void BuildInputInterface();
  void BuildList(int cpt,int height);
  void BuildCheckBox(int cpt,int height,Fl_Choice *inputChoice);

protected:
  /** Constructor */
  InputViewGUI();
  /** Destructor */
  virtual ~InputViewGUI(){}

  /** Callbacks */
  virtual void Ok();
  virtual void Cancel();
  static void ActivateInputChoice(Fl_Widget * w, void * v);


private:
  InputViewGUI(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  static void AddInputToList(Fl_Widget * w, void * v);
  static void RemoveInputFromList(Fl_Widget * w, void * v);
  static void ClearList(Fl_Widget * w, void * v);
  typedef std::map<std::string,InputChoiceDescriptorPointerType> InputChoiceDescriptorMapType;

  MonteverdiModel::Pointer                m_Model;
  MonteverdiControllerInterface::Pointer  m_Controller;
  std::string                             m_ModuleInstanceId;
  InputChoiceDescriptorMapType            m_InputChoiceMap;
};
}//end namespace otb

#endif
