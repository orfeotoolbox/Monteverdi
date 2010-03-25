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
#ifndef __otbCachingModule_h
#define __otbCachingModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbCachingModuleGUI.h"

// include the OTB elements
#include "otbVectorImage.h"
#include "otbImageFileWriter.h"
#include "otbImageFileReader.h"


namespace otb
{
/** \class CachingModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT CachingModule
  : public Module, public CachingModuleGUI
{
public:
  /** Standard class typedefs */
  typedef CachingModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(CachingModule,Module);

  /** OTB typedefs */
  /// Dataset
  typedef VectorImage<double,2>         FloatingVectorImageType;
  typedef VectorImage<unsigned char,2>  CharVectorImageType;
  typedef Image<double,2>               FloatingImageType;

   /// Writers
  typedef ImageFileWriter<FloatingVectorImageType> FPVWriterType;
  typedef ImageFileWriter<CharVectorImageType>     CharVWriterType;
  typedef ImageFileWriter<FloatingImageType>       FPWriterType;

  /// Readers
  typedef ImageFileReader<FloatingVectorImageType> FPVReaderType;
  typedef ImageFileReader<CharVectorImageType>     CharVReaderType;
  typedef ImageFileReader<FloatingImageType>       FPReaderType;


  itkSetMacro(WatchProgress,bool);
  itkBooleanMacro(WatchProgress);

  itkSetMacro(EraseFile,bool);
  itkBooleanMacro(EraseFile);
  
  // Return the writing progress
  double GetProgress() const;

  /** Run the module */
  virtual void Run();

  //Remove the Caching directory
  bool RemoveCachingDirectory() const;
protected:
  /** Constructor */
  CachingModule();
  /** Destructor */
  virtual ~CachingModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Callbacks */
  virtual void ThreadedRun();
  virtual void ThreadedWatch();

  // Update the progress bar
  void UpdateProgress();

  // Hide the window
  void HideWindow();

  // Show the window
  void ShowWindow();

private:
  CachingModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  // Callback to update the window label
  static void UpdateProgressCallback(void * data);

  // Callback to show window
  static void ShowWindowCallback(void * data);

  // Callback to hide window
  static void HideWindowCallback(void * data);

  // Callback to Error reporter window
  static void SendErrorCallback(void * data);
   
  // The writing process
  itk::ProcessObject::Pointer m_WritingProcess;
  
  // The reading process
  itk::ProcessObject::Pointer m_ReadingProcess;

  // The caching path (could be later read from parameters file)
  std::string m_CachingPath;

  // The file path
  std::string m_FilePath;

  // Report visual progress
  bool m_WatchProgress;
  
  //error msg
  std::string m_ErrorMsg;
  
  //erase cached file
  bool m_EraseFile;
};


} // End namespace otb

#endif
