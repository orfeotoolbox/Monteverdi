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

#ifndef __otbProjectionControllerInterface_h
#define __otbProjectionControllerInterface_h

//#include "otbImageViewerFullResolutionEventsInterface.h"
#include "otbProjectionModel.h"
#include "itkObject.h"

namespace otb
{
class ITK_ABI_EXPORT ProjectionControllerInterface
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef ProjectionControllerInterface            Self;
  typedef itk::Object                              Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  typedef ProjectionModel ModelType;

  /** Standard type macros */
  itkTypeMacro(ProjectionControllerInterface, Superclass);

  itkSetObjectMacro(Model, ProjectionModel);
  itkGetObjectMacro(Model, ProjectionModel);

  /** Virtual pure Methods*/
  virtual void ReprojectImage() = 0;
  virtual void UpdateUTMTransform(int zone, bool north) = 0;
  virtual void UpdateEPSGTransform(int rsidNumber) = 0;
  virtual void InitializeLambertIITransform() = 0;
 // virtual void UpdateTMTransform(double scale, double falseEasting, double falseNorthing) = 0;
  virtual void ProjectRegion(unsigned int sizeX,
                             unsigned int sizeY,
                             double spacingX,
                             double spacingY,
                             double originX,
                             double originY)  = 0;
  virtual void UpdateInputUTMTransform(int zone, bool north) = 0;
  virtual void UpdateInputEPSGTransform(int rsidNumber) = 0;
  virtual void InitializeInputLambertIITransform() = 0;
 // virtual void UpdateInputTMTransform(double scale, double falseEasting, double falseNorthing) = 0;
  virtual void InitializeWGS84Transform() = 0;
  virtual void SetDEMPath(std::string dem) = 0;
  

protected:
  /** Constructor */
  ProjectionControllerInterface() {}
  /** Destructor */
  virtual ~ProjectionControllerInterface() {}

  /** The instance of the model (for commodity) */
  ModelType::Pointer m_Model;

private:
  ProjectionControllerInterface(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
