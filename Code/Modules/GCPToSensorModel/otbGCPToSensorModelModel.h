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
//#include "otbStreamingResampleImageFilter.h"
#include "otbDEMHandler.h"
#include "itkContinuousIndex.h"

//Vis
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"

// Estimation
//#include "itkEuclideanDistancePointMetric.h"
//#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSet.h"
//#include "itkPointSetToPointSetRegistrationMethod.h"

// Transformation
//#include "otbTransformEnumType.h"
//#include "itkTransformBase.h"
#include "itkAffineTransform.h"
//#include "itkSimilarity2DTransform.h"
//#include "itkTranslationTransform.h"


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
  typedef std::vector<IndexType>           IndexListType;
  typedef std::vector<Continuous3DIndexType> Continuous3DIndexListType;

  typedef enum { MEAN, DEM, GCP } ElevManagementEnumType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                              RGBPixelType;
  typedef Image<RGBPixelType,2>                                     RGBImageType;
  typedef ImageLayer<VectorImageType,RGBImageType>                  LayerType;
  typedef ImageLayerGenerator<LayerType>                            LayerGeneratorType;
  typedef LayerGeneratorType::Pointer                               LayerGeneratorPointerType;
  typedef ImageLayerRenderingModel<RGBImageType>                    VisualizationModelType;
  typedef VisualizationModelType::Pointer                           VisualizationModelPointerType;
  typedef std::vector<VisualizationModelPointerType>                VisualizationModelListType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;
  typedef BlendingFunctionType::Pointer                                                               BlendingFunctionPointerType;
 
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

  /** Transform performing */
  void ComputeTransform();

  /** Compute the transform on one point */
  Continuous3DIndexType TransformPoint(ContinuousIndexType id );

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
  itkGetMacro(ElevMgt, ElevManagementEnumType);
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
  ossimBilinearProjection * m_Projection;
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
};

}//end namespace otb
#endif
