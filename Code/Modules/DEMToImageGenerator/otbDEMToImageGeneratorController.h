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

#ifndef __otbDEMToImageGeneratorController_h
#define __otbDEMToImageGeneratorController_h

#include "otbDEMToImageGeneratorControllerInterface.h"
#include "otbDEMToImageGeneratorView.h"

namespace otb
{
class ITK_EXPORT DEMToImageGeneratorController
  : public DEMToImageGeneratorControllerInterface
{
public:
  /** Standard class typedefs */
  typedef DEMToImageGeneratorController          Self;
  typedef DEMToImageGeneratorControllerInterface Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  typedef DEMToImageGeneratorView    ViewType;
  typedef ViewType::Pointer ViewPointerType;

  /** Standard type macros */
  itkTypeMacro(DEMToImageGeneratorController, Superclass);
  itkNewMacro(Self);

  // Set the view
  void SetView(ViewPointerType pView)
  {
    m_View = pView;
  }

  // Methods
  virtual void ReprojectImage();
  virtual void ProjectRegion(unsigned int sizeX,
                             unsigned int sizeY,
                             double spacingX,
                             double spacingY,
                             double originX,
                             double originY);
  virtual void SetDEMDirectoryPath(const char* DEMDirectory);
protected:
  /** Constructor */
  DEMToImageGeneratorController();
  /** Destructor */
  virtual ~DEMToImageGeneratorController();

private:
  DEMToImageGeneratorController(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;
};
} //end namespace otb

#endif
