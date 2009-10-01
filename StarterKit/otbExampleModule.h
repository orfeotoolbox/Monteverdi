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
#ifndef __otbExampleModule_h
#define __otbExampleModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbImage.h"
#include "itkPointSet.h"

namespace otb
{
/** \class ExampleModule
 *  \brief This is the Example module
 * 
 * Description of the module.
 *
 */

class ITK_EXPORT ExampleModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef ExampleModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ExampleModule,Module);

  /** Data typedefs */
  /// Dataset
  typedef VectorImage<double,2>         FloatingVectorImageType;
  typedef Image<double,2>               FloatingImageType;
  typedef VectorData<double>            VectorType;
  typedef itk::PointSet<double,2>       FloatingPointSetType;

protected:
  /** Constructor */
  ExampleModule();

  /** Destructor */
  virtual ~ExampleModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  ExampleModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};


} // End namespace otb

#endif
