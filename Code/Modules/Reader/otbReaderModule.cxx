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
#include "itkCastImageFilter.h"
#include "otbImageFileWriter.h"

#include "otbConfigure.h"
#if defined(OTB_USE_JPEG2000)
# include "otbJPEG2000ImageIO.h"
#endif

#include <iostream>
#include <fstream>

namespace otb
{

enum ImageType
{
  ImageType_Unknown = 0,
  ImageType_RealImage,
  ImageType_ComplexImage,
  ImageType_VectorData,
  ImageType_PleiadesImage,
  ImageType_RawImage
};

enum EnviPixelType
{
  EnviPixelType_Byte =0,
  EnviPixelType_Int16,
  EnviPixelType_UInt16,
  EnviPixelType_Int32,
  EnviPixelType_UInt32,
  EnviPixelType_Int64,
  EnviPixelType_UInt64,
  EnviPixelType_Float32,
  EnviPixelType_Float64,
  EnviPixelType_Cplx32,
  EnviPixelType_Cplx64
};

enum EnviInterleave
{
  EnviInterleave_BSQ =0,
  EnviInterleave_BIP,
  EnviInterleave_BIL
};

enum EnviByteOrder
{
  EnviByteOrder_LSF = 0,
  EnviByteOrder_MSF
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
  this->NeedsPipelineLockingOn();
  
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
  vType->add(otbGetTextMacro("Raw Image"));
  vType->value(ImageType_Unknown);

  // Deactivate ok for now
  bOk->deactivate();

  // No name for now
  vName->value("");
  
  // Fill ENVI types for raw data
  vPixType->add(otbGetTextMacro("[ 1]: 8-bit byte"));
  m_MapEnviPixelType[0]=1;
  vPixType->add(otbGetTextMacro("[ 2]: 16-bit signed integer"));
  m_MapEnviPixelType[1]=2;
  vPixType->add(otbGetTextMacro("[12]: 16-bit unsigned integer"));
  m_MapEnviPixelType[2]=12;
  vPixType->add(otbGetTextMacro("[ 3]: 32-bit signed integer"));
  m_MapEnviPixelType[3]=3;
  vPixType->add(otbGetTextMacro("[13]: 32-bit unsigned integer"));
  m_MapEnviPixelType[4]=13;
  vPixType->add(otbGetTextMacro("[14]: 64-bit signed integer"));
  m_MapEnviPixelType[5]=14;
  vPixType->add(otbGetTextMacro("[15]: 64-bit unsigned integer"));
  m_MapEnviPixelType[6]=15;
  vPixType->add(otbGetTextMacro("[ 4]: 32-bit floating point"));
  m_MapEnviPixelType[7]=4;
  vPixType->add(otbGetTextMacro("[ 5]: 64-bit double precision floating point"));
  m_MapEnviPixelType[8]=5;
  vPixType->add(otbGetTextMacro("[ 6]: 2 x 32-bit complex (real-imaginary pair)"));
  m_MapEnviPixelType[9]=6;
  vPixType->add(otbGetTextMacro("[ 9]: 2 x 64-bit double precision complex (real-imaginary pair)"));
  m_MapEnviPixelType[10]=9;
  vPixType->value(EnviPixelType_Byte);
  
  vInterleave->add(otbGetTextMacro("BSQ"));
  vInterleave->add(otbGetTextMacro("BIP"));
  vInterleave->add(otbGetTextMacro("BIL"));
  vInterleave->value(EnviInterleave_BSQ);
  
  vByteOrder->add(otbGetTextMacro("LSF : Least Significant byte First"));
  vByteOrder->add(otbGetTextMacro("MSF : Most Significant byte First"));
  vByteOrder->tooltip(otbGetTextMacro("LSF = little endian; MSF = big endian"));
  vByteOrder->value(EnviByteOrder_LSF);
  
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
      bSaveQuicklook->set_visible();
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
        wFileChooserWindow->hide();
        // Notify all listener
        // TODO: this should not be done by the user
        this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
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
        wFileChooserWindow->hide();
        // Notify all listener
        // TODO: this should not be done by the user
        this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
        break;
      case ImageType_VectorData:
        this->OpenVector();
        wFileChooserWindow->hide();
        // Notify all listener
        // TODO: this should not be done by the user
        this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
        break;
      case ImageType_PleiadesImage:
        this->OpenRealImageWithQuicklook();
        break;
      default:
        itkExceptionMacro(<< "Unknow dataset type.");
        break;
      }
    }
  catch (itk::ExceptionObject& err)
    {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
    }

}

