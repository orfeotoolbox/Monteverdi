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
#ifndef __otbMeanShiftModule_h
#define __otbMeanShiftModule_h

// include the base class
#include "otbModule.h"

// the MVC classes
#include "otbMeanShiftModuleController.h"
#include "otbMeanShiftModuleModel.h"
#include "otbMeanShiftModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"

namespace otb
{
/** \class MeanShiftModule
 *    
 *  \brief This is the MeanShift module, which allows to perform
 *  MeanShift filtering, segmentation and clustering.
 * 
 *
 */

class ITK_EXPORT MeanShiftModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef MeanShiftModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(MeanShiftModule,Module);

  /** Data typedefs */
  /// Dataset
  typedef VectorImage<double,2>         FloatingVectorImageType;
  typedef Image<double,2>               FloatingImageType;
  typedef Image<unsigned long int,2>    LabelImageType;

  /** MVC typedefs */
  typedef otb::MeanShiftModuleController ControllerType;
  typedef otb::MeanShiftModuleModel      ModelType;
  typedef otb::MeanShiftModuleView       ViewType;


protected:
  /** Constructor */
  MeanShiftModule();

  /** Destructor */
  virtual ~MeanShiftModule();

  /** Notify Monteverdi application that featureExtraction has a result */
  void Notify();
  
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  MeanShiftModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  ControllerType::Pointer m_Controller;
  ViewType::Pointer m_View;
  ModelType::Pointer m_Model;
};


} // End namespace otb

#endif
