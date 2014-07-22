/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbRGBImageToVectorImageCastFilter_h
#define __otbRGBImageToVectorImageCastFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkVariableLengthVector.h"
#include "itkMetaDataDictionary.h"
#include "otbImageKeywordlist.h"
#include "itkMetaDataObject.h"

namespace otb
{
namespace Functor
{
/**
 * \class VectorCastRGBtoVLV
 * \brief This functor can be used to convert an itk::RGBPixel to a VariableLengthVector with a different precision
 * \ingroup Functor
 *
 */
template <class TInput, class TOutput>
class VectorCastRGBtoVLV
{
public:
  typedef typename TOutput::ValueType OutputValueType;
  VectorCastRGBtoVLV()
    {
    };
  virtual ~VectorCastRGBtoVLV() {}

  bool operator != (const VectorCastRGBtoVLV &) const
    {
    return false;
    }
  bool operator == (const VectorCastRGBtoVLV &other) const
    {
    return (*this != other);
    }
  inline TOutput operator() (const TInput &A)
    {
    TOutput output;
    output.SetSize(3);
    output[0] = static_cast<OutputValueType>(A[0]);
    output[1] = static_cast<OutputValueType>(A[1]);
    output[2] = static_cast<OutputValueType>(A[2]);
    return output;
    }
};
} // End namespace Functor

/**
 * \class RGBImageToVectorImageCastFilter
 * \brief This is a helper class that convert an otb::Image into a single-channel otb::VectorImage.
 * \ingroup Singlethreaded
 *
 */
template <class TInputImage, class TOutputVectorImage>
class ITK_ABI_EXPORT RGBImageToVectorImageCastFilter
: public itk::UnaryFunctorImageFilter<TInputImage, TOutputVectorImage,
                                      Functor::VectorCastRGBtoVLV<typename TInputImage::PixelType,
                                                                  typename TOutputVectorImage::PixelType> >
{
public:
  /// Standard class typedefs
  typedef RGBImageToVectorImageCastFilter
                                                                                                             Self;
  typedef itk::UnaryFunctorImageFilter<TInputImage, TOutputVectorImage,
                                       Functor::VectorCastRGBtoVLV<typename TInputImage::PixelType,
                                                                   typename TOutputVectorImage::PixelType> > Superclass;
  typedef itk::SmartPointer<Self>
                                                                                                             Pointer;
  typedef itk::SmartPointer<const Self>
                                                                                                             ConstPointer;

  /// Method for creation through the object factory
  itkNewMacro(Self);

  /// Runtime information
  itkTypeMacro(RGBImageToVectorImageCastFilter, itk::UnaryFunctorImageFilter);

protected:
  /// Constructor
  RGBImageToVectorImageCastFilter()
    {
    }

  /// Detructor
  virtual ~RGBImageToVectorImageCastFilter() {}

  /// Additionnal output information for allocation
  virtual void GenerateOutputInformation(void)
    {
      Superclass::GenerateOutputInformation();
      this->GetOutput()->SetNumberOfComponentsPerPixel(3);
      
      //// change SENSOR ID IN KWL TO ESCAPE THE DEFAULT PIXEL DISPLAY
      // Load MetaDataDictionary
      itk::MetaDataDictionary dict = this->GetInput()->GetMetaDataDictionary();
      ImageKeywordlist imageKeywordlist;
      // Extract KWL
      if (dict.HasKey(MetaDataKey::OSSIMKeywordlistKey))
        {
          itk::ExposeMetaData<ImageKeywordlist>(dict, MetaDataKey::OSSIMKeywordlistKey, imageKeywordlist);
        }
      imageKeywordlist.ClearMetadataByKey("sensor");

      itk::EncapsulateMetaData<ImageKeywordlist>(dict, MetaDataKey::OSSIMKeywordlistKey, imageKeywordlist);
      
      // Update the output MetaDataDictionary
      this->GetOutput()->SetMetaDataDictionary(dict);
    }

  /// Copy output requested region to input requested region
  virtual void GenerateInputRequestedRegion(void)
  {
    if (this->GetInput())
      {
      typename TInputImage::Pointer    input = const_cast<TInputImage *>(this->GetInput());
      typename TInputImage::RegionType inputRegion;

      this->CallCopyOutputRegionToInputRegion(inputRegion, this->GetOutput()->GetRequestedRegion());
      input->SetRequestedRegion(inputRegion);
      }
  }


private:
  RGBImageToVectorImageCastFilter(const Self &); //purposely not implemented
  void  operator = (const Self &); // purposely not implemented
};
} // end namespace otb
#endif
