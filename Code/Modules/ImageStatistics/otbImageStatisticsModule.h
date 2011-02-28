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
#ifndef __otbImageStatisticsModule_h
#define __otbImageStatisticsModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbImageStatisticsModuleGUI.h"

// include the OTB/ITK elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbStreamingStatisticsVectorImageFilter.h"
#include "otbStreamingMinMaxVectorImageFilter.h"

namespace otb
{
/** \class ImageStatisticsModule
 *  \brief This is the ImageStatistics module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT ImageStatisticsModule
  : public Module, public ImageStatisticsModuleGUI
{
public:
  /** Standard class typedefs */
  typedef ImageStatisticsModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ImageStatisticsModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Image       ImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef VectorImageType::SizeType SizeType; 

  typedef ImageToVectorImageCastFilter<ImageType, VectorImageType> ImageToVectorImageCastFilterType;

  typedef StreamingStatisticsVectorImageFilter<VectorImageType> StatisticsImageFilterType;
  typedef StatisticsImageFilterType::MatrixType MatrixType;

  typedef StreamingMinMaxVectorImageFilter<VectorImageType> MinMaxImageFilterType;
    
  /** Show the Module GUI */
  virtual bool CanShow(){return true; }
  /** Show the Module GUI */
  virtual void Show()
  {
    wMainWindow->show();
	wCovarianceWindow->hide();
  }

  virtual void Hide()
  {
    wMainWindow->hide();
	wCovarianceWindow->hide();
   }

  virtual void Quit();


protected:
  /** Constructor */
  ImageStatisticsModule();

  /** Destructor */
  virtual ~ImageStatisticsModule();

  /** Fill the text that represnts the covariance matrix */
  void UpdateCovarianceText();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Update statistic display fr a specified channel */
  virtual void UpdateChannelStat();

  /**Copute full stat : sum, mean, covariance values */
  virtual void ComputeFullStat();

private:
  ImageStatisticsModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ImageToVectorImageCastFilterType::Pointer m_ImageToVectorImageFilter;
  StatisticsImageFilterType::Pointer m_StatisticsFilter;
  MinMaxImageFilterType::Pointer m_MinMaxFilter;
	VectorImageType::Pointer m_InputImage;
  bool m_FullStat;
  };

} // End namespace otb

#endif
