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
#ifndef __otbWriterModule_h
#define __otbWriterModule_h

// include the base class
#include "otbModule.h"
// include the GUI
#include "otbWriterModuleGUI.h"
// include the OTB elements
#include "otbVectorImage.h"
#include "otbImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "otbImageToVectorImageCastFilter.h"
#include "otbVectorData.h"
#include "otbVectorDataFileWriter.h"

namespace otb
{
/** \class WriterModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT WriterModule
  : public Module, public WriterModuleGUI
{
public:
  /** Standard class typedefs */
  typedef WriterModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(WriterModule,Module);

  /** OTB typedefs */
  /// Dataset
  typedef VectorImage<double,2>                    FloatingVectorImageType;
  typedef VectorImage<unsigned char,2>             FloatingFloatVectorImageType;
  typedef VectorImage<unsigned char,2>             CharVectorImageType;
  typedef Image<double,2>                          FloatingImageType;
  typedef Image<unsigned short,2>                  UnsignedShortImageType;
  typedef VectorData<double>                       VectorType;
  typedef VectorData<double,2,short unsigned int>  LabeledVectorType;
  // Casters double ->float
  typedef itk::CastImageFilter<FloatingVectorImageType, FloatingFloatVectorImageType> DoubleToFloatCasterType;
  typedef ImageToVectorImageCastFilter<FloatingImageType, FloatingFloatVectorImageType> ImageDoubleToVImageFloatCasterType;
  /// Writers
  typedef ImageFileWriter<FloatingVectorImageType>      FPVWriterType;
  typedef ImageFileWriter<FloatingFloatVectorImageType> FFPVWriterType;
  typedef ImageFileWriter<CharVectorImageType>          CharVWriterType;
  typedef ImageFileWriter<FloatingImageType>            FPWriterType;
  typedef ImageFileWriter<UnsignedShortImageType>       USWriterType;
  typedef VectorDataFileWriter<VectorType>              VectorWriterType;
  typedef VectorDataFileWriter<LabeledVectorType>       LabeledVectorWriterType;



protected:
  /** Constructor */
  WriterModule();
  /** Destructor */
  virtual ~WriterModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void SaveDataSet();
  virtual void Browse();
  virtual void Cancel();
  virtual void ThreadedRun();
  virtual void ThreadedWatch();

  // Update the progress bar
  void UpdateProgress();

  // Hide the window
  void HideWindow();

private:
  // Callback to update the window label
  static void UpdateProgressCallback(void * data);

  // Callback to hide window
  static void HideWindowCallback(void * data);

  // Callback to Error reporter window
  static void SendErrorCallback(void * data);
  
  WriterModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

//   void SetIntensityChannelAvailability();
  // Pointer to the process object
  itk::ProcessObject::Pointer m_ProcessObject;
  
  //error msg
  std::string m_ErrorMsg;
};


} // End namespace otb

#endif
