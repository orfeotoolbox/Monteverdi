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
#ifndef __otbTextureExtractionModule_h
#define __otbTextureExtractionModule_h

#include "otbFeatureExtractionBaseModule.h"

#include "otbTextureExtractionController.h"
#include "otbTextureExtractionModel.h"
#include "otbTextureExtractionViewGUI.h"

namespace otb
{
/** \class TextureExtractionModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT TextureExtractionModule
  : public FeatureExtractionBaseModule
{
public:
  /** Standard class typedefs */
  typedef TextureExtractionModule       Self;
  typedef FeatureExtractionBaseModule              Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(TextureExtractionModule, FeatureExtractionBaseModule);

//  itkGetObjectMacro(View, FeatureExtractionBaseViewGUI);


  /** Show the Module GUI */
   virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    m_ViewTex->Show();
  }

protected:
/** Constructor */
  TextureExtractionModule();
  /** Destructor */
  virtual ~TextureExtractionModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  TextureExtractionModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
  
  /** Specific MVC for radiometric indices */
  TextureExtractionController::Pointer m_ControllerTex;
  TextureExtractionModel::Pointer m_ModelTex;
  TextureExtractionViewGUI::Pointer m_ViewTex;

};

}

#endif
