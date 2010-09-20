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
#ifndef __otbBandMathModule_h
#define __otbBandMathModule_h

#include "otbModule.h"
#include "otbBandMathModuleGUI.h"

// include the OTB elements
#include "otbMultiToMonoChannelExtractROI.h"

// BandMath Filter
#include "otbBandMathImageFilter.h"

namespace otb
{
/** \class BandMathModule
 *  \brief This is the band math module
 *
 *
 */

class ITK_EXPORT BandMathModule
  : public Module, public BandMathModuleGUI
{
public:
  /** Standard class typedefs */
  typedef BandMathModule                Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(BandMathModule, Module);

  // Convenient typedefs
  typedef TypeManager::Floating_Point_Image            ImageType;
  typedef TypeManager::Floating_Point_VectorImage      VectorImageType;
  typedef otb::MultiToMonoChannelExtractROI<ImageType::PixelType, ImageType::PixelType> ExtractROIFilterType;
  typedef otb::ObjectList<ExtractROIFilterType>        ExtractROIFilterListType;
  typedef BandMathImageFilter<ImageType>               BandMathFilterType;
  typedef Parser                                       ParserType;

protected:
  /** Constructor */
  BandMathModule();

  /** Destructor */
  virtual ~BandMathModule();

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
  BandMathModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Class attributes
  BandMathFilterType::Pointer       m_BandMathFilter;
  ExtractROIFilterListType::Pointer m_ChannelExtractorList;
  ImageType::Pointer                m_Output;
  unsigned int                      m_NumberOfInputBands;

};

} // End namespace otb

#endif
