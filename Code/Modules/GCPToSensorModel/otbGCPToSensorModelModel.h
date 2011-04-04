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

#ifndef __otbGCPToSensorModelModel_h
#define __otbGCPToSensorModelModel_h

#include <iostream>
#include <utility>

#include "otbTypeManager.h"
#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbVectorImage.h"
#include "otbImage.h"

#include "otbTileMapImageIO.h"
#include "otbTileMapImageIOHelper.h"
#include "otbImageFileReader.h"
#include "ossimTileMapModel.h"
#include "otbInverseSensorModel.h"
#include "otbForwardSensorModel.h"

// GCPsToSensorModel
#include "otbGCPsToRPCSensorModelImageFilter.h"

//Visu
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"

#include "itkPointSet.h"

namespace otb {

/** \class GCPToSensorModelModel
 *
 *
 *
 */
class ITK_EXPORT GCPToSensorModelModel
  : public MVCModel<ListenerBase>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef GCPToSensorModelModel         Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(GCPToSensorModelModel, Object);

  /** New macro */
  itkNewMacro(Self);

  typedef TypeManager::Floating_Point_Precision   PixelType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef VectorImageType::Pointer                VectorImagePointerType;
  typedef VectorImageType::IndexType              IndexType;
  typedef VectorImageType::SizeType               SizeType;
  typedef VectorImageType::PointType              ImagePointType;

  typedef enum { MEAN, DEM, GCP } ElevManagementEnumType;
  typedef enum { BILINEAR, RPC }  ProjectionEnumType;

  /** GCPToRPCSensorModel */
  typedef GCPsToRPCSensorModelImageFilter<VectorImageType>         GCPsToRPCSensorModelImageFilterType;
  typedef GCPsToRPCSensorModelImageFilterType::Pointer             GCPsToRPCSensorModelImageFilterPointerType;
  typedef GCPsToRPCSensorModelImageFilterType::GCPsContainerType   GCPsContainerType;
  typedef GCPsToRPCSensorModelImageFilterType::ErrorsContainerType ErrorsContainerType;
  typedef std::vector<IndexType>                                   CrossIndexesContainerType;
  typedef GCPsToRPCSensorModelImageFilterType::GCPType             GCPType;
  typedef GCPsToRPCSensorModelImageFilterType::Point2DType         Point2DType;
  typedef GCPsToRPCSensorModelImageFilterType::Point3DType         Point3DType;

  typedef itk::ContinuousIndex<>          ContinuousIndexType;
  typedef itk::ContinuousIndex<double, 3> Continuous3DIndexType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                            RGBPixelType;
  typedef Image<RGBPixelType, 2>                                  RGBImageType;
  typedef ImageLayer<VectorImageType, RGBImageType>               LayerType;
  typedef ImageLayerGenerator<LayerType>                          LayerGeneratorType;
  typedef LayerGeneratorType::Pointer                             LayerGeneratorPointerType;
  typedef ImageLayerRenderingModel<RGBImageType>                  VisualizationModelType;
  typedef VisualizationModelType::Pointer                         VisualizationModelPointerType;
  typedef LayerGeneratorType::ImageLayerType::OutputPixelType     OutputPixelType;
  typedef Function::UniformAlphaBlendingFunction<OutputPixelType> BlendingFunctionType;
  typedef BlendingFunctionType::Pointer                           BlendingFunctionPointerType;

  /** Map viewer */
  typedef unsigned char                 MapPixelType;
  typedef VectorImage<MapPixelType, 2>  MapImageType;
  typedef ImageFileReader<MapImageType> MapReaderType;
  typedef MapReaderType::Pointer        MapReaderPointerType;
  typedef TileMapImageIO                TileMapType;
  typedef TileMapType::Pointer          TileMapPointerType;

  typedef VisualizationModelType::RegionType RegionType;

  typedef otb::InverseSensorModel<double> InverseModelType;
  typedef InverseModelType::Pointer       InverseModelPointerType;

  typedef otb::ForwardSensorModel<double> ForwardModelType;
  typedef ForwardModelType::Pointer       ForwardModelPointerType;

  typedef ImageLayer<MapImageType, RGBImageType> MapLayerType;
  typedef ImageLayerGenerator<MapLayerType>      MapLayerGeneratorType;
  typedef MapLayerGeneratorType::Pointer         MapLayerGeneratorPointerType;

  /** Point Set */
  typedef itk::PointSet<float, 2>   PointSetType;
  typedef PointSetType::Pointer     PointSetPointerType;
  typedef PointSetType::PointType   PointType;
  typedef itk::Point<double, 2>     OutPointType;
  typedef std::vector<OutPointType> OutPointListType;

  /** Get the visualization model */
  itkGetObjectMacro(VisualizationModel, VisualizationModelType);

  /** Input Image Pointer */
  itkGetConstObjectMacro(InputImage, VectorImageType);
  void SetImage(VectorImagePointerType image);

  /** GCP list manipulation. */
  GCPsContainerType GetGCPsContainer() const { return m_GCPsContainer; }
  void AddGCP(float x, float y, float lon, float lat, float elev);
  void ClearGCPsContainer();
  void RemovePointFromGCPsContainer(unsigned int id);

  /** Import/Export GCPs to/from an xml file */
  void ImportGCPsFromXmlFile(const char * fname);
  void ExportGCPsToXmlFile(const char * fname) const;

  /** Load GCP */
  void LoadGCP();

