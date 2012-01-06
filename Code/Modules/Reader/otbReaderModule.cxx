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

#include "otbStreamingShrinkImageFilter.h"
#include "otbFltkFilterWatcher.h"

#include "otbConfigure.h"
#if defined(OTB_USE_JPEG2000)
# include "otbJPEG2000ImageIO.h"
#endif

namespace otb
{

enum ImageType
{
  ImageType_Unknown = 0,
  ImageType_RealImage,
  ImageType_ComplexImage,
  ImageType_VectorData,
  ImageType_PleiadesImage
};


/** Constructor */
ReaderModule::ReaderModule()
: m_FPVReader(FPVReaderType::New()),
  m_VectorReader(VectorReaderType::New()),
  m_ComplexReader(ComplexImageReaderType::New()),
  m_VComplexReader(VComplexImageReaderType::New()),
  m_LabeledVectorReader(LabeledVectorReaderType::New()),
  m_TypeHdf(false),
  m_TypeJPEG2000(false),
  m_MultibandComplexImage(false)
{
  this->BuildGUI();

  // Hide busy bar
  pBusyBar->minimum(0);
  pBusyBar->maximum(1);
  pBusyBar->hide();


  // Expose supported data type:
  vType->add(otbGetTextMacro("Unknown"));
  vType->add(otbGetTextMacro("Real image"));
  vType->add(otbGetTextMacro("Complex image"));
  vType->add(otbGetTextMacro("Vector Data"));
  vType->add(otbGetTextMacro("Pleiades Image"));
  vType->value(ImageType_Unknown);

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

  m_TypeJPEG2000 = IsJPEG2000File(filepath); // For us a JPEG2000 file is readable with JPEG2000ImageIO

  // GDAL Jasper driver segfaults on Pleiades files
  if (!m_TypeJPEG2000)
    {
    // For us a hdf file is composed of subdataset and readable with GDAL
    m_TypeHdf = IsHdfFile(filepath);
    }

  if (m_TypeHdf)
    {
    CheckDataSetString();
    // Fill vDataset with subdataset descriptor info
    for (unsigned int itSubDataset = 0; itSubDataset < (unsigned int) m_Desc.size(); itSubDataset++)
      {
      vDataset->add(m_Desc[itSubDataset].c_str());
      }
    vDataset->set_visible();
    vDataset->value(ImageType_Unknown);
    vDataset->activate();
    vDataset->copy_label("Please select the hdf sub dataset you want to open");

    vType->value(ImageType_RealImage); // We assume that hdf file is composed of real image
    bOk->activate();
    }
  else
    if (m_TypeJPEG2000)
      {
      // Fill vDataset with resolution descriptor info
      for (unsigned int itRes = 0; itRes < (unsigned int) m_Desc.size(); itRes++)
        {
        vDataset->add(m_Desc[itRes].c_str());
        }
      vDataset->set_visible();
      vDataset->value(0);
      vDataset->activate();
      vDataset->copy_label("Please select the JPEG2000 resolution you want to open");

      vType->value(ImageType_PleiadesImage);
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
            vType->value(ImageType_ComplexImage);
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
            {
            vType->value(ImageType_RealImage);
            typeFound = true;
            }
          }
        else // if we don't use GDAL Image IO
          {
          switch (m_FPVReader->GetImageIO()->GetPixelType())
            {
            case itk::ImageIOBase::COMPLEX: // handle the radar case
              vType->value(ImageType_ComplexImage);
              // If we are still here, this is a readable image
              typeFound = true;
              break;
            default: // handle the real case
              vType->value(ImageType_RealImage);
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
          vType->value(ImageType_VectorData);
          typeFound = true;
          }
        catch (itk::ExceptionObject&)
          {
          // Silent catch
          vType->value(ImageType_Unknown);
          }
        }

      // Activate/ deactivate ok
      if (!typeFound)
        {
        vType->value(ImageType_Unknown);
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
    if (m_TypeHdf)
      {
      vName->value(m_Desc[0].c_str());
      }
    else
      if (m_TypeJPEG2000)
        {
        vName->value(m_Names[0].c_str());
        }
      else
        {
        std::string fname = itksys::SystemTools::GetFilenameWithoutLastExtension(vFilePath->value());
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
      case ImageType_RealImage:
        this->OpenRealImage();
        break;
      case ImageType_ComplexImage:
        if (m_MultibandComplexImage)
          {
          // Only sar image readable by gdal can go here
          this->OpenMultiComplexImage();
          }
        else
          {
          if (strcmp(m_FPVReader->GetImageIO()->GetNameOfClass(), "GDALImageIO") == 0)
            {
            // Check if it is a multiband image of scalar (only monoband complex image can go here)
            if ( (!(dynamic_cast<GDALImageIO*> (m_FPVReader->GetImageIO()))->GDALPixelTypeIsComplex()) && (m_FPVReader->GetImageIO()->GetNumberOfComponents() == 2))
              {
              itk::OStringStream oss;
              oss << "You try to open a two bands image of scalar as a Complex image, \n" << \
                     "we consider that first band is Real part and second band is Imaginary part.\n"<< \
                     "This warning message occurred with: " << vFilePath->value();
              MsgReporter::GetInstance()->SendWarning(oss.str());
              }
            else if (m_FPVReader->GetImageIO()->GetNumberOfComponents()  > 2)
              {
              itk::OStringStream oss;
              oss << "You try to open a Multiband ( number of bands > 2) image of scalar as a Complex image, \n" << \
                     "we consider only that first band is Real part and second band is Imaginary part. \n"<< \
                     "Rest of data are not considered.\n" << \
                     "This warning message occurred with: " << vFilePath->value();
              MsgReporter::GetInstance()->SendWarning(oss.str());
              }
            }
          this->OpenComplexImage();
          }
        break;
      case ImageType_VectorData:
        this->OpenVector();
        break;
      case ImageType_PleiadesImage:
        this->OpenRealImageWithQuicklook();
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
  // Is type found is correct?
  bool typeFoundCorrect = false;
  if (vType->value() != ImageType_Unknown)
    {
    // Get the path
    std::string filepath = vFilePath->value();

    // Check if the choice is correct
    try
      {
      // Read the image
      m_FPVReader->SetFileName(filepath);
      m_FPVReader->GenerateOutputInformation();
      if (vType->value() == ImageType_RealImage
          || vType->value() == ImageType_ComplexImage
          || vType->value() == ImageType_PleiadesImage)
        {
        typeFoundCorrect = true;
        bOk->activate();
        }

      }
    catch (itk::ExceptionObject&)
      {
      // Silent catch
      }

    if (!typeFoundCorrect)
      {
      try
        {
        VectorReaderType::Pointer vectorReader = VectorReaderType::New();
        vectorReader->SetFileName(filepath);
        vectorReader->GenerateOutputInformation();
        if ( vType->value() == ImageType_VectorData)
          {
          bOk->activate();
          }
        }
      catch (itk::ExceptionObject&)
        { // Silent catch
        vType->value(ImageType_Unknown);
        bOk->deactivate();
        }
      }
    }
  else
    {
    bOk->deactivate();
    }
}

void ReaderModule::DatasetChanged()
{
  if (m_TypeJPEG2000)
         vName->value(m_Names[vDataset->value()].c_str());
  if (m_TypeHdf)
    vName->value(m_Desc[vDataset->value()].c_str());
}

void ReaderModule::OpenRealImage()
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

  m_FPVReader->SetFileName(filepath);
  m_FPVReader->GenerateOutputInformation();
  this->AddOutputDescriptor(m_FPVReader->GetOutput(), ossId.str(), oss.str(), true);
}

void ReaderModule::OpenRealImageWithQuicklook()
{
  // First, clear any existing output
  this->ClearOutputDescriptors();
  std::ostringstream oss, ossId, ossDatasetId;

  std::string   filepath = vFilePath->value();
  std::string   otbFilepath = filepath;
  unsigned int resolution = 0;

  if (!m_Desc.empty() && vDataset->visible() ) // it is a hdf file
  {
    resolution = vDataset->value();
    otbFilepath += ":";
    ossDatasetId << resolution; // Following the convention in GDALImageIO
    otbFilepath += ossDatasetId.str();
  }

  // Add the full data set as a descriptor
  if (!m_Desc.empty() && vDataset->visible() ) // it is a file which need additional info
    {
    if (m_TypeHdf)
      oss << "Image read from file: " << itksys::SystemTools::GetFilenameName(filepath) << " SUBDATASET = " << ossDatasetId.str();
    if (m_TypeJPEG2000)
      oss << "Image read from file: " << itksys::SystemTools::GetFilenameName(filepath) << " RESOLUTION = " << ossDatasetId.str();

    ossId << vName->value(); //m_Desc[vDataset->value()];
    }
  else
    {
    oss << "Image read from file: " <<  itksys::SystemTools::GetFilenameName(filepath);
    ossId << vName->value();
    }

  m_FPVReader->SetFileName(otbFilepath);
  m_FPVReader->GenerateOutputInformation();

  ImageWithQuicklook::Pointer imageWithQL = ImageWithQuicklook::New();
  imageWithQL->SetImage(m_FPVReader->GetOutput());
  unsigned int shrinkFactor = 1;
  FloatingVectorImageType::Pointer quicklook = MakeQuicklook(filepath, shrinkFactor);
  imageWithQL->SetQuicklook(quicklook);

  // We may read the image at lower resolution.
  imageWithQL->SetShrinkFactor(shrinkFactor / (1<<resolution));
  this->AddOutputDescriptor(imageWithQL, ossId.str(), oss.str(), true);
}

ReaderModule::FloatingVectorImageType::Pointer ReaderModule::MakeQuicklook(std::string filepath, unsigned int& shrinkFactor)
{
  FloatingVectorImageType::Pointer quicklook;
  if (m_TypeJPEG2000)
    {
    FPVReaderType::Pointer qlReader = FPVReaderType::New();
    unsigned int resolution = 0;
    qlReader->SetFileName(filepath);
    if (!m_Desc.empty() && vDataset->visible() )
      {
      resolution = m_Desc.size() - 1;
      qlReader->SetAdditionalNumber(resolution);

      }
    
    this->pBusyBar->value(1);
    this->pBusyBar->show();
    Fl::check();
    qlReader->Update();
    this->pBusyBar->value(0);
    this->pBusyBar->hide();
    Fl::check();

    quicklook = qlReader->GetOutput();
    quicklook->DisconnectPipeline();
    shrinkFactor = (1 << resolution);

    }

  if (quicklook.IsNull())
    {
    typedef otb::StreamingShrinkImageFilter<FloatingVectorImageType> StreamingShrinkImageFilterType;
    StreamingShrinkImageFilterType::Pointer shrinker = StreamingShrinkImageFilterType::New();
    shrinker->SetInput(m_FPVReader->GetOutput());
    FltkFilterWatcher qlwatcher(shrinker->GetStreamer(), 0, 0, 200, 20,
                                otbGetTextMacro("Generating QuickLook ..."));
    this->pBusyBar->value(1);
    this->pBusyBar->show();
    Fl::check();
    shrinker->Update();
    this->pBusyBar->value(0);
    this->pBusyBar->hide();
    Fl::check();
    shrinkFactor = shrinker->GetShrinkFactor();

    quicklook = shrinker->GetOutput();
    quicklook->DisconnectPipeline();
    }

  return quicklook;
}

void ReaderModule::OpenMultiComplexImage()
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

void ReaderModule::OpenComplexImage()
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

  m_VComplexReader->SetFileName(filepath);
  m_VComplexReader->GenerateOutputInformation();

  // Add the full data set as a descriptor
  if (!m_Desc.empty() && vDataset->visible() ) // it is a file which need additional info
    {
    if (m_TypeHdf)
      oss << "Image read from file: " << itksys::SystemTools::GetFilenameName(filepath) << " SUBDATASET = " << ossDatasetId.str();
    if (m_TypeJPEG2000)
      oss << "Image read from file: " << itksys::SystemTools::GetFilenameName(filepath) << " RESOLUTION = " << ossDatasetId.str();

    ossId << vName->value(); //m_Desc[vDataset->value()];
    }
  else
    {
    oss << "Complex image read from file: " << itksys::SystemTools::GetFilenameName(filepath);
    ossId << vName->value();
    }

  this->AddOutputDescriptor(m_VComplexReader->GetOutput(), ossId.str(), oss.str(), true);
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
  vName->value("");

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
  std::vector<string> names;

  // in case of hdr file (.hdr), GDAL want the header file as filepath
  string::size_type loc = filepath.find( ".hdr", 0 );
  if ( loc != string::npos )
    {
    filepath.erase(loc, 4);
    }
  readerGDAL->SetFileName(filepath);
  if (readerGDAL->CanReadFile(filepath.c_str()))
    {
    if (!readerGDAL->GetSubDatasetInfo(names, m_Desc))
      {
      return false; // There are no subdataset in this file
      }
    }
  else
    {
    return false; // GDAL cannot read this file
    }

  for (unsigned int itID = 0; itID < names.size(); itID++)
    {
    std::string fname = itksys::SystemTools::GetFilenameWithoutLastExtension(filepath);

    std::ostringstream oss;
    oss << fname.c_str()  << "_SubDatasetID_"<< itID;
    m_Names.push_back(oss.str());
    }

  return true;
}

bool ReaderModule::IsJPEG2000File(std::string filepath)
{
#if defined(OTB_USE_JPEG2000)
  JPEG2000ImageIO::Pointer readerJPEG2000 = otb::JPEG2000ImageIO::New();

  std::vector<unsigned int> res;

  readerJPEG2000->SetFileName(filepath);
  if (readerJPEG2000->CanReadFile(filepath.c_str()))
    {
    if (!readerJPEG2000->GetResolutionInfo(res, m_Desc))
      {
      return false; // There are no resolution in this file
      }
    }
  else
    {
    return false; // JPEG2000ImageIO cannot read this file
    }

  for (std::vector<unsigned int>::iterator itRes = res.begin(); itRes < res.end(); itRes++)
    {
    std::string fname = itksys::SystemTools::GetFilenameWithoutExtension(filepath);

         std::ostringstream oss;
         oss << fname.c_str()  << "_res_"<< *itRes;
         m_Names.push_back(oss.str());
    }

  return true;
#else
  return false;
#endif
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
