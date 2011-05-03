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
#ifndef __otbReaderModule_cxx
#define __otbReaderModule_cxx

#include "otbReaderModule.h"

#include "itksys/SystemTools.hxx"
#include "FLU/Flu_File_Chooser.h"

#include "otbMsgReporter.h"
#include "otbI18n.h"

namespace otb
{
/** Constructor */
ReaderModule::ReaderModule()
{
  // Build filters
  m_FPVReader = FPVReaderType::New();
  m_VectorReader = VectorReaderType::New();
  m_LabeledVectorReader = LabeledVectorReaderType::New();
  m_ComplexReader = ComplexImageReaderType::New();
  m_VComplexReader = VComplexImageReaderType::New();

  m_MultibandComplexImage = false;

  this->BuildGUI();

  // Expose supported data type:
  vType->add(otbGetTextMacro("Unknown"));
  vType->add(otbGetTextMacro("Optical image"));
  vType->add(otbGetTextMacro("SAR image"));
  vType->add(otbGetTextMacro("Vector Data"));
  vType->value(0);

  // Deactivate ok for now
  bOk->deactivate();

  // No name for now
  vName->value("");
}

/** Destructor */
ReaderModule::~ReaderModule()
{}

/** PrintSelf method */
void ReaderModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void ReaderModule::Run()
{
  wFileChooserWindow->show();
}

void ReaderModule::Analyse()
{
  // Get the path
  std::string filepath = vFilePath->value();

  // Is type found ?
  bool typeFound = false;

  // Is hdf type
  bool typeHdf = IsHdfFile(filepath); // For us a hdf file is composed of subdataset and readable with GDAL

  if (typeHdf)
    {
    CheckDataSetString();
    // Fill vDataset with subdataset descriptor info
    for (unsigned int itSubDataset = 0; itSubDataset < (unsigned int) m_Desc.size(); itSubDataset++)
      {
      vDataset->add(m_Desc[itSubDataset].c_str());
      }
    vDataset->set_visible();
    vDataset->value(0);

    vType->value(1); // We assume that hdf file is composed of optical image
    bOk->activate();
    }
  else
    {
    // Try different types
    try
      {
      // Read the image
      m_FPVReader->SetFileName(filepath);
      m_FPVReader->GenerateOutputInformation();

      // Special action if we use the GDAL image IO
      if (strcmp(m_FPVReader->GetImageIO()->GetNameOfClass(), "GDALImageIO") == 0)
        {
        if ((dynamic_cast<GDALImageIO*> (m_FPVReader->GetImageIO()))->GDALPixelTypeIsComplex())
          { // Complex Data
          vType->value(2);
          typeFound = true;
          // Detect if it is a Mono or Multiband Complex Image
          if (m_FPVReader->GetImageIO()->GetNumberOfComponents() / 2 == 1) // cf otb and its management of conversion of Multi/MonobandComplexImage for /2
            {
            m_MultibandComplexImage = false;
            }
          else
            {
            m_MultibandComplexImage = true;
            }
          }
        else
          { // Real Data
          vType->value(1);
          typeFound = true;
          }
        }
      else // if we don't use GDAL Image IO
        {
        switch (m_FPVReader->GetImageIO()->GetPixelType())
          {
          // handle the radar case
          case itk::ImageIOBase::COMPLEX:
            vType->value(2);
            // If we are still here, this is a readable image
            typeFound = true;
            break;
            // handle the optical case
          default:
            vType->value(1);
            // If we are still here, this is a readable image
            typeFound = true;
            break;
          }
        }
      }
    catch (itk::ExceptionObject&)
      {
      // Silent catch
      }

    if (!typeFound)
      {
      try
        {
        VectorReaderType::Pointer vectorReader = VectorReaderType::New();
        vectorReader->SetFileName(filepath);
        vectorReader->GenerateOutputInformation();
        vType->value(3);
        typeFound = true;
        }
      catch (itk::ExceptionObject&)
        {
        // Silent catch
        vType->value(0);
        }
      }

    // Activate/ deactivate ok
    if (!typeFound)
      {
      vType->value(0);
      bOk->deactivate();
      }
    else
      {
      bOk->activate();
      }
    }

  std::string name = vName->value();

  if (name.empty())
    {
    if (typeHdf)
      {
      vName->value(m_Desc[0].c_str());
      }
    else
      {
      std::string fname = itksys::SystemTools::GetFilenameWithoutExtension(vFilePath->value());
      vName->value(fname.c_str());
      }
    }
}

void ReaderModule::OpenDataSet()
{
  try
    {
    switch (vType->value())
      {
      case 1:
        this->OpenOpticalImage();
        break;
      case 2:
        if (m_MultibandComplexImage)
          {
          // Only sar image readable by gdal can go here
          this->OpenMultiSarImage();
          }
        else
          {
          if (strcmp(m_FPVReader->GetImageIO()->GetNameOfClass(), "GDALImageIO") == 0)
            {
            // Check if it is a multiband image of scalar (only monoband complex image can go here)
            if ( (!(dynamic_cast<GDALImageIO*> (m_FPVReader->GetImageIO()))->GDALPixelTypeIsComplex()) && (m_FPVReader->GetImageIO()->GetNumberOfComponents() == 2))
              {
              itk::OStringStream oss;
              oss << "You try to open a two bands image of scalar as a SAR image, \n" << \
                     "we consider that first band is Real part and second band is Imaginary part.\n"<< \
                     "This warning message occurred with: " << vFilePath->value();
              MsgReporter::GetInstance()->SendWarning(oss.str());
              }
            else if (m_FPVReader->GetImageIO()->GetNumberOfComponents()  > 2)
              {
              itk::OStringStream oss;
              oss << "You try to open a Multiband ( number of bands > 2) image of scalar as a SAR image, \n" << \
                     "we consider only that first band is Real part and second band is Imaginary part. \n"<< \
                     "Rest of data are not considered.\n" << \
                     "This warning message occurred with: " << vFilePath->value();
              MsgReporter::GetInstance()->SendWarning(oss.str());
              }
            }
          this->OpenSarImage();
          }
        break;
      case 3:
        this->OpenVector();
        break;
      default:
        itkExceptionMacro(<< "Unknow dataset type.");
        break;
      }

    wFileChooserWindow->hide();

    // Notify all listener
    // TODO: this should not be done by the user
    this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

void ReaderModule::TypeChanged()
{
  if (vType->value() > 0)
    {
    bOk->activate();
    }
  else
    {
    bOk->deactivate();
    }
}

void ReaderModule::DatasetChanged()
{
       vName->value(m_Desc[vDataset->value()].c_str());
}

void ReaderModule::OpenOpticalImage()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  std::ostringstream oss, ossId, ossDatasetId;
  std::string   filepath = vFilePath->value();

  if (!m_Desc.empty() && vDataset->visible() ) // it is a hdf file
  {
    filepath += ":";
    ossDatasetId << vDataset->value(); // Following the convention in GDALImageIO
    filepath += ossDatasetId.str();
  }

  m_FPVReader->SetFileName(filepath);
  m_FPVReader->GenerateOutputInformation();

  // Add the full data set as a descriptor
  if (!m_Desc.empty() && vDataset->visible() ) // it is a hdf file
    {
    oss << "Image read from file: " << itksys::SystemTools::GetFilenameName(filepath) << " SUBDATASET = " << ossDatasetId.str();
    ossId << vName->value(); //m_Desc[vDataset->value()];
    }
  else
    {
    oss << "Image read from file: " <<  itksys::SystemTools::GetFilenameName(filepath);
    ossId << vName->value();
    }

  this->AddOutputDescriptor(m_FPVReader->GetOutput(), ossId.str(), oss.str(), true);
}

void ReaderModule::OpenMultiSarImage()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  std::ostringstream oss, ossId, ossDatasetId;
  std::string   filepath = vFilePath->value();

  m_VComplexReader->SetFileName(filepath);
  m_VComplexReader->GenerateOutputInformation();

  // Add the full data set as a descriptor
  oss << "Vector Float Complex image read from file: " << itksys::SystemTools::GetFilenameName(filepath);
  ossId << vName->value();

  this->AddOutputDescriptor(m_VComplexReader->GetOutput(), ossId.str(), oss.str(), true);
}

void ReaderModule::OpenSarImage()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  std::ostringstream oss, ossId, ossDatasetId;
  std::string   filepath = vFilePath->value();

