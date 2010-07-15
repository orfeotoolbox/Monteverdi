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
#ifndef __otbInputViewComponent_h
#define __otbInputViewComponent_h

#include <string>
#include <vector>

#include "itkObject.h"
#include "itkObjectFactory.h"

#include <FL/Fl_Choice.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>

#include "otbInputDataDescriptor.h"
#include "otbMonteverdiModel.h"
#include "otbMonteverdiControllerInterface.h"

namespace otb
{

/** \class InputViewComponent
 *  \brief This small class describes the different input choices of a InputViewGUI.
 *
 *  The Optional flag allows to define optional inputs.
 *
 *  The Multiple flag allows to define unbounded multiple inputs.
 *
 */

class ITK_EXPORT InputViewComponent
  : public itk::Object, public Fl_Group
{
public:

  /** Standard class typedefs */
  typedef InputViewComponent            Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macros */
  itkNewMacro(Self);
  itkTypeMacro(InputViewComponent, itk::Object);

  /** Typedefs */
  // contains a module instance Id and a data key
  typedef std::pair<std::string, std::string> StringPairType;
  typedef std::map<int, StringPairType>       StringPairMapType;
  typedef std::vector<StringPairType>         StringPairVectorType;

  itkSetObjectMacro(Model, MonteverdiModel);
  itkGetObjectMacro(Model, MonteverdiModel);

  itkSetObjectMacro(Controller, MonteverdiControllerInterface);
  itkGetObjectMacro(Controller, MonteverdiControllerInterface);

  /** Set the input data descriptor */
  void SetInputDataDescriptor(const InputDataDescriptor& desc);

  /** Add Choice */
  void AddChoice(const StringPairType& choice, bool locked = false, const std::string& locker = NULL);

  /** Rebuild interface */
  void Rebuild();

  /** Get the selected pair */
  StringPairVectorType GetSelected() const;

  /** Do we have any selected pair ? */
  bool HasSelected() const;

  /** Activate all */
  void Activate();

  /** Deactivate all */
  void Deactivate();

  /** Update caching progress if caching is active */
  void UpdateCachingProgress();

  /** Check if the input is ready */
  bool IsReady();

  /** Get the number of available choices */
  unsigned int GetNumberOfChoices() const
  {
    return m_ChoiceMap.size();
  }

  /** Select the nth choice */
  void SelectNthChoice(unsigned int idx)
  {
    m_FlChoice->value(idx);
  }

protected:
  /** Constructor */
  InputViewComponent();
  /** Destructor */
  virtual ~InputViewComponent();

private:
  InputViewComponent(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Callbacks */
  static void Switch(Fl_Widget *w, void * v);
  static void AddInput(Fl_Widget * w, void * v);
  static void RemoveInput(Fl_Widget * w, void * v);
  static void ClearInputs(Fl_Widget * w, void * v);
  static void InputChanged(Fl_Widget * w, void * v);
  static void StartCaching(Fl_Widget * w, void * v);

  /** InputDataDescriptor describing the input */
  InputDataDescriptor m_InputDataDescriptor;

  /** if input is multiple, we keep the indexes */
  std::vector<int> m_Indices;

  /** The choice map */
  StringPairMapType m_ChoiceMap;

  /** The GUI components */
  Fl_Choice *       m_FlChoice;
  Fl_Button *       m_StatusBox;
  Fl_Browser *      m_FlBrowser;
  Fl_Check_Button * m_CheckButton;
  Fl_Button*        m_AddButton;
  Fl_Button*        m_RemoveButton;
  Fl_Button*        m_ClearButton;
  Fl_Progress *     m_ProgressBar;

  /** Sizes */
  unsigned int m_UpperMargin;
  unsigned int m_LeftMargin;
  unsigned int m_SimpleWidgetsHeight;
  unsigned int m_MultipleWidgetsHeight;
  unsigned int m_CentralWidgetsLength;
  unsigned int m_WidgetsMargin;

  /* Model */
  MonteverdiModel::Pointer m_Model;

  /** Controller */
  itk::WeakPointer<MonteverdiControllerInterface> m_Controller;

  /** Is Caching in progress ? */
  bool m_CachingInProgress;

};

} // End namespace otb

#endif