void ReaderModule::SynchronizeThreads()
{
  while(this->IsBusy())
    {
    otb::Threads::Sleep(500);
    }
}

void ReaderModule::TypeChanged()
{
  // Raw image special case : ask the user for image informations
  if (vType->value() == ImageType_RawImage)
    {
    vType->value(ImageType_Unknown);
    std::string filepath = vFilePath->value();
    
    if (filepath.size()==0)
      {
      return;
      }
    wRawTypeWindow->show();
    return;
    }
  
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
  if(m_TypeJPEG2000)
    {
    itk::OStringStream oss;
    oss<<filepath<<"?&resol="<<vDataset->value();
    filepath = oss.str();
    }
  else if(m_TypeHdf)
    {
    itk::OStringStream oss;
    oss<<filepath<<"?&sdataidx="<<vDataset->value();
    filepath= oss.str();
    }
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

  m_Filepath = vFilePath->value();
  std::string   otbFilepath = m_Filepath;
  m_Resolution = 0;

  if (!m_Desc.empty() && vDataset->visible() ) // it is a hdf file
  {
  if(m_TypeJPEG2000)
    {
    itk::OStringStream oss;
    oss<<otbFilepath<<"?&resol="<<vDataset->value();
    otbFilepath = oss.str();
    }
  else if(m_TypeHdf)
    {
    itk::OStringStream oss;
    oss<<otbFilepath<<"?&sdataidx="<<vDataset->value();
    otbFilepath= oss.str();
    }
  }

  // Add the full data set as a descriptor
  if (!m_Desc.empty() && vDataset->visible() ) // it is a file which need additional info
    {
    if (m_TypeHdf)
      oss << "Image read from file: " << itksys::SystemTools::GetFilenameName(m_Filepath) << " SUBDATASET = " << ossDatasetId.str();
    if (m_TypeJPEG2000)
      oss << "Image read from file: " << itksys::SystemTools::GetFilenameName(m_Filepath) << " RESOLUTION = " << ossDatasetId.str();

    ossId << vName->value(); //m_Desc[vDataset->value()];
    }
  else
    {
    oss << "Image read from file: " <<  itksys::SystemTools::GetFilenameName(m_Filepath);
    ossId << vName->value();
    }

  m_FPVReader->SetFileName(otbFilepath);
  m_FPVReader->GenerateOutputInformation();

  m_ImageWithQL = ImageWithQuicklook::New();
  m_ImageWithQL->SetImage(m_FPVReader->GetOutput());
  m_ShrinkFactor = 1;
  
  m_KeyForQL = ossId.str();
  m_DescForQL = oss.str();
  
  // Launch quicklook computation
  this->pBusyBar->value(0);
  this->pBusyBar->show();
  Fl::check();

  this->BusyOn(); // Initialize the Busy variable in the main thread
  this->StartProcess2(); // Launch ThreadedRun()
  this->StartProcess1(); // Launch ThreadedWatch()
}

/** UpdateProgress */
void ReaderModule::UpdateProgress()
{
  double progress = pBusyBar->value();
  
  if (m_progressIndex >= 4)
    {
    m_progressIndex = 0;
    }
    
  switch (m_progressIndex)
    {
    case 0:
      {
      progress += 0.001;
      if (progress > 1.0)
        {
        progress = 0.0;
        pBusyBar->color(FL_RED);
        pBusyBar->selection_color(55);
        m_progressIndex++;
        }
      break;
      }
    case 1:
      {
      progress += 0.001;
      if (progress > 1.0)
        {
        progress = 1.0;
        m_progressIndex++;
        }
      break;
      }
    case 2:
      {
      progress -= 0.001;
      if (progress < 0.0)
        {
        progress = 1.0;
        pBusyBar->color(55);
        pBusyBar->selection_color(FL_RED);
        m_progressIndex++;
        }
      break;
      }
    case 3:
      {
      progress -= 0.001;
      if (progress < 0.0)
        {
        progress = 0.0;
        m_progressIndex++;
        }
      break;
      }
    default:
      {
      m_progressIndex = 0;
      break;
      }
    }

//   double progress = m_ProcessObject->GetProgress();
//
//   itk::OStringStream oss1, oss2;
//   oss1.str("");
//   oss1 << otbGetTextMacro("Generating QuickLook") << "  (" << std::floor(100 * progress) << "%)";
//   oss2.str("");
//   oss2 << std::floor(100 * progress);
//   oss2 << "%";
  pBusyBar->value(progress);
  //wFileChooserWindow->copy_label(oss1.str().c_str());
  //pBar->copy_label(oss2.str().c_str());
}

