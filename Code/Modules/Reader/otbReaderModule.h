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
#ifndef __otbReaderModule_h
#define __otbReaderModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbReaderModuleGUI.h"

// include the OTB elements
#include "otbVectorImage.h"
#include "otbImageFileReader.h"

#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"

namespace otb
{
/** \class ReaderModule
 *  \brief 
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT ReaderModule
  : public Module, public ReaderModuleGUI
{
public:
  /** Standard class typedefs */
  typedef ReaderModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ReaderModule,Module);

  /** OTB typedefs */
  /// Dataset
  typedef VectorImage<double,2>         FPVImageType;
  typedef VectorData<double>            VectorType;
  /// Readers
  typedef ImageFileReader<FPVImageType>    FPVReaderType;
  typedef VectorDataFileReader<VectorType> VectorReaderType;

protected:
  /** Constructor */
  ReaderModule();
  /** Destructor */
  virtual ~ReaderModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Retrieve output by key  This method must be reimplemented in subclasses.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual const DataObjectWrapper RetrieveOutputByKey(const std::string & key) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void OpenDataSet();
  virtual void Browse();
  virtual void Cancel();

private:
  ReaderModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  FPVReaderType::Pointer m_FPVReader;
  VectorReaderType::Pointer m_VectorReader;
};


} // End namespace otb

#endif
