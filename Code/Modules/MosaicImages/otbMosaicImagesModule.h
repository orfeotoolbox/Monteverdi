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
#ifndef __otbMosaicImagesModule_h
#define __otbMosaicImagesModule_h

// include the base class
#include "otbModule.h"

// include the OTB/ITK elements
#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbImageList.h"
#include "otbTileImageFilter.h"

namespace otb
{
/** \class MosaicImagesModule
 *  \brief This is the MosaicImages module
 *
 * 
 *
 */

class ITK_EXPORT MosaicImagesModule
  : public Module
{
public:
  /** Standard class typedefs */
  typedef MosaicImagesModule            Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(MosaicImagesModule, Module);

  /** Data typedefs */
  /// Dataset
  typedef TypeManager::Floating_Point_Image       ImageType;
  typedef TypeManager::Floating_Point_VectorImage VectorImageType;
  typedef TypeManager::FloatImageWithQuicklook    ImageWithQuicklook;

  typedef otb::TileImageFilter<VectorImageType>   VectorTileImageFilterType;

  typedef std::pair<unsigned int, unsigned int> TileIndexType;
  typedef std::pair<VectorImageType::Pointer,VectorImageType::Pointer> TileType;
  typedef std::map<TileIndexType,TileType> TileMapType;

protected:
  /** Constructor */
  MosaicImagesModule();

  /** Destructor */
  virtual ~MosaicImagesModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

private:
  MosaicImagesModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  bool ParsePleiadeTiles(const std::string & name, unsigned int & row, unsigned int & col);

  VectorTileImageFilterType::Pointer m_VectorTileFilter;
  VectorTileImageFilterType::Pointer m_QLVectorTileFilter;
};

} // End namespace otb

#endif
