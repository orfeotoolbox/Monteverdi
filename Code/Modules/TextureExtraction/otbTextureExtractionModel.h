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
#ifndef __otbTextureExtractionModel_h
#define __otbTextureExtractionModel_h

#include "otbFeatureExtractionBaseModel.h"

#include "otbFeatureTex.h"

namespace otb
{
/** \class TextureExtractionModel
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT TextureExtractionModel
  : public FeatureExtractionBaseModel
{
public:
  /** Standard class typedefs */
  typedef TextureExtractionModel        Self;
  typedef FeatureExtractionBaseModel        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  typedef HaralickTexture::TextureType     HaralickTextureType;
  typedef AdvancedTexture::TextureType     AdvancedTextureType;
  typedef std::vector<HaralickTextureType> HaralickTextureVectorType;
  typedef std::vector<AdvancedTextureType> AdvancedTextureVectorType;
  
  typedef FeatureInfoBase::FeatureType    FeatureBaseType;
  
  /** Standard type macro */
  itkTypeMacro(TextureExtractionModel, FeatureExtractionBaseModel);
  
  /** Standard type macro */
  itkNewMacro(Self);
  //itkGetMacro(HasInput, bool);
  
  /** Generic method to create a texture filter*/
  void AddTextureFilter(FeatureBaseType featureType, SizeType radius, OffsetType offset, unsigned int bin);

  void AddSFSTexturesFilter(FeatureBaseType type,
                            double spectralThr,
                            unsigned int spatialThr,
                            unsigned int nbDirection,
                            double alpha,
                            unsigned int ratioMaxConsNb);
  
  /** Add Haralick texture with given min max */
  void AddHaralickTextureFilter(HaralickTextureVectorType harList, SizeType radius, OffsetType offset, double minHar, double maxHar, unsigned int bin);
  /** Add Haralick texture with default min max */
 void AddHaralickTextureFilter(HaralickTextureVectorType harList, SizeType radius, OffsetType offset, unsigned int bin);

  /** Add Haralick texture with given min max */
  void AddAdvancedTextureFilter(AdvancedTextureVectorType advList, SizeType radius, OffsetType offset, double minHar, double maxHar, unsigned int bin);
  /** Add Haralick texture with default min max */
  void AddAdvancedTextureFilter(AdvancedTextureVectorType harList, SizeType radius, OffsetType offset, unsigned int bin);
  
  /** Get filtered single output image */
  virtual SingleImagePointerType GetSingleImage(int i);
  
protected:
  /** Constructor */
  TextureExtractionModel();
  /** Destructor */
  virtual ~TextureExtractionModel();

private:

};

}

#endif
