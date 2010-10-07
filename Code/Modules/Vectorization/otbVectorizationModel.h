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
#ifndef __otbVectorizationModel_h
#define __otbVectorizationModel_h

#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbEventsSender.h"
#include "otbTypeManager.h"

#include "otbVectorDataProjectionFilter.h"
#include "otbVectorDataExtractROI.h"

// Visu
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"

// Vectorization
#include "otbVectorDataModel.h"

////////////////////////////////////////////
// AUTOMATIC MODE INCLUDES
////////////////////////////////////////////

//Transform clustering result to polygon
#include "itkLabelObject.h"
#include "otbLabelObjectToPolygonFunctor.h"

//Polygons selected
#include "otbObjectList.h"

#include "itkLabelMap.h"
#include "itkLabelImageToLabelMapFilter.h"

#include "otbMeanShiftVectorImageFilter.h"

// Extract ROI
#include "itkExtractImageFilter.h"
#include "otbMultiToMonoChannelExtractROI.h"

///////////////////////////////////////////
// END AUTOMATIC MODE INCLUDES
////////////////////////////////////////////


namespace otb {

/** \class VectorizationModel
 *
 */
class ITK_EXPORT VectorizationModel
  : public  MVCModel<ListenerBase>, public itk::Object, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef VectorizationModel            Self;
  typedef MVCModel<ListenerBase>        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(VectorizationModel, MVCModel);

  /** New macro */
  itkNewMacro(Self);

  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef VectorImageType::Pointer                VectorImagePointerType;
  typedef VectorImageType::IndexType              IndexType;
  typedef VectorImageType::SizeType               SizeType;
  typedef VectorImageType::PointType              ImagePointType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>            RGBPixelType;
  typedef Image<RGBPixelType, 2>                  RGBImageType;
  typedef ImageLayer<VectorImageType,
      RGBImageType>                               LayerType;
  typedef ImageLayerGenerator<LayerType>          LayerGeneratorType;
  typedef LayerGeneratorType::Pointer             LayerGeneratorPointerType;
  typedef ImageLayerRenderingModel
  <RGBImageType>                                  VisualizationModelType;
  typedef VisualizationModelType::Pointer         VisualizationModelPointerType;
  typedef LayerGeneratorType::ImageLayerType
  ::OutputPixelType                               OutputPixelType;
  typedef VisualizationModelType::RegionType      RegionType;

  typedef TypeManager::Vector_Data                VectorDataType;
  typedef VectorDataType::Pointer                 VectorDataPointerType;
  typedef VectorDataModel                         VectorDataModelType;
  typedef VectorDataModelType::Pointer            VectorDataModelPointerType;
  typedef VectorDataType::DataNodeType            DataNodeType;
  typedef DataNodeType::PointType                 PointType;
  typedef DataNodeType::PolygonType::VertexType   VertexType; 
  typedef DataNodeType::PolygonType 		  PolygonType;
  
  // Reprojection filter
  typedef VectorDataProjectionFilter<
    VectorDataType,VectorDataType>                VectorDataProjectionFilterType;
  typedef VectorDataExtractROI<VectorDataType>    VectorDataExtractROIType;
  typedef VectorDataExtractROIType::RegionType    RemoteSensingRegionType;

  /** Get the visualization model */
  itkGetObjectMacro(VisualizationModel, VisualizationModelType);

  /** Get the vector data model */
  itkGetObjectMacro(VectorDataModel, VectorDataModelType);

  /** Input Image Pointer */
  itkGetConstObjectMacro(InputImage, VectorImageType);
  void SetImage(VectorImagePointerType image);
  /** Load a vector data */
  void AddVectorData(VectorDataPointerType vData);

  void RemoveDataNode(DataNodeType * node);
  void SetDataNodeFieldAsInt(DataNodeType * node, const std::string& name, int value);
  void SetDataNodeFieldAsFloat(DataNodeType * node, const std::string& name, float value);
  void SetDataNodeFieldAsString(DataNodeType* node, const std::string& name, const std::string& value);
  void RemoveFieldFromDataNode(DataNodeType * node, const std::string& name);
  void RemovePointFromDataNode(DataNodeType * node,
                               const unsigned long& index,
                               bool interiorRing,
                               const unsigned int& interiorRingIndex = 0);
  void UpdatePointFromDataNode(DataNodeType * node,
                               const unsigned long& index,
                               const PointType& point,
                               bool interiorRing,
                               const unsigned int& interiorRingIndex = 0);
  void UpdateAlpha(double alpha);
  void OK();
  void FocusOnDataNode(const PointType& index);
  
  void DeleteGeometry(void){}

  ////////////////////////////////////////////
  // AUTOMATIC MODE METHODS AND TYPEDEFS
  ////////////////////////////////////////////

