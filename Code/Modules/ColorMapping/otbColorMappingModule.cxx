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
#include "itkScalarToRGBColormapImageFilter.h"
#include "otbMsgReporter.h"

namespace otb
{

/**
 * Constructor
 */
ColorMappingModule
::ColorMappingModule()
{
  m_ColorMapFilter = ColorMapFilterType::New();
  m_RGBtoVectorImageCastFilter = RGBtoVectorImageCastFilterType::New();

  // Describe inputs
  this->AddInputDescriptor<SingleImageType>("InputImage", otbGetTextMacro("Scalar image to process"));

  // Build the GUI
  this->BuildGUI();
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

  // open the GUI
  this->Show();
  this->BusyOff();
}


void
ColorMappingModule
::ColorMappingProcess()
{
  std::string ColorMapName;

  m_ColorMapFilter->SetInput( m_InputImage );

  switch (iColorMap->value())
  {
    case 0 :
      ColorMapName = "Red";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Red );
      break;
    case 1 :
      ColorMapName = "Green";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Green );
      break;
    case 2 :
      ColorMapName = "Blue";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Blue );
      break;
    case 3 :
      ColorMapName = "Grey";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Grey );
      break;
    case 4 :
      ColorMapName = "Hot";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Hot );
      break;
    case 5 :
      ColorMapName = "Cool";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Cool );
      break;
    case 6 :
      ColorMapName = "Spring";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Spring );
      break;
    case 7 :
      ColorMapName = "Summer";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Summer );
      break;
    case 8 :
      ColorMapName = "Autumn";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Autumn );
      break;
    case 9 :
      ColorMapName = "Winter";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Winter );
      break;
    case 10 :
      ColorMapName = "Copper";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Copper );
      break;
    case 11 :
      ColorMapName = "Jet";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::Jet );
      break;
    case 12 :
      ColorMapName = "HSV";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::HSV );
      break;
    case 13 :
      ColorMapName = "OverUnder";
      m_ColorMapFilter->SetColormap( ColorMapFilterType::OverUnder );
      break;
    default:
      itkExceptionMacro(<< "Colormap not implemented");
      break;
  }

  m_RGBtoVectorImageCastFilter->SetInput( m_ColorMapFilter->GetOutput() );

  this->ClearOutputDescriptors();
  this->AddOutputDescriptor(m_RGBtoVectorImageCastFilter->GetOutput(),
                            ColorMapName +" ColorMap Image",
                            otbGetTextMacro("RGB ColorMap Image") );
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
