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
#include <FLU/Flu_File_Chooser.h>
#include "base/ossimFilename.h"
#include "otbMsgReporter.h"
#include "otbI18n.h"
#include "otbGDALImageIO.h"

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

  // Get the filename from the filepath
  ossimFilename lFile = ossimFilename(filepath);

  // Is hdf type
  bool typeHdf = false;
  if (lFile.ext() == "hdf")
    {
    typeHdf = true;
    }

  if (typeHdf)
    {
    otb::GDALImageIO::Pointer readerGDAL = otb::GDALImageIO::New();

    readerGDAL->SetFileName(filepath);
    if (readerGDAL->CanReadFile(filepath.c_str()))
      {
      bool readingSubDatasetInfo = readerGDAL->GetSubDatasetInfo(m_Names, m_Desc);
      if (readingSubDatasetInfo == false) return;
      }
    else
      {
      return;
      }

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
      ossimFilename fname(vFilePath->value());
      vName->value(fname.fileNoExtension());
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
        this->OpenSarImage();
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
  ostringstream oss, ossId, ossDatasetId;
  std::string   filepath = vFilePath->value();
  ossimFilename lFile = ossimFilename(filepath);

  if (lFile.ext() == "hdf")
  {
    filepath += ":";
    ossDatasetId << vDataset->value() ; // Following the convention in GDALImageIO
    filepath += ossDatasetId.str();
  }

  m_FPVReader->SetFileName(filepath);
  m_FPVReader->GenerateOutputInformation();

  // Add the full data set as a descriptor
  if (lFile.ext() == "hdf")
    {
    oss << "Image read from file: " << lFile.file() << " SUBDATASET = " << ossDatasetId.str();
    ossId << vName->value();//m_Desc[vDataset->value()];
    }
  else
    {
    oss << "Image read from file: " << lFile.file();
    ossId << vName->value();
    }

  this->AddOutputDescriptor(m_FPVReader->GetOutput(), ossId.str(), oss.str(), true);
}

void ReaderModule::OpenSarImage()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  ostringstream oss, ossId;
  std::string   filepath = vFilePath->value();
  ossimFilename lFile = ossimFilename(filepath);

  m_ComplexReader->SetFileName(filepath);
  m_ComplexReader->GenerateOutputInformation();

  oss << "Complex image read from file: " << lFile.file();
  ossId << vName->value();
  this->AddOutputDescriptor(m_ComplexReader->GetOutput(), ossId.str(), oss.str(), true);
}

void ReaderModule::OpenVector()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  ostringstream oss, ossId;
  std::string   filepath = vFilePath->value();
  ossimFilename lFile = ossimFilename(filepath);

  m_VectorReader->SetFileName(filepath);
  m_VectorReader->GenerateOutputInformation();

  oss << "Vector read from file: " << lFile.file();
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
} // End namespace otb

#endif
