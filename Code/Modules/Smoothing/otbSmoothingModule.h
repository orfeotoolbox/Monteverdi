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
#ifndef __otbSmoothingModule_h
#define __otbSmoothingModule_h

#include "otbFeatureExtractionBaseModule.h"

#include "otbSmoothingController.h"
#include "otbSmoothingModel.h"
#include "otbSmoothingViewGUI.h"

namespace otb
{
/** \class SmoothingModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT SmoothingModule
  : public FeatureExtractionBaseModule
{
public:
  /** Standard class typedefs */
  typedef SmoothingModule       Self;
  typedef FeatureExtractionBaseModule              Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(SmoothingModule, FeatureExtractionBaseModule);

//  itkGetObjectMacro(View, FeatureExtractionBaseViewGUI);


  /** Show the Module GUI */
   virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_ViewSmooth->Show();
  }

protected:
/** Constructor */
  SmoothingModule();
  /** Destructor */
  virtual ~SmoothingModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  SmoothingModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
  
  /** Specific MVC for radiometric indices */
  SmoothingController::Pointer m_ControllerSmooth;
  SmoothingModel::Pointer m_ModelSmooth;
  SmoothingViewGUI::Pointer m_ViewSmooth;

};

}

#endif
