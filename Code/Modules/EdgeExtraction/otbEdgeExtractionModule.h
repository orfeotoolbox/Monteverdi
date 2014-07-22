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
#ifndef __otbEdgeExtractionModule_h
#define __otbEdgeExtractionModule_h

#include "otbFeatureExtractionBaseModule.h"

#include "otbEdgeExtractionController.h"
#include "otbEdgeExtractionModel.h"
#include "otbEdgeExtractionViewGUI.h"

namespace otb
{
/** \class EdgeExtractionModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT EdgeExtractionModule
  : public FeatureExtractionBaseModule
{
public:
  /** Standard class typedefs */
  typedef EdgeExtractionModule       Self;
  typedef FeatureExtractionBaseModule              Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(EdgeExtractionModule, FeatureExtractionBaseModule);

//  itkGetObjectMacro(View, FeatureExtractionBaseViewGUI);


  /** Show the Module GUI */
   virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_ViewEdge->Show();
  }

protected:
/** Constructor */
  EdgeExtractionModule();
  /** Destructor */
  virtual ~EdgeExtractionModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  EdgeExtractionModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
  
  /** Specific MVC for radiometric indices */
  EdgeExtractionController::Pointer m_ControllerEdge;
  EdgeExtractionModel::Pointer m_ModelEdge;
  EdgeExtractionViewGUI::Pointer m_ViewEdge;

};

}

#endif
