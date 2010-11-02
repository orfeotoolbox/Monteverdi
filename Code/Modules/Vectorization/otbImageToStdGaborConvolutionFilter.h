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
#ifndef __otbImageToStdGaborConvolutionFilter_h
#define __otbImageToStdGaborConvolutionFilter_h

#include "itkImageToImageFilter.h"

//Visu
#include "itkExtractImageFilter.h"
#include "otbVectorImage.h"

// Gabor Generator
#include "otbGaborFilterGenerator.h"

// Convolution filter
#include "otbConvolutionImageFilter.h"

// Standard Deviation 
#include "otbVarianceImageFilter.h"
#include "itkSqrtImageFilter.h"
#include "otbConvolutionImageFilter.h"
#include "otbImageListToVectorImageFilter.h"

// 
#include "otbVectorRescaleIntensityImageFilter.h"
#include "otbImageList.h"
#include "otbImage.h"

#include "otbVectorImageToIntensityImageFilter.h"
#include "otbVectorImageToImageListFilter.h"

namespace otb
{
/** \class ImageToStdGaborConvolutionFilter
 * \brief Convolution with a Gabor kernel
 *        
 *  This class generate a Gabor Kernel following the direction set
 *  by the user with the method SetDirection. The input image is than
 *  convoluted with the kernel using the streamed and multithreaded
 *  filter  otb::ConvolutionImageFilter.      
 * 
 * The output image is the concatenation of the amplitude image, the
 * standard deviation of each convoluted band with the gabor filter in
 * each direction set.
 * 
 * You can have access to the Gabor filter parameters to tune the
 * kernel you need using the same interface than the
 * otb::GaborFilterGenerator class. 
 * The filter implements also the GenerateOutputInformation to compute
 * the number of components per pixel of the output image.
 *
 * \sa GaborFilterGenerator
 * \ingroup 
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT ImageToStdGaborConvolutionFilter :
    public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:

  /** Standard class typedefs. */
  typedef ImageToStdGaborConvolutionFilter                          Self;
  typedef itk::ImageToImageFilter<TInputImage ,TOutputImage > Superclass;
  typedef itk::SmartPointer<Self>                                   Pointer;
  typedef itk::SmartPointer<const Self>                             ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToStdGaborConvolutionFilter, ImageToImageFilter);

  /** Convenient typedefs for simplifying declarations. */
  typedef TInputImage          InputImageType;
  typedef TOutputImage         OutputImageType;
  
  /** Image typedef support. */
  typedef typename InputImageType::PixelType               InputPixelType;
  typedef typename OutputImageType::InternalPixelType      OutputInternalPixelType;
    
  typedef typename InputImageType::RegionType  InputImageRegionType;
  typedef typename OutputImageType::RegionType OutputImageRegionType;
  typedef typename InputImageType::SizeType    InputSizeType;

  /** Image List */ 
  typedef otb::Image<OutputInternalPixelType>             SingleImageType;
  typedef otb::ImageList<SingleImageType>                 ImageListType;

  typedef VectorImageToIntensityImageFilter<InputImageType, 
                                            SingleImageType> IntensityChannelFilterType;
  
  
  typedef typename InputImageType::InternalPixelType        InternalInputPixelType;

  typedef GaborFilterGenerator<InternalInputPixelType>      GaborFilterGeneratorType;

  typedef ConvolutionImageFilter<SingleImageType,  
                                 SingleImageType,
                                 itk::ZeroFluxNeumannBoundaryCondition<SingleImageType>, 
                                 OutputInternalPixelType                                
                                 >                           ConvolutionFilterType;
  typedef typename ConvolutionFilterType::ArrayType                   ArrayType;
  
  typedef VarianceImageFilter<SingleImageType, 
                              SingleImageType >              VarianceFilterType;
  typedef typename VarianceFilterType::InputSizeType         RadiusType;

  typedef itk::SqrtImageFilter<SingleImageType, 
                               SingleImageType >             SqrtImageFilterType;



  typedef VectorRescaleIntensityImageFilter
  <InputImageType,OutputImageType>                           RescalerFilterType;


  typedef ImageListToVectorImageFilter<ImageListType, 
                                       OutputImageType>      ImageToVectorFilterType;

  typedef otb::VectorImageToImageListFilter<InputImageType, 
                                            ImageListType>    VectorImageToImageListFilterType;
  

  /** Method to access the gabor filter parameters*/
  itkSetMacro(Radius,RadiusType);
  itkSetMacro(A,InternalInputPixelType);
  itkSetMacro(B,InternalInputPixelType);
  itkSetMacro(Theta,InternalInputPixelType);
  itkSetMacro(U0, InternalInputPixelType);
  itkSetMacro(V0, InternalInputPixelType);
  itkSetMacro(Phi, InternalInputPixelType);
  
  /** Method to get the parameters values used*/
  itkGetMacro(Radius,RadiusType);
  itkGetMacro(A,InternalInputPixelType);
  itkGetMacro(B,InternalInputPixelType);
  itkGetMacro(Theta,InternalInputPixelType);
  itkGetMacro(U0, InternalInputPixelType);
  itkGetMacro(V0, InternalInputPixelType);
  itkGetMacro(Phi, InternalInputPixelType);

  // Set radius with the same value in x and y axis
  void SetRadius(unsigned int val)
  {
    RadiusType   radius;
    radius.Fill(val);
    this->SetRadius(radius);
  }
  
  // Variance Radius : cannot use the object macro cause a SetRadius
  // method is already defined for the m_GaborFilter attribute. 
  void SetVarianceRadius(const RadiusType& radius)
  {
    m_VarianceRadius = radius;
  }

  // With the same value in x and y axis
  void SetVarianceRadius(unsigned int val)
  {
    RadiusType   radius;
    radius.Fill(val);
    this->SetVarianceRadius(radius);
  }

  const RadiusType& GetVarianceRadius()
  {
    return m_VarianceRadius;
  }

  // Set the Number of direction for the gabor
  itkSetMacro(NumberOfDirection, unsigned int);
  itkGetMacro(NumberOfDirection, unsigned int);

  // Set the first direction to begin with in Degree
  itkSetMacro(InitialDirection, double);
  
