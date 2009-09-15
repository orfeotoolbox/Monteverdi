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
#ifndef __otbSpeckleFilteringControllerInterface_h
#define __otbSpeckleFilteringControllerInterface_h

#include "otbImageViewerFullResolutionEventsInterface.h"
#include "otbSpeckleFilteringModel.h"


namespace otb
{
class ITK_EXPORT  SpeckleFilteringControllerInterface
      : public ImageViewerFullResolutionEventsInterface
{
public:
  /** Standard class typedefs */
  typedef SpeckleFilteringControllerInterface       Self;
  typedef ImageViewerFullResolutionEventsInterface  Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;
  

  typedef SpeckleFilteringModel                     ModelType;

  /** Standard type macros */
  itkTypeMacro(SpeckleFilteringControllerInterface,Superclass);

  /** Users actions */
  virtual void ProcessLeeFilter(unsigned int radius) =0;
  virtual void ProcessFrostFilter(unsigned int radius, double DeRamp) =0;
  
protected:
  /** Constructor */
  SpeckleFilteringControllerInterface()
  {
    m_Model = ModelType::GetInstance();
  }
  /** Destructor */
  ~SpeckleFilteringControllerInterface() {};

  /** The instance of the model (for commodity) */
  ModelType * m_Model;

private:
  SpeckleFilteringControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
