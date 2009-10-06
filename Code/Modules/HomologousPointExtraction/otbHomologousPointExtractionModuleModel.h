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

#ifndef __otbHomologousPointExtractionModuleModel_h
#define __otbHomologousPointExtractionModuleModel_h

#include <iostream>
#include <utility>
#include "otbMVCModel.h"
#include "otbListenerBase.h"
#include "otbVectorImage.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"

//Vis
#include "otbImageLayerRenderingModel.h"
#include "otbImageLayerGenerator.h"
#include "otbImageLayer.h"

namespace otb {

/** \class HomologousPointExtractionModuleModel
 *
 *
 *
 */
class ITK_EXPORT HomologousPointExtractionModuleModel
      : public MVCModel<ListenerBase>, public itk::Object
{

public:
  /** Standard class typedefs */
  typedef HomologousPointExtractionModuleModel                         Self;
  typedef MVCModel<ListenerBase> Superclass;
  typedef itk::SmartPointer<Self>                        Pointer;
  typedef itk::SmartPointer<const Self>                  ConstPointer;

  /** Standard type macro */
  itkTypeMacro(HomologousPointExtractionModuleModel, Object);

  typedef double                              PixelType;
  typedef VectorImage<PixelType,2>            VectorImageType;
  typedef VectorImageType::Pointer            VectorImagePointerType;
  typedef VectorImageType::IndexType          IndexType;
  typedef std::vector<VectorImagePointerType> ImageListType;    

  typedef std::pair<IndexType, IndexType> IndexCoupleType;
  typedef std::vector<IndexCoupleType>    IndexesListType;

  /** Visualization model */
  typedef itk::RGBPixel<unsigned char>                              RGBPixelType;
  typedef Image<RGBPixelType,2>                                     RGBImageType;
  typedef ImageLayer<VectorImageType,RGBImageType>                  LayerType;
  typedef ImageLayerGenerator<LayerType>                            LayerGeneratorType;
  typedef LayerGeneratorType::Pointer                               LayerGeneratorPointerType;
  typedef std::vector<LayerGeneratorPointerType>                    LayerGeneratorListType;
  typedef ImageLayerRenderingModel<RGBImageType>                    VisualizationModelType;
  typedef VisualizationModelType::Pointer                           VisualizationModelPointerType;
  typedef std::vector<VisualizationModelPointerType>                VisualizationModelListType;
  typedef Function::UniformAlphaBlendingFunction<LayerGeneratorType::ImageLayerType::OutputPixelType> BlendingFunctionType;
  typedef BlendingFunctionType::Pointer                                                               BlendingFunctionPointerType;
  typedef std::vector<BlendingFunctionPointerType>                                                    BlendingFunctionListType;

  /** New macro */
  itkNewMacro(Self);
  
  /** Get the unique instanc1e of the model */
  static Pointer GetInstance();

  /** Get the visualization models */
  VisualizationModelPointerType GetVisualizationModel(unsigned int id)
  {
    if( id != 0 && id != 1 )
      {
	itkExceptionMacro(<<"invalid id "<<id<<".");
      }
    return m_VisualizationModel[id];
  }

  /** Input Images Pointer */
  void SetFirstInputImage( VectorImagePointerType img );
  itkGetConstObjectMacro(FirstInputImage, VectorImageType);
  void SetSecondInputImage( VectorImagePointerType img );
  itkGetConstObjectMacro(SecondInputImage, VectorImageType);
  void SetImage( unsigned int id, VectorImagePointerType image );

  /** Indexes list manipulation. */
  IndexesListType GetIndexesList() const
  {
    return m_IndexesList;
  }
  void AddIndexesToList( IndexType id1, IndexType id2 )
  {
    IndexCoupleType paire(id1, id2);
    m_IndexesList.push_back(paire);
  }
 void ClearIndexesList()
  {
    m_IndexesList.clear();
  }
 void RemoveFromList( unsigned int id )
  {
    if(m_IndexesList.size())
      itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size.");

    m_IndexesList.erase(m_IndexesList.begin()+id);
  }

  /** Get the output changed flag */
  itkGetMacro(OutputChanged,bool);
  


protected:

  /** Constructor */
  HomologousPointExtractionModuleModel();
  /** Destructor */
  ~HomologousPointExtractionModuleModel();

  
private:
  HomologousPointExtractionModuleModel(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Notify a given listener of changes */
  virtual void Notify(ListenerBase * listener);

  /** Singleton instance */
  static Pointer                     Instance;

  /** Visualization */
  VisualizationModelListType m_VisualizationModel;
  LayerGeneratorListType     m_ImageGenerator;
  BlendingFunctionListType   m_BlendingFunction;
  /** Input Images */
  ImageListType              m_Images;
  VectorImagePointerType m_FirstInputImage;
  VectorImagePointerType m_SecondInputImage;

  /** First and second input image indexes list */
  IndexesListType m_IndexesList;

  bool m_OutputChanged;

};

}//end namespace otb
#endif
