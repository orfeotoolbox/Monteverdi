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

#ifndef __otbProjectionModel_h
#define __otbProjectionModel_h

#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "itkObject.h"


#include "otbVectorImage.h"
#include "otbGenericRSTransform.h"

#include "otbStreamingResampleImageFilter.h"
#include "otbPerBandVectorImageFilter.h"


namespace otb
{
class ITK_EXPORT ProjectionModel
  : public MVCModel<ListenerBase>, public itk::Object
{

 public:
  /** Standard class typedefs */
  typedef ProjectionModel                           Self;
  typedef MVCModel<ListenerBase>                    Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;

  /** Standard type macro */
  itkTypeMacro(ProjectionModel, MVCModel);
  itkNewMacro(Self);

  /** typedefs */
  typedef double                                   PixelType;
  typedef VectorImage<PixelType,2>                 InputImageType;
  typedef InputImageType::IndexType                IndexType;
  typedef InputImageType::SizeType                 SizeType;
  typedef InputImageType::PointType                PointType;
  typedef InputImageType::SpacingType              SpacingType;
 
  /** typedef the Remote Sensing transform*/
  typedef GenericRSTransform<>                        TransformType;
  //typedef RSTransformType::TransformType            TransformType;
  typedef TransformType::OutputPointType              OutputPointType;

    /** Output */
  typedef Image<PixelType,2>                            SingleImageType;
  typedef StreamingResampleImageFilter<SingleImageType, SingleImageType, double>         ResampleFilterType;
  typedef PerBandVectorImageFilter<InputImageType, InputImageType, ResampleFilterType> PerBandFilterType;

  /** SetInputImage */
  itkSetObjectMacro(InputImage,InputImageType);
  itkGetObjectMacro(InputImage,InputImageType);

  /** Get Output Image */
  itkGetObjectMacro(Output,InputImageType);

  /** Get the output changed flag */
  itkGetMacro(OutputChanged,bool);

  /** Get the transform changed flag */
  itkGetMacro(TransformChanged,bool);

  /** Get the transform changed flag */
  itkGetMacro(TempTransformChanged,bool);

  /** Get a  reference on the Transform */
  itkGetObjectMacro(Transform,TransformType);

  /** Get the instance of the resampler*/
  itkGetObjectMacro(Resampler,ResampleFilterType);

  /** Get Spacing / Size & Origin*/
  itkGetMacro(OutputSize,SizeType);
  itkGetMacro(OutputOrigin,PointType);
  itkGetMacro(OutputSpacing,SpacingType);
  
  /** Compute the output region*/
  virtual void UpdateOutputParameters();

  /** Reproject the input image*/
  virtual void ReprojectImage();

  /** Tell the resampler to region to project*/
  virtual void ProjectRegion(unsigned int sizeX, unsigned int sizeY, double spacingX, double spacingY, double originX, double originY , bool isUl);

  /** Update the UTM Projection*/
  virtual void UpdateUTMTransform(int zone,bool north);

  /** Initialize the Lambert II Projection*/
  virtual void InitializeLambertIITransform();
  
  /** Initialize the transmecator proejction */
  virtual void UpdateTMTransform(double scale, double falseEasting , double falseNorthing );

    /** Update the Input UTM Projection*/
  virtual void UpdateInputUTMTransform(int zone,bool north);

  /** Initialize the Input Lambert II Projection*/
  virtual void InitializeInputLambertIITransform();
  
  /** Initialize the Input transmecator proejction */
  virtual void UpdateInputTMTransform(double scale, double falseEasting , double falseNorthing );

 protected:
  /** Constructor */
  ProjectionModel();

  /** Destructor */
  virtual ~ProjectionModel();

 private:
  ProjectionModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);
  
  InputImageType::Pointer       m_InputImage;
  InputImageType::Pointer       m_Output;
  bool                          m_OutputChanged;
  bool                          m_TransformChanged;
  bool                          m_TempTransformChanged;

  // Output Image Information
  SizeType                      m_OutputSize;
  PointType                     m_OutputOrigin;
  SpacingType                   m_OutputSpacing;
  
  // Instance of the composite transform
  TransformType::Pointer   m_Transform;
  TransformType::Pointer   m_InverseTransform;

  // Outputs
  ResampleFilterType::Pointer  m_Resampler;
  PerBandFilterType::Pointer   m_PerBander;

  char *                 m_OutputProjectionRef;
};
}
#endif


