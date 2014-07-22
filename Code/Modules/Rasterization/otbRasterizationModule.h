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
#ifndef __otbRasterizationModule_h
#define __otbRasterizationModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbVectorData.h"
#include "otbVectorDataExtractROI.h"
#include "otbVectorDataProjectionFilter.h"
#include "otbVectorDataProperties.h"
#include "otbVectorDataToMapFilter.h"


namespace otb
{
/** \class RasterizationModule
 *  \brief This is the Rasterization module
 *
 * Description of the module.
 *
 */

class ITK_ABI_EXPORT RasterizationModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef RasterizationModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(RasterizationModule, Module);

  /** Data typedefs */
  /// Dataset

  // Input Image
  typedef TypeManager::Floating_Point_Image       ImageType;

  typedef ImageType::PointType                            PointType;
  typedef ImageType::SizeType                             SizeType;
  typedef ImageType::SpacingType                          SpacingType;
  typedef ImageType::IndexType                            IndexType;

  // Vector Data
  typedef TypeManager::Vector_Data                              VectorDataType;
  typedef VectorDataProjectionFilter<VectorDataType, VectorDataType>   VectorDataProjectionFilterType;
  typedef VectorDataExtractROI<VectorDataType>            VectorDataExtractROIType;
  typedef VectorDataProperties<VectorDataType>            VectorDataPropertiesType;

  // Rasterization
   typedef otb::VectorDataToMapFilter<VectorDataType,
     ImageType>                                                                   VectorDataToMapFilterType;

  // Misc
  typedef otb::RemoteSensingRegion<double>                RemoteSensingRegionType;
  typedef RemoteSensingRegionType::SizeType               SizePhyType;


  /** Check if the input file contains valid support information. */
  bool CheckMetadata();


protected:
  /** Constructor */
  RasterizationModule();

  /** Destructor */
  virtual ~RasterizationModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  RasterizationModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  ImageType::Pointer                        m_InputImage; //image used to retrieve data support
  VectorDataType::Pointer                                   m_InputVectorData; // Data Vector
  VectorDataProjectionFilterType::Pointer   m_VectorDataProjFilter; // Data vector projection filter
  VectorDataPropertiesType::Pointer               m_VectorDataProperties;
  VectorDataExtractROIType::Pointer            m_VectorDataExtractROI;
  VectorDataToMapFilterType::Pointer      m_VectorDataRendering;
  ImageType::Pointer                        m_OutputImage;
};

} // End namespace otb

#endif
