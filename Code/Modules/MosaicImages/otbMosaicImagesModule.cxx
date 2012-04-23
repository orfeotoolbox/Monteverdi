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
#include "otbMosaicImagesModule.h"

namespace otb
{
/** Constructor */
MosaicImagesModule::MosaicImagesModule()
{
  // Handle both images with quicklook and image without
  this->AddInputDescriptor<ImageWithQuicklook>("InputImage", otbGetTextMacro("Image to read"), false, true);
  //this->AddTypeToInputDescriptor<VectorImageType>("InputImage");

  // Instanciate filters
  m_VectorTileFilter   = VectorTileImageFilterType::New();
  m_QLVectorTileFilter = VectorTileImageFilterType::New();
}

/** Destructor */
MosaicImagesModule::~MosaicImagesModule()
{}

/** PrintSelf method */
void MosaicImagesModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void MosaicImagesModule::Run()
{
  VectorTileImageFilterType::SizeType layout;
  layout.Fill(0);

  TileMapType tileMap;

  unsigned int nbComp = 0;
  unsigned int shrinkFactor = 0;

  // First step is to retrieve the inputs
  // Get the number of input image
  unsigned int numberOfInputsImages = this->GetNumberOfInputDataByKey("InputImage");
  if (numberOfInputsImages == 0)
    {
    itkExceptionMacro(<< "The number of inputs images is Null.");
    }

  // Parse input images
  for (unsigned int i = 0; i < numberOfInputsImages; i++)
    {
    ImageWithQuicklook::Pointer vectorImageQL = this->GetInputData<ImageWithQuicklook>("InputImage", i);
    
    if(vectorImageQL.IsNull())
      {
      itkExceptionMacro("The image pointer is not initialized!!");
      }

    // Retrieve image and quicklook
    VectorImageType::Pointer image = vectorImageQL->GetImage();
    VectorImageType::Pointer ql    = vectorImageQL->GetQuicklook();

    nbComp = image->GetNumberOfComponentsPerPixel();
    shrinkFactor = vectorImageQL->GetShrinkFactor();

    // Retrieve band name
    std::string imageName = this->GetInputDataDescription<ImageWithQuicklook>("InputImage", i);

    // Parse image name to determine the position of the tile
    unsigned int row, col;

    // Try to parse pleiades tiles
    bool parseSucceed = ParsePleiadeTiles(imageName, row, col);

    // If parsing succeed, insert into tile map
    if(parseSucceed)
      {
      TileIndexType tindex(row, col);
      TileType      tile(image, ql);
      tileMap[tindex] = tile;
      
      std::cout<<"Image "<<imageName<<" will be placed at "<<row<<", "<<col<<" location"<<std::endl;

      // Update layout
      if(layout[0] < col)
        {
        layout[0] = col;
        }
      if(layout[1] < row)
        {
        layout[1] = row;
        }
      }
    }

  layout[0]+=1;
  layout[1]+=1;

  // Set the filters layout
  m_VectorTileFilter->SetLayout(layout);
  m_QLVectorTileFilter->SetLayout(layout);

  // Populate filters input
  for(TileMapType::const_iterator it = tileMap.begin();
      it!=tileMap.end(); ++it)
    {
    unsigned int linearIndex = it->first.first * layout[0] + it->first.second;
    std::cout<<"Adding image with linear index: "<<linearIndex<<std::endl;
    m_VectorTileFilter->SetInput(linearIndex, it->second.first);
    m_QLVectorTileFilter->SetInput(linearIndex, it->second.second);
    }
 
  // First, clear any previous output
  this->ClearOutputDescriptors();
 
  // Synthetize new output
  ImageWithQuicklook::Pointer output = ImageWithQuicklook::New();
  output->SetImage(m_VectorTileFilter->GetOutput());
  output->SetQuicklook(m_QLVectorTileFilter->GetOutput());
  output->SetShrinkFactor(shrinkFactor);

  std::cout<<"Layout: "<<layout<<std::endl;

  m_VectorTileFilter->GetOutput()->UpdateOutputInformation();
  m_QLVectorTileFilter->GetOutput()->UpdateOutputInformation();

  //std::cout<<"Largest region: "<<m_VectorTileFilter->GetOutput()->GetLargestPossibleRegion()<<std::endl;
  //std::cout<<"Largest QLregion: "<<m_QLVectorTileFilter->GetOutput()->GetLargestPossibleRegion()<<std::endl;

 // Add an output (single version)
  this->AddOutputDescriptor(output, "OutputImage",
                            otbGetTextMacro("Mosaicked image"));
  // Last, when all outputs where declared, notify listeners
  this->NotifyOutputsChange();
}

bool MosaicImagesModule::ParsePleiadeTiles(const std::string& imageName, unsigned int & row, unsigned int & col)
{
  // Look for R and C flags
  std::size_t lastchar = imageName.find_last_of(".");

  std::size_t rpos = imageName.find_last_of("R", lastchar);
  std::size_t cpos = imageName.find_last_of("C", lastchar);
  
  // Sanity check
  if(rpos >= imageName.size() || cpos >= imageName.size())
    {
    return false;
    }

  // Extract R and C index
  try
    {
    row = strtol(imageName.substr(rpos+1, cpos - rpos + 1).c_str(), NULL, 0)-1;
    col = strtol(imageName.substr(cpos+1).c_str(), NULL, 0)-1;
    }
  catch(...)
    {
    return false;
    }

  return true;
}

} // End namespace otb
