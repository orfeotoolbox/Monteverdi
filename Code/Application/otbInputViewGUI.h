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
//#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Choice.H>
#include "otbMonteverdiModel.h"
#include "otbMonteverdiControllerInterface.h"


namespace otb
{
/** \class InputViewGUI
 *
 */
class InputViewGUI
   : public InputViewGroup, public itk::Object
{
public:

  /** Standard class typedefs */
  typedef InputViewGUI                  Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macros */
  itkNewMacro(Self);
  itkTypeMacro(InputViewGUI,itk::Object);


  typedef Module::OutputDataDescriptorMapType             OutputDataDescriptorMapType;
  typedef Module::InputDataDescriptorMapType              InputDataDescriptorMapType;

  typedef std::pair<std::string,std::string>              StringPairType;
  

  /** Getters/Setters */
  itkGetObjectMacro(Model,MonteverdiModel);
  itkSetObjectMacro(Model,MonteverdiModel);
  itkGetObjectMacro(Controller,MonteverdiControllerInterface);
  itkSetObjectMacro(Controller,MonteverdiControllerInterface);
  itkGetMacro(ModuleInstanceId,std::string);
  itkSetMacro(ModuleInstanceId,std::string);


  void Show();
  void BuildInputInterface();


protected:
  /** Constructor */
  InputViewGUI(){};
  /** Destructor */
  virtual ~InputViewGUI(){};

  class InputChoiceDescriptor
  {
  public:
    Fl_Choice *                   m_FlChoice;
    std::vector<StringPairType>   m_ChoiceVector;

    StringPairType GetSelected() const
    {
        std::cout<<"Selected item "<<m_FlChoice->value()<<std::endl;
       return m_ChoiceVector[m_FlChoice->value()];
    }
  };

  typedef std::map<std::string,InputChoiceDescriptor> InputChoiceDescriptorMapType;

  /** Callbacks */
  void Ok();
  void Cancel();
  MonteverdiModel::Pointer                m_Model;
  MonteverdiControllerInterface::Pointer  m_Controller;
  std::string                             m_ModuleInstanceId;
  InputChoiceDescriptorMapType            m_InputChoiceMap;
};
}//end namespace otb

#endif
