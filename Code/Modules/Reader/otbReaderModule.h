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

#include "otbGDALImageIO.h"

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
  typedef ReaderModule                  Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ReaderModule, Module);

  virtual void Analyse();
  /** OTB typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Image         FloatingImageType;
  typedef TypeManager::Floating_Point_Complex_Image ComplexImageType;
  typedef TypeManager::Floating_Point_VectorImage   FloatingVectorImageType;
  typedef TypeManager::Floating_Point_Complex_VectorImage   VectorComplexImageType;

  typedef TypeManager::Vector_Data         VectorType;
  typedef TypeManager::Labeled_Vector_Data LabeledVectorType;

  typedef TypeManager::FloatImageWithQuicklook ImageWithQuicklook;

  /// Readers
  typedef ImageFileReader<FloatingVectorImageType> FPVReaderType;
  typedef ImageFileReader<ComplexImageType>        ComplexImageReaderType;
  typedef ImageFileReader<VectorComplexImageType>  VComplexImageReaderType;
  typedef VectorDataFileReader<VectorType>         VectorReaderType;
  typedef VectorDataFileReader<LabeledVectorType>  LabeledVectorReaderType;

protected:
  /** Constructor */
  ReaderModule();
  /** Destructor */
  virtual ~ReaderModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void OpenDataSet();
  virtual void Browse();
  virtual void Cancel();

  virtual void TypeChanged();
  virtual void DatasetChanged();
  virtual void Hide();

  void OpenRealImage();
  void OpenRealImageWithQuicklook();

  void OpenComplexImage();

  void OpenMultiComplexImage();

  void OpenVector();

private:
  ReaderModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Readers
  FPVReaderType::Pointer           m_FPVReader;
  VectorReaderType::Pointer        m_VectorReader;
  ComplexImageReaderType::Pointer  m_ComplexReader;
  VComplexImageReaderType::Pointer m_VComplexReader;
  LabeledVectorReaderType::Pointer m_LabeledVectorReader;

  // Vector of string needed to manage hdf file
  std::vector<std::string>         m_Names;
  std::vector<std::string>         m_Desc;

  /** Flag needed to select the openSAR method */
  bool m_MultibandComplexImage;

  /** Detect if the file is a hdf file read its subdataset info. In our case a hdf file is composed of subdataset and readable with GDAL */
  bool IsHdfFile(std::string filepath);

  /** Detect if the file is a JP2 file read its subdataset info */
  bool IsJP2File(std::string filepath);

  /** Detect and replace "/" in m_Desc strings to avoid problem with Choice Widget vDataset*/
  bool CheckDataSetString();

};

} // End namespace otb

#endif
