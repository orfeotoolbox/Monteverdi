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
#ifndef __otbDEMToImageGeneratorModel_h
#define __otbDEMToImageGeneratorModel_h

#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbTypeManager.h"
#include "itkObject.h"

#include "otbVectorImage.h"
#include "otbGenericRSTransform.h"

#include "otbGenericRSResampleImageFilter.h"
#include "otbDEMToImageGenerator.h"
namespace otb
{
class ITK_EXPORT DEMToImageGeneratorModel
  : public MVCModel<ListenerBase>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef DEMToImageGeneratorModel               Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(DEMToImageGeneratorModel, MVCModel);
  itkNewMacro(Self);

  /** typedefs */
  typedef TypeManager::Floating_Point_Image       InputImageType;
  typedef InputImageType::IndexType               IndexType;
  typedef InputImageType::SizeType                SizeType;
  typedef InputImageType::PointType               PointType;
  typedef InputImageType::SpacingType             SpacingType;

  typedef TypeManager::Floating_Point_Image       SingleImageType;

  /** typedef the Remote Sensing transform*/
  typedef GenericRSTransform<>           TransformType;
  typedef TransformType::OutputPointType OutputPointType;

  typedef otb::DEMToImageGenerator<InputImageType> DEMToImageGeneratorType;
  /** Output */
  typedef GenericRSResampleImageFilter<InputImageType, 
                                       InputImageType>       ResampleFilterType;
  
  /** Get Output Image */
  itkGetObjectMacro(Output, InputImageType);

  /** SET/Get UseInputImage*/
  itkSetMacro(UseInputImage, bool);
  itkGetMacro(UseInputImage, bool);

  /** Get Spacing / Size & Origin*/
  itkGetMacro(OutputSize, SizeType);
  itkGetMacro(OutputOrigin, PointType);
  itkGetMacro(OutputSpacing, SpacingType);

  /** Get DEMToImage generator object */
  itkGetObjectMacro(DEMToImageGenerator,DEMToImageGeneratorType);

  /** Compute the output region*/
  virtual void UpdateOutputParametersFromImage(InputImageType::Pointer inputImage);

  /** Reproject the input image*/
  virtual void ReprojectImage();

  /** Tell the resampler to region to project*/
  virtual void ProjectRegion(unsigned int sizeX,
                             unsigned int sizeY,
                             double spacingX,
                             double spacingY,
                             double originX,
                             double originY);

  
  /** Set the DEM Path to the DEMToImageGenerator */
  void SetDEMDirectoryPath(const char* DEMDirectory);
  
protected:
  /** Constructor */
  DEMToImageGeneratorModel();

  /** Destructor */
  virtual ~DEMToImageGeneratorModel();

private:
  DEMToImageGeneratorModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);

  bool       m_UseInputImage;

  InputImageType::Pointer m_Output;

  // Output Image Information
  SizeType    m_OutputSize;
  PointType   m_OutputOrigin;
  SpacingType m_OutputSpacing;

  // Outputs
  DEMToImageGeneratorType::Pointer m_DEMToImageGenerator;

};
}
#endif
