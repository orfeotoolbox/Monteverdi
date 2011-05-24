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
#ifndef __otbTileExportModule_h
#define __otbTileExportModule_h

#include "otbModule.h"
#include "otbTileExportModuleGUI.h"

// include the OTB/ITK elements
#include "otbVectorImage.h"
#include "otbMultiChannelExtractROI.h"
#include "otbImageFileWriter.h"
#include "otbVectorRescaleIntensityImageFilter.h"
#include "otbImageToVectorImageCastFilter.h"

#include "otbStreamingShrinkImageFilter.h"

#include "otbGenericRSTransform.h"

#include "itkCastImageFilter.h"

//#include <ogr_spatialref.h>

#include "itkTimeProbe.h"

//kmz creation
#include "kml/engine/kmz_file.h"
#include "kml/base/file.h"

#include "itkFixedArray.h"

namespace otb
{

/** This is a struct class, so attributes are public*/

class ProductInformations
{
public:
  typedef itk::FixedArray<unsigned int, 3> CompositionType;
  typedef itk::FixedArray<double, 4>       CornersType;

  /** Unique id of the class*/
  unsigned int m_Id;
  /** Name of the product*/
  std::string m_Name;
  /** Description of the product*/
  std::string m_Description;
  /** Array for color composition*/
  CompositionType m_Composition;
  /** Legendes associees*/
  std::vector<unsigned int> m_AssociatedLegends;
  /** Vector of corners coordinates when no kWL nor porjection ref
    * available
    */
  CornersType m_CornerList;
};

/** \class TileExportModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT TileExportModule
  : public Module, public TileExportModuleGUI
{
public:
  /** Standard class typedefs */
  typedef TileExportModule              Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  // Image file
  typedef TypeManager::Floating_Point_Precision   InternalPixelType;
  typedef TypeManager::Label_Char_Precision       OutputPixelType;
  typedef TypeManager::Floating_Point_VectorImage FloatingVectorImageType;
  typedef TypeManager::Labeled_Char_VectorImage   CharVectorImageType;
  typedef TypeManager::Floating_Point_Image       SingleImageType;
  
  // Region
  typedef FloatingVectorImageType::RegionType RegionType;
  typedef FloatingVectorImageType::SizeType   SizeType;
  typedef FloatingVectorImageType::IndexType  IndexType;

  /// Multi channels Extract ROI filter
  typedef MultiChannelExtractROI<InternalPixelType,
                                 OutputPixelType>                    VectorImageExtractROIFilterType;

  // Writer
  typedef ImageFileWriter<CharVectorImageType>                       VectorWriterType;

  // Resampler
  typedef StreamingShrinkImageFilter<FloatingVectorImageType,
                                     FloatingVectorImageType>        StreamingShrinkImageFilterType;

  // Intensity Rescale
  typedef VectorRescaleIntensityImageFilter<FloatingVectorImageType,
                                            FloatingVectorImageType> VectorRescaleIntensityImageFilterType;

  // Transformer
  typedef GenericRSTransform<>           TransformType;
  typedef TransformType::InputPointType  InputPointType;
  typedef TransformType::OutputPointType OutputPointType;

  // Cast Image Filter
  typedef itk::CastImageFilter<FloatingVectorImageType,
                               CharVectorImageType>                 CastFilterType;

  //
  typedef ProductInformations                                       ProductInformationType;
  typedef std::vector<ProductInformationType>                       ProductInformationVectorType;

  /** Cast SingleImage to VectorImageType*/
  typedef ImageToVectorImageCastFilter<SingleImageType,
                                       FloatingVectorImageType>     CastToVectorImageFilterType;

  // Timer
  typedef itk::TimeProbe            TimerType;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(TileExportModule, Module);

protected:
  /** Constructor */
  TileExportModule();
  /** Destructor */
  virtual ~TileExportModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    guiMainWindow->show();
  }

  /** Show the Module GUI */
  virtual void Hide()
  {
    guiMainWindow->hide();
  }

  /** Browse callback */
  virtual void Browse();

  /** Save callback*/
  virtual void SaveDataSet();

  /** Cancel callback*/
  virtual void Cancel();

  /**
   * Add file to KMZ : return the number of error when writing the
   * file in the kmz
   */
  virtual int  AddFileToKMZ(std::ostringstream&  absolutePath, std::ostringstream&   kmz_in_path);

  /**Cut the image file name to built the directory name*/
  std::string GetCuttenFileName(std::string description, unsigned int idx);

  /** Add product to the list*/
  //void AddProductToList(unsigned int id);

  /** Method for checking and correcting color composition*/
  bool CheckAndCorrectComposition(unsigned int clickedIndex);

  /** CallBack to store the association*/
  virtual void StoreAssociations();

  /**
    * Update the product informations to store the Corners values when
    * non geo
    */
  virtual void UpdateProductInformations();

  /** Export non geo products*/
  virtual void ExportNonGeoreferencedProduct(unsigned int curIdx);

  /** Handle Corners coordinates group*/
  virtual void HandleCornersGroup();

  /** Use dem */
  virtual void UseDEM();
  
  /** Browse DEM */
  virtual void BrowseDEM();

