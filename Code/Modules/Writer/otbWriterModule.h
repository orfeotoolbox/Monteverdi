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

#include "otbPixelType.h"

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
  typedef otb::Image<unsigned char>        UCharImageType;
  typedef otb::Image<unsigned short>       UShortImageType;
  typedef otb::Image<short>                ShortImageType;
  typedef otb::Image<unsigned int>         UIntImageType;
  typedef otb::Image<int>                  IntImageType;
  typedef otb::Image<float>                FloatImageType;
  typedef otb::Image<double>               DoubleImageType;

  typedef otb::VectorImage<unsigned char>  UCharVectorImageType;
  typedef otb::VectorImage<unsigned short> UShortVectorImageType;
  typedef otb::VectorImage<short>          ShortVectorImageType;
  typedef otb::VectorImage<unsigned int>   UIntVectorImageType;
  typedef otb::VectorImage<int>            IntVectorImageType;
  typedef otb::VectorImage<float>          FloatVectorImageType;
  typedef otb::VectorImage<double>         DoubleVectorImageType;

  typedef TypeManager::Vector_Data                 VectorType;
  typedef TypeManager::Labeled_Vector_Data         LabeledVectorType;

  typedef VectorDataFileWriter<VectorType>         VectorWriterType;
  typedef VectorDataFileWriter<LabeledVectorType>  LabeledVectorWriterType;

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

  template <typename TInputImage, typename TOutputImage>
  void DoWrite(TInputImage* image)
  {
    typedef itk::CastImageFilter<TInputImage,TOutputImage> CastFilterType;
    typedef otb::ImageFileWriter<TOutputImage> WriterType;

    typename CastFilterType::Pointer caster = CastFilterType::New();
    typename WriterType::Pointer     writer = WriterType::New();
    caster->SetInput( image );
    caster->SetInPlace( true );
    writer->SetInput( caster->GetOutput() );
    writer->SetFileName( m_Filename );
    m_ProcessObject =  writer;
    writer->Update();
  }


  template <typename TInputImage>
  void DoWriteSingleBand(TInputImage* image, PixelType pixType)
  {
    switch (pixType) {
    case otb::UNSIGNEDCHAR:
      this->DoWrite<TInputImage, UCharImageType> (image);
      break;
    case otb::SHORTINT:
      this->DoWrite<TInputImage, ShortImageType> (image);
      break;
    case otb::INT:
      this->DoWrite<TInputImage, IntImageType> (image);
      break;
    case otb::FLOAT:
      this->DoWrite<TInputImage, FloatImageType> (image);
      break;
    case otb::DOUBLE:
      this->DoWrite<TInputImage, DoubleImageType> (image);
      break;
    case otb::UNSIGNEDSHORTINT:
      this->DoWrite<TInputImage, UShortImageType> (image);
      break;
    case otb::UNSIGNEDINT:
      this->DoWrite<TInputImage, UIntImageType> (image);
      break;
    default:
      break;
    }
  }

  template <typename TInputImage>
  void DoWriteMultiBand(TInputImage* image, PixelType pixType)
  {
    switch (pixType) {
    case otb::UNSIGNEDCHAR:
      this->DoWrite<TInputImage, UCharVectorImageType> (image);
      break;
    case otb::SHORTINT:
      this->DoWrite<TInputImage, ShortVectorImageType> (image);
      break;
    case otb::INT:
      this->DoWrite<TInputImage, IntVectorImageType> (image);
      break;
    case otb::FLOAT:
      this->DoWrite<TInputImage, FloatVectorImageType> (image);
      break;
    case otb::DOUBLE:
      this->DoWrite<TInputImage, DoubleVectorImageType> (image);
      break;
    case otb::UNSIGNEDSHORTINT:
      this->DoWrite<TInputImage, UShortVectorImageType> (image);
      break;
    case otb::UNSIGNEDINT:
      this->DoWrite<TInputImage, UIntVectorImageType> (image);
      break;
    default:
      break;
    }
  }
  // Pointer to the process object
  itk::ProcessObject::Pointer m_ProcessObject;
  
  //error msg
  std::string m_ErrorMsg;

  //file name
  std::string m_Filename;


  std::map<otb::PixelType, std::string> m_OutputTypesChoices;
};


} // End namespace otb

#endif
