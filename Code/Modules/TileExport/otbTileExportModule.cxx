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
#ifndef __otbTileExportModule_cxx
#define __otbTileExportModule_cxx

#include "otbTileExportModule.h"
#include "itksys/SystemTools.hxx"
#include "base/ossimDirectory.h"
#include <FLU/Flu_File_Chooser.h>
#include "base/ossimFilename.h"
#include "otbMsgReporter.h"

namespace otb
{
/**
 * Constructor
 */
TileExportModule::TileExportModule(): m_Logo(NULL), m_LogoFilename(),
    m_HasLegend(false), m_HasLogo(false), m_TileSize(-1), m_KmzFile(NULL),
    m_RootKmlFile(), m_TempRootKmlFile(), m_Cancel(false),
    m_MaxDepth(0), m_CurIdx(0), m_NbOfInput(0), m_CurrentProduct(0), m_CurrentDepth(-1),
    m_RegionOfInterestKmlGenerated(false), m_CenterPointVector(), m_ProductVector()

{
  // Add a multiple inputs
  this->AddInputDescriptor<FloatingVectorImageType>("InputImage",otbGetTextMacro("Input image"),false);
  this->AddInputDescriptor<FloatingVectorImageType>("InputLegend",otbGetTextMacro("Input Legend"),true,true);
  this->AddInputDescriptor<FloatingVectorImageType>("InputLogo",otbGetTextMacro("Input Logos"), true);

  // Build the GUI
  this->BuildGUI();
}

/**
 * Destructor
 */
TileExportModule::~TileExportModule()
{}

/**
 * PrintSelf method
 */
void TileExportModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void TileExportModule::Run()
{
  // Get & rescale intensity of the input image
   m_VectorImage = this->GetInputData<FloatingVectorImageType>("InputImage");
   m_VectorImage->UpdateOutputInformation();

  // Set channel GUI
  unsigned int numberOfChannel = m_VectorImage->GetNumberOfComponentsPerPixel();
  
  for (unsigned int i = 1; i <= numberOfChannel; i++)
    {
    std::ostringstream val;
    val << i;
    this->cRedChannel->add(val.str().c_str());
    this->cGreenChannel->add(val.str().c_str());
    this->cBlueChannel->add(val.str().c_str());
    }

  this->cRedChannel->value(0);
  this->cGreenChannel->value(1);
  this->cBlueChannel->value(2);

  // Set tile size GUI
  this->cTileSize->add("64");
  this->cTileSize->add("128");
  this->cTileSize->add("256");
  this->cTileSize->add("512");
  this->cTileSize->add("1024");
  
  // Set default value to 256
  this->cTileSize->value(2);

  // Get Legends
  if (this->GetNumberOfInputDataByKey("InputLogo") > 0)
    {
    m_HasLogo = true;
    m_Logo = this->GetInputData<FloatingVectorImageType>("InputLogo");
    }

  // Show the GUI
  this->Show();

  // Add the product to the list of product 
  for(unsigned int i = 0 ; i < this->GetNumberOfInputDataByKey("InputImage"); i++)
    {
    // Get the filename
    std::string fname  = this->GetInputDataDescription<FloatingVectorImageType>("InputImage",i);
    std::string cuttenName = this->GetCuttenFileName(fname,i);

    // Create an information class for each new product
    ProductInformationType newProduct;
    newProduct.m_Id   = i;
    newProduct.m_Name = cuttenName;
    newProduct.m_Description = "Image Product";

    // Get the current input
    FloatingVectorImageType::Pointer image = this->GetInputData<FloatingVectorImageType>("InputImage",i);

    // While there are multiple inputs, it is difficult to tune the
    // gui with the right values for the channels, 
    // Try to guess them
    if(image->GetNumberOfComponentsPerPixel() > 3)
      {
      newProduct.m_Composition[0] = 2;
      newProduct.m_Composition[1] = 1;
      newProduct.m_Composition[2] = 0;
      }
    else if(image->GetNumberOfComponentsPerPixel() == 3)
      {
      newProduct.m_Composition[0] = 0;
      newProduct.m_Composition[1] = 1;
      newProduct.m_Composition[2] = 2;
      }
    else if(image->GetNumberOfComponentsPerPixel() == 1)
      {
      newProduct.m_Composition[0] = 0;
      newProduct.m_Composition[1] = 0;
      newProduct.m_Composition[2] = 0;
      }

    // Initialize the corners with a high value
    for(unsigned int it=0;it < newProduct.m_CornerList.Size();it++)
      newProduct.m_CornerList[it]=1000.;

    
    // Add the product class to the list
    m_ProductVector.push_back(newProduct);
    }
}

/** 
 * Browse Callback
 */
void TileExportModule::Browse()
{
  const char * filename = NULL;

  filename = flu_file_chooser("Choose the dataset file...", "*.kmz","");
  
  if (filename == NULL)
    {
    otbMsgDebugMacro(<<"Empty file name!");
    return ;
    }
  vFilePath->value(filename);
}



/**
 * OK CallBack
 */
void TileExportModule::SaveDataSet()
{
  std::string filepath = vFilePath->value();
  if(!filepath.empty())
    {
    m_Path = itksys::SystemTools::GetFilenamePath(filepath);
    m_FileName = itksys::SystemTools::GetFilenameWithoutExtension(filepath);
  
    // Create the extension following the user choice
    if(gExtended->value())
      {
      m_KmzExtension = "xt.kmz";
      m_KmlExtension = "xt.kml";
      }
    else
      {
      m_KmzExtension = ".kmz";
      m_KmlExtension = ".kml";
      }

    // Create a kmz file 
    m_KmzFileName << m_Path << "/" <<m_FileName<<m_KmzExtension;
    m_KmzFile = kmlengine::KmzFile::Create(m_KmzFileName.str().c_str());

    // Set tile size
    switch (this->cTileSize->value())
      {
      case 0:
	m_TileSize = 64;
	break;
      case 1:
	m_TileSize = 128;
	break;
      case 2:
	m_TileSize = 256;
	break;
      case 3:
	m_TileSize = 512;
	break;
      case 4:
	m_TileSize = 1024;
	break;
      default:
	m_TileSize = 256;
	break;
      }
  
    // Generate Logo
    if (m_HasLogo)
      {
      m_LogoFilename << m_Path;
      m_LogoFilename << "/logo.jpeg";
     
      ossimFilename cachingDir(m_Path);
      cachingDir.createDirectory();
  
      CastFilterType::Pointer castFiler = CastFilterType::New();
      castFiler->SetInput(m_Logo);
    
      m_VectorWriter = VectorWriterType::New();
      m_VectorWriter->SetFileName(m_LogoFilename.str());
      m_VectorWriter->SetInput(castFiler->GetOutput());
      m_VectorWriter->Update();

      // Add the logo to the kmz
      itk::OStringStream  logo_root_path_in_kmz;
      logo_root_path_in_kmz<<"logo.jpeg";

      itk::OStringStream  logo_absolut_path;
      logo_absolut_path<<m_LogoFilename.str();

      this->AddFileToKMZ(logo_absolut_path, logo_root_path_in_kmz);
    
      // Remove the logo file with stdio method :remove 
      if (remove(logo_absolut_path.str().c_str()) != 0)
	{
	itkExceptionMacro(<<"Error while deleting the file" << logo_absolut_path.str());
	}
      }

    // Store the legend associations
    this->StoreAssociations();
    
    // process all the input images
    m_NbOfInput = this->GetNumberOfInputDataByKey("InputImage");
  
    // Progress bar
    pBarProd->minimum(0.0);
    pBarProd->maximum(1.0);
    pBarProd->value(0);
  
    std::ostringstream labelValueStart;
    labelValueStart << " 0 / ";
    labelValueStart << m_NbOfInput;
  
    pBarProd->copy_label(labelValueStart.str().c_str());
    
    // Mutliple Inputs
    for(unsigned int i = 0; i < m_NbOfInput;i++)
      {          
      if(m_Cancel)
	{
	break;
	}
      // Get the filename
      std::string fname  = this->GetInputDataDescription<FloatingVectorImageType>("InputImage",i);
      m_CurrentImageName = this->GetCuttenFileName(fname,i);
    
      // Get the current input
      m_VectorImage  = this->GetInputData<FloatingVectorImageType>("InputImage",i);

      // While there are multiple inputs, it is difficult to tune the
      // gui with the right values for the channels, 
      // Try to guess them
      if(m_VectorImage->GetNumberOfComponentsPerPixel() > 3)
	{
	this->cRedChannel->value(2);
	this->cGreenChannel->value(1);
	this->cBlueChannel->value(0);
	}
      else if(m_VectorImage->GetNumberOfComponentsPerPixel() == 3)
	{
	this->cRedChannel->value(0);
	this->cGreenChannel->value(1);
	this->cBlueChannel->value(2);
	}
      else if(m_VectorImage->GetNumberOfComponentsPerPixel() == 1)
	{
	this->cRedChannel->value(0);
	this->cGreenChannel->value(0);
	this->cBlueChannel->value(0);
	}

      // Do the tiling for the current image
      if(this->IsProductHaveMetaData(i))
	this->Tiling(i);

      // Progress bar
      float progressValue = i + 1;

      std::ostringstream labelValue;
      labelValue << progressValue;
      labelValue << " / ";
      labelValue << m_NbOfInput;
        
      pBarProd->copy_label(labelValue.str().c_str());
        
      Fl::check();
    
      m_CurrentProduct++;
      }

    // Add the covered region kml
    this->RegionOfInterestProcess();

    // Reset the boost::intrusive_ptr<KmzFile> :
    // TODO : when upgrading boost > 1.42 use method release().
    m_KmzFile = NULL;

    // close the GUI
    this->Hide();
    }
}

/**
 * Cancel callback
 */
void TileExportModule::Cancel()
{
  m_Cancel = true;
  this->Hide();
}

 
void TileExportModule::Tiling(unsigned int curIdx)
{
  unsigned int numberOfChannel = m_VectorImage->GetNumberOfComponentsPerPixel();
  
  /** Image statistics*/
  FloatingVectorImageType::PixelType inMin(numberOfChannel),inMax(numberOfChannel),outMin(numberOfChannel),outMax(numberOfChannel);
  outMin.Fill(0);
  outMax.Fill(255);
  
  // Build wgs ref to compute long/lat
  OGRSpatialReference oSRS;
  oSRS.SetWellKnownGeogCS("WGS84");
  char * wgsRef = NULL;
  oSRS.exportToWkt(&wgsRef);
  
  // Update image base information
  m_VectorImage->UpdateOutputInformation();
  
  // Get the image size
  SizeType size;
  size = m_VectorImage->GetLargestPossibleRegion().GetSize();
  
  int sizeX = size[0];
  int sizeY = size[1];
  
  // Compute max depth
  int maxDepth = static_cast<int>(max( vcl_ceil(vcl_log(static_cast<float>(sizeX) / static_cast<float>(m_TileSize)) / vcl_log(2.0)), vcl_ceil(vcl_log(static_cast<float>(sizeY) / static_cast<float>(m_TileSize)) / vcl_log(2.0))));
  m_MaxDepth = maxDepth;
  m_CurIdx = curIdx;
  
  // Compute nbTile
  int nbTile = 0;
  
  for (int i=0; i<=maxDepth; i++)
    {
    int ratio = static_cast<int>(vcl_pow(2.,(maxDepth - i)));
    nbTile += (((sizeX / ratio) / m_TileSize) + 1)  * (((sizeY / ratio) / m_TileSize) + 1);
    }
  
  // Progress bar
  pBar->minimum(0.0);
  pBar->maximum(static_cast<float>(nbTile));
  pBar->value(0);
  
  // Extract size & index
  SizeType extractSize;
  IndexType extractIndex;

  for (int depth = 0; depth <= maxDepth; depth++)
    {

    // update the attribute value Current Depth
    m_CurrentDepth = depth;
    
    // Check if cancel button was pushed
    if (m_Cancel == true)
      {
      break;
      }
    
    // Resample image to the max Depth
    int sampleRatioValue = static_cast<int>(vcl_pow(2.,(maxDepth - depth)));
    
    if (sampleRatioValue > 1)
      {
      m_StreamingShrinkImageFilter = StreamingShrinkImageFilterType::New();
      
      m_StreamingShrinkImageFilter->SetShrinkFactor(sampleRatioValue);
      m_StreamingShrinkImageFilter->SetInput(m_VectorImage);
      
      m_VectorRescaleIntensityImageFilter = VectorRescaleIntensityImageFilterType::New();
      m_VectorRescaleIntensityImageFilter->SetInput(m_StreamingShrinkImageFilter->GetOutput());//m_ResampleVectorImage);
      m_VectorRescaleIntensityImageFilter->SetOutputMinimum(outMin);
      m_VectorRescaleIntensityImageFilter->SetOutputMaximum(outMax);
      
      if(depth == 0)
        {
        m_VectorRescaleIntensityImageFilter->Update();
        inMin = m_VectorRescaleIntensityImageFilter->GetInputMinimum();
        inMax = m_VectorRescaleIntensityImageFilter->GetInputMaximum();             
        }
      else
        {
        m_VectorRescaleIntensityImageFilter->SetInputMinimum(inMin);
        m_VectorRescaleIntensityImageFilter->SetInputMaximum(inMax);
        m_VectorRescaleIntensityImageFilter->SetAutomaticInputMinMaxComputation(false);
        }
      
      // New resample vector image
      m_ResampleVectorImage = m_VectorRescaleIntensityImageFilter->GetOutput();     
      }
    else 
      {
      m_VectorRescaleIntensityImageFilter = VectorRescaleIntensityImageFilterType::New();
      m_VectorRescaleIntensityImageFilter->SetInput(m_VectorImage);
      m_VectorRescaleIntensityImageFilter->SetOutputMinimum(outMin);
      m_VectorRescaleIntensityImageFilter->SetOutputMaximum(outMax);
      
      m_VectorRescaleIntensityImageFilter->SetInputMinimum(inMin);
      m_VectorRescaleIntensityImageFilter->SetInputMaximum(inMax);
      m_VectorRescaleIntensityImageFilter->SetAutomaticInputMinMaxComputation(false);
      
      m_ResampleVectorImage = m_VectorRescaleIntensityImageFilter->GetOutput();
      }
    
    m_ResampleVectorImage->UpdateOutputInformation();
    
    // Get the image size
    size = m_ResampleVectorImage->GetLargestPossibleRegion().GetSize();
    
    sizeX = size[0];
    sizeY = size[1];
    
    int x = 0;
    int y = 0;
    
    // Tiling resample image
    for (int tx = 0; tx < sizeX; tx+=m_TileSize)
      {
      for (int ty = 0; ty < sizeY; ty+=m_TileSize)
        {
        if ((tx + m_TileSize) >= sizeX)
          {
          extractIndex[0] = tx;
          extractSize[0] = sizeX - tx;
          }
        else
          {
          extractIndex[0] = tx;
          extractSize[0] = m_TileSize;
          }
        
        if ((ty + m_TileSize) >= sizeY)
          {
          extractIndex[1] = ty;
          extractSize[1] = sizeY - ty;
          }
        else
          {
          extractIndex[1] = ty;
          extractSize[1] = m_TileSize;
          }
        
        // Generate pathname
        std::ostringstream path;
        path << m_Path;
        
        ossimFilename cachingDir(path.str());
        cachingDir.createDirectory();
        
        // Generate Tile filename
        std::ostringstream ossFileName;
        ossFileName << path.str();
        ossFileName << "/";
        ossFileName << y;
        ossFileName << ".jpg";
        
        // Extract ROI
        m_VectorImageExtractROIFilter = VectorImageExtractROIFilterType::New();
        
        // Set extract roi parameters
        m_VectorImageExtractROIFilter->SetStartX(extractIndex[0]);
        m_VectorImageExtractROIFilter->SetStartY(extractIndex[1]);
        m_VectorImageExtractROIFilter->SetSizeX(extractSize[0]);
        m_VectorImageExtractROIFilter->SetSizeY(extractSize[1]);
	        
        // Set Channel to extract
	m_VectorImageExtractROIFilter->SetChannel(m_ProductVector[m_CurrentProduct].m_Composition[0]+1);
        m_VectorImageExtractROIFilter->SetChannel(m_ProductVector[m_CurrentProduct].m_Composition[1]+1);
        m_VectorImageExtractROIFilter->SetChannel(m_ProductVector[m_CurrentProduct].m_Composition[2]+1);
        
        // Set extract roi input
        m_VectorImageExtractROIFilter->SetInput(m_ResampleVectorImage);
        
        // Configure writer
        m_VectorWriter = VectorWriterType::New();
        m_VectorWriter->SetFileName(ossFileName.str().c_str());
        m_VectorWriter->SetInput(m_VectorImageExtractROIFilter->GetOutput());
        m_VectorWriter->Update();
        
        /** TODO : Generate KML for this tile */
        // Search Lat/Lon box      
        m_Transform = TransformType::New();
	m_Transform->SetInputKeywordList(m_ResampleVectorImage->GetImageKeywordlist());
	m_Transform->SetInputProjectionRef(m_VectorImage->GetProjectionRef());
        m_Transform->SetOutputProjectionRef(wgsRef);
        m_Transform->InstanciateTransform();
        
        InputPointType inputPoint;
        OutputPointType outputPoint;
        IndexType indexTile;
        SizeType sizeTile, demiSizeTile;
        
        sizeTile = extractSize;
        demiSizeTile[0] = (sizeTile[0] / 2) - 1;
        demiSizeTile[1] = (sizeTile[1] / 2) - 1;
	
        // Compute North value
        indexTile[0] = extractIndex[0] + demiSizeTile[0];
        indexTile[1] = extractIndex[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        double north = outputPoint[1];
        
        // Compute South value
        indexTile[0] = extractIndex[0] + demiSizeTile[0];
        indexTile[1] = extractIndex[1] + sizeTile[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        double south = outputPoint[1];
        
        // Compute East value
        indexTile[0] = extractIndex[0] + sizeTile[0];
        indexTile[1] = extractIndex[1] + demiSizeTile[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        double east = outputPoint[0];
        
        // Compute West value
        indexTile[0] = extractIndex[0];
        indexTile[1] = extractIndex[1] + demiSizeTile[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        double west = outputPoint[0];
        
        // Compute center value (lat / long)
        indexTile[0] = extractIndex[0] + demiSizeTile[1];
        indexTile[1] = extractIndex[1] + demiSizeTile[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        double centerLat = outputPoint[1];
        double centerLong = outputPoint[0];
        
        /** GX LAT LON **/        
        // Compute lower left corner
        indexTile[0] = extractIndex[0];
        indexTile[1] = extractIndex[1] + sizeTile[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        OutputPointType lowerLeftCorner = outputPoint;
        
        // Compute lower right corner
        indexTile[0] = extractIndex[0] + sizeTile[0];
        indexTile[1] = extractIndex[1] + sizeTile[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        OutputPointType lowerRightCorner = outputPoint;
        
        // Compute upper right corner
        indexTile[0] = extractIndex[0] + sizeTile[0];
        indexTile[1] = extractIndex[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        OutputPointType upperRightCorner = outputPoint;


        
        // Compute upper left corner
        indexTile[0] = extractIndex[0];
        indexTile[1] = extractIndex[1];
        m_ResampleVectorImage->TransformIndexToPhysicalPoint(indexTile, inputPoint);
        outputPoint = m_Transform->TransformPoint(inputPoint);
        OutputPointType upperLeftCorner = outputPoint;
	
        /** END GX LAT LON */
        
        // Create KML - Filename - PathName - tile number - North - South - East - West
        if (sampleRatioValue == 1)
          {
          if(!gExtended->value())  // Extended format 
            this->GenerateKML(path.str(), depth, x, y, north, south, east, west);
          else
            this->GenerateKMLExtended(path.str(), depth, x, y, lowerLeftCorner, lowerRightCorner, upperRightCorner, upperLeftCorner);
          }
        else
          {
          // Search tiles to link
          int tileXStart = extractIndex[0] / (m_TileSize / 2);
          int tileYStart = extractIndex[1] / (m_TileSize / 2);
          
          // Create KML with link
          if(!gExtended->value())
            this->GenerateKMLWithLink(path.str(), depth, x, y, tileXStart, tileYStart,
                                      north, south, east, west, centerLong, centerLat);
          else
            this->GenerateKMLExtendedWithLink(path.str(), depth, x, y, tileXStart, tileYStart,
                                              lowerLeftCorner, lowerRightCorner, upperRightCorner, upperLeftCorner, centerLong, centerLat);
          }
        
        if (depth == 0)
          {
          // Add the headers and the basic stuffs in the kml only once.
          if(curIdx == 0)
            {
	    this->RootKmlProcess(north,south,east,west);
            }
	  
	  // Add the bounding box kml 
	  this->BoundingBoxKmlProcess(north,south, east, west);
	  }  
	
        // Add the files to the kmz file
        // Relative path to the root directory  in the kmz file
        std::ostringstream  jpg_in_kmz, kml_in_kmz;
        jpg_in_kmz<<m_CurrentImageName<<"/"<<depth<<"/"<<x<<"/"<<y<<".jpg";
        kml_in_kmz<<m_CurrentImageName<<"/"<<depth<<"/"<<x<<"/"<<y<<m_KmlExtension;
        
        // Absolute path where are stored the files
        std::ostringstream  jpg_absolute_path, kml_absolute_path;
        jpg_absolute_path<<m_Path<<"/"<<y<<".jpg";
        kml_absolute_path<<m_Path<<"/"<<y<<m_KmlExtension;
        
        //Add the files to the kmz
        this->AddFileToKMZ(jpg_absolute_path, jpg_in_kmz);
        this->AddFileToKMZ(kml_absolute_path, kml_in_kmz);
        
        // Remove the unecessary files with stdio method :remove 
        if ( remove(kml_absolute_path.str().c_str()) ||  remove(jpg_absolute_path.str().c_str()))
          {
          itkExceptionMacro(<<"Error while deleting the file"<<kml_absolute_path.str()<< "or file "<<jpg_absolute_path.str());
          }
        
        // Progress bar
        float progressValue = pBar->value() + 1;
        pBar->value(progressValue);
        
        std::ostringstream labelValue;
        labelValue << progressValue;
        labelValue << " / ";
        labelValue << nbTile;
        
        pBar->copy_label(labelValue.str().c_str());
        
        // General progress bar
        float nbPart = 1.0 / m_NbOfInput;
        float increment = nbPart / nbTile;

        progressValue = pBarProd->value() + increment;
        pBarProd->value(progressValue);   
        
        Fl::check();
        
        y++;
        }
      x++;
      y = 0;
      }
    }
}


/**
 * Add the region of interest kml if any
 * 
 */
void
TileExportModule::RegionOfInterestProcess()
{
  // Add the region of interest kml once
  // Add the region of interest kml at the end
  if(m_RegionOfInterestKmlGenerated)
    {
    // Add the root kml in the kmz
    std::ostringstream roi_in_kmz;
    roi_in_kmz <<"roi/regionOfInterest"<<m_KmlExtension;
    std::ostringstream roi_absolute_path;
    roi_absolute_path <<m_Path<<"/regionOfInterest"<<m_KmlExtension;

    // Add the root file in the kmz
    this->AddFileToKMZ(roi_absolute_path, roi_in_kmz);
	
    // Remove the roiing files with stdio method :remove 
    if (remove(roi_absolute_path.str().c_str()) != 0)
      {
      itkExceptionMacro(<<"Error while deleting the file" << roi_absolute_path.str());
      }
    }
}


/**
 */
std::string
TileExportModule::GetCuttenFileName(std::string description, unsigned int idx)
{
//  size_t found       = description.find_first_of(":");
//  size_t found2      = description.find_first_of(".");
	    
  std::string currentImageName;
  std::string tempName;
//  if(found == std::string::npos ||  found2 == std::string::npos || found == found2)
//    tempName =  description.substr(found+1,found2-found-1);
//  else
//    {
    itk::OStringStream oss;
    oss<<"tiles_"<<idx;
    tempName= oss.str();
//    }

  // Replace all the blanks in the string
  unsigned int i = 0;
  while( i < tempName.length())
    {
    if(tempName[i] != ' ')
      currentImageName += tempName[i];
    i++;
    }
  
  return currentImageName;
}

/**
* 
*/
int
TileExportModule::AddFileToKMZ(std::ostringstream & absolutePath ,std::ostringstream & kmz_in_path)
{
  std::string  absolute = absolutePath.str();
  std::string  relative = kmz_in_path.str();
  std::string file_data;
  int errs = 0;
  if (!kmlbase::File::ReadFileToString(absolute, &file_data))
    {
    itkExceptionMacro(<<"Error while reading file "<<absolute);
    }
  else
    {
    m_KmzFile->AddFile(file_data, relative);
    }

  return errs;
}

/**
 * Actually the root kml is not fully generated : 
 * It generates only the part till the network link
 * In case of multiple inputs, other network liks are 
 * needed. See the method Add NetworkLinkToRootKML.
 */
void 
TileExportModule::GenerateKMLRoot(std::string title, double north, double south, double east, double west, bool extended)
{
  // Give a name to the root file
  std::ostringstream kmlname;
  kmlname << m_Path;
  kmlname << "/";
  kmlname << m_FileName;
  kmlname << m_KmlExtension;
  m_RootKmlFile.open(kmlname.str().c_str());
  m_RootKmlFile << fixed << setprecision(6);
  
  m_RootKmlFile<< "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
  m_RootKmlFile<< "<kml xmlns=\"http://www.opengis.net/kml/2.2\"" << std::endl;
  m_RootKmlFile<< " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">" << std::endl; 
  m_RootKmlFile << "\t<Document>" << std::endl;
  m_RootKmlFile << "\t\t<name>" << title << "</name>" << std::endl;
  m_RootKmlFile << "\t\t<description></description>" << std::endl;
  m_RootKmlFile << "\t\t<LookAt>" << std::endl;
  m_RootKmlFile << "\t\t\t<longitude>"<<(east+west)/2.<<"</longitude>" << std::endl;
  m_RootKmlFile << "\t\t\t<latitude>"<< (south+north)/2.<<"</latitude>" << std::endl;
  m_RootKmlFile << "\t\t\t<altitude>35000</altitude>"<< std::endl;
  m_RootKmlFile << "\t\t\t<range>35000</range>"<< std::endl;
  m_RootKmlFile << "\t\t</LookAt>"<< std::endl;
}

/**
 * Close the root kml 
 */
void
TileExportModule::
CloseRootKML()
{
  if (m_HasLogo)
    {
    RegionType logoReg = m_Logo->GetLargestPossibleRegion();
    SizeType logoSize = logoReg.GetSize();
    double lx = static_cast<double>(logoSize[0]);
    double ly = static_cast<double>(logoSize[1]);
    int sizey = 150;
    int sizex = static_cast<int>((lx / ly * sizey));
    
    /** LOGO **/
    m_RootKmlFile << "\t\t<ScreenOverlay>" << std::endl;
    m_RootKmlFile << "\t\t\t<Icon>" << std::endl;
    m_RootKmlFile << "\t\t\t\t<href>logo.jpeg</href>" << std::endl;
    m_RootKmlFile << "\t\t\t</Icon>" << std::endl;
    m_RootKmlFile << "\t\t\t<name>Logo</name>" << std::endl;
    m_RootKmlFile << "\t\t\t<overlayXY x=\"1\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>" << std::endl;
    m_RootKmlFile << "\t\t\t<screenXY x=\"1\" y=\"1\" xunits=\"fraction\" yunits=\"fraction\"/>" << std::endl;
    m_RootKmlFile << "\t\t\t<size x=\""<< sizex << "\" y=\"" << sizey << "\" xunits=\"pixels\" yunits=\"pixels\"/> " << std::endl;
    m_RootKmlFile << "\t\t</ScreenOverlay>" << std::endl;
    
    /** LOGO **/
    }
  
  m_RootKmlFile << "\t</Document>" << std::endl;
  m_RootKmlFile << "</kml>" << std::endl;
  
  m_RootKmlFile.close();
}



/**
 * Add A networkLink to the root kml
 *
 */
void
TileExportModule::
AddNetworkLinkToRootKML(double north, double south, double east, double west, std::string directory, bool addRegion, unsigned int pos)
{
  m_RootKmlFile << "\t\t<Document>" << std::endl;
  m_RootKmlFile << "\t\t\t<name>" << m_ProductVector[pos].m_Name << "</name>" << std::endl;
  m_RootKmlFile << "\t\t\t<Description>" << m_ProductVector[pos].m_Description << "</Description>" << std::endl;
  m_RootKmlFile << "\t\t\t<LookAt>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<longitude>"<<(east+west)/2.<<"</longitude>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<latitude>"<< (south+north)/2.<<"</latitude>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<altitude>35000</altitude>"<< std::endl;
  m_RootKmlFile << "\t\t\t\t<range>35000</range>"<< std::endl;
  m_RootKmlFile << "\t\t\t</LookAt>"<< std::endl;

  // Georeferenced Products
  // Add network link
  // If not geo add a ground Overlay with image
  // as an icon
  if(this->IsProductHaveMetaData(pos))
    {
    m_RootKmlFile << "\t\t\t<NetworkLink>" << std::endl;
    m_RootKmlFile << "\t\t\t\t<name>"<<m_ProductVector[pos].m_Name<<"</name>" <<  std::endl;
    m_RootKmlFile << "\t\t\t\t<open>1</open>" << std::endl;
    m_RootKmlFile << "\t\t\t<Style>" << std::endl;
    m_RootKmlFile << "\t\t\t\t<ListStyle id=\"hideChildren\">" << std::endl;
    m_RootKmlFile << "\t\t\t\t\t<listItemType>checkHideChildren</listItemType>" << std::endl;
    m_RootKmlFile << "\t\t\t\t</ListStyle>" << std::endl;
    m_RootKmlFile << "\t\t\t</Style>" << std::endl;
    if(addRegion)
      {
      m_RootKmlFile << "\t\t\t\t<Region>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t<Lod>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
      /** If the last depth, put level of detail LOD to infinite (ie -1)*/
      if(m_CurrentDepth == m_MaxDepth)
	m_RootKmlFile << "\t\t\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
      else
	m_RootKmlFile << "\t\t\t\t\t\t<maxLodPixels>"<<m_TileSize*4<<"</maxLodPixels>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t</Lod>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t<LatLonAltBox>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t\t<north>" << north << "</north>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t\t<south>" << south << "</south>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t\t<east>" << east << "</east>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t\t<west>" << west << "</west>" << std::endl;
      m_RootKmlFile << "\t\t\t\t\t</LatLonAltBox>" << std::endl;
      m_RootKmlFile << "\t\t\t\t</Region>" << std::endl;
      }

    m_RootKmlFile << "\t\t\t\t<Link>" << std::endl;
    m_RootKmlFile << "\t\t\t\t\t<href>"<<directory<<"/0/0/0"<<m_KmlExtension<<"</href>" << std::endl;
    m_RootKmlFile << "\t\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
    m_RootKmlFile << "\t\t\t\t</Link>" << std::endl;
    m_RootKmlFile << "\t\t\t</NetworkLink>" << std::endl;  
    }
  else
    {
    m_RootKmlFile <<"\t\t\t<GroundOverlay>"<< std::endl;  
    m_RootKmlFile <<"\t\t\t\t<name>"<<m_ProductVector[pos].m_Name<<"</name>"<< std::endl; 
    m_RootKmlFile <<"\t\t\t\t<Icon>"<< std::endl; 
    m_RootKmlFile <<"\t\t\t\t\t<href>"<<directory<<"/0.jpg"<<"</href>"<<std::endl; 
    m_RootKmlFile <<"\t\t\t\t</Icon>"<<std::endl;
    m_RootKmlFile <<"\t\t\t\t<LatLonBox>"<<std::endl;
    m_RootKmlFile <<"\t\t\t\t\t<north>"<<north<<"</north>"<<std::endl;
    m_RootKmlFile <<"\t\t\t\t\t<south>"<<south<<"</south>"<<std::endl;
    m_RootKmlFile <<"\t\t\t\t\t<east>"<<east<<"</east>"<<std::endl;
    m_RootKmlFile <<"\t\t\t\t\t<west>"<<west<<"</west>"<<std::endl;
    m_RootKmlFile <<"\t\t\t\t</LatLonBox>"<<std::endl;
    m_RootKmlFile <<"\t\t\t</GroundOverlay>"<< std::endl; 
    }
  
  m_RootKmlFile << "\t\t\t<Folder>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<name>The bounding box </name>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<Style>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t<ListStyle id=\"hideChildren\">" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t\t<listItemType>checkHideChildren</listItemType>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t</ListStyle>" << std::endl;
  m_RootKmlFile << "\t\t\t\t</Style>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<NetworkLink>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<Region>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t<Lod>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t</Lod>" << std::endl;
  m_RootKmlFile << "\t\t\t\t</Region>" << std::endl;
  m_RootKmlFile << "\t\t\t\t<Link>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t<href>bounds/bound_"<<pos<< m_KmlExtension<<"</href>" << std::endl;
  m_RootKmlFile << "\t\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  m_RootKmlFile << "\t\t\t\t</Link>" << std::endl;
  m_RootKmlFile << "\t\t\t\t</NetworkLink>" << std::endl;
  m_RootKmlFile << "\t\t\t</Folder>" << std::endl;
  
  // Add a placemark with the images used as legend
  // If any
  this->AddLegendToRootKml(north, south, east, west, pos);
  
  m_RootKmlFile << "\t\t</Document>" << std::endl;
}


/**
 * Add a legend associated to the current product
 *
 */
void
TileExportModule::
AddLegendToRootKml(double north, double south, double east, double west, unsigned int pos)
{
  
  double lat = (north+south)/2.;
  double lon = (east+west)/2.;

  if(m_ProductVector[pos].m_AssociatedLegends.size() > 0)
    {
    m_RootKmlFile << "\t\t\t\t<Placemark>" << std::endl;
    m_RootKmlFile << "\t\t\t\t\t<name>Legend</name>" << std::endl;
    m_RootKmlFile << "\t\t\t\t\t<description>"<< std::endl;
    m_RootKmlFile << "\t\t\t\t\t\t<![CDATA[ Legend of the product "<<m_ProductVector[pos].m_Name <<std::endl;
    for(unsigned int i = 0 ; i <m_ProductVector[pos].m_AssociatedLegends.size();i++ )
      m_RootKmlFile << "\t\t\t\t\t\t<img src=\"legends/legend_"<<pos<<m_ProductVector[pos].m_AssociatedLegends[i]<<".jpeg\" width=\"215\" height=\"175\"  >";
    m_RootKmlFile << "\t\t\t\t\t\t ]]>"<< std::endl;
    
    m_RootKmlFile <<"\t\t\t\t\t</description>"<< std::endl;
    m_RootKmlFile <<"\t\t\t\t\t<Point>"<< std::endl;
    m_RootKmlFile <<"\t\t\t\t\t\t<coordinates>"<<lon<<","<<lat<<"</coordinates>"<< std::endl;
    m_RootKmlFile <<"\t\t\t\t\t</Point>"<< std::endl;
    m_RootKmlFile << "\t\t\t\t</Placemark>" << std::endl;
    }
}

void
TileExportModule::
GenerateBoundingKML(double north, double south, double east, double west)
{
  std::ostringstream kmlname;
  kmlname << m_Path;
  kmlname <<"/";
  kmlname <<"bound_"<<m_CurrentProduct<<m_KmlExtension;
  std::ofstream fileTest(kmlname.str().c_str());

  fileTest << fixed << setprecision(6);

  fileTest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
  fileTest << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"" << std::endl;
  fileTest << " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">" << std::endl; 

  fileTest << "\t<Document>" << std::endl;
  fileTest << "\t\t<name> Bounding box of the  product "<<m_ProductVector[m_CurrentProduct].m_Name<<"</name>" << std::endl;
  fileTest << "\t\t<open>1</open>" << std::endl;
  fileTest << "\t\t<Placemark>" << std::endl;
  fileTest << "\t\t\t<description>The bounding Box of the image</description>" << std::endl;
  fileTest << "\t\t<LineString>" << std::endl;
  fileTest << "\t\t\t<extrude>0</extrude>" << std::endl;
  fileTest << "\t\t\t<tessellate>1</tessellate>" << std::endl;
  fileTest << "\t\t\t<altitudeMode>clampedToGround</altitudeMode>" << std::endl;
  fileTest << "\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t" <<  west<< ","<< north << std::endl;
  fileTest << "\t\t\t\t\t" <<  east<< ","<< north << std::endl;
  fileTest << "\t\t\t\t\t" <<  east<< ","<< south << std::endl;
  fileTest << "\t\t\t\t\t" <<  west<< ","<< south << std::endl;
  fileTest << "\t\t\t\t\t" <<  west <<","<< north << std::endl;
  fileTest << "\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t</LineString>" << std::endl;
  fileTest << "\t\t</Placemark>" << std::endl;

  fileTest << "\t</Document>" << std::endl;
  fileTest << "</kml>" << std::endl;
  
  fileTest.close();
}


void
TileExportModule::
GenerateKML(std::string pathname, int depth, int x, int y, double north, double south, double east, double west)
{
  std::ostringstream kmlname;
  kmlname << pathname;
  kmlname << "/";
  kmlname << y << ".kml";
  std::ofstream fileTest(kmlname.str().c_str());
  
  fileTest << fixed << setprecision(6);

  fileTest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
  fileTest << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"" << std::endl;
  fileTest << " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">" << std::endl; 
  fileTest << "\t<Document>" << std::endl;
  fileTest << "\t\t<name>" << y << ".kml</name>" << std::endl;
  fileTest << "\t\t<Region>" << std::endl;
  fileTest << "\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  if(m_CurrentDepth == m_MaxDepth)
    fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  else
      fileTest << "\t\t\t\t<maxLodPixels>"<<m_TileSize*4<<"</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t<north>" << north << "</north>" << std::endl;
  fileTest << "\t\t\t\t<south>" << south << "</south>" << std::endl;
  fileTest << "\t\t\t\t<east>" << east << "</east>" << std::endl;
  fileTest << "\t\t\t\t<west>" << west << "</west>" << std::endl;
  fileTest << "\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t</Region>" << std::endl;
  fileTest << "\t\t<GroundOverlay>" << std::endl;
  fileTest << "\t\t\t<drawOrder>" << depth + (m_CurIdx * m_MaxDepth) << "</drawOrder>" << std::endl;
  fileTest << "\t\t\t<Icon>" << std::endl;
  fileTest << "\t\t\t\t<href>" << y << ".jpg" << "</href>" << std::endl;
  fileTest << "\t\t\t</Icon>" << std::endl;
  fileTest << "\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t<north>" << north << "</north>" << std::endl;
  fileTest << "\t\t\t\t<south>" << south << "</south>" << std::endl;
  fileTest << "\t\t\t\t<east>" << east << "</east>" << std::endl;
  fileTest << "\t\t\t\t<west>" << west << "</west>" << std::endl;
  fileTest << "\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t</GroundOverlay>" << std::endl;
  fileTest << "\t</Document>" << std::endl;
  fileTest << "</kml>" << std::endl;
  
  fileTest.close();
}
                                    
void
TileExportModule::
GenerateKMLWithLink(std::string pathname,
                    int depth, int x, int y, int tileStartX, int tileStartY,
                    double north, double south, double east, double west, double centerLong, double centerLat)
{
  std::ostringstream kmlname;
  kmlname << pathname;
  kmlname << "/";
  kmlname << y << ".kml";
  std::ofstream fileTest(kmlname.str().c_str());
  
  fileTest << fixed << setprecision(6);
  
  fileTest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
  fileTest << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"" << std::endl;
  fileTest << " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">" << std::endl; 
  fileTest << "\t<Document>" << std::endl;
  fileTest << "\t\t<name>" << y << ".kml</name>" << std::endl;
  fileTest << "\t\t<Region>" << std::endl;
  fileTest << "\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  if(m_CurrentDepth == m_MaxDepth)
    fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  else
    fileTest << "\t\t\t\t<maxLodPixels>"<<m_TileSize*2<<"</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t<north>" << north << "</north>" << std::endl;
  fileTest << "\t\t\t\t<south>" << south << "</south>" << std::endl;
  fileTest << "\t\t\t\t<east>" << east << "</east>" << std::endl;
  fileTest << "\t\t\t\t<west>" << west << "</west>" << std::endl;
  fileTest << "\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t</Region>" << std::endl;
  fileTest << "\t\t<GroundOverlay>" << std::endl;
  fileTest << "\t\t\t<drawOrder>" << depth + (m_CurIdx * m_MaxDepth) << "</drawOrder>" << std::endl;
  fileTest << "\t\t\t<Icon>" << std::endl;
  fileTest << "\t\t\t\t<href>" << y << ".jpg" << "</href>" << std::endl;
  fileTest << "\t\t\t</Icon>" << std::endl;
  fileTest << "\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t<north>" << north << "</north>" << std::endl;
  fileTest << "\t\t\t\t<south>" << south << "</south>" << std::endl;
  fileTest << "\t\t\t\t<east>" << east << "</east>" << std::endl;
  fileTest << "\t\t\t\t<west>" << west << "</west>" << std::endl;
  fileTest << "\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t</GroundOverlay>" << std::endl;
  
  /* Sous tuile 1 */
  std::ostringstream fileTile1;
  fileTile1 << "../../";
  fileTile1 << depth + 1;
  fileTile1 << "/";
  fileTile1 << tileStartX;
  fileTile1 << "/";
  fileTile1 << tileStartY;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile1.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  if(m_CurrentDepth == m_MaxDepth)
    fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  else
    fileTest << "\t\t\t\t<maxLodPixels>"<<m_TileSize*2<<"</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t\t<north>" << north << "</north>" << std::endl;
  fileTest << "\t\t\t\t\t<south>" << centerLat << "</south>" << std::endl;
  fileTest << "\t\t\t\t\t<east>" << centerLong << "</east>" << std::endl;
  fileTest << "\t\t\t\t\t<west>" << west << "</west>" << std::endl;
  fileTest << "\t\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile1.str() << ".kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;
  
  /* Sous tuile 2 */
  std::ostringstream fileTile2;
  fileTile2 << "../../";
  fileTile2 << depth + 1;
  fileTile2 << "/";
  fileTile2 << tileStartX + 1;
  fileTile2 << "/";
  fileTile2 << tileStartY;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile2.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  if(m_CurrentDepth == m_MaxDepth)
    fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  else
    fileTest << "\t\t\t\t<maxLodPixels>"<<m_TileSize*4<<"</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t\t<north>" << north << "</north>" << std::endl;
  fileTest << "\t\t\t\t\t<south>" << centerLat << "</south>" << std::endl;
  fileTest << "\t\t\t\t\t<east>" << east << "</east>" << std::endl;
  fileTest << "\t\t\t\t\t<west>" << centerLong << "</west>" << std::endl;
  fileTest << "\t\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile2.str() << ".kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;  
  
  /* Sous tuile 3 */
  std::ostringstream fileTile3;
  fileTile3 << "../../";
  fileTile3 << depth + 1;
  fileTile3 << "/";
  fileTile3 << tileStartX + 1;
  fileTile3 << "/";
  fileTile3 << tileStartY + 1;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile3.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  if(m_CurrentDepth == m_MaxDepth)
    fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  else
    fileTest << "\t\t\t\t<maxLodPixels>"<<m_TileSize*4<<"</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t\t<north>" << centerLat << "</north>" << std::endl;
  fileTest << "\t\t\t\t\t<south>" << south << "</south>" << std::endl;
  fileTest << "\t\t\t\t\t<east>" << east << "</east>" << std::endl;
  fileTest << "\t\t\t\t\t<west>" << centerLong << "</west>" << std::endl;
  fileTest << "\t\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile3.str() << ".kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;  
  
  /* Sous tuile 4 */
  std::ostringstream fileTile4;
  fileTile4 << "../../";
  fileTile4 << depth + 1;
  fileTile4 << "/";
  fileTile4 << tileStartX;
  fileTile4 << "/";
  fileTile4 << tileStartY + 1;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile4.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  if(m_CurrentDepth == m_MaxDepth)
    fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  else
    fileTest << "\t\t\t\t<maxLodPixels>"<<m_TileSize*4<<"</maxLodPixels>" << std::endl;

  fileTest << "\t\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t\t\t<north>" << centerLat << "</north>" << std::endl;
  fileTest << "\t\t\t\t\t<south>" << south << "</south>" << std::endl;
  fileTest << "\t\t\t\t\t<east>" << centerLong << "</east>" << std::endl;
  fileTest << "\t\t\t\t\t<west>" << west << "</west>" << std::endl;
  fileTest << "\t\t\t\t</LatLonAltBox>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile4.str() << ".kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;  
  
  fileTest << "\t</Document>" << std::endl;
  fileTest << "</kml>" << std::endl;
  fileTest.close();
  
}

void
TileExportModule::
GenerateKMLExtended(std::string pathname, int depth, int x, int y,
                    OutputPointType lowerLeft, OutputPointType lowerRight,
                    OutputPointType upperRight, OutputPointType upperLeft)
{
  std::ostringstream kmlname;
  kmlname << pathname;
  kmlname << "/";
  kmlname << y << "xt.kml";
  std::ofstream fileTest(kmlname.str().c_str());
  
  fileTest << fixed << setprecision(6);
  
  fileTest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
  fileTest << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"" << std::endl;
  fileTest << " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">" << std::endl; 
  fileTest << "\t<Document>" << std::endl;
  fileTest << "\t\t<name>" << y << "xt.kml</name>" << std::endl;
  fileTest << "\t\t<Region>" << std::endl;
  fileTest << "\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t" << lowerLeft[0]  << "," << lowerLeft[1];
  fileTest << " " << lowerRight[0] << "," << lowerRight[1];
  fileTest << " " << upperRight[0] << "," << upperRight[1];
  fileTest << " " << upperLeft[0]  << "," << upperLeft[1]  << std::endl;
  fileTest << "\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t</Region>" << std::endl;
  fileTest << "\t\t<GroundOverlay>" << std::endl;
  fileTest << "\t\t\t<drawOrder>" << depth + (m_CurIdx * m_MaxDepth) << "</drawOrder>" << std::endl;
  fileTest << "\t\t\t<Icon>" << std::endl;
  fileTest << "\t\t\t\t<href>" << y << ".jpg" << "</href>" << std::endl;
  fileTest << "\t\t\t</Icon>" << std::endl;
  fileTest << "\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t" << lowerLeft[0]  << "," << lowerLeft[1];
  fileTest << " " << lowerRight[0] << "," << lowerRight[1];
  fileTest << " " << upperRight[0] << "," << upperRight[1];
  fileTest << " " << upperLeft[0]  << "," << upperLeft[1]  << std::endl;
  fileTest << "\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t</GroundOverlay>" << std::endl;
  fileTest << "\t</Document>" << std::endl;
  fileTest << "</kml>" << std::endl;
  
  fileTest.close();
}
                                    
void
TileExportModule::
GenerateKMLExtendedWithLink(std::string pathname,
                            int depth, int x, int y, int tileStartX, int tileStartY,
                            OutputPointType lowerLeft, OutputPointType lowerRight,
                            OutputPointType upperRight, OutputPointType upperLeft,
                            double centerLong, double centerLat)
{
  std::ostringstream kmlname;
  kmlname << pathname;
  kmlname << "/";
  kmlname << y << "xt.kml";
  std::ofstream fileTest(kmlname.str().c_str());
  
  fileTest << fixed << setprecision(6);
  
  fileTest << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
  fileTest << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"" << std::endl;
  fileTest << " xmlns:gx=\"http://www.google.com/kml/ext/2.2\">" << std::endl; 
  fileTest << "\t<Document>" << std::endl;
  fileTest << "\t\t<name>" << y << "xt.kml</name>" << std::endl;
  fileTest << "\t\t<Region>" << std::endl;
  fileTest << "\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t" << lowerLeft[0]  << "," << lowerLeft[1];
  fileTest << " " << lowerRight[0] << "," << lowerRight[1];
  fileTest << " " << upperRight[0] << "," << upperRight[1];
  fileTest << " " << upperLeft[0]  << "," << upperLeft[1]  << std::endl;
  fileTest << "\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t</Region>" << std::endl;
  fileTest << "\t\t<GroundOverlay>" << std::endl;
  fileTest << "\t\t\t<drawOrder>" << depth + (m_CurIdx * m_MaxDepth) << "</drawOrder>" << std::endl;
  fileTest << "\t\t\t<Icon>" << std::endl;
  fileTest << "\t\t\t\t<href>" << y << ".jpg" << "</href>" << std::endl;
  fileTest << "\t\t\t</Icon>" << std::endl;
  fileTest << "\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t" << lowerLeft[0]  << "," << lowerLeft[1];
  fileTest << " " << lowerRight[0] << "," << lowerRight[1];
  fileTest << " " << upperRight[0] << "," << upperRight[1];
  fileTest << " " << upperLeft[0]  << "," << upperLeft[1]  << std::endl;
  fileTest << "\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t</GroundOverlay>" << std::endl;
  
  /* Sous tuile 1 */
  std::ostringstream fileTile1;
  fileTile1 << "../../";
  fileTile1 << depth + 1;
  fileTile1 << "/";
  fileTile1 << tileStartX;
  fileTile1 << "/";
  fileTile1 << tileStartY;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile1.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  fileTest << "\t\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t\t" << lowerLeft[0]  << "," << centerLat;
  fileTest << " " << centerLong << "," << centerLat;
  fileTest << " " << centerLong << "," << upperLeft[1];
  fileTest << " " << upperLeft[0]  << "," << upperLeft[1]  << std::endl;
  fileTest << "\t\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile1.str() << "xt.kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;
  
  /* Sous tuile 2 */
  std::ostringstream fileTile2;
  fileTile2 << "../../";
  fileTile2 << depth + 1;
  fileTile2 << "/";
  fileTile2 << tileStartX + 1;
  fileTile2 << "/";
  fileTile2 << tileStartY;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile2.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  fileTest << "\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t\t" << centerLong  << "," << centerLat;
  fileTest << " " << lowerRight[0] << "," << centerLat;
  fileTest << " " << upperRight[0] << "," << upperRight[1];
  fileTest << " " << centerLong << "," << upperLeft[1]  << std::endl;
  fileTest << "\t\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile2.str() << "xt.kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;  
  
  /* Sous tuile 3 */
  std::ostringstream fileTile3;
  fileTile3 << "../../";
  fileTile3 << depth + 1;
  fileTile3 << "/";
  fileTile3 << tileStartX + 1;
  fileTile3 << "/";
  fileTile3 << tileStartY + 1;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile3.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  fileTest << "\t\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t\t" << centerLong  << "," << lowerLeft[1];
  fileTest << " " << lowerRight[0] << "," << lowerRight[1];
  fileTest << " " << upperRight[0] << "," << centerLat;
  fileTest << " " << centerLong  << "," << centerLat  << std::endl;
  fileTest << "\t\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile3.str() << "xt.kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;  
  
  /* Sous tuile 4 */
  std::ostringstream fileTile4;
  fileTile4 << "../../";
  fileTile4 << depth + 1;
  fileTile4 << "/";
  fileTile4 << tileStartX;
  fileTile4 << "/";
  fileTile4 << tileStartY + 1;
  
  fileTest << "\t\t<NetworkLink>" << std::endl;
  fileTest << "\t\t\t<name>" << fileTile4.str() << ".jpg</name>" << std::endl;
  fileTest << "\t\t\t<Region>" << std::endl;
  fileTest << "\t\t\t\t<Lod>" << std::endl;
  fileTest << "\t\t\t\t\t<minLodPixels>" << m_TileSize / 2 << "</minLodPixels>" << std::endl;
  fileTest << "\t\t\t\t\t<maxLodPixels>-1</maxLodPixels>" << std::endl;
  fileTest << "\t\t\t\t</Lod>" << std::endl;
  fileTest << "\t\t\t\t<gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t\t\t<coordinates>" << std::endl;
  fileTest << "\t\t\t\t\t\t" << lowerLeft[0]  << "," << lowerLeft[1];
  fileTest << "\t\t\t\t\t\t" << centerLong << "," << lowerRight[1];
  fileTest << "\t\t\t\t\t\t" << centerLong << "," << centerLat;
  fileTest << "\t\t\t\t\t\t" << upperLeft[0] << "," << centerLat  << std::endl;
  fileTest << "\t\t\t\t\t</coordinates>" << std::endl;
  fileTest << "\t\t\t\t</gx:LatLonQuad>" << std::endl;
  fileTest << "\t\t\t</Region>" << std::endl;
  fileTest << "\t\t\t<Link>" << std::endl;
  fileTest << "\t\t\t\t<href>" << fileTile4.str() << "xt.kml</href>" << std::endl;
  fileTest << "\t\t\t\t<viewRefreshMode>onRegion</viewRefreshMode>" << std::endl;
  fileTest << "\t\t\t\t<viewFormat/>" << std::endl;
  fileTest << "\t\t\t</Link>" << std::endl;
  fileTest << "\t\t</NetworkLink>" << std::endl;  
  
  fileTest << "\t</Document>" << std::endl;
  fileTest << "</kml>" << std::endl;
  fileTest.close();

}




/**
 */
void
TileExportModule::RootKmlProcess(double north, double south, double east, double west)
{
  bool extended = gExtended->value();
  this->GenerateKMLRoot(m_FileName, north, south, east, west,extended);

  // Add the legend for this product if any
  this->AddCurrentProductLegends(0);
            
  // Add the flag netwotk link for each input image
  this->AddNetworkLinkToRootKML(north, south, east, west, m_CurrentImageName,true,0);	
            
  // Root kml must be the first kml created
  // Mutliple Inputs
  for(unsigned int i = 1; i < this->GetNumberOfInputDataByKey("InputImage");i++)
    {
    if (m_Cancel)
      {
      break;
      }
	      
    // Method to write a legend in the kmz
    this->AddCurrentProductLegends(i);
	     
    // Get the filename
    std::string fname  = this->GetInputDataDescription<FloatingVectorImageType>("InputImage",i);
    std::string currentImageName = this->GetCuttenFileName(fname,i);
	      
    // Compute the center of the image because
    // needed to edit the flag LookAt to fly from an
    // image to an other
    
    if(IsProductHaveMetaData(i))
      {
      // Get the pĥysical coordinate of the center
      SizeType tempSize =  this->GetInputData<FloatingVectorImageType>("InputImage",i)->GetLargestPossibleRegion().GetSize();
      InputPointType tempPoint,tempPointOrigin;
      IndexType      tempIndex,tempIndexOrigin;
      tempIndex[0] = tempSize[0];
      tempIndex[1] = tempSize[1];
      tempIndexOrigin.Fill(0);

      this->GetInputData<FloatingVectorImageType>("InputImage",i)->TransformIndexToPhysicalPoint(tempIndex,tempPoint);
      this->GetInputData<FloatingVectorImageType>("InputImage",i)->TransformIndexToPhysicalPoint(tempIndexOrigin,tempPointOrigin);
	      
      // Compute the transform
      TransformType::Pointer tempTransform = TransformType::New();
      tempTransform->SetInputKeywordList(this->GetInputData<FloatingVectorImageType>("InputImage",i)->GetImageKeywordlist());
      tempTransform->SetInputProjectionRef(this->GetInputData<FloatingVectorImageType>("InputImage",i)->GetProjectionRef());
      //tempTransform->SetOutputProjectionRef(wgsRef);
      tempTransform->InstanciateTransform();
	      
      OutputPointType tempOutputPoint,tempOutputPointOrigin;
      tempOutputPoint = tempTransform->TransformPoint(tempPoint);
      tempOutputPointOrigin = tempTransform->TransformPoint(tempPointOrigin);

      this->AddNetworkLinkToRootKML(tempOutputPointOrigin[1],
				    tempOutputPoint[1],tempOutputPointOrigin[0],
				    tempOutputPoint[0], currentImageName,false,
				    i);
      }
    }
	    
  // Last thing to do is to close the root kml 
  this->CloseRootKML();
            
  // Add the root kml in the kmz
  std::ostringstream root_in_kmz;
  root_in_kmz <<m_FileName<<m_KmlExtension;
            
  std::ostringstream root_absolute_path;
  root_absolute_path << m_Path <<"/"<<root_in_kmz.str();
            
  // Add the root file in the kmz
  this->AddFileToKMZ(root_absolute_path, root_in_kmz);
            
  // Remove the root files with stdio method :remove 
  if (remove(root_absolute_path.str().c_str()) != 0)
    {
    itkExceptionMacro(<<"Error while deleting the file" << root_absolute_path.str());
    }
}




/** Add the legend to the product*/
void 
TileExportModule::AddCurrentProductLegends(unsigned int curProd)
{
  for(unsigned int i=0; i<m_ProductVector[curProd].m_AssociatedLegends.size();i++)
    {
    std::ostringstream  legendName;
    legendName << m_Path;
    legendName << "/legend_"<<curProd<<m_ProductVector[curProd].m_AssociatedLegends[i]<<".jpeg";
     
    FloatingVectorImageType::Pointer legend = this->GetInputData<FloatingVectorImageType>("InputLegend",m_ProductVector[curProd].m_AssociatedLegends[i]);
    CastFilterType::Pointer castFiler = CastFilterType::New();
    castFiler->SetInput(legend);
    
    m_VectorWriter = VectorWriterType::New();
    m_VectorWriter->SetFileName(legendName.str().c_str());
    m_VectorWriter->SetInput(castFiler->GetOutput());
    m_VectorWriter->Update();

    // Add the legend to the kmz
    itk::OStringStream  legend_root_path_in_kmz;
    legend_root_path_in_kmz<< "legends/legend_"<<curProd<<m_ProductVector[curProd].m_AssociatedLegends[i]<<".jpeg";

    itk::OStringStream  legend_absolut_path;
    legend_absolut_path<<legendName.str();

    this->AddFileToKMZ(legend_absolut_path, legend_root_path_in_kmz);
    
    // Remove the legend file with stdio method :remove 
    if (remove(legend_absolut_path.str().c_str()) != 0)
      {
      itkExceptionMacro(<<"Error while deleting the file" << legend_absolut_path.str());
      }
    }
}


/** 
  * Add the bounding box kml
  */

void 
TileExportModule::BoundingBoxKmlProcess(double north,double south,double  east,double west)
{
  // Create the bounding kml
  this->GenerateBoundingKML(north, south,  east, west);
	  
  // Add the root kml in the kmz
  std::ostringstream bound_in_kmz;
  bound_in_kmz <<"bounds/bound_"<<m_CurrentProduct<<m_KmlExtension;
  std::ostringstream bound_absolute_path;
  bound_absolute_path << m_Path <<"/bound_"<<m_CurrentProduct<<m_KmlExtension;
	  
  // Add the root file in the kmz
  this->AddFileToKMZ(bound_absolute_path, bound_in_kmz);
	
  // Remove the bounding files with stdio method :remove 
  if (remove(bound_absolute_path.str().c_str()) != 0)
    {
    itkExceptionMacro(<<"Error while deleting the file" << bound_absolute_path.str());
    }
}


/** 
  * Change product name
  */
void 
TileExportModule::UpdateProductInformations()
{
  // Set the composition values
  if(this->CheckAndCorrectComposition(0))
    {
    m_ProductVector[0].m_Composition[0] = this->cRedChannel->value();
    m_ProductVector[0].m_Composition[1] = this->cGreenChannel->value();
    m_ProductVector[0].m_Composition[2] = this->cBlueChannel->value();
    }
}



/**
  * Is Product Selected have geograhical 
  * Informations
  */
bool 
TileExportModule::
IsProductHaveMetaData(unsigned int indexClicked)
{
  // Update Output Information for the product tested
  this->GetInputData<FloatingVectorImageType>("InputImage",indexClicked)->UpdateOutputInformation();
  
  bool emptyProjRef = this->GetInputData<FloatingVectorImageType>("InputImage",indexClicked)->GetProjectionRef().empty();
  bool emptyKWL     = this->GetInputData<FloatingVectorImageType>("InputImage",indexClicked)->GetImageKeywordlist().GetSize()==0?true:false;
  return !emptyProjRef || !emptyKWL;
}

/**
 * Check that the composition is well done
 */
bool
TileExportModule::CheckAndCorrectComposition(unsigned int clickedIndex)
{
  unsigned int nbComponent = this->GetInputData<FloatingVectorImageType>("InputImage",clickedIndex)->GetNumberOfComponentsPerPixel();

  if(this->cRedChannel->value() >= static_cast<int>(nbComponent))
    this->cRedChannel->value(nbComponent-1); // Set the RedChannel on
					     // the component - 1
					     // position if the value
					     // exceed the number of components
  
  if(this->cGreenChannel->value() >= static_cast<int>(nbComponent))
    this->cGreenChannel->value(nbComponent-1);
  
  if(this->cBlueChannel->value() >= static_cast<int>(nbComponent))
    this->cBlueChannel->value(nbComponent-1);
  
  Fl::flush();
  
  return true;
}

/**
 * Check that the composition is well done
 */
void
TileExportModule::StoreAssociations()
{
  unsigned int nbLegends = this->GetNumberOfInputDataByKey("InputLegend");
  
  // associate the legend to the product
  for(unsigned int i = 0; i < nbLegends; i++)
    m_ProductVector[0].m_AssociatedLegends.push_back(i);
}


} // End namespace otb

#endif