  if (!m_Desc.empty() && vDataset->visible() ) // it is a hdf file
  {
    filepath += ":";
    ossDatasetId << vDataset->value(); // Following the convention in GDALImageIO
    filepath += ossDatasetId.str();
  }

  m_ComplexReader->SetFileName(filepath);
  m_ComplexReader->GenerateOutputInformation();

  // Add the full data set as a descriptor
  if (!m_Desc.empty() && vDataset->visible() ) // it is a hdf file
    {
    oss << "Complex Image read from file: " << itksys::SystemTools::GetFilenameName(filepath) << " SUBDATASET = " << ossDatasetId.str();
    ossId << vName->value(); //m_Desc[vDataset->value()];
    }
  else
    {
    oss << "Complex image read from file: " << itksys::SystemTools::GetFilenameName(filepath);
    ossId << vName->value();
    }

  this->AddOutputDescriptor(m_ComplexReader->GetOutput(), ossId.str(), oss.str(), true);
}

void ReaderModule::OpenVector()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  std::ostringstream oss, ossId;
  std::string   filepath = vFilePath->value();

  m_VectorReader->SetFileName(filepath);
  m_VectorReader->GenerateOutputInformation();

  oss << "Vector read from file: " << itksys::SystemTools::GetFilenameName(filepath);
  ossId << vName->value() << " (whole dataset)";
  this->AddOutputDescriptor(m_VectorReader->GetOutput(), ossId.str(), oss.str());
}

