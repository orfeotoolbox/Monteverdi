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

#ifndef __otbDEMToImageGeneratorControllerInterface_h
#define __otbDEMToImageGeneratorControllerInterface_h

#include "itkObject.h"
#include "otbDEMToImageGeneratorModel.h"

namespace otb
{
class ITK_ABI_EXPORT DEMToImageGeneratorControllerInterface
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef DEMToImageGeneratorControllerInterface   Self;
  typedef itk::Object                              Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  typedef DEMToImageGeneratorModel ModelType;

  /** Standard type macros */
  itkTypeMacro(DEMToImageGeneratorControllerInterface, Superclass);

  itkSetObjectMacro(Model, DEMToImageGeneratorModel);
  itkGetObjectMacro(Model, DEMToImageGeneratorModel);

  /** Virtual pure Methods*/
  virtual void ReprojectImage() = 0;
  virtual void ProjectRegion(unsigned int sizeX,
                             unsigned int sizeY,
                             double spacingX,
                             double spacingY,
                             double originX,
                             double originY)  = 0;
  virtual void SetDEMDirectoryPath(const char* DEMDirectory) = 0;
  virtual void ProcessHillShading(double azimutAngle,
                                  double elevationAngle,
                                  unsigned int radius) = 0;
  virtual void ProcessColorRelief(double min, double max, bool withHillShading) = 0;

protected:
  /** Constructor */
  DEMToImageGeneratorControllerInterface() {}
  /** Destructor */
  virtual ~DEMToImageGeneratorControllerInterface() {}

  /** The instance of the model (for commodity) */
  ModelType::Pointer m_Model;

private:
  DEMToImageGeneratorControllerInterface(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
