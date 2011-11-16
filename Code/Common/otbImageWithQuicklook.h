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
#ifndef __otbImageWithQuicklook_h
#define __otbImageWithQuicklook_h

#include "otbVectorImage.h"

namespace otb
{
/** \class ImageWithQuicklook
 * \brief This class holds an otb::VectorImage and its quicklook
 *
 */
template <class TPixel, unsigned int VImageDimension = 2>
class ImageWithQuicklook
  : public itk::DataObject
{
public:
  /** Standard class typedefs */
  typedef ImageWithQuicklook            Self;
  typedef itk::DataObject               Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard macros */
  itkNewMacro(Self);
  itkTypeMacro(ImageWithQuicklook, DataObject);

  itkStaticConstMacro(Dimension, unsigned int, VImageDimension);

  typedef otb::VectorImage<TPixel, VImageDimension> VectorImageType;

  itkGetObjectMacro(Image, VectorImageType);
  itkSetObjectMacro(Image, VectorImageType);

  itkGetObjectMacro(Quicklook, VectorImageType);
  itkSetObjectMacro(Quicklook, VectorImageType);

protected:
  /** Constructor */
  ImageWithQuicklook() {}
  /** Destructor */
  virtual ~ImageWithQuicklook() {}
  /** PrintSelf method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "Image     : " << m_Image << std::endl;
    os << indent << "Quicklook : " << m_Quicklook << std::endl;
  }

private:
  ImageWithQuicklook(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  typename VectorImageType::Pointer m_Image;
  typename VectorImageType::Pointer m_Quicklook;

};
} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbVectorData.txx"
#endif

#endif
