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



// include the OTB elements
#include "otbVectorImage.h"
#include "otbImageFileWriter.h"

#include "otbVectorData.h"
#include "otbVectorDataFileWriter.h"

#include "otbImage.h"
#include "otbImageList.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbImageListToVectorImageFilter.h"

#include "otbWriterController.h"
#include "otbWriterModel.h"
#include "otbWriterView.h"

namespace otb
{
/** \class WriterModule
 *  \brief 
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT WriterModule
  : public Module
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
  typedef VectorImage<double,2>         FPVImageType;
  typedef FPVImageType::Pointer         FPVImagePointerType;
  typedef VectorData<double>            VectorType;
  /// Writers
//   typedef ImageFileWriter<FPVImageType>    FPVWriterType;
//   typedef VectorDataFileWriter<VectorType> VectorWriterType;

  /** Typedefs for layers generation*/
//   typedef Image<double,2>                                                  SingleImageType;
//   typedef SingleImageType::Pointer                                         SingleImagePointerType;
//   typedef SingleImageType::PixelType                                       SinglePixelType;
//   typedef SingleImageType::InternalPixelType                               SingleInternalPixelType;
//   typedef SingleImageType::RegionType                                      SingleRegionType;
//   typedef SingleImageType::IndexType                                       SingleIndexType;
//   typedef SingleImageType::RegionType                                      SingleSpacingType;
//   typedef SingleImageType::SizeType                                        SingleSizeType;
//   typedef ImageList< SingleImageType >                                     ImageListType;
//   typedef ImageListType::Pointer                                           ImageListPointerType;
  
//   typedef VectorImageToImageListFilter<FPVImageType, ImageListType>       VectorToImageListType;
//   typedef ImageListToVectorImageFilter< ImageListType, FPVImageType >     ImageListToVectorImageFilterType;
  
  
  
  

  itkGetObjectMacro(View,WriterView);
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
//   virtual void OpenDataSet();
//   virtual void Browse();
//   virtual void Cancel();
  
  
  /** Write the data Options*/
//   virtual void GenerateOutputLayers();
//   virtual void RescaleOutputImage();
//   virtual void ExportPixelType();
  
  
  /** Manage the layer list */
//   void AddToOutputListOrder(int val)
//   {
//     m_OutputListOrder.push_back(val);
//     this->Modified();
//   };
//   void RemoveFromOutputListOrder(int id)
//   {
//     m_OutputListOrder.erase(m_OutputListOrder.begin()+id-1);
//     this->Modified();
//   };
//   
//   virtual void Show();
      
private:
  WriterModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

//   FPVWriterType::Pointer m_FPVWriter;
//   VectorWriterType::Pointer m_VectorWriter;
//   
  /** Layer generation*/
//   ImageListPointerType m_InputImageList;
//   ImageListPointerType m_OutputImageList;
  
  /** Contains the filter list order for outputs */
//   std::vector<unsigned int>         m_OutputListOrder;
  
  /** VectorImage for generation*/
//   FPVImagePointerType m_InputFPVImage;
//   FPVImagePointerType m_OutputFPVImage;
  
  /** Output Pixel Type*/
//   std::string m_OutputPixelType;
  
  /** Scale factor*/
//   double m_ScaleFactor;
  
  // The view
  WriterView::Pointer        m_View;

  // The controller
  WriterController::Pointer  m_Controller;

  // The model
  WriterModel::Pointer       m_Model;
};


} // End namespace otb

#endif
