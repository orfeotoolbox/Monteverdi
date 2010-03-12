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
#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbVectorImage.h"
#include "otbImage.h"
#include "projection/ossimBilinearProjection.h"
#include "projection/ossimRpcProjection.h"
#include "projection/ossimRpcModel.h"
#include "projection/ossimProjection.h"
#include "projection/ossimSensorModel.h"

#include "otbDEMHandler.h"
#include "itkContinuousIndex.h"

#include "otbTileMapImageIO.h"
#include "otbImageFileReader.h"
#include "ossim/projection/ossimTileMapModel.h"
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
  typedef GCPToSensorModelModel                         Self;
  typedef MVCModel<ListenerBase> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Standard type macro */
  itkTypeMacro(GCPToSensorModelModel, Object);

  typedef double                              PixelType;
  typedef VectorImage<PixelType,2>            VectorImageType;
  typedef Image<PixelType,2>                  ImageType;
  typedef VectorImageType::Pointer            VectorImagePointerType;
  typedef VectorImageType::IndexType          IndexType;
  typedef VectorImageType::SizeType           SizeType;
  typedef VectorImageType::PointType          ImagePointType;
  typedef itk::ContinuousIndex<>              ContinuousIndexType;
  typedef itk::ContinuousIndex<double, 3>     Continuous3DIndexType;

  typedef std::pair<ContinuousIndexType, ContinuousIndexType>  IndexCoupleType;
  typedef std::vector<IndexCoupleType>     IndexesListType;
  typedef std::vector<Continuous3DIndexType> Continuous3DIndexListType;

  typedef enum { MEAN, DEM, GCP } ElevManagementEnumType;
  typedef enum { BILINEAR, RPC } ProjectionEnumType;

  /** GCPToRPCSensorModel */
  typedef GCPsToRPCSensorModelImageFilter<VectorImageType>          GCPsToRPCSensorModelImageFilterType;
  typedef GCPsToRPCSensorModelImageFilterType::GCPsContainerType        GCPsContainerType;
  typedef GCPsToRPCSensorModelImageFilterType::GCPType                  GCPType;
  typedef GCPsToRPCSensorModelImageFilterType::Point2DType              Point2DType;  // Sensor coordinates
  typedef GCPsToRPCSensorModelImageFilterType::Point3DType              Point3DType;  // Ground coordinates

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                              RGBPixelType;
  typedef Image<RGBPixelType,2>                                     RGBImageType;
  typedef ImageLayer<VectorImageType,RGBImageType>                  LayerType;
  typedef ImageLayerGenerator<LayerType>                            LayerGeneratorType;
  typedef LayerGeneratorType::Pointer                               LayerGeneratorPointerType;
  typedef ImageLayerRenderingModel<RGBImageType>                    VisualizationModelType;
  typedef VisualizationModelType::Pointer                           VisualizationModelPointerType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;
  typedef BlendingFunctionType::Pointer                                                               BlendingFunctionPointerType;

  /** Map viewer */
  typedef unsigned char                         MapPixelType;
  typedef VectorImage<MapPixelType, 2>          MapImageType;
  typedef ImageFileReader<MapImageType>         MapReaderType;
  typedef MapReaderType::Pointer                MapReaderPointerType;
  typedef TileMapImageIO                        TileMapType;
  typedef TileMapType::Pointer                  TileMapPointerType;
  
  typedef VisualizationModelType::RegionType    RegionType;
  
  typedef otb::InverseSensorModel<double>       MapModelType;
  typedef MapModelType::Pointer                 MapModelPointerType;

  typedef otb::ForwardSensorModel<double>       MapForwardModelType;
  typedef MapForwardModelType::Pointer          MapForwardModelPointerType;

  typedef ImageLayer<MapImageType,RGBImageType>                        MapLayerType;
  typedef ImageLayerGenerator<MapLayerType>                            MapLayerGeneratorType;
  typedef MapLayerGeneratorType::Pointer                               MapLayerGeneratorPointerType;

  /** New macro */
  itkNewMacro(Self);

  /** Point Set */
  typedef itk::PointSet< float, 2 > PointSetType;
  typedef PointSetType::Pointer     PointSetPointerType;
  typedef PointSetType::PointType   PointType;
  typedef itk::Point<double,2>      OutPointType;
  typedef std::vector<OutPointType> OutPointListType;

  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** Get the visualization model */
  itkGetObjectMacro(VisualizationModel, VisualizationModelType);

  /** Input Image Pointer */
  itkGetConstObjectMacro(InputImage, VectorImageType);
  void SetImage(VectorImagePointerType image);

  /** Indexes list manipulation. */
  IndexesListType GetIndexesList() const { return m_IndexesList; }
  void AddIndexesToList( ContinuousIndexType id1,  ContinuousIndexType id2, double elev );
  void ClearIndexesList()
  {
    m_IndexesList.clear();
    m_GCPsElevation.clear();
    m_UsedElevation.clear();
    m_DEMsElevation.clear();
  }
  void RemovePointFromList( unsigned int id );

  /// Import/Export GCPs to/from an xml file
  void ImportGCPsFromXmlFile(const char * fname);
  void ExportGCPsToXmlFile(const char * fname) const;

  /** Projection performing */
  void ComputeBilinearProjection();
  void ComputeRPCProjection();
  void ComputeTransform()
  {
    if(m_ProjectionType == BILINEAR)
      this->ComputeBilinearProjection();
    else if(m_ProjectionType == RPC)
      this->ComputeRPCProjection();
  };
  /** Compute the transform on one point */
  Continuous3DIndexType TransformPoint(ContinuousIndexType id, double height=0. );

  /** Compute the transform the points of m_IndexList */
  Continuous3DIndexListType TransformPoints();

  /** Load GCP */
  void LoadGCP();

  /** Update Output */
  void OK();

  /** Get the output changed flag */
  itkGetMacro(OutputChanged,bool);
  /** Get Output image */
  itkGetObjectMacro(Output, VectorImageType);

  /** Get/Set DEMPath */
  void SetDEMPath( const std::string & DEMPath );
  itkGetConstMacro(DEMPath, std::string);

  /** Set/Get Use DEM */
  itkSetMacro(ElevMgt, ElevManagementEnumType);
  itkGetConstMacro(ElevMgt, ElevManagementEnumType);
  /** Set/Get mean elevation */
  void SetMeanElevation(double meanElev);
  itkGetMacro(MeanElevation, double);

  /** Get HasNewLoadedGCPs */
  itkGetConstMacro(HasNewImage, bool);

  /** Get ProjectionUpdated */
  itkGetConstMacro(ProjectionUpdated, bool);

  /** Get used elevation */
  double GetUsedElevation(unsigned int i)
  {
    if( i>m_UsedElevation.size() )
      itkExceptionMacro("Invalid index, "<<i<<" outside vector size: "<<m_UsedElevation.size());

    return m_UsedElevation[i];
  };
  std::vector<double> GetUsedElevation() { return m_UsedElevation; };
  /** According to the type of elevation manageme,t generate the used list. */
  void GenerateUsedElevation();

  /** Set/Get Projection type */
  itkSetMacro(ProjectionType, ProjectionEnumType);
  itkGetConstMacro(ProjectionType, ProjectionEnumType);

  /** Get Ground error projection */
  itkGetConstMacro(GroundError, double);

  /** Map Viewer */
  void SetMap(long int sizeX, long int sizeY);
  void SearchLatLong(std::string placename);
  void SearchPlaceName(double latitude, double longitude);
  void DisplayMap(std::string placename, double latitude, double longitude, unsigned int depth, long int sizeX, long int sizeY);

  itkGetConstMacro(PlaceNameChanged, bool);
  itkGetConstMacro(LatLongChanged, bool);
  itkGetConstMacro(DepthChanged, bool);
  itkGetConstMacro(HasNewMap, bool);
  itkGetConstMacro(SelectedPointChanged, bool);

  itkGetStringMacro(PlaceName);
  itkGetMacro(Latitude, double);
  itkGetMacro(Longitude, double);
  
  itkGetMacro(SelectedLatitude, double);
  itkGetMacro(SelectedLongitude, double);
  
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

