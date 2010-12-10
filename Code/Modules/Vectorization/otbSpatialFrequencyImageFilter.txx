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

#ifndef __otbSpatialFrequencyImageFilter_txx
#define __otbSpatialFrequencyImageFilter_txx



#include "otbSpatialFrequencyImageFilter.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

namespace otb
{

template <class TInputImage, class TOutputImage>
SpatialFrequencyImageFilter<TInputImage, TOutputImage>
::SpatialFrequencyImageFilter()
{
  m_WindowSize = 16;  // default value could also be 32
  m_Radius.Fill(m_WindowSize / 2);
}

template <class TInputImage, class TOutputImage>
void 
SpatialFrequencyImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion() throw (itk::InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  typename Superclass::InputImagePointer inputPtr = 
    const_cast< TInputImage * >( this->GetInput() );
  typename Superclass::OutputImagePointer outputPtr = this->GetOutput();
  
  if ( !inputPtr || !outputPtr )
    {
    return;
    }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( m_Radius );

  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    
    // build an exception
    itk::InvalidRequestedRegionError e(__FILE__, __LINE__);
    e.SetLocation(ITK_LOCATION);
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
}

template< class TInputImage, class TOutputImage>
void
SpatialFrequencyImageFilter< TInputImage, TOutputImage>
::BeforeThreadedGenerateData() {

  // Memory allocation
  int nbOfPixels = m_WindowSize * m_WindowSize;
  int usedWidth = m_WindowSize / 2 + 1;
  int fftSize = m_WindowSize * (m_WindowSize / 2 + 1);

  fftin = static_cast<float **>(malloc(this->GetNumberOfThreads() * sizeof(float *)));
  fftout = static_cast<fftwf_complex **>(malloc(this->GetNumberOfThreads() * sizeof(fftwf_complex *)));
  plan = static_cast<fftwf_plan *>(malloc(this->GetNumberOfThreads() * sizeof(fftwf_plan)));

  for (int i = 0; i < this->GetNumberOfThreads(); i++) {
    fftin[i] = static_cast<float *>(fftwf_malloc(nbOfPixels * sizeof(float)));
    fftout[i] = static_cast<fftwf_complex *>(fftwf_malloc(fftSize * sizeof(fftwf_complex)));
    plan[i] = fftwf_plan_dft_r2c_2d(m_WindowSize, m_WindowSize, fftin[i], fftout[i], FFTW_MEASURE);
  }

  fftwf_print_plan(plan[0]);
  double add, mul, fma;
  fftwf_flops(plan[0], &add, &mul, &fma);
  //cout << (add + mul + 2 * fma) << " flops : " << add << " adds, " << mul << " muls, " << fma << " fmas" << endl;
}

template< class TInputImage, class TOutputImage>
void
SpatialFrequencyImageFilter< TInputImage, TOutputImage>
::AfterThreadedGenerateData() {
  for (int i = 0; i < this->GetNumberOfThreads(); i++) 
    {
    fftwf_destroy_plan(plan[i]);
    fftwf_free(fftin[i]);
    fftwf_free(fftout[i]);
    }
  free(plan);
  free(fftin);
  free(fftout);
}


template< class TInputImage, class TOutputImage>
void
SpatialFrequencyImageFilter< TInputImage, TOutputImage>
::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                       int threadId)
{
  typename OutputImageType::Pointer azimuthOutputImage = this->GetOutput();
  
  typename InputImageType::ConstPointer image = this->GetInput();
  typename InputImageType::IndexType inputImageRequestedRegionStartPixelIndex = image->GetRequestedRegion().GetIndex();
  typename OutputImageType::IndexType outputRegionForThreadStartPixelIndex = outputRegionForThread.GetIndex();

  typename OutputImageType::RegionType requestedRegionForThread = outputRegionForThread;

  // pad the region by the operator radius
  requestedRegionForThread.PadByRadius( m_Radius );

  // crop the region at the input's largest possible region
  // ZZ should check if ok ?
  requestedRegionForThread.Crop(image->GetLargestPossibleRegion());
  typename InputImageType::SizeType imageSize = requestedRegionForThread.GetSize();
  typename OutputImageType::IndexType adjustedOutputRegionForThreadStartPixelIndex = requestedRegionForThread.GetIndex();

  typename InputImageType::IndexType pixelIndex;
  const InputPixelType *buf = image->GetBufferPointer();
  int width = imageSize[0]; 
  int height = imageSize[1];
  const int ROW = 1;
  const int COL = 0;

  buf += (adjustedOutputRegionForThreadStartPixelIndex[ROW] - inputImageRequestedRegionStartPixelIndex[ROW]) 
    * width  
    + adjustedOutputRegionForThreadStartPixelIndex[COL] 
    - inputImageRequestedRegionStartPixelIndex[COL] ;

  int usedWidth = m_WindowSize / 2 + 1;
  int fftSize = m_WindowSize * (m_WindowSize / 2 + 1);

  // Copy input window data
  unsigned int i, j; // coordinates within window
  unsigned int I, J; // window coordinates (upper left corner)
  unsigned int k;
  unsigned int idx;
  float spacing = image->GetSpacing()[0];

  //for (iit.GoToBegin(), azit.GoToBegin(), wlit.GoToBegin(); ! iit.IsAtEnd(); ++iit, ++azit, ++wlit) {
  for (I = 0; I < /*<=*/ height - m_WindowSize; I++)
    for (J = 0; J < /*<=*/ width - m_WindowSize; J++) {
      // Copy window data
      k = 0;
      idx = I * width + J;
      for (i = 0; i < m_WindowSize; i++, idx += (width - m_WindowSize)) {
        for (j = 0; j < m_WindowSize; j++, idx++) 
          {
          fftin[threadId][k++] = buf[idx];
          }
      }

      // FFT
      fftwf_execute(plan[threadId]);

      // Compute power spectrum (PS) (square modulus of DFT), store it in fftin
      for (i = 0; i < fftSize; i++)
        fftin[threadId][i] = fftout[threadId][i][0] * fftout[threadId][i][0] + fftout[threadId][i][1] * fftout[threadId][i][1];

      // Remove systematic maximum at 0 frequency (average)
      // and also at [0,1], [1,0] and [-1,0] (uninteresting wavelength = windows size)
      fftin[threadId][0] = 0;
      fftin[threadId][1] = 0;
      fftin[threadId][usedWidth] = 0;
      fftin[threadId][(m_WindowSize - 1) * usedWidth] = 0;

      // Look for 1st, 2nd and 3rd PS maxima
      float max[3];
      int imax[3];
      for (i = 0; i < 3; i++) {
        max[i] = 0;
        imax[i] = -1;
      }
      for (i = 0; i < fftSize; i++)
        for (j = 0; j < 3; j++) {
          if (fftin[threadId][i] > max[j]) {
            if (imax[j] >= 0 && (j + 1) < 3) { // valid max already seen -> propagate to following max ZZ should propagate in chain to be complete
              max[j + 1] = max[j];
              imax[j + 1] = imax[j];
            }
            max[j] =  fftin[threadId][i];
            imax[j] = i;
            break; // exit j for loop, so that following "max" do not "see" the first maximum values
          }
        }
    
      float lambda[3], theta[3]; // wavelength and angle of maxima, starting at the second one (first maximum is always at [0,0])
      int x[3], y[3]; // shifted coordinates, from the second one
      //max at [0,0] removed : for (i = 1; i < 3; i++) {
      for (i = 0; i < 3; i++) {
        x[i] = imax[i] % usedWidth;
        y[i] = imax[i] / usedWidth; // from 0 to (height - 1)
        if (y[i] >= static_cast<int>(m_WindowSize / 2))
          y[i] = m_WindowSize - y[i]; // shifted to : from -h/2 to h/2
        else
          y[i] = -y[i];
        theta[i] = 90 - vcl_atan( y[i] / static_cast<float>(x[i]) ) * 180 / M_PI; // azimuth, from 0� (North) to 90� (East) and 180� (South)
    
        float wx = x[i] / static_cast<float>(m_WindowSize);
        float wy = y[i] / static_cast<float>(m_WindowSize);
        lambda[i] = 1.0 / vcl_sqrt(wx * wx + wy * wy); // in pixels, multiply by spacing to get meters
      }
    
      pixelIndex[ROW] = I + m_WindowSize / 2 + adjustedOutputRegionForThreadStartPixelIndex[ROW];
      pixelIndex[COL] = J + m_WindowSize / 2 + adjustedOutputRegionForThreadStartPixelIndex[COL];
      azimuthOutputImage->SetPixel(pixelIndex, theta[0]); //theta[1]);

    }
}

/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutput>
void
SpatialFrequencyImageFilter<TInputImage, TOutput>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf( os, indent );
  os << indent << "Radius: " << m_Radius << std::endl;
}

} // end namespace otb

#endif
