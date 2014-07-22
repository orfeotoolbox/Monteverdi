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
#ifndef __otbPanSharpeningModule_h
#define __otbPanSharpeningModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbSimpleRcsPanSharpeningFusionImageFilter.h"

namespace otb
{
/** \class PanSharpeningModule
 *  \brief This is the PanSharpening module
 *
 * Description of the module.
 *
 */

class ITK_ABI_EXPORT PanSharpeningModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef PanSharpeningModule           Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(PanSharpeningModule, Module);

  /** Data typedefs */
  typedef TypeManager::Floating_Point_VectorImage FloatingVectorImageType;
  typedef TypeManager::Floating_Point_Image       FloatingImageType;

  typedef otb::SimpleRcsPanSharpeningFusionImageFilter
  <FloatingImageType, FloatingVectorImageType, FloatingVectorImageType> FusionFilterType;

protected:
  /** Constructor */
  PanSharpeningModule();

  /** Destructor */
  virtual ~PanSharpeningModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  PanSharpeningModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  FusionFilterType::Pointer m_PanSharpeningFilter;

};

} // End namespace otb

#endif
