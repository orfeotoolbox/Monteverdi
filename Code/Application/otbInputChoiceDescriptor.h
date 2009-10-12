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
#ifndef __otbInputChoiceDescriptor_h
#define __otbInputChoiceDescriptor_h

#include <string>
#include <vector>

#include "itkObject.h"
#include "itkObjectFactory.h"

#include <FL/Fl_Choice.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Box.H>
#include <Fl/Fl_Button.H>
#include <Fl/Fl_Check_Button.H>

#include "otbInputDataDescriptor.h"

namespace otb
{

/** \class InputChoiceDescriptor
 *  \brief This small class describes the different input choices of a InputViewGUI.
 *
 *  The Optional flag allows to define optional inputs.
 *
 *  The Multiple flag allows to define unbounded multiple inputs.
 *
 */
 
class ITK_EXPORT InputChoiceDescriptor
  : public itk::Object, public Fl_Group
{
public:

 /** Standard class typedefs */
  typedef InputChoiceDescriptor         Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macros */
  itkNewMacro(Self);
  itkTypeMacro(InputChoiceDescriptor,itk::Object);

  /** Typedefs */
  // contains a module instance Id and a data key
  typedef std::pair<std::string,std::string>   StringPairType;
  typedef std::map<int, StringPairType>        StringPairMapType;
 
  /** Set the input data descriptor */
  void SetInputDataDescriptor(const InputDataDescriptor & desc);

  /** Add Choice */
  void AddChoice(const StringPairType & choice);

  /** Rebuild interface */
  void Rebuild();

  /** Get the selected pair */
  StringPairType GetSelected() const;

  /** Do we have a selected pair ? */
  bool HasSelected() const;

protected:
  /** Constructor */
  InputChoiceDescriptor();
  /** Destructor */
  virtual ~InputChoiceDescriptor();


private:
  InputChoiceDescriptor(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** InputDataDescriptor describing the input */
  InputDataDescriptor m_InputDataDescriptor;

  /** if input is multiple, we keep the indexes */
  std::vector<int>              m_Indexes;

  /** The choice map */
  StringPairMapType             m_ChoiceMap;

  /** The GUI components */
  Fl_Choice *                   m_FlChoice;
  Fl_Box *                      m_StatusBox;
  Fl_Browser *                  m_FlBrowser;
  Fl_Check_Button *             m_CheckButton;
  Fl_Button*                    m_AddButton;
  Fl_Button*                    m_RemoveButton;
  Fl_Button*                    m_ClearButton;

  /** Sizes */
  unsigned int m_UpperMargin;
  unsigned int m_LeftMargin;
  unsigned int m_SimpleWidgetsHeight;
  unsigned int m_MultipleWidgetsHeight;
  unsigned int m_CentralWidgetsLength;
  unsigned int m_WidgetsMargin;

};

} // End namespace otb

#endif
