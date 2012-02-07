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
  virtual void ThreadedRun();
  virtual void ThreadedWatch();

  /** Update the progress bar */
  void UpdateProgress();
  

  void OpenRealImage();
  void OpenRealImageWithQuicklook();

  void OpenComplexImage();

  void OpenMultiComplexImage();

  void OpenVector();
  
  virtual void RawTypeSetup();

private:
  ReaderModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Callback to update the window label */
  static void UpdateProgressCallback(void * data);

  /** Callback to hide window */
  static void HideWindowCallback(void * data);

  /** Callback to Error reporter window */
  static void SendErrorCallback(void * data);

  // Readers
  FPVReaderType::Pointer           m_FPVReader;
  VectorReaderType::Pointer        m_VectorReader;
  ComplexImageReaderType::Pointer  m_ComplexReader;
  VComplexImageReaderType::Pointer m_VComplexReader;
  LabeledVectorReaderType::Pointer m_LabeledVectorReader;

  // Vector of string needed to manage hdf file
  std::vector<std::string>         m_Names;
  std::vector<std::string>         m_Desc;
  bool m_TypeHdf;
  bool m_TypeJPEG2000;

  /** Flag needed to select the openSAR method */
  bool m_MultibandComplexImage;

  /** Detect if the file is a hdf file read its subdataset info. In our case a hdf file is composed of subdataset and readable with GDAL */
  bool IsHdfFile(std::string filepath);

  /** Detect if the file is a JPEG2000 file and read its resolution info. In our case a JPEG2000 file is readable with JPEG2000ImageIO */
  bool IsJPEG2000File(std::string filepath);

  /** Detect and replace "/" in m_Desc strings to avoid problem with Choice Widget vDataset*/
  bool CheckDataSetString();

  /* Get the coarsest resolution overview from the file
   * This function is optimized for JPEG2000 files
   * If the file format does not provide support for overviews,
   * use the standard StreamingShrinkImageFilter
   *
   * TODO: support GDAL Overviews */
  FloatingVectorImageType::Pointer MakeQuicklook(std::string filepath, unsigned int& shrinkFactor);
  
  /** Pointer to a process object (containing the quicklook reader) */
  itk::ProcessObject::Pointer m_ProcessObject;
  
  /** Error message */
  std::string m_ErrorMsg;
  
  /** Image with quicklook */
  ImageWithQuicklook::Pointer m_ImageWithQL;
  
  /** File name for image with quicklook */
  std::string   m_Filepath;
  
  /** Shrink factor for quicklook */
  unsigned int m_ShrinkFactor;
  
  /** Resolution for quicklook */
  unsigned int m_Resolution;
  
  /** Key for quicklook */
  std::string m_KeyForQL;
  
  /** Desc for quicklook */
  std::string m_DescForQL;
  
  unsigned int m_progressIndex;
  
  /** map for Envi pixel type in hdr files  */
  int m_MapEnviPixelType[11];
};

} // End namespace otb

#endif
