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
#ifndef __otbExtractROIModule_h
#define __otbExtractROIModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbExtractROIModuleGUI.h"

// include the OTB elements
#include "otbVectorImage.h"
#include "otbMultiChannelExtractROI.h"

namespace otb
{
/** \class ExtractROIModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT ExtractROIModule
  : public Module, public ExtractROIModuleGUI
{
public:
  /** Standard class typedefs */
  typedef ExtractROIModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ExtractROIModule,Module);

  /** OTB typedefs */
  typedef double                                  InternalPixelType;
  /// Dataset
  typedef VectorImage<InternalPixelType,2>        FloatingVectorImageType;
  /// Extract ROI filter
  typedef MultiChannelExtractROI< InternalPixelType, InternalPixelType >  ExtractROIFilterType;

protected:
  /** Constructor */
  ExtractROIModule();
  /** Destructor */
  virtual ~ExtractROIModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void Ok();
  virtual void Cancel();

private:

  ExtractROIModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Pointer to the process object
  ExtractROIFilterType::Pointer m_ExtractROIFilter;

};

} // End namespace otb

#endif
