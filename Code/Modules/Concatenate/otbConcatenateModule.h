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
#ifndef __otbConcatenateModule_h
#define __otbConcatenateModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageList.h"
#include "otbImageListToVectorImageFilter.h"

namespace otb
{
/** \class ConcatenateModule
 *  \brief This is the Concatenate module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT ConcatenateModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef ConcatenateModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ConcatenateModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Image       ImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;

  typedef ImageList<ImageType>                                         ImageListType;
  typedef ImageListToVectorImageFilter<ImageListType, VectorImageType> ImageListToVectorImageFilterType;

protected:
  /** Constructor */
  ConcatenateModule();

  /** Destructor */
  virtual ~ConcatenateModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  ConcatenateModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ImageListToVectorImageFilterType::Pointer m_ImageListToVectorImageFilter;
  ImageListType::Pointer                    m_ImageList;
};

} // End namespace otb

#endif