protected:

  /** Constructor */
  GCPToSensorModelModel();
  /** Destructor */
  ~GCPToSensorModelModel();


private:
  GCPToSensorModelModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Singleton instance */
  static Pointer                     Instance;

  /** Visualization */
  VisualizationModelPointerType m_VisualizationModel;
  LayerGeneratorPointerType     m_ImageGenerator;
  BlendingFunctionPointerType   m_BlendingFunction;
  /** Input Images */
  VectorImagePointerType        m_InputImage;
  /** First and second input image indexes list */
  IndexesListType m_IndexesList;
  /** Resampled  image */
  VectorImagePointerType m_Output;
  /** Projection */
  ossimRefPtr<ossimProjection> m_Projection;
  ///*ossimSensorModel*/ ossimRpcModel * m_Projection;
  ossimRefPtr<ossimRpcProjection> m_RpcProjection;
  ossimRefPtr<ossimRpcModel> m_RpcModel;


  bool m_OutputChanged;
  /** DEM directory path*/
  std::string m_DEMPath;
  /** Use DEM or mean elevation */
  ElevManagementEnumType m_ElevMgt;
  /** Store the mea elevation value */
  double m_MeanElevation;
  /** DEM handler */
  DEMHandler::Pointer m_DEMHandler;
  /** GCP elevation list */
  std::vector<double> m_GCPsElevation;
  /** Used elevation list */
  std::vector<double> m_UsedElevation;
  /** DEM elevations*/
  std::vector<double> m_DEMsElevation;
  /** GCPs were loaded from the image */
  bool m_HasNewImage;
  /** Projection has been updated */
  bool m_ProjectionUpdated;
  /** Projection type */
  ProjectionEnumType m_ProjectionType;
  /** Ground error projection */
  double m_GroundError;
  
  /** GCP To RCP Sensor Model Image Filter */
  GCPsToRPCSensorModelImageFilterType::Pointer m_GCPsToRPCSensorModelImageFilter;
  
  /** Map viewer */
  MapReaderPointerType            m_MapReader;
  TileMapPointerType              m_TileIO;
  MapModelPointerType             m_MapModel;
  MapForwardModelPointerType      m_MapInverseModel;
  
  std::string m_PlaceName;
  double m_Latitude;
  double m_Longitude;
  unsigned int m_Depth;
  
  double m_SelectedLatitude;
  double m_SelectedLongitude;
  
  long int m_SizeX;
  long int m_SizeY;
  
  RegionType m_Region;
  
  std::string m_ServerName;
  std::string m_CacheDirectory;

  /** Visualization */
  VisualizationModelPointerType m_MapVisualizationModel;
  MapLayerGeneratorPointerType  m_MapImageGenerator;
  BlendingFunctionPointerType   m_MapBlendingFunction;
  
  bool m_PlaceNameChanged;
  bool m_LatLongChanged;
  bool m_DepthChanged;
  bool m_HasNewMap;
  bool m_SelectedPointChanged;
};

}//end namespace otb
#endif