private:
  TileExportModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // Tiling
  void Tiling(unsigned int curIdx);

  /** KML root generate */
  void GenerateKMLRoot(std::string title, double north, double south, double east, double west, bool extended);

  /** KML generate  Filename - PathName - tile number - North - South - East - West */
  void GenerateKML(std::string pathname, int depth, int x, int y, double north, double south, double east, double west);
  void GenerateKMLExtended(std::string pathname,
                           int depth,
                           int x,
                           int y,
                           OutputPointType lowerLeft,
                           OutputPointType lowerRight,
                           OutputPointType upperRight,
                           OutputPointType upperLeft);

  /** KML with link generate */
  void GenerateKMLWithLink(std::string pathname,
                           int depth, int x, int y, int tileStartX, int tileStartY,
                           double north, double south, double east, double west, double centerLong, double centerLat);
  void GenerateKMLExtendedWithLink(std::string pathname,
                                   int depth, int x, int y, int tileStartX, int tileStartY,
                                   OutputPointType lowerLeft, OutputPointType lowerRight,
                                   OutputPointType upperRight, OutputPointType upperLeft,
                                   double centerLong, double centerLat);

  /** Method to create the bounding kml of the "iteration" th product */
  void GenerateBoundingKML(double north, double south,
                           double east,  double west);

  /** Method to add the legends in a placemark */
  void AddLegendToRootKml(double north, double south,
                          double east, double west,
                          unsigned int pos);

  /**
    * Add a networkLink <NetworkLink> ....  </NetworkLink>
    * to root kml (ued to handle several image in the same kmz)
    */
  void AddNetworkLinkToRootKML(double north,
                               double south,
                               double east,
                               double west,
                               std::string directory,
                               bool addRegion,
                               unsigned int pos);

  /**
    * Close the root kml by adding the last flags and close the
    * std::ofstream
    */
  void  CloseRootKML();

  /** Method to add the legends relative to the current product */
  void AddCurrentProductLegends(unsigned int curProd);

  /** If Product selected have metadatas*/
  bool IsProductHaveMetaData(unsigned int itkNotUsed(indexClicked));

  /** Method to avoid code duplication*/
  /** call the method that generates the bounding box kml*/
  void BoundingBoxKmlProcess(double north, double south, double east, double west);

  /** Method to generate the root kml*/
  void RootKmlProcess(double north, double south, double east, double west);

  /** Add Roi to kmz*/
  void RegionOfInterestProcess();

  // File and path name
  std::string m_FileName;
  std::string m_Path;
  std::string m_CurrentImageName;

  // Convenience string
  std::string m_KmzExtension;
  std::string m_KmlExtension;

  // Pointer to the vector image
  FloatingVectorImageType::Pointer m_VectorImage;
  FloatingVectorImageType::Pointer m_ResampleVectorImage;

  // Extract ROI
  VectorImageExtractROIFilterType::Pointer m_VectorImageExtractROIFilter;

  // Writer
  VectorWriterType::Pointer m_VectorWriter;

  // Resampler
  StreamingShrinkImageFilterType::Pointer m_StreamingShrinkImageFilter;

  // Rescale intensity
  VectorRescaleIntensityImageFilterType::Pointer m_VectorRescaleIntensityImageFilter;

  // Transformer
  TransformType::Pointer m_Transform;

  // DEM directory
  std::string m_DEMDirectory;

  // Legend
  FloatingVectorImageType::Pointer m_Logo;
  std::ostringstream               m_LogoFilename;
  bool                             m_HasLegend;
  bool                             m_HasLogo;

  // Tile size
  int m_TileSize;

  // KMZ file
  kmlengine::KmzFilePtr m_KmzFile;

  // KMZ file name
  std::ostringstream m_KmzFileName;

  // the kml root ofstream
  std::ofstream m_RootKmlFile;
  std::ofstream m_TempRootKmlFile;

  // Cancel
  bool m_Cancel;

  // Max
  int          m_MaxDepth;
  unsigned int m_CurIdx;
  unsigned int m_NbOfInput;
  unsigned int m_CurrentProduct;
  int          m_CurrentDepth;

  // Center points vector
  bool                                    m_RegionOfInterestKmlGenerated;
  std::vector<std::pair<double, double> > m_CenterPointVector;

  // Product Info vector type
  ProductInformationVectorType            m_ProductVector;

  // Geo Corners Coordinates
  OutputPointType                         m_UpperLeftCorner;
  OutputPointType                         m_UpperRightCorner;
  OutputPointType                         m_LowerLeftCorner;
  OutputPointType                         m_LowerRightCorner;
  
  // Cast to vectorImage filter
  CastToVectorImageFilterType::Pointer    m_CastToVectorImageFilter;
};

} // End namespace otb

#endif
