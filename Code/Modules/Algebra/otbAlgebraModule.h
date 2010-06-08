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
#ifndef __otbAlgebraModule_h
#define __otbAlgebraModule_h

#include "otbModule.h"
#include "otbAlgebraGroup.h"

// include the OTB elements
#include "otbVectorImage.h"
#include "otbImage.h"

// Algebra opertor Filter
#include "itkAddImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkMultiplyImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkShiftScaleImageFilter.h"

namespace otb
{
/** \class AlgebraModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT AlgebraModule
  : public Module, public AlgebraGroup
{
public:
  /** Standard class typedefs */
  typedef AlgebraModule        Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(AlgebraModule,Module);
  
  // Convenient typedefs
  typedef TypeManager::Floating_Point_Image          ImageType;
  typedef TypeManager::Floating_Point_VectorImage    VectorImageType;

  // Algebra Class typedefs
  typedef itk::AddImageFilter<ImageType,ImageType,ImageType>              AddFilterType;
  typedef itk::SubtractImageFilter<ImageType,ImageType>                   SubtractFilterType;
  typedef itk::MultiplyImageFilter<ImageType,ImageType,ImageType>         MultiplyFilterType;
  typedef itk::DivideImageFilter<ImageType,ImageType,ImageType>           DivideFilterType;
  typedef itk::ShiftScaleImageFilter<ImageType,ImageType>                 ShiftScaleFilterType;
  
protected:
  /** Constructor */
  AlgebraModule();
  /** Destructor */
  virtual ~AlgebraModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true;};
  
  /** Show the Module GUI */
  virtual void Show()
  {
    guiMainWindow->show();
  };

  /** Hide the Module GUI */
  virtual void Hide()
  {
    guiMainWindow->hide();
  };

  /** Algebra Methods*/
  virtual void AddImage();
  virtual void SubtractImage();
  virtual void MultiplyImage();
  virtual void DivideImage();
  virtual void ShiftScaleImage();
                            
  /** OK callback*/
  virtual void OK();
  
private:
  AlgebraModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Class attributes
  AddFilterType::Pointer           m_AddFilter;
  SubtractFilterType::Pointer      m_SubtractFilter;
  MultiplyFilterType::Pointer      m_MultiplyFilter;
  DivideFilterType::Pointer        m_DivideFilter;
  ShiftScaleFilterType::Pointer    m_ShiftScaleFilter;
  
  ImageType::Pointer         m_InputImage1;
  ImageType::Pointer         m_InputImage2;
  ImageType::Pointer         m_Output;

  bool                      m_HasSecondInput;
};


} // End namespace otb

#endif
