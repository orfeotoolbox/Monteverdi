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
#ifndef __otbFineCorrelationModule_h
#define __otbFineCorrelationModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "itkFixedArray.h"
#include "itkUnaryFunctorImageFilter.h"
#include "otbFineCorrelationImageFilter.h"
#include "otbFineCorrelationGroup.h"

namespace otb
{

template<class TInput, class TOutput>
class FieldComponentExtractor
{
public:
  // Operator on vector pixel type
  inline TOutput operator()(const TInput & inputVector)
  {
    return static_cast<TOutput>(inputVector[m_Index]);
  }

  FieldComponentExtractor() : m_Index(0) {};
  virtual ~FieldComponentExtractor() {};

  void SetIndex(unsigned int channel)
  {
    m_Index = channel;
  }
  unsigned int GetIndex()
  {
    return m_Index;
  }

private:
  unsigned int m_Index;
};


/** \class FineCorrelationModule
 *  \brief This is the Fine Correlation module
 *
 * Description of the module.
 *
 */

class ITK_EXPORT FineCorrelationModule
  : public Module, public FineCorrelationGroup
{
public:
  /** Standard class typedefs */
  typedef FineCorrelationModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(FineCorrelationModule,Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Precision                         PixelType;
  typedef TypeManager::Floating_Point_Image                             ImageType;
  typedef TypeManager::Deformation_Field_Type                           DeformationFieldType;
  typedef TypeManager::Deformation_Value_Type                           DeformationValueType;
  
  typedef FineCorrelationImageFilter<ImageType,ImageType,DeformationFieldType> CorrelationFilterType;
  typedef FieldComponentExtractor<DeformationValueType, PixelType> ExtractFunctorType;
  typedef itk::UnaryFunctorImageFilter<DeformationFieldType, ImageType, ExtractFunctorType> ExtractFilterType; 
  
protected:
  /** Constructor */
  FineCorrelationModule();

  /** Destructor */
  virtual ~FineCorrelationModule();

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

  /** Callback for the OK button */
  virtual void OK();


private:
  FineCorrelationModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  CorrelationFilterType::Pointer m_CorrelationFilter;
  ExtractFilterType::Pointer m_ExtractXField;
  ExtractFilterType::Pointer m_ExtractYField;
};


} // End namespace otb

#endif
