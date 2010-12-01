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
#ifndef __otbColorMappingModule_cxx
#define __otbColorMappingModule_cxx

#include "otbColorMappingModule.h"
#include "otbMsgReporter.h"
#include "itkMetaDataDictionary.h"
#include "otbImageKeywordlist.h"
#include "itkMetaDataObject.h"
#include "base/ossimKeywordlist.h"


namespace otb
{

/**
 * Constructor
 */
ColorMappingModule
::ColorMappingModule()
{
  m_ShiftScaleImageFilter = ShiftScaleImageFilterType::New();
  m_ColorMapFilter = ColorMapFilterType::New();
  m_ReliefColorMapFunctor = ReliefColorMapFunctorType::New();
  m_RGBtoVectorImageCastFilter = RGBtoVectorImageCastFilterType::New();
  m_ColorBarWidget = ColorBarWidgetType::New();
  m_ColorBarWidget->SetColormap(m_ColorMapFilter->GetColormap());

  // Describe inputs
  this->AddInputDescriptor<SingleImageType>("InputImage", otbGetTextMacro("Scalar image to process"));

}

/**
 * Destructor
 */
ColorMappingModule
::~ColorMappingModule()
{}

/** The custom run command */
void
ColorMappingModule
::Run()
{
  this->BusyOn();

  m_InputImage = this->GetInputData<SingleImageType>("InputImage");
  if (m_InputImage.IsNull())
    {
      this->BusyOff();
      itkExceptionMacro(<< "Input image is NULL");
    }


  // Build the GUI
  this->BuildGUI();
  m_ColorBarWidget->Init(oColorBar->x(),oColorBar->y(),
                   oColorBar->w(),oColorBar->h(),"Color Bar");
  oColorBar->add(m_ColorBarWidget);
  oColorBar->box(FL_NO_BOX);
  m_ColorBarWidget->show();
  m_ColorBarWidget->redraw();

  this->UpdateColorBar();
  // open the GUI
  this->Show();
  this->BusyOff();
}


void
ColorMappingModule
::ColorMappingProcess()
{
  std::string ColorMapName;
  double shift = (oShift->value());
  double scale = (oScale->value());

  m_ShiftScaleImageFilter->SetInput(m_InputImage);
  m_ShiftScaleImageFilter->SetShift(shift);
  m_ShiftScaleImageFilter->SetScale(scale);


  m_InputImage->UpdateOutputInformation();
  //// change SENSOR ID IN KWL TO ESCAPE THE DEFAULT PIXEL DISPLAY
  // Load MetaDataDictionary
  itk::MetaDataDictionary dict = m_InputImage->GetMetaDataDictionary();
  ImageKeywordlist imageKeywordlist;
  // Extract ossimKWL
  if (dict.HasKey(MetaDataKey::OSSIMKeywordlistKey))
    {
      itk::ExposeMetaData<ImageKeywordlist>(dict, MetaDataKey::OSSIMKeywordlistKey, imageKeywordlist);
    }
  // Change sensor to empty string
  ossimKeywordlist         kwl;
  imageKeywordlist.convertToOSSIMKeywordlist(kwl);
  kwl.add("", "sensor", "");

  // new kwl to save the ossimkwl
  ImageKeywordlist otb_kwl;
  otb_kwl.SetKeywordlist(kwl);
  // Save in the MetaDataDictionary of the image
  itk::EncapsulateMetaData<ImageKeywordlist>(dict, MetaDataKey::OSSIMKeywordlistKey, otb_kwl);

  // Update the output MetaDataDictionary
  m_ColorMapFilter->GetOutput()->SetMetaDataDictionary(dict);


  m_ColorMapFilter->SetInput(m_ShiftScaleImageFilter->GetOutput());
  m_ColorMapFilter->UseInputImageExtremaForScalingOff();

  m_RGBtoVectorImageCastFilter->SetInput(m_ColorMapFilter->GetOutput());

  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_RGBtoVectorImageCastFilter->GetOutput(),
                            m_ColormapName +" ColorMap Image",
                            otbGetTextMacro("RGB ColorMap Image") );
}


/** Cancel */
void
ColorMappingModule
::UpdateColorBar()
{
  ColorMapFilterType::ColormapEnumType     colormapEnum;

  switch (iColorMap->value())
  {
    case 0 :
      m_ColormapName = "Red";
      colormapEnum = ColorMapFilterType::Red;
      break;
    case 1 :
      m_ColormapName = "Green";
      colormapEnum = ColorMapFilterType::Green;
      break;
    case 2 :
      m_ColormapName = "Blue";
      colormapEnum = ColorMapFilterType::Blue;
      break;
    case 3 :
      m_ColormapName = "Grey";
      colormapEnum = ColorMapFilterType::Grey;
      break;
    case 4 :
      m_ColormapName = "Hot";
      colormapEnum = ColorMapFilterType::Hot;
      break;
    case 5 :
      m_ColormapName = "Cool";
      colormapEnum = ColorMapFilterType::Cool;
      break;
    case 6 :
      m_ColormapName = "Spring";
      colormapEnum = ColorMapFilterType::Spring;
      break;
    case 7 :
      m_ColormapName = "Summer";
      colormapEnum = ColorMapFilterType::Summer;
      break;
    case 8 :
      m_ColormapName = "Autumn";
      colormapEnum = ColorMapFilterType::Autumn;
      break;
    case 9 :
      m_ColormapName = "Winter";
      colormapEnum = ColorMapFilterType::Winter;
      break;
    case 10 :
      m_ColormapName = "Copper";
      colormapEnum = ColorMapFilterType::Copper;
      break;
    case 11 :
      m_ColormapName = "Jet";
      colormapEnum = ColorMapFilterType::Jet;
      break;
    case 12 :
      m_ColormapName = "HSV";
      colormapEnum = ColorMapFilterType::HSV;
      break;
    case 13 :
      m_ColormapName = "OverUnder";
      colormapEnum = ColorMapFilterType::OverUnder;
      break;
    case 14 :
      m_ColormapName = "Relief";
      break;
    default:
      itkExceptionMacro(<< "Colormap not implemented");
      break;
  }

  if(m_ColormapName != "Relief")
    {
    m_ColorMapFilter->SetColormap(colormapEnum);
    }
  else
    {
    m_ColorMapFilter->SetColormap( m_ReliefColorMapFunctor);

    }

  m_ColorBarWidget->SetColormap(m_ColorMapFilter->GetColormap());
  m_ColorBarWidget->redraw();
}

/** Cancel */
void
ColorMappingModule
::Cancel()
{
  this->Hide();
}

void
ColorMappingModule
::OK()
{
  this->ColorMappingProcess();
  this->NotifyOutputsChange();

  // close the GUI
  this->Hide();
  this->BusyOff();
}

} // End namespace otb

#endif