void ReaderModule::Browse()
{
  const char * filename = NULL;

  filename = flu_file_chooser("Choose the dataset file...", "*.*", "");

  if (filename == NULL)
    {
    otbMsgDebugMacro(<< "Empty file name!");
    return;
    }
  vFilePath->value(filename);
  // Need to clear these variables
  m_Desc.clear();
  m_Names.clear();
  vDataset->clear();
  this->Analyse();
}

void ReaderModule::Cancel()
{
  this->Hide();
}

void ReaderModule::Hide()
{
  wFileChooserWindow->hide();
}

bool ReaderModule::IsHdfFile(std::string filepath)
{
  GDALImageIO::Pointer readerGDAL = otb::GDALImageIO::New();
  // in case of hdr file (.hdr), GDAL want the header file as filepath
  string::size_type loc = filepath.find( ".hdr", 0 );
  if ( loc != string::npos )
    {
    filepath.erase(loc, 4);
    }
  readerGDAL->SetFileName(filepath);
  if (readerGDAL->CanReadFile(filepath.c_str()))
    {
    if (!readerGDAL->GetSubDatasetInfo(m_Names, m_Desc))
      {
      return false; // There are no subdataset in this file
      }
    }
  else
    {
    return false; // GDAL cannot read this file
    }
  return true;
}

bool ReaderModule::CheckDataSetString()
{
  if (!m_Desc.empty())
    {
    for (size_t it = 0; it < m_Desc.size(); it++)
      {
      string key("/");
      size_t found;
      do
        {
        found = m_Desc[it].find(key);
        if (found!=string::npos)
          {
          m_Desc[it].replace(found, key.length()," ");
          }
        }
      while(found!=string::npos);
      }
    return true;
    }
  else
    {
    return false;
    }
}

} // End namespace otb
#endif
