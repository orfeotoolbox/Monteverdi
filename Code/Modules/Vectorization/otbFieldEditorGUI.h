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
#ifndef __otbFieldEditorGUI_h
#define __otbFieldEditorGUI_h

#include "itkObject.h"
#include "otbFieldEditorGroup.h"
#include "itkObjectFactory.h"
#include "otbDataNode.h"
#include "otbVectorizationControllerInterface.h"

namespace otb
{

/** \class FieldEditorGUI
 *  \brief
 */
class ITK_EXPORT FieldEditorGUI
  : public itk::Object, public FieldEditorGroup
{
public:
/** Standard class typedefs */
  typedef FieldEditorGUI                Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  typedef otb::DataNode<>       DataNodeType;
  typedef DataNodeType::Pointer DataNodePointerType;

/** Creation through object factory */
  itkNewMacro(Self);

/** RTT type info */
  itkTypeMacro(FieldEditorGUI, itk::Object);

  itkSetObjectMacro(Controller, VectorizationControllerInterface);
  itkGetObjectMacro(Controller, VectorizationControllerInterface);

  itkSetObjectMacro(DataNode, DataNodeType);
  itkGetObjectMacro(DataNode, DataNodeType);

protected:
/** Constructor */
  FieldEditorGUI(){}

/** Destructor */
  virtual ~FieldEditorGUI(){}

  virtual void FieldEditorOkCallback()
  {
    switch (vFieldType->value())
      {
      // String case
      case 0:
        {
        m_Controller->SetDataNodeFieldAsString(m_DataNode, vFieldName->value(), vFieldValueString->value());
        break;
        }
      case 1:
        {
        m_Controller->SetDataNodeFieldAsInt(m_DataNode, vFieldName->value(), atoi(vFieldValueInt->value()));
        break;
        }
      case 2:
        {
        m_Controller->SetDataNodeFieldAsFloat(m_DataNode, vFieldName->value(), atof(vFieldValueFloat->value()));
        break;
        }
      default:
        {
        break;
        }
      }
    wFieldEditor->hide();
  }

private:
  FieldEditorGUI(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  VectorizationControllerInterface::Pointer m_Controller;
  DataNodePointerType                       m_DataNode;

}; // End class FieldEditorGUI
} // End namespace otb

#endif