protected:
  ImageToStdGaborConvolutionFilter();
  virtual ~ImageToStdGaborConvolutionFilter() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  virtual void GenerateData();

  virtual void GenerateOutputInformation();
  
private:
  ImageToStdGaborConvolutionFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<typename GaborFilterGeneratorType::Pointer>    m_GaborFilterList;
  std::vector<typename VarianceFilterType::Pointer>          m_VarianceFilterList;    
  std::vector<typename ConvolutionFilterType::Pointer>       m_ConvolutionFilterList;
  std::vector<typename SqrtImageFilterType::Pointer>         m_SqrtImageFilterList;
  
  typename RescalerFilterType::Pointer          m_RescaleFilter;
  typename IntensityChannelFilterType::Pointer  m_IntensityFilter; 
  
  typename ImageToVectorFilterType::Pointer     m_ImageListToImageVectorFilter;
  typename VectorImageToImageListFilterType::Pointer  m_VectorImageToListFilter;

  // Gabor stuff 
  unsigned int                          m_NumberOfDirection;
  double                                m_InitialDirection;
  RadiusType                            m_Radius;
  InternalInputPixelType                m_A;
  InternalInputPixelType                m_B;
  InternalInputPixelType                m_Theta;
  InternalInputPixelType                m_Phi;
  InternalInputPixelType                m_U0;
  InternalInputPixelType                m_V0;
  
  /** Radius for the variance filter */
  RadiusType                           m_VarianceRadius;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "otbImageToStdGaborConvolutionFilter.txx"
#endif

//#endif

#endif
