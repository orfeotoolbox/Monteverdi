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
#ifndef __otbViewerModule_h
#define __otbViewerModule_h

// include the base class
#include "otbModule.h"

// include the OTB elements
#include "otbVectorImage.h"
//#include "otbImageFileViewer.h"

#include "otbVectorData.h"
#include "otbVectorDataFileReader.h"

//standardImage Viewer
#include "otbStandardImageViewer.h"

namespace otb
{
/** \class ViewerModule
 *  \brief 
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT ViewerModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef ViewerModule                 Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ViewerModule,Module);

  /** OTB typedefs */
  /// Dataset
  typedef VectorImage<double,2>               ImageType;
  typedef VectorData<double>                  VectorType;
  typedef StandardImageViewer<ImageType>      ViewerType; 
  
  /*   typedef ImageFileReader<FPVImageType>    FPVViewerType; */
  /*   typedef VectorDataFileReader<VectorType> VectorViewerType; */

  /** Set InputImage*/
  itkSetObjectMacro(InputImage,ImageType);

protected:
  /** Constructor */
  ViewerModule();
  /** Destructor */
  virtual ~ViewerModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;


  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  //virtual void OpenDataSet();

  /** Assign input by key.
   *  When this method is called, key checking and data type matching
   *  is already done. */
  virtual void AssignInputByKey(const std::string & key, const DataObjectWrapper & data);
  
private:
  ViewerModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


  ImageType::Pointer                       m_InputImage;
  ViewerType::Pointer                      m_StandardViewer;
  
  /*   FPVViewerType::Pointer              m_FPVViewer; */
  /*   VectorViewerType::Pointer           m_VectorViewer; */
};


} // End namespace otb

#endif
