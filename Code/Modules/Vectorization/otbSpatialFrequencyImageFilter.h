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

#ifndef __otbSpatialFrequencyImageFilter_h
#define __otbSpatialFrequencyImageFilter_h


#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkNumericTraits.h"

#include "fftw3.h"

namespace otb
{
template <class TInputImage, class TOutputImage>
class ITK_EXPORT SpatialFrequencyImageFilter :
    public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  /** Extract dimension from input and output image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TOutputImage::ImageDimension);

  /** Convenient typedefs for simplifying declarations. */
  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;

  /** Standard class typedefs. */
  typedef SpatialFrequencyImageFilter                          Self;
  typedef itk::ImageToImageFilter< InputImageType, OutputImageType> Superclass;
  typedef itk::SmartPointer<Self>                                   Pointer;
  typedef itk::SmartPointer<const Self>                             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(SpatialFrequencyImageFilter, ImageToImageFilter);
  
  /** Image typedef support. */
  typedef typename InputImageType::PixelType               InputPixelType;
  typedef typename OutputImageType::PixelType              OutputPixelType;
  typedef typename itk::NumericTraits<InputPixelType>::RealType InputRealType;
  
  typedef typename InputImageType::RegionType  InputImageRegionType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;
  typedef typename InputImageType::SizeType    InputSizeType;

  /** Set the radius of the neighborhood used to compute the mean. */
  itkSetMacro(Radius, InputSizeType);

  /** Get the radius of the neighborhood used to compute the mean */
  itkGetConstReferenceMacro(Radius, InputSizeType);
  
  /** Set the size of the window used to compute the fft. */
  //itkSetMacro(WindowSize, unsigned int); Do it by hand to also set the radius
  void SetWindowSize(unsigned int w) {
    m_WindowSize = w;
    m_Radius.Fill(w / 2);
  }

  /** Get the size of the window used to compute the fft */
  itkGetConstReferenceMacro(WindowSize, unsigned int);
  
  /** SpatialFrequencyImageFilter needs a larger input requested region than
   * the output requested region.  As such, SpatialFrequencyImageFilter needs
   * to provide an implementation for GenerateInputRequestedRegion()
   * in order to inform the pipeline execution model.
   *
   * \sa ImageToImageFilter::GenerateInputRequestedRegion() */
  virtual void GenerateInputRequestedRegion() throw(itk::InvalidRequestedRegionError);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(InputHasNumericTraitsCheck,
                  (itk::Concept::HasNumericTraits<InputPixelType>));
  /** End concept checking */
#endif

protected:
  SpatialFrequencyImageFilter();
  virtual ~SpatialFrequencyImageFilter() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** SpatialFrequencyImageFilter can be implemented as a multithreaded filter.
   * Therefore, this implementation provides a ThreadedGenerateData()
   * routine which is called for each processing thread. The output
   * image data is allocated automatically by the superclass prior to
   * calling ThreadedGenerateData().  ThreadedGenerateData can only
   * write to the portion of the output image specified by the
   * parameter "outputRegionForThread"
   *
   * \sa ImageToImageFilter::ThreadedGenerateData(),
   *     ImageToImageFilter::GenerateData() */
  virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                            int threadId );

  virtual void BeforeThreadedGenerateData();
  virtual void AfterThreadedGenerateData();

private:
  SpatialFrequencyImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  InputSizeType m_Radius;
  unsigned int  m_WindowSize;


  float **fftin;
  fftwf_complex **fftout;
  fftwf_plan *plan;
};
  
} // end namespace otb

#ifndef ITK_MANUAL_INSTANTIATION
#include "otbSpatialFrequencyImageFilter.txx"
#endif

#endif
