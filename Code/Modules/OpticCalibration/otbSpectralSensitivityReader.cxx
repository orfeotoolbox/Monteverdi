/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/

#include "otbSpectralSensitivityReader.h"

#include "base/ossimFilename.h"
#include <fstream>
#include "otbSpotImageMetadataInterface.h"
#include "itkExceptionObject.h"

namespace otb
{
SpectralSensitivityReader
::SpectralSensitivityReader()
{
  this->Superclass::SetNumberOfRequiredOutputs(1);
  this->Superclass::SetNthOutput(0, WavelenghtSpectralBandVectorType::New().GetPointer());

  m_FileName = "";
  m_DataPath = "";
  m_Image = ImageType::New();;
}


SpectralSensitivityReader
::~SpectralSensitivityReader()
{
}


SpectralSensitivityReader::WavelenghtSpectralBandVectorType *
SpectralSensitivityReader
::GetOutput()
{
  if (this->GetNumberOfOutputs()<1)
    {
      return 0;
    }
  return static_cast<WavelenghtSpectralBandVectorType *> (this->ProcessObject::GetOutput(0));
}


const SpectralSensitivityReader::WavelenghtSpectralBandVectorType *
SpectralSensitivityReader
::GetOutput() const
{
  if (this->GetNumberOfOutputs()<1)
    {
      return 0;
    }
  return static_cast<const WavelenghtSpectralBandVectorType *> (this->ProcessObject::GetOutput(0));
}


void
SpectralSensitivityReader
::FindFileName()
{
  if( m_Image.IsNull() )
    {
      itkExceptionMacro(<<"An input image has to be set or set directly the filename");
    }

  ossimString sensor("");
  itk::OStringStream oss;

  try
    {
      SpotImageMetadataInterface::Pointer lImageMetadata = SpotImageMetadataInterface::New();
      sensor = ossimString(lImageMetadata->GetSensorID( m_Image->GetMetaDataDictionary() ));
      sensor.upcase();
      // supress spaces
      for(unsigned int i = 0; i<sensor.size()-1; i++)
	{
	  if( sensor.compare(i, 1, " ")==0  )
	    {
	      sensor.erase(i, 1);
	      i--;
	    }
	}


      oss.str("");
      oss<<lImageMetadata->GetInstrument(m_Image->GetMetaDataDictionary());
      oss<<lImageMetadata->GetInstrumentIndex(m_Image->GetMetaDataDictionary());
    }
  catch( itk::ExceptionObject & err )
    {
      itkExceptionMacro(<<"Invalid input image");
    }

  std::string instrument = oss.str();

  oss.str("");
  oss<<m_DataPath;
  oss<< "/";
  oss<<sensor;
  oss<< "/";
  oss<<instrument;
  oss<<"/rep6S.dat";

  m_FileName = oss.str();
}


void
SpectralSensitivityReader
::GenerateData()
{
  if(m_FileName == "")
    this->FindFileName();
 
  WavelenghtSpectralBandVectorType * wavelenghtSpectralBand = this->GetOutput();

  ossimFilename fname(m_FileName);
  if(!fname.exists())
    itkExceptionMacro(<<m_FileName<<" does not exist.";);

  std::ifstream file( fname.c_str() );
  if ( !file )
    itkExceptionMacro(<<"Enable to read "<<fname<<" file.");

  std::string line;
  ossimString separator = " ";
  double mini = 0;
  double maxi = 0.;
  bool firstLine = true;

  unsigned int nbBands = 0;
  // used to store the coef
  std::vector<ValuesVectorType> valuesVector;

  while ( std::getline( file, line ) )
  {
    ossimString osLine(line);

    // Supress multiple spaces
    for(unsigned int i = 0; i<osLine.size()-1; i++)
      {
	if( osLine.compare(i, 1, " ")==0 && osLine.compare(i+1, 1, " ")==0 )
	  {
	    osLine.erase(i+1, 1);
	    i--;
	  }
      }
    // if the first character is a space, erase it
    if( osLine.compare(0, 1, " ")==0 )
      osLine.erase(0, 1);

    std::vector<ossimString> keywordStrings = osLine.split(separator);

    if(keywordStrings.size() < 3)
      itkExceptionMacro(<<"Invalid file format");

    // Store min wavelength
    if( firstLine )
      {
	mini = keywordStrings[0].toDouble();
	nbBands = keywordStrings.size()-2;
	for( unsigned int j=0; j<nbBands; j++ )
	  {
	    wavelenghtSpectralBand->PushBack( FilterFunctionValues::New() );
	    ValuesVectorType temp;
	    valuesVector.push_back( temp);
	  }
	firstLine = false;
      }
    
    if( nbBands != keywordStrings.size()-2 )
      itkExceptionMacro(<<"Invalid file format");

    for(unsigned int i=0; i<nbBands; i++)
      {
	valuesVector[i].push_back( keywordStrings[i+2].toDouble() );
      }

    maxi = keywordStrings[0].toDouble();
  }//while ( std::getline( file, line ) )
 

  
  for( unsigned int j=0; j<nbBands; j++ )
    {
      wavelenghtSpectralBand->GetNthElement(j)->SetFilterFunctionValues(valuesVector[j]);
      wavelenghtSpectralBand->GetNthElement(j)->SetMinSpectralValue(mini);
      wavelenghtSpectralBand->GetNthElement(j)->SetMaxSpectralValue(maxi);
    }  
}

 /**PrintSelf method */
void
SpectralSensitivityReader
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  os << "DataPath: " << m_DataPath << std::endl;
  if(m_FileName != "")
    os << "FileName: " << m_FileName << std::endl;

  // Function values print :
  const WavelenghtSpectralBandVectorType * wavelenghtSpectralBand = this->GetOutput();
  os << "Filter function values: " << std::endl;
  for (unsigned int i=0; i<wavelenghtSpectralBand->Size(); ++i)
  {
    os << indent << "Channel "<< i+1 <<" : " << std::endl;
    os << indent << wavelenghtSpectralBand->GetNthElement(i)<< std::endl;
  }
}


} // end namespace otb

