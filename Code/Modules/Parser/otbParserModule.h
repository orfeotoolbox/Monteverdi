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
#include "otbMultiToMonoChannelExtractROI.h"

// Parser opertor Filter
#include "otbBandMathImageFilter.h"

namespace otb
{
/** \class ParserModule
 *  \brief This is the Parser module
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
  typedef TypeManager::Floating_Point_VectorImage      VectorImageType;
  typedef otb::MultiToMonoChannelExtractROI<ImageType::PixelType, ImageType::PixelType> ExtractROIFilterType;
  typedef otb::ObjectList<ExtractROIFilterType>        ExtractROIFilterListType;
 
  // Parser Class typedefs
  typedef BandMathImageFilter<ImageType>             ParserFilterType;
  typedef Parser                                       ParserType;

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
  virtual void Hide();
  
  /** OK callback*/
  virtual void OK();

  /** Help Initialization */
  virtual void InitHelp();

  /** Change a Variable Name */
  virtual void ChangeVarName();

  /** Quick add a variable name into the expression */
  virtual void QuickAdd(unsigned int idx);

  /** Expression live checking */
  virtual void LiveCheck();

private:
  ParserModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Class attributes
  ParserFilterType::Pointer         m_ParserFilter;
  ExtractROIFilterListType::Pointer m_ChannelExtractorList;
  ImageType::Pointer                m_Output;
  unsigned int                      m_NumberOfInputBands;

};

} // End namespace otb

#endif
