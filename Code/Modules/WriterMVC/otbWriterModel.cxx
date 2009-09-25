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
#ifndef __otbWriterModel_cxx
#define __otbWriterModel_cxx

#include "otbWriterModel.h"
#include "itkExceptionObject.h"
#include "otbMacro.h"
#include "otbOGRVectorDataIO.h"

namespace otb
{
  
  /** Initialize the singleton */
WriterModel::Pointer WriterModel::Instance = NULL;
  
  
/** Constructor */
WriterModel::WriterModel()
{
  m_FPVWriter = FPVWriterType::New();
  m_VectorWriter = VectorWriterType::New();
  m_InputImage = InputImageType::New(); 
}

/** Destructor */
WriterModel::~WriterModel()
{}

/** Manage the singleton */
WriterModel::Pointer
    WriterModel::GetInstance()
    {
      if (!Instance)
      {
        Instance = WriterModel::New();
      }
      return Instance;
    }
    
void
WriterModel
::SetInputImage(InputImagePointerType image)
  {
// Set the input image
    m_InputImage = image;
    m_InputImage->UpdateOutputInformation();
    
    m_FPVWriter->SetInput(m_InputImage);
    
    m_NumberOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();

// Notify the observers
    this->NotifyAll();
  }

void WriterModel
::UpdateWriter(std::string & fname)
{
  typedef OGRVectorDataIO<VectorType> OGRVectorDataIOType;
  OGRVectorDataIOType::Pointer test=OGRVectorDataIOType::New() ;
  
  if ( test->CanWriteFile(fname.c_str()) ) 
  {
//     m_VectorWriter->SetFileName(filepath);
//     m_VectorWriter->Update();
    this->UpdateVectorWriter(fname);
  }
  else
  {
//     m_FPVWriter->SetFileName(filepath);
//     m_FPVWriter->Update();
    this->UpdateImageWriter(fname);
  }
}
void WriterModel
::UpdateVectorWriter(std::string & fname)
{
  m_VectorWriter->SetFileName(fname);
  m_VectorWriter->Update();
}
void WriterModel
::UpdateImageWriter(std::string & fname)   
{
  m_FPVWriter->SetFileName(fname);
  m_FPVWriter->Update();
}
} // End namespace otb

#endif
