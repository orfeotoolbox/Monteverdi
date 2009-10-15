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

#ifndef __otbProjectionController_h
#define __otbProjectionController_h

#include "otbProjectionControllerInterface.h"
#include "otbProjectionView.h"


namespace otb
{
class ITK_EXPORT ProjectionController
      : public ProjectionControllerInterface
{
public:
  /** Standard class typedefs */
  typedef ProjectionController                     Self;
  typedef ProjectionControllerInterface            Superclass;
  typedef itk::SmartPointer<Self>                  Pointer;
  typedef itk::SmartPointer<const Self>            ConstPointer;

  typedef ProjectionView                           ViewType;
  typedef ViewType::Pointer                        ViewPointerType;

  /** Standard type macros */
  itkTypeMacro(ProjectionController,Superclass);
  itkNewMacro(Self);

  // Set the view
  void SetView(ViewPointerType pView)
    {
      m_View = pView;
    }
  
  // Methods
  virtual void ReprojectImage();
  virtual void UpdateUTMTransform(int zone,bool north);
  virtual void InitializeLambertIITransform();
  virtual void UpdateTMTransform(double scale, double falseEasting , double falseNorthing );
  virtual void ProjectRegion(unsigned int sizeX, 
			     unsigned int sizeY,
			     double spacingX,
			     double spacingY,
			     double originX,
			     double originY,
			     bool isUl);
  
 protected:
  /** Constructor */
  ProjectionController();
  /** Destructor */
  virtual ~ProjectionController();

private:
  ProjectionController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;
};
} //end namespace otb

#endif

