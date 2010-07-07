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

#ifndef __otbSpeckleFilteringModel_h
#define __otbSpeckleFilteringModel_h

#include "otbMVCModel.h"
#include "otbTypeManager.h"
#include "otbListenerBase.h"
#include "itkObject.h"
#include "itkObjectFactory.h"

#include "otbImage.h"
#include "otbFrostImageFilter.h"
#include "otbLeeImageFilter.h"

namespace otb
{
class ITK_EXPORT SpeckleFilteringModel
  : public MVCModel<ListenerBase>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef SpeckleFilteringModel         Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(SpeckleFilteringModel, MVCModel);
  itkNewMacro(Self);

  /** typedefs */
  typedef TypeManager::Floating_Point_Image InputImageType;

  typedef LeeImageFilter<InputImageType, InputImageType>   LeeFilterType;
  typedef FrostImageFilter<InputImageType, InputImageType> FrostFilterType;

  /** SetInputImage */
  itkSetObjectMacro(InputImage, InputImageType);
  itkGetObjectMacro(InputImage, InputImageType);

  /** Get Output Image */
  itkGetObjectMacro(Output, InputImageType);

  /** Get the output changed flag */
  itkGetMacro(OutputChanged, bool);

  /** Lee Filter Processing */
  virtual void LeeFiltering(unsigned int radius);

  /** Frost Filter Processing */
  virtual void FrostFiltering(unsigned int radius, double deRamp);

protected:
  /** Constructor */
  SpeckleFilteringModel();

  /** Destructor */
  virtual ~SpeckleFilteringModel();

private:
  SpeckleFilteringModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);

  InputImageType::Pointer m_InputImage;
  InputImageType::Pointer m_Output;

  LeeFilterType::Pointer   m_LeeFilter;
  FrostFilterType::Pointer m_FrostFilter;

  bool m_OutputChanged;

};

}
#endif
