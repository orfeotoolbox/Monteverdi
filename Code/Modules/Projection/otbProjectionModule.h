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
#ifndef __otbProjectionModule_h
#define __otbProjectionModule_h

// Disabling deprecation warning
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif
#include "otbProjectionModuleGroup.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include "otbObjectList.h"
#include "otbVectorImage.h"
#include <FL/Fl_File_Chooser.H>

#include "otbDEMToImageGenerator.h"
#include "itkInterpolateImageFunction.h"

#include "otbMVCModel.h"
#include "otbListenerBase.h"


namespace otb
{
/** \class ProjectionModule
 *  \brief
 *
 * \ingroup
 * \ingroup
 */
class ITK_EXPORT ProjectionModule
  : public itk::ProcessObject, public ProjectionModuleGroup, public MVCModel<ListenerBase>
  {
public:
  /** Standard typedefs */
  typedef ProjectionModule                   Self;
  typedef itk::ProcessObject            Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Type macro */
  itkNewMacro(Self);

  /** Creation through object factory macro */
  itkTypeMacro(ProjectionModule,ProcessObject);

  /** Template parameters typedefs */
  typedef double                                    PixelType;
  typedef VectorImage<PixelType,2>                  ImageType;
  typedef ImageType::Pointer                        ImagePointerType;
  typedef ImageType::SizeType                       SizeType;
  typedef ImageType::IndexType                      IndexType;
  typedef ImageType::PointType                      PointType;
  typedef ImageType::SpacingType                    SpacingType;

  
  // Mono Channel Image Type
  typedef Image<double,2>                           SingleImageType;

  typedef std::vector<std::string>                  StringVectorType;
  typedef std::vector<int>                          IntVectorType;
  typedef std::vector<double>                       DoubleVectorType;


  /** DEM computation typedefs */
  typedef DEMToImageGenerator<ImageType>            DEMToImageGeneratorType;
  typedef DEMToImageGeneratorType::DEMHandlerType   DEMHandlerType;
  typedef DEMHandlerType::PointType                 DEMPointType;
  typedef DEMToImageGeneratorType::SizeType         DEMSizeType;
  typedef DEMToImageGeneratorType::SpacingType      DEMSpacingType;

  /** Interpolator definition*/
  typedef itk::InterpolateImageFunction<SingleImageType, double> InterpType;
  typedef InterpType::Pointer                       InterpPointerType;

  // Input Image Accessor
  itkGetObjectMacro(InputImage,ImageType);
  itkSetObjectMacro(InputImage,ImageType);
  
  //Get Output Image
  itkGetObjectMacro(Output,ImageType);
  
  // Show the GUI
  virtual void Show();

  // Inherited methods
  virtual void OpenDEM();

  // Check the size, spacing, long and lat parameters.
  int CheckImageParameters();
  // Call when clic an image list
  virtual void SelectAction();

protected:

  // Quit the application
  virtual void Quit();
  
  // Launch the orthorectification
  virtual void OK();
  
  // Show the selected ortho area in the extent view
  virtual void InsightExtent();
  
  // Set tile maximum size for streaming
  virtual void SetMaxTileSize();
  
  // Set/Get the used map projection
  void SetMapType(MapType map);
  MapType GetMapType();
  
  // Set/Get the used interpolator
  void SetInterpolatorType(InterpolatorType interp);
  InterpolatorType GetInterpolatorType();

  virtual void  UpdateEastNorth();

  // Compute number of tiles used.
  void ComputeTileNumber();
  
  // Check the map parameters.
  int CheckMapParameters();
  
  // Create the output
  template<class TMapProjection> int CreateOutput(TMapProjection* mapProj);
  
  // Update parameter for UTM projection
  void UpdateUTMParam();
  // Change east/north when change Long/Lat values
  virtual void UpdateMapParam();
  // Update parameter for output image
  void UpdateOutputParameters();
  // Upadate longitude and latitude values
  virtual void UpdateLongLat();
  // Take care that the ortho ref and image ref are different (lower left vs upper left)
  ForwardSensorInputPointType ChangeOrigin(ForwardSensorInputPointType point);
  // Update interpolator
  int UpdateInterpolator();
  // Compute long/long of a carto point IN the original REF
  DoubleVectorType LongLatPointToCartoInOriginRef(ForwardSensorInputPointType latLongPoint);
  // Compûte DEM spacing from the carto spacing
  void  UpdateDEMSpacing();

  itkSetMacro(OutputSize, SizeType);
  itkGetMacro(OutputSize, SizeType);
  itkSetMacro(OutputOrigin, PointType);
  itkGetMacro(OutputOrigin, PointType);
  itkSetMacro(OutputSpacing, SpacingType);
  itkGetMacro(OutputSpacing, SpacingType);

  /** Constructor */
  ProjectionModule();
  /** Destructor */
  virtual ~ProjectionModule()
  {
  };
  /**PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

private:
  ProjectionModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void NotifyListener(ListenerBase * listener);

  SizeType                 m_MainWindowInitSize;
  MapType                  m_MapType;
  InterpolatorType         m_InterpType;
  InterpPointerType        m_Interp;
  SizeType                 m_OutputSize;
  PointType                m_OutputOrigin;
  SpacingType              m_OutputSpacing;
  DoubleVectorType         m_EastNorth;
  SpacingType              m_DEMSpacing;
  
  // Store ref Zone and Hemisphere
  int                      m_UTMZoneRef;
  char                     m_UTMHemRef;
  int                      m_TileNumber;
  double                   m_MaxTileSize;
  
  //Filter Instanciation
  //OrthorectificationFilterType::Pointer m_OrthorectificationFilter;

  // This pointer is used to store the main filter of the application
  itk::ProcessObject::Pointer m_PerBandFilter;

  //Input & Outputs Images 
  ImagePointerType            m_InputImage;
  ImagePointerType            m_Output;

  // Flag to determine if there is an output
  bool m_HasOutput;
};

}// End namespace otb

#endif

