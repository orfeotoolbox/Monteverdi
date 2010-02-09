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
#ifndef __otbTileMapImportModule_h
#define __otbTileMapImportModule_h

// include the base class
#include "otbModule.h"

// include the GUI
#include "otbTileMapImportModuleGUI.h"

// include the OTB elements - Placename
#include "otbPlaceNameToLonLat.h"
#include "otbCoordinateToName.h"

// include the OTB elements - Extract Tile Map
#include "itkRGBPixel.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbTileMapImageIO.h"
#include "otbInverseSensorModel.h"
#include "otbMultiChannelExtractROI.h"
#include "otbImageFileWriter.h"
#include "ossim/projection/ossimTileMapModel.h"

#include "otbMsgReporter.h"

namespace otb
{
  /** \class TileMapImportModule
   *  \brief
   *
   *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
   */
  
  class ITK_EXPORT TileMapImportModule
  : public Module, public TileMapImportModuleGUI
  {
  public:
    /** Standard class typedefs */
    typedef TileMapImportModule                           Self;
    typedef Module                                        Superclass;
    typedef itk::SmartPointer<Self>                       Pointer;
    typedef itk::SmartPointer<const Self>                 ConstPointer;
    
    typedef otb::PlaceNameToLonLat                        PlaceNameToLonLatType;
    typedef PlaceNameToLonLatType::Pointer                PlaceNameToLonLatPointerType;
    
    typedef otb::CoordinateToName                         CoordinateToNameType;
    typedef CoordinateToNameType::Pointer                 CoordinateToNamePointerType;
    
    typedef otb::VectorImage<double, 2>                   ImageType;
    
    typedef otb::ImageFileReader<ImageType>               ReaderType;
    typedef ReaderType::Pointer                           ReaderPointerType;
    
    typedef otb::TileMapImageIO                           ImageIOType;
    typedef ImageIOType::Pointer                          ImageIOPointerType;
    
    typedef otb::InverseSensorModel<double>               ModelType;
    typedef ModelType::Pointer                            ModelPointerType;
    
    typedef otb::MultiChannelExtractROI<double, double>   ExtractROIFilterType;
    typedef ExtractROIFilterType::Pointer                 ExtractROIPointerType;
    
    typedef itk::Point <double, 2>                        PointType;
    
    /** New macro */
    itkNewMacro(Self);
    
    /** Type macro */
    itkTypeMacro(ExtractROIModule,Module);
    
    /** OTB typedefs */
    
    /** Show the Module GUI */
    virtual bool CanShow(){return true;};
    /** Show the Module GUI */
    virtual void Show()
    {
      wTileMapImportWindow->show();
    };
    
  protected:
    /** Constructor */
    TileMapImportModule();
    /** Destructor */
    virtual ~TileMapImportModule();
    /** PrintSelf method */
    virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;
    
    /** The custom run command */
    virtual void Run();
    
    /** Callbacks */
    virtual void Ok();
    virtual void Cancel();
    virtual void SearchPlace();
    virtual void UpdatePlace();
    
  private:
    
    TileMapImportModule(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented
    
    /** PlaceName to lat/long */
    PlaceNameToLonLatPointerType    m_PlaceNameToLonLat;
    CoordinateToNamePointerType     m_CoordinateToName;
    
    /** Extract ROI */
    ImageIOPointerType              m_TileIO;
    ReaderPointerType               m_ReaderTile;
    ModelPointerType                m_Model;
    ExtractROIPointerType           m_ExtractROIOsmFilter;
    
    std::string                     m_PlaceName;
    std::string                     m_CountryName;
    
    /** Lat / Long */
    double                          m_Longitude;
    double                          m_Latitude;
    
    /** Depth */
    unsigned int                    m_Depth;
    
    /** Size ROI */
    unsigned int                    m_SizeX;
    unsigned int                    m_SizeY;
    
    /** TileIo */
    std::string                     m_ServerName;
    std::string                     m_CacheDirectory;
  };
  
} // End namespace otb

#endif
