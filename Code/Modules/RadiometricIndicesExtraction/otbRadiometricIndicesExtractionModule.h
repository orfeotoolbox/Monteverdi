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
#ifndef __otbRadiometricIndicesExtractionModule_h
#define __otbRadiometricIndicesExtractionModule_h

#include "otbFeatureExtractionBaseModule.h"

#include "otbRadiometricIndicesExtractionController.h"
#include "otbRadiometricIndicesExtractionModel.h"
#include "otbRadiometricIndicesExtractionViewGUI.h"

namespace otb
{
/** \class RadiometricIndicesExtractionModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT RadiometricIndicesExtractionModule
  : public FeatureExtractionBaseModule
{
public:
  /** Standard class typedefs */
  typedef RadiometricIndicesExtractionModule       Self;
  typedef FeatureExtractionBaseModule              Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(RadiometricIndicesExtractionModule, FeatureExtractionBaseModule);

  itkGetObjectMacro(ViewRI, RadiometricIndicesExtractionViewGUI);


  /** Show the Module GUI */
   virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_ViewRI->Show();
  }

protected:
/** Constructor */
  RadiometricIndicesExtractionModule();
  /** Destructor */
  virtual ~RadiometricIndicesExtractionModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  RadiometricIndicesExtractionModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
  
  /** Specific MVC for radiometric indices */
  RadiometricIndicesExtractionController::Pointer m_ControllerRI;
  RadiometricIndicesExtractionModel::Pointer m_ModelRI;
  RadiometricIndicesExtractionViewGUI::Pointer m_ViewRI;

};

}

#endif