/** UpdateProgressCallback */
void ReaderModule::UpdateProgressCallback(void * data)
{
  Self::Pointer qlReader = static_cast<Self *>(data);

  if (qlReader.IsNotNull())
    {
    qlReader->UpdateProgress();
    }
}

/** ThreadedWatch */
void ReaderModule::ThreadedWatch()
{
  // Deactivate window buttons
  Fl::lock();
  bCancel->deactivate();
  bOk->deactivate();
  bBrowse->deactivate();
  vFilePath->deactivate();
  vType->deactivate();
  vName->deactivate();
  bSaveQuicklook->deactivate();
  Fl::unlock();

  double last = 0;
  double updateThres = 0.01;
  double current = 0;
  m_progressIndex = 0;

  while ( (m_ProcessObject.IsNull() || this->IsBusy()) )
    {
//    if (m_ProcessObject.IsNotNull())
//      {
//       current = m_ProcessObject->GetProgress();
//       if (current - last > updateThres)
//         {
        // Make the main fltk loop update progress fields
        Fl::awake(&UpdateProgressCallback, this);
//        last = current;
//        }
//      }
    // Sleep for a while
    Sleep(1000);
    }

  // Update progress one last time
  Fl::awake(&UpdateProgressCallback, this);

  Fl::lock();
  // Reactivate window buttons
  bCancel->activate();
  bOk->activate();
  bBrowse->activate();
  vFilePath->activate();
  vType->activate();
  vName->activate();
  bSaveQuicklook->activate();
  Fl::unlock();

  Fl::awake(&HideWindowCallback, this);
}

/** ThreadedRun */
void ReaderModule::ThreadedRun()
{
  std::string qlKey = "_ql_by_otb.tif";
  FloatingVectorImageType::Pointer quicklook;
  
  if (m_TypeJPEG2000)
    {
    FPVReaderType::Pointer qlReader = FPVReaderType::New();

    unsigned int resolution = 0;
    if (!m_Desc.empty() && vDataset->visible() )
      {
      resolution = m_Desc.size() - 1;
      }

    // Compute Quicklook path
    std::string qlFilePath = m_Filepath + qlKey;
    bool qlReadFromFile = true;

    if (itksys::SystemTools::FileExists(qlFilePath.c_str()))
      {
      qlReader->SetFileName(qlFilePath);

      try
        {
        qlReader->Update();
        }
      catch (const itk::ExceptionObject &)
        {
        qlReadFromFile = false;
        }
      }
    else
      qlReadFromFile = false;

    if(!qlReadFromFile)
      {
      // try to compute QL from file
      try
        {
        qlReader = FPVReaderType::New();
        if (!m_Desc.empty() && vDataset->visible() )
          {
          itk::OStringStream oss;
          oss<<m_Filepath<<"?&resol="<<resolution;
          qlReader->SetFileName(oss.str());
          }
        m_ProcessObject = qlReader;
        qlReader->Update();
        }
      catch (itk::ExceptionObject& err)
        {
        m_ErrorMsg = err.GetDescription();
        Fl::awake(&SendErrorCallback, &m_ErrorMsg);
        }
      
      }

    quicklook = qlReader->GetOutput();
    quicklook->DisconnectPipeline();
    m_ShrinkFactor = (1 << resolution);

    // Try to write the ql
    if(!qlReadFromFile && (bSaveQuicklook->value() == 1))
      {
      typedef itk::CastImageFilter<FloatingVectorImageType, otb::VectorImage<unsigned short> > CastFilterType;
      typedef otb::ImageFileWriter<otb::VectorImage<unsigned short> > WriterType;

      CastFilterType::Pointer caster = CastFilterType::New();
      caster->SetInput(quicklook);

      WriterType::Pointer writer = WriterType::New();
      writer->SetInput(caster->GetOutput());
      writer->SetFileName(qlFilePath);
      try
        {
        writer->Update();
        }
      catch(const itk::ExceptionObject &)
        {
        // Silent catch
        }
      }
    }// end Jpeg2000

  if (quicklook.IsNull())
    {
    typedef otb::StreamingShrinkImageFilter<FloatingVectorImageType> StreamingShrinkImageFilterType;
    try
      {
      StreamingShrinkImageFilterType::Pointer shrinker = StreamingShrinkImageFilterType::New();
      shrinker->SetInput(m_FPVReader->GetOutput());
      //FltkFilterWatcher qlwatcher(shrinker->GetStreamer(), 0, 0, 200, 20,
      //                            otbGetTextMacro("Generating QuickLook ..."));
      m_ProcessObject = shrinker->GetStreamer();
      shrinker->Update();
      m_ShrinkFactor = shrinker->GetShrinkFactor();
      quicklook = shrinker->GetOutput();
      }
    catch (itk::ExceptionObject& err)
      {
      m_ErrorMsg = err.GetDescription();
      Fl::awake(&SendErrorCallback, &m_ErrorMsg);
      }
 
    quicklook->DisconnectPipeline();
    }

  m_ImageWithQL->SetQuicklook(quicklook);
  // We may read the image at lower resolution.
  m_ImageWithQL->SetShrinkFactor(m_ShrinkFactor / (1<<m_Resolution ));
  this->AddOutputDescriptor(m_ImageWithQL, m_KeyForQL, m_DescForQL, true);
  this->NotifyAll(MonteverdiEvent("OutputsUpdated", m_InstanceId));
  
  m_ProcessObject = m_FPVReader;
  this->BusyOff();
}

