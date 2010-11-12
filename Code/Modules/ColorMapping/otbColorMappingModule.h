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
#ifndef __otbColorMappingModule_h
#define __otbColorMappingModule_h

#include "otbModule.h"
#include "otbColorMappingModuleGUI.h"

// include the OTB elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "otbRGBImageToVectorImageCastFilter.h"

#include "otbColorBarWidget.h"

namespace otb
{
/** \class ColorMappingModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT ColorMappingModule
  : public Module, public ColorMappingModuleGUI
{
public:
  /** Standard class typedefs */
  typedef ColorMappingModule          Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(ColorMappingModule, Module);

  // Convenient typedefs
  typedef TypeManager::Floating_Point_Precision     Floating_Point_PrecisionType;
  typedef TypeManager::Floating_Point_Image         SingleImageType;
  typedef TypeManager::Floating_Point_VectorImage   OutputImageType;


  typedef itk::RGBPixel<unsigned char>              RGBPixelType;
  typedef otb::Image<RGBPixelType, 2>               RGBImageType;

    // ColorMapping Class typedefs
  typedef itk::ScalarToRGBColormapImageFilter<SingleImageType, RGBImageType>  ColorMapFilterType;
  typedef otb::RGBImageToVectorImageCastFilter<RGBImageType, OutputImageType> RGBtoVectorImageCastFilterType;

  typedef otb::ColorBarWidget ColorBarWidgetType;

protected:
  /** Constructor */
  ColorMappingModule();
  /** Destructor */
  virtual ~ColorMappingModule();

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    wMainWindow->show();
  }

  /** Show the Module GUI */
  virtual void Hide()
  {
    wMainWindow->hide();
  }


  /** Callbacks */
  virtual void OK();
  virtual void Cancel();

  /** ColorMapping Methods*/
  virtual void ColorMappingProcess();

  /** Update color bar*/
  virtual void UpdateColorBar();

private:
  ColorMappingModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Class attributes
  SingleImageType::Pointer        m_InputImage;
  OutputImageType::Pointer        m_OutputImage;

  ColorMapFilterType::Pointer              m_ColorMapFilter;
  RGBtoVectorImageCastFilterType::Pointer  m_RGBtoVectorImageCastFilter;

  ColorBarWidgetType::Pointer              m_ColorBarWidget;
  std::string                              m_ColormapName;
};

} // End namespace otb

#endif
