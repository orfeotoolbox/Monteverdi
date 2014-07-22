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
#ifndef __otbMorphologyExtractionModule_h
#define __otbMorphologyExtractionModule_h

#include "otbFeatureExtractionBaseModule.h"

#include "otbMorphologyExtractionController.h"
#include "otbMorphologyExtractionModel.h"
#include "otbMorphologyExtractionViewGUI.h"

namespace otb
{
/** \class MorphologyExtractionModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT MorphologyExtractionModule
  : public FeatureExtractionBaseModule
{
public:
  /** Standard class typedefs */
  typedef MorphologyExtractionModule       Self;
  typedef FeatureExtractionBaseModule              Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(MorphologyExtractionModule, FeatureExtractionBaseModule);

//  itkGetObjectMacro(View, FeatureExtractionBaseViewGUI);


  /** Show the Module GUI */
   virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_ViewMorpho->Show();
  }

protected:
/** Constructor */
  MorphologyExtractionModule();
  /** Destructor */
  virtual ~MorphologyExtractionModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  MorphologyExtractionModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
  
  /** Specific MVC for morphology features */
  MorphologyExtractionController::Pointer m_ControllerMorpho;
  MorphologyExtractionModel::Pointer m_ModelMorpho;
  MorphologyExtractionViewGUI::Pointer m_ViewMorpho;

};

}

#endif
