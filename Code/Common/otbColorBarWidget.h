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
#ifndef __otbColorBarWidget_h
#define __otbColorBarWidget_h

#include "FL/Fl_Window.H"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkMacro.h"

#include "otbModule.h"
#include "otbImage.h"
#include "otbVectorImage.h"
#include "itkScalarToRGBColormapImageFilter.h"
#include "otbRGBImageToVectorImageCastFilter.h"


namespace otb
{
/** \class ColorBarWidget
 *  \brief
 *
 */
class ColorBarWidget
      : public itk::Object, public Fl_Window
{
public:
  /** Standard class typedefs */
  typedef ColorBarWidget                Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(ColorBarWidget, Object);

  // Convenient typedefs
  typedef TypeManager::Floating_Point_Precision     Floating_Point_PrecisionType;
  typedef TypeManager::Floating_Point_Image         SingleImageType;
  typedef TypeManager::Floating_Point_VectorImage   OutputImageType;


  typedef itk::RGBPixel<unsigned char>              RGBPixelType;
  typedef otb::Image<RGBPixelType, 2>               RGBImageType;

  // ColorMapping Class typedefs
  typedef itk::ScalarToRGBColormapImageFilter<SingleImageType, RGBImageType>  ColorMapFilterType;
  typedef ColorMapFilterType::ColormapType ColormapType;
  typedef otb::RGBImageToVectorImageCastFilter<RGBImageType, OutputImageType> RGBtoVectorImageCastFilterType;


  /** Initialize the widget */
  virtual void Init(int x, int y, int w, int h, const char * l);


  /** Set Colormap */
  itkSetMacro(Colormap, ColormapType::Pointer);

protected:
  /** Constructor */
  ColorBarWidget();
  /** Destructor */
  ~ColorBarWidget();

  /** Draw the widget */
  virtual void draw(void);

private:
  ColorBarWidget(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

  ColormapType::Pointer     m_Colormap;
  SingleImageType::Pointer  m_ScalarImage;

};
} // end namespace otb
#endif
