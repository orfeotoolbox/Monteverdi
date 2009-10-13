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

#include "otbHomologousPointExtractionModuleModel.h"
#include "otbFltkFilterWatcher.h"

#include "otbPointSetRegister.h"
#include "otbImageFileWriter.h"
namespace otb
{
/** Initialize the singleton */
HomologousPointExtractionModuleModel::Pointer HomologousPointExtractionModuleModel::Instance = NULL;
/** Manage the singleton */
HomologousPointExtractionModuleModel::Pointer
HomologousPointExtractionModuleModel::GetInstance()
{
  if (!Instance)
  {
    Instance = HomologousPointExtractionModuleModel::New();
  }
  return Instance;
}

void HomologousPointExtractionModuleModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

HomologousPointExtractionModuleModel::HomologousPointExtractionModuleModel() : m_VisualizationModel(), m_BlendingFunction(), m_Output(), m_Resampler(), m_PerBander()
{
  VisualizationModelType::Pointer visualizationModel1 = VisualizationModelType::New();
  VisualizationModelType::Pointer visualizationModel2 = VisualizationModelType::New();
  m_VisualizationModel.push_back(visualizationModel1);
  m_VisualizationModel.push_back(visualizationModel2);

  BlendingFunctionType::Pointer blendingFunction1 = BlendingFunctionType::New();
  BlendingFunctionType::Pointer blendingFunction2 = BlendingFunctionType::New();
  blendingFunction1->SetAlpha(0.6);
  blendingFunction2->SetAlpha(0.6);
  m_BlendingFunction.push_back(blendingFunction1);
  m_BlendingFunction.push_back(blendingFunction2);


  LayerGeneratorType::Pointer imageGenerator1 = LayerGeneratorType::New();
  LayerGeneratorType::Pointer imageGenerator2 = LayerGeneratorType::New();
  m_ImageGenerator.push_back(imageGenerator1);
  m_ImageGenerator.push_back(imageGenerator2);

  m_FirstInputImage = VectorImageType::New();
  m_SecondInputImage = VectorImageType::New();
  m_Images.push_back(m_FirstInputImage);
  m_Images.push_back(m_SecondInputImage);

  m_Output = VectorImageType::New();
  m_Resampler = ResampleFilterType::New();
  m_PerBander = PerBandFilterType::New();
  m_IndexesList.clear();
  
  m_OutputChanged = false;  

}

HomologousPointExtractionModuleModel
::~HomologousPointExtractionModuleModel() {}


void
HomologousPointExtractionModuleModel
::SetFirstInputImage(VectorImagePointerType img)
{
  this->SetImage(0, img);
}
  

void
HomologousPointExtractionModuleModel
::SetSecondInputImage(VectorImagePointerType img)
{
  this->SetImage(1, img);
}

void
HomologousPointExtractionModuleModel
::SetImage(unsigned int id, VectorImagePointerType image)
{
  if( id != 0 && id != 1 )
    itkExceptionMacro(<<"Invalid image id "<<id<<".");
 
  m_OutputChanged = false;  

  image->UpdateOutputInformation();
  if( id == 0 )
    m_FirstInputImage = image;
 else
    m_SecondInputImage = image;

  m_Images[id] = image;
  
  // Generate the layer
  m_ImageGenerator[id]->SetImage(image);
  m_ImageGenerator[id]->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(m_ImageGenerator[id]->GetResampler(),0,0,200,20,"Generating QuickLook ...");
  m_ImageGenerator[id]->GenerateLayer();
  
  std::vector<unsigned int> channels;
  if(image->GetNumberOfComponentsPerPixel()==3)
    {
      channels.push_back(0);
      channels.push_back(1);
      channels.push_back(2);
    }
  else if(image->GetNumberOfComponentsPerPixel()>3)
    {
      channels.push_back(2);
      channels.push_back(1);
      channels.push_back(0);
    }
  if( channels.size() == 3 )
    m_ImageGenerator[id]->GetLayer()->GetRenderingFunction()->SetChannelList(channels);
  
  if( id == 0 )
    m_ImageGenerator[id]->GetLayer()->SetName("FirstImage");
  else
    m_ImageGenerator[id]->GetLayer()->SetName("SecondImage");

  // Clear previous layers
  m_VisualizationModel[id]->ClearLayers();
  
  // Add the layer to the models
  m_VisualizationModel[id]->AddLayer(m_ImageGenerator[id]->GetLayer());
  m_VisualizationModel[id]->Update();

  this->NotifyAll();
  //m_IsImageReady = true;
}

void
HomologousPointExtractionModuleModel
::AddIndexesToList( IndexType id1, IndexType id2 )
{
  bool found = false;
  unsigned int j=0;
  while( j<m_IndexesList.size() && !found )
    {
      if( m_IndexesList[j].first == id1 || m_IndexesList[j].second == id2 )
	{
	  found = true;
	  itkExceptionMacro(<<"At most one of the 2 given index "<<id1<<" or "<<id2<<" already appears in the list.");
	}
      j++;
    }

  IndexCoupleType paire(id1, id2);
  m_IndexesList.push_back(paire);
}
  
void
HomologousPointExtractionModuleModel
::RemovePointFromList( unsigned int id )
{
  if( id>=m_IndexesList.size() )
    itkExceptionMacro(<<"Impossible to erase the "<<id<<" element. Out of vector size ("<<m_IndexesList.size()<<").");
  
  m_IndexesList.erase(m_IndexesList.begin()+id);
}


void 
HomologousPointExtractionModuleModel
::ConvertList( PointSetPointerType fix, PointSetPointerType mov )
{
  if(m_IndexesList.size()==0)
    itkExceptionMacro(<<"No point selected...");

  PointType fixedPoint;
  PointType movingPoint;
  
  for(unsigned int i=0; i<m_IndexesList.size(); i++)
    {
      IndexType idFix, idMov;;
      idFix = m_IndexesList[i].first;
      fixedPoint[0] = static_cast<float>(idFix[0]);
      fixedPoint[1] = static_cast<float>(idFix[1]);
      fix->SetPoint(i,fixedPoint);

      idMov = m_IndexesList[i].second;
      movingPoint[0] = static_cast<float>(idMov[0]);
      movingPoint[1] = static_cast<float>(idMov[1]);   
      mov->SetPoint(i,movingPoint);
    }
}  
 

void 
HomologousPointExtractionModuleModel
::ComputeTransform( TransformEnumType transformType )
{
  switch (transformType)
    {
    case otb::TRANSLATION:
      {	
	this->GenericRegistration<TranslationTransformType>();
	break;
      }
    case otb::AFFINE:
      {
	this->GenericRegistration<AffineTransformType>();
	break;
      }
    default:
      {
	itkExceptionMacro(<<"Not supported transform type "<<transformType);
	return;
      }
    }
 
  return;
}


template<typename T>
void 
HomologousPointExtractionModuleModel
::GenericRegistration()
{
  // Create the point set.
  PointSetPointerType fixPointSet = PointSetType::New();
  PointSetPointerType movingPointSet = PointSetType::New();
  this->ConvertList( fixPointSet, movingPointSet );

  typedef PointSetRegister<T> PointSetRegisterType;
  typename PointSetRegisterType::Pointer reg = PointSetRegisterType::New();
  reg->SetFixPointSet(fixPointSet);
  reg->SetMovingPointSet(movingPointSet);
  reg->ComputeTransform();

  m_TransformParameters = reg->GetTransformParameters();
  //  m_Transform = T::New();
  //  m_Transform = reg->GetTransform();

  typename T::Pointer transform = T::New();
  transform->SetParameters(m_TransformParameters);
  m_Resampler->SetTransform(transform);
}

HomologousPointExtractionModuleModel::OutPointType
HomologousPointExtractionModuleModel
:: TransformPoint( TransformEnumType transformType, IndexType id )
{
  OutPointType out;
  switch (transformType)
    {
    case otb::TRANSLATION:
      {	
	out = this->GenericTransformPoint<TranslationTransformType>(id);
	break;
      }
    case otb::AFFINE:
      {
	out = this->GenericTransformPoint<AffineTransformType>(id);
	break;
      }
    default:
      {
	itkExceptionMacro(<<"Not supported transform type "<<transformType);
      }
    }
    return out;
}

template<typename T>
HomologousPointExtractionModuleModel::OutPointType 
HomologousPointExtractionModuleModel
::GenericTransformPoint( IndexType index )
{
  typename T::Pointer transform = T::New();
  transform->SetParameters( m_TransformParameters);
 
  OutPointListType            out;
  typename T::InputPointType  inPoint; 
  typename T::OutputPointType outPoint;
  OutPointType                idOut;
  
  inPoint[0] = index[0]; 
  inPoint[1] = index[1];
  outPoint = transform->TransformPoint(inPoint);
  idOut[0] = static_cast<double>(outPoint[0]);
  idOut[1] = static_cast<double>(outPoint[1]);
    
  return idOut;
}


HomologousPointExtractionModuleModel::OutPointListType 
HomologousPointExtractionModuleModel
::TransformPoints( TransformEnumType transformType )
{
  IndexListType inList;
  OutPointListType outList;
  for(unsigned int i=0; i<m_IndexesList.size(); i++)
    {
      inList.push_back(m_IndexesList[i].first);
    }

  switch (transformType)
    {
    case otb::TRANSLATION:
      {	
	outList = this->GenericTransformPoints<TranslationTransformType>(inList);
	break;
      }
    case otb::AFFINE:
      {
	outList = this->GenericTransformPoints<AffineTransformType>(inList);
	break;
      }
    default:
      {
	itkExceptionMacro(<<"Not supported transform type "<<transformType);
      }
    }
    return outList;
}


template<typename T>
HomologousPointExtractionModuleModel::OutPointListType 
HomologousPointExtractionModuleModel
::GenericTransformPoints( IndexListType inList )
{
  typename T::Pointer transform = T::New();
  transform->SetParameters( m_TransformParameters);
 
  OutPointListType            outList;
  IndexCoupleType             couple;
  typename T::InputPointType  inPoint; 
  typename T::OutputPointType outPoint;
  OutPointType                idOut;

  for(unsigned int i=0; i<inList.size(); i++)
    {
      inPoint[0] = inList[i][0]; 
      inPoint[1] = inList[i][1];
      outPoint = transform->TransformPoint(inPoint);
      idOut[0] = static_cast<double>(outPoint[0]);
      idOut[1] = static_cast<double>(outPoint[1]);

      outList.push_back(idOut);
    }

  return outList;
}



void 
HomologousPointExtractionModuleModel
::OK()
{
//   TranslationTransformType::Pointer transfo = TranslationTransformType::New();
//   TranslationTransformType::OutputVectorType param;
//   std::cout<<param<<std::endl;
//   param[0] = 1;
//   param[1] = 1;
//   transfo->SetOffset(param);
//   m_Resampler->SetTransform(transfo);
  //std::cout<<transfo->GetParameters()<<std::endl;
  //std::cout<<"OKKKKKKKKKKKKKKKKKKKKKKKKK"<<std::endl;
  std::cout<<m_FirstInputImage<<std::endl;
  std::cout<<m_SecondInputImage<<std::endl;
  m_PerBander->SetInput( m_SecondInputImage );

  ResampleTransformType::InputPointType  inPoint; 
  ResampleTransformType::OutputPointType outPoint;
  OutPointType                   idOut;
  IndexType id;
  inPoint[0] = m_SecondInputImage->GetOrigin()[0]; 
  inPoint[1] = m_SecondInputImage->GetOrigin()[1];
  outPoint = m_Resampler->GetTransform()->TransformPoint(inPoint);
  outPoint[0] = -outPoint[0];
  outPoint[1] = -outPoint[1];
  id[0] = static_cast<unsigned long>(outPoint[0]);
  id[1] = static_cast<unsigned long>(outPoint[1]);
  std::cout<<id<<std::endl;
  m_Resampler->SetOutputStartIndex(id);

//   id[0] = m_SecondInputImage->GetOrigin()[0];
//   id[1] = m_SecondInputImage->GetOrigin()[1]; 
//   inPoint[0] = m_SecondInputImage->GetLargestPossibleRegion().GetSize()[0];
//   inPoint[1] = m_SecondInputImage->GetLargestPossibleRegion().GetSize()[1];
//   outPoint = m_Resampler->GetTransform()->TransformPoint(inPoint);
//   SizeType size;
//   size[0] = static_cast<unsigned long>(vcl_abs(outPoint[0]));
//   size[1] = static_cast<unsigned long>(vcl_abs(outPoint[1]));
//   std::cout<<inPoint<<" -> "<<size<<std::endl;
  m_Resampler->SetSize(m_FirstInputImage->GetLargestPossibleRegion().GetSize());//m_SecondInputImage->GetLargestPossibleRegion().GetSize());

  ImagePointType pt;
  pt[0] = 0;
  pt[1] = 0;  
  m_Resampler->SetOutputSpacing(m_SecondInputImage->GetSpacing());
  m_Resampler->SetOutputOrigin( pt );//m_SecondInputImage->GetOrigin());
  m_Resampler->SetOutputDirection(m_SecondInputImage->GetDirection());
  //m_Resampler->SetSize(m_SecondInputImage->GetLargestPossibleRegion().GetSize());

  m_Resampler->UpdateOutputInformation();
  m_PerBander->SetFilter(m_Resampler);
  m_PerBander->UpdateOutputInformation();
  m_Output = m_PerBander->GetOutput();

  typedef ImageFileWriter<VectorImageType> WType;
  WType::Pointer writer = WType::New();
  writer->SetInput(m_PerBander->GetOutput());
  writer->SetFileName("tototatatititutu.tif");
  writer->Update();

  m_OutputChanged = true;
  this->NotifyAll();
std::cout<<"OKKKKKKKKKKKKKKKKKKKKKKKKK"<<std::endl;
}

}// namespace otb
