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
#ifndef __otbWriterModel_h
#define __otbWriterModel_h

#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "itkObject.h"

#include "otbVectorImage.h"
#include "otbVectorData.h"
#include "otbImageFileWriter.h"
#include "otbVectorDataFileWriter.h"
namespace otb
{
/** \class WriterModel
 *  \brief 
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT WriterModel
  : public MVCModel<ListenerBase>, public itk::Object
{
public:
  /** Standard class typedefs */
  typedef WriterModel                 Self;
  typedef MVCModel<ListenerBase>                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
//   itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(WriterModel,Module);

  /** Images typedefs */
  typedef double                                                          PixelType;
  /** Inpout Image Type*/
  typedef VectorImage<PixelType,2>                                        InputImageType;
  typedef InputImageType::Pointer                                         InputImagePointerType;
  
  /** inpuyt vector data */
  typedef VectorData<double>            VectorType;
  /// Writers
  typedef ImageFileWriter<InputImageType>    FPVWriterType;
  typedef VectorDataFileWriter<VectorType> VectorWriterType;
  
  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();
  
  /** Open Input Image */
  void SetInputImage(std::string strfilename);
  
  /** Input Image Pointer */
  void SetInputImage(InputImagePointerType image);
  
  /** update writers*/
  void UpdateWriter(std::string & fname);
  void UpdateVectorWriter(std::string & fname);
  void UpdateImageWriter(std::string & fname);   
protected:
  
  /** This is protected for the singleton. Use GetInstance() instead. */
  itkNewMacro(Self);
  
  /** Constructor */
  WriterModel();
  /** Destructor */
  virtual ~WriterModel();
  /** PrintSelf method */
//   virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  
  
private:
  WriterModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The instance singleton */
  static Pointer Instance;
  
  FPVWriterType::Pointer m_FPVWriter;
  VectorWriterType::Pointer m_VectorWriter;
  
  /** Imput Image Pointer */
  InputImagePointerType m_InputImage;
  /** Input number of channels */
  unsigned int m_NumberOfChannels;
};


} // End namespace otb

#endif