  /** Update Output */
  void OK();

  /** Get the output changed flag */
  itkGetMacro(OutputChanged, bool);

  /** Get Output image */
  itkGetObjectMacro(Output, VectorImageType);

  /** Get/Set DEMPath */
  void SetDEMPath(const std::string& DEMPath);
  itkGetConstMacro(DEMPath, std::string);

  /** Set/Get Use DEM */
  itkSetMacro(ElevMgt, ElevManagementEnumType);
  itkGetConstMacro(ElevMgt, ElevManagementEnumType);

  /** Set/Get mean elevation */
  void SetMeanElevation(double meanElev);
  itkGetMacro(MeanElevation, double);

  /** Set To GCP Management */
  void SetGCPElevation();

  /** Get GCPsContainer has changed */
  itkGetConstMacro(GCPsContainerHasChanged, bool);

  /** Get Ground error projection */
  itkGetConstMacro(GroundError, double);

  /** Get Mean error */
  itkGetConstMacro(MeanError, double);

  /** Get errors */
  ErrorsContainerType GetErrorsContainer() const { return m_ErrorsContainer; }

  /** Get Cross indexes container */
  CrossIndexesContainerType GetCrossIndexesContainer() const { return m_CrossIndexesContainer; }

  /** Map Viewer */
  void SetMap(long int sizeX, long int sizeY);
  void SearchLonLat(std::string placename);
  void SearchPlaceName(double longitude, double latitude);
  void DisplayMap(std::string placename,
                  double longitude,
                  double latitude,
                  unsigned int depth,
                  long int sizeX,
                  long int sizeY);

  /** Notifiers */
  itkGetConstMacro(PlaceNameChanged, bool);
  itkGetConstMacro(LatLongChanged, bool);
  itkGetConstMacro(DepthChanged, bool);
  itkGetConstMacro(HasNewMap, bool);
  itkGetConstMacro(SelectedPointChanged, bool);

  /** Get Lat/Lon and placement were the map is center to */
  itkGetMacro(Latitude, double);
  itkGetMacro(Longitude, double);
  itkGetStringMacro(PlaceName);

  /** Get Lat/Lon selected in the map */
  itkGetMacro(SelectedLatitude, double);
  itkGetMacro(SelectedLongitude, double);

  /** Set/Get Depth for map manipulation */
  itkGetMacro(Depth, unsigned int);
  itkSetMacro(Depth, unsigned int);

  /** Increase Depth with a value */
  void IncreaseDepth(int value, long int x, long y);

  /** MoveMap with on x and y axes */
  void DragMap(long int x, long int y);

  /** Change Selected point */
  void SetSelectedPoint(long int x, long int y);

  /** Select point on map */
  void CenterMapOnSelectedPoint(long int x, long int y, int depth);

  /** Get the visualization model */
  itkGetObjectMacro(MapVisualizationModel, VisualizationModelType);

  /** Get the Scale from the depth */
  const std::string ConvertDepthToScale(const unsigned int depth) const;
protected:

  /** Constructor */
  GCPToSensorModelModel();
  /** Destructor */
  ~GCPToSensorModelModel();

private:
  GCPToSensorModelModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Update the local GCPs Container, refresh the sensor model, notify it */
  void UpdateContainer();

  /** GCPs To RCP Sensor Model Image Filter */
  GCPsToRPCSensorModelImageFilterPointerType m_GCPsToRPCSensorModelImageFilter;

  /** GCP List */
  GCPsContainerType m_GCPsContainer;

  /** Errors container */
  ErrorsContainerType m_ErrorsContainer;

  /** Ground error projection */
  double m_GroundError;

  /** Mean error */
  double m_MeanError;

  /** Visualization */
  VisualizationModelPointerType m_VisualizationModel;
  LayerGeneratorPointerType     m_ImageGenerator;
  BlendingFunctionPointerType   m_BlendingFunction;

  /** Input Images */
  VectorImagePointerType m_InputImage;

  /** Resampled  image */
  VectorImagePointerType m_Output;

  /** Output changed */
  bool m_OutputChanged;

  /** DEM directory path*/
  std::string m_DEMPath;

  /** Use DEM or mean elevation */
  ElevManagementEnumType m_ElevMgt;

  /** Store the mean elevation value */
  double m_MeanElevation;

  /** DEM handler */
  DEMHandler::Pointer m_DEMHandler;

  /** GCPs list has changed */
  bool m_GCPsContainerHasChanged;

  /** Map viewer */
  MapReaderPointerType m_MapReader;
  TileMapPointerType   m_TileIO;

  std::string  m_PlaceName;
  double       m_Latitude;
  double       m_Longitude;
  unsigned int m_Depth;

  double m_SelectedLatitude;
  double m_SelectedLongitude;

  long int m_SizeX;
  long int m_SizeY;

  RegionType m_Region;

  std::string m_ServerName;
  std::string m_CacheDirectory;

  /** Map Visualization */
  VisualizationModelPointerType m_MapVisualizationModel;
  MapLayerGeneratorPointerType  m_MapImageGenerator;
  BlendingFunctionPointerType   m_MapBlendingFunction;

  bool m_PlaceNameChanged;
  bool m_LatLongChanged;
  bool m_DepthChanged;
  bool m_HasNewMap;
  bool m_SelectedPointChanged;

  /** Cross indexes container */
  CrossIndexesContainerType m_CrossIndexesContainer;

};

} //end namespace otb
#endif