/** HideWindowCallback */
void ReaderModule::HideWindowCallback(void * data)
{
  Self::Pointer qlReader = static_cast<Self *>(data);

  if (qlReader.IsNotNull())
    {
    qlReader->Hide();
    }
}

/** SendErrorCallback */
void ReaderModule::SendErrorCallback(void * data)
{
  std::string * error = static_cast<std::string *>(data);
  //TODO test if error is null
  if (error == NULL)
    {
    MsgReporter::GetInstance()->SendError("Unknown error during update");
    }
  else
    {
    MsgReporter::GetInstance()->SendError(error->c_str());
    }
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
  if(m_TypeJPEG2000)
    {
    itk::OStringStream oss;
    oss<<filepath<<"?&resol="<<vDataset->value();
    filepath = oss.str();
    }
  else if(m_TypeHdf)
    {
    itk::OStringStream oss;
    oss<<filepath<<"?&sdataidx="<<vDataset->value();
    filepath= oss.str();
    }
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
  wRawTypeWindow->hide();
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

/** RawTypeSetup */
void ReaderModule::RawTypeSetup()
{
  std::string filepath = vFilePath->value();
  
  // Write hdr file
  std::ofstream hdrFile;
  std::ostringstream oss;
  oss << filepath << ".hdr";
  std::string headerFilepath = oss.str();
  
  // Check write permissions
  if (itksys::SystemTools::Touch(headerFilepath.c_str(), true) == false)
    {
    MsgReporter::GetInstance()->SendError("Can't write header file. ");
    wRawTypeWindow->hide();
    return;
    }
  
  hdrFile.open(headerFilepath.c_str(), std::ios_base::out | std::ios_base::trunc);
  if (hdrFile.is_open())
    {
    hdrFile << "ENVI"<< std::endl;
    hdrFile << "description = {"<< vDescription->value() << "}"<<std::endl;
    hdrFile << "samples = "<< vWidth->value() << std::endl;
    hdrFile << "lines = "<< vHeight->value() << std::endl;
    hdrFile << "bands = "<< vBands->value() << std::endl;
    hdrFile << "header offset = 0"<< std::endl;
    hdrFile << "file type = ENVI standard" << std::endl;
    hdrFile << "interleave = ";
    switch (vInterleave->value())
      {
      case 0 : hdrFile << "bsq"<<std::endl;
        break;
      case 1 : hdrFile << "bip"<<std::endl;
        break;
      case 2 : hdrFile << "bil"<<std::endl;
        break;
      default : hdrFile << "bsq"<<std::endl;
        break;
      }
    hdrFile << "sensor type = Unknown" << std::endl;
    hdrFile << "data type = " << m_MapEnviPixelType[vPixType->value()] << std::endl;
    hdrFile << "byte order = " << vByteOrder->value() << std::endl;
    
    hdrFile.close();
    
    this->Analyse();
    this->OpenDataSet();
    }
  wRawTypeWindow->hide();
}

} // End namespace otb
#endif
