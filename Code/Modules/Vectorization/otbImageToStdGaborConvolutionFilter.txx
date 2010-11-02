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
#ifndef __otbImageToStdGaborConvolutionFilter_txx
#define __otbImageToStdGaborConvolutionFilter_txx


namespace otb
{

template <class TInputImage, class TOutputImage>
ImageToStdGaborConvolutionFilter<TInputImage, TOutputImage>
::ImageToStdGaborConvolutionFilter():m_NumberOfDirection(3),
                     m_InitialDirection(0.),
                     m_A(0.25),m_B(0.35)
{
  m_RescaleFilter  = RescalerFilterType::New();
  m_IntensityFilter = IntensityChannelFilterType::New();
  m_ImageListToImageVectorFilter  = ImageToVectorFilterType::New();
  m_VectorImageToListFilter = VectorImageToImageListFilterType::New();

  // Initialize  the variance radius
  m_Radius.Fill(20);
  m_VarianceRadius.Fill(10);
}


template <class TInputImage, class TOutputImage>
void
ImageToStdGaborConvolutionFilter<TInputImage, TOutputImage>
::GenerateData()
{
  typename ImageListType::Pointer   imageList = ImageListType::New();; 
  
  // Get the intensity image if the nb components are greater than 1
  unsigned int nbComponents = this->GetInput()->GetNumberOfComponentsPerPixel();
  
  // Add the components to the list
  m_VectorImageToListFilter->SetInput(this->GetInput());
  m_VectorImageToListFilter->UpdateOutputInformation();
  
  // Add the channels of the image to the image list to add them to
  // the output image
  for(unsigned int i = 0; i < nbComponents; i++)
    {
    imageList->PushBack(m_VectorImageToListFilter->GetOutput()->GetNthElement(i));
    }
  
  /** add the intensity filter only if the image have more than one component*/
  if (nbComponents > 1)
    {
    m_IntensityFilter->SetInput(this->GetInput());
    imageList->PushBack(m_IntensityFilter->GetOutput());
    }
  
  // Standard Deviation computation of the gabored filterd image
  for (unsigned int currentDir = 0; currentDir<m_NumberOfDirection; currentDir++)
    {
    // Compute the current direction
    double currentDirection = m_InitialDirection + (360.0 / m_NumberOfDirection) * currentDir;
    
    typename GaborFilterGeneratorType::Pointer gaborFilter  = GaborFilterGeneratorType::New();
    // Generate the filter profile
    gaborFilter->SetTheta(-currentDirection);
    gaborFilter->SetU0(0.25*cos(-currentDirection/180.0*M_PI));
    gaborFilter->SetV0(0.25*sin(-currentDirection/180.0*M_PI));
    gaborFilter->SetA(m_A);
    gaborFilter->SetB(m_B);
    gaborFilter->SetRadius(m_Radius);
    gaborFilter->SetPhi(m_Phi);

    // Store it
    m_GaborFilterList.push_back(gaborFilter);

    // Perform convolution
    typename ConvolutionFilterType::Pointer conv = ConvolutionFilterType::New();
    conv->SetRadius(m_Radius);
    conv->NormalizeFilterOn();
    conv->SetFilter(gaborFilter->GetFilter());
    conv->SetInput(m_IntensityFilter->GetOutput());
    m_ConvolutionFilterList.push_back(conv);

    // Perform local variance
    typename VarianceFilterType::Pointer  varianceFilter = VarianceFilterType::New();
    varianceFilter->SetInput(conv->GetOutput());
    varianceFilter->SetRadius(this->GetVarianceRadius());
    m_VarianceFilterList.push_back(varianceFilter);

    // Perform square root
    typename SqrtImageFilterType::Pointer sqrt = SqrtImageFilterType::New();
    sqrt->SetInput(varianceFilter->GetOutput());
    m_SqrtImageFilterList.push_back(sqrt);
      
    // Add the image to the output image list
    imageList->PushBack(sqrt->GetOutput());
    }
  
  // Concatenate the output image
  m_ImageListToImageVectorFilter->SetInput(imageList);


  m_ImageListToImageVectorFilter->GraftOutput(this->GetOutput());
  m_ImageListToImageVectorFilter->Update();
  this->GraftOutput(m_ImageListToImageVectorFilter->GetOutput());
}


/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutput>
void
ImageToStdGaborConvolutionFilter<TInputImage, TOutput>
::GenerateOutputInformation() 
{
 // call the superclass's implementation of this method
 Superclass::GenerateOutputInformation();
 
 // Compute the number of components  : input number of components(N) +
 //  m_NumberOfDirection  
 unsigned int outputNumberOfComponentsPerPixel = 
   this->GetInput()->GetNumberOfComponentsPerPixel() + m_NumberOfDirection;
 
 // Add the intensity if vector image
 if (this->GetInput()->GetNumberOfComponentsPerPixel() > 1)
   outputNumberOfComponentsPerPixel += 1;
 
 if (this->GetOutput())
   {
   this->GetOutput()->SetLargestPossibleRegion(this->GetInput()->GetLargestPossibleRegion());
   this->GetOutput()->SetNumberOfComponentsPerPixel(outputNumberOfComponentsPerPixel);
   }
}

/**
 * Standard "PrintSelf" method
 */
template <class TInputImage, class TOutput>
void
ImageToStdGaborConvolutionFilter<TInputImage, TOutput>
::PrintSelf( std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf( os, indent );
}

}
#endif
