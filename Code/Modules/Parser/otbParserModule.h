 
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
#ifndef __otbParserModule_h
#define __otbParserModule_h

#include "otbModule.h"
#include "otbParserModuleGUI.h"

// include the OTB elements
#include "otbImage.h"

// Parser opertor Filter
#include "otbNaryParserImageFilter.h"

namespace otb
{
/** \class ParserModule
 *  \brief This it the Parser module
 *
 *
 */

class ITK_EXPORT ParserModule
  : public Module, public ParserModuleGUI
{
public:
  /** Standard class typedefs */
  typedef ParserModule                  Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ParserModule, Module);

  // Convenient typedefs
  typedef TypeManager::Floating_Point_Image            ImageType;

  // Parser Class typedefs
  typedef NaryParserImageFilter<ImageType>             ParserFilterType;

protected:
  /** Constructor */
  ParserModule();

  /** Destructor */
  virtual ~ParserModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    guiMainWindow->show();
  }

  /** Hide the Module GUI */
  virtual void Hide()
  {
    guiMainWindow->hide();
  }
  
  /** OK callback*/
  virtual void OK();

  /** Change a Variable Name */
  void ChangeVarName();

  /** Quick add a variable name into the expression */
  void QuickAdd(unsigned int idx);

private:
  ParserModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Class attributes
  ParserFilterType::Pointer        m_ParserFilter;
  ImageType::Pointer               m_Output;
};

} // End namespace otb

#endif