  // Label Type
  typedef TypeManager::Label_Short_Precision        LabelType;
  typedef TypeManager::Labeled_Short_Image          LabeledImageType; 
  typedef LabeledImageType::Pointer                 LabeledImagePointerType;

  typedef ObjectList<LabeledImageType>              LabeledImageListType;
  
  // Extract ROI
  typedef itk::ExtractImageFilter< VectorImageType, 
                                   VectorImageType > ExtractImageFilterType;
  typedef ExtractImageFilterType::Pointer            ExtractImageFilterPointerType;
  
  /** Transform label image to label map */
  typedef itk::LabelObject<LabelType, 2>	     LabelObjectType;
  typedef itk::LabelMap<LabelObjectType>	     LabelMapType;
  typedef LabelMapType::Pointer 		     LabelMapPointerType;
  typedef itk::LabelImageToLabelMapFilter<
    LabeledImageType, LabelMapType>	             LabelImageToLabelMapFilterType;
  
  typedef Functor::LabelObjectToPolygonFunctor<
    LabelObjectType, PolygonType>                    LabelObject2PolygonFunctorType;

  //MeanShift
  typedef MeanShiftVectorImageFilter<
    VectorImageType, 
    VectorImageType, 
    LabeledImageType>                                MeanShiftVectorImageFilterType;

  // Method the extract the region seen on the full widget
  void ExtractRegionOfImage(RegionType ExtRegion);

  // Generate the segmented image of the full region seen on the full
  // image 
  void GenerateLayers();

  // Methods necessary for the automatic mode : called from the
  // automatic vectordata handler
  void RightIndexClicked(const IndexType & index, RegionType ExtRegion);
  
  void LeftIndexClicked(const IndexType & index, RegionType ExtRegion);

  // Put the next methods protected 
  LabeledImagePointerType GenerateMeanshiftClustering(int SpatialRadius, 
                                                      double RangeRadius, 
                                                      int MinRegionSize);
  
  LabelMapPointerType ConvertLabelImageToLabelMap(LabeledImagePointerType inputImage);

  // Delete the generated label map relative to the previous extracted
  // image 
  void DeleteLayers();
  
  /** Macro to get the selected polygon to visualize */
  itkGetObjectMacro(SelectedVectorData,VectorDataType);

  /** Get the list of the selected algorithms */
  //itkGetStringMacro(AlgorithmsNameList);
  std::vector<std::string> GetAlgorithmsNameList()
    { 
      return m_AlgorithmsNameList;
    }
  
  /** Get the actual layer number */
  itkGetMacro(ActualLayerNumber,int);
  
  /** Get the flag for the extract region update */
  itkGetMacro(ExtractRegionUpdated,bool);
  
  ////////////////////////////////////////
  // END AUTOMATIC MODE METHODS 
  ////////////////////////////////////////
  
  /** Output accessor. */
  itkGetObjectMacro(Output, VectorDataType);
  
  /** Output changed accessor. */
  itkGetMacro( OutputChanged, bool );
  
  /** DEM directory accessors. */
  itkGetConstMacro(DEMPath, std::string);
  itkSetMacro(DEMPath, std::string);

  /** Use DEM or not accessor. */
  itkGetMacro(UseDEM, bool);
  itkSetMacro(UseDEM, bool);

  /** Receive notifications */
  virtual void Notify();

protected:
  /** Constructor */
  VectorizationModel();
  /** Destructor */
  virtual ~VectorizationModel();


private:
  VectorizationModel(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Visualization */
  VisualizationModelPointerType m_VisualizationModel;
  LayerGeneratorPointerType     m_ImageGenerator;

  /** Input Images */
  VectorImagePointerType        m_InputImage;

  /** VectorData model */
  VectorDataModelPointerType    m_VectorDataModel;

  /**Output vector data. */
  VectorDataPointerType         m_Output;
  
  /** Has a new output or not. */
  bool                          m_OutputChanged;

  /** DEM directoryu path. */
  std::string        m_DEMPath;

  /** Use DEM or not. */
  bool               m_UseDEM;

  /** Extract Full Widget region */
  RegionType                    m_LastRegionSelected;
  ExtractImageFilterPointerType m_ExtractImageFilter;
  bool                          m_ExtractRegionUpdated;

  // ObjectList to store the LabeledImage as result of the several
  // segmentation algorithms used in the automatic mode
  //  --> LabeledImageListType::Pointer 
  std::vector<LabeledImagePointerType>   m_LabelImageVector;
  std::vector<LabelMapPointerType>       m_LabelMapVector;

  // Selected Polygon on full image right click
  PolygonType::Pointer		  m_SelectedPolygon;
  DataNodeType::Pointer           m_SelectedPolygonNode;

  int                             m_ActualLayerNumber;

  VectorDataType::Pointer         m_SelectedVectorData;

  std::vector<std::string>        m_AlgorithmsNameList;
};

} //end namespace otb
#endif
