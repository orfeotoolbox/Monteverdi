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
#include "itkLandmarkBasedTransformInitializer.h"
#include "otbImageFileWriter.h"

#include <vnl/algo/vnl_lsqr.h>
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/vnl_least_squares_function.h>

namespace otb
{

HomologousPointExtractionModuleModel::HomologousPointExtractionModuleModel() : m_VisualizationModel(),
  m_BlendingFunction(), m_Output(), m_Resampler(), m_PerBander()
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

void HomologousPointExtractionModuleModel::Notify(ListenerBase * listener)
{
  listener->Notify();
}

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
  if (id != 0 && id != 1) itkExceptionMacro(<< "Invalid image id " << id << ".");

  m_OutputChanged = false;

  image->UpdateOutputInformation();
  if (id == 0) m_FirstInputImage = image;
  else m_SecondInputImage = image;

  m_Images[id] = image;

  // Generate the layer
  m_ImageGenerator[id]->SetImage(image);
  m_ImageGenerator[id]->GenerateQuicklookOn();
  FltkFilterWatcher qlwatcher(m_ImageGenerator[id]->GetResampler(), 0, 0, 200, 20, "Generating QuickLook ...");
  m_ImageGenerator[id]->GenerateLayer();

  std::vector<unsigned int> channels;
  if (image->GetNumberOfComponentsPerPixel() == 3)
    {
    channels.push_back(0);
    channels.push_back(1);
    channels.push_back(2);
    }
  else if (image->GetNumberOfComponentsPerPixel() > 3)
    {
    channels.push_back(2);
    channels.push_back(1);
    channels.push_back(0);
    }
  if (channels.size() == 3) m_ImageGenerator[id]->GetLayer()->GetRenderingFunction()->SetChannelList(channels);

  if (id == 0) m_ImageGenerator[id]->GetLayer()->SetName("FirstImage");
  else m_ImageGenerator[id]->GetLayer()->SetName("SecondImage");

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
::AddIndexesToList(IndexType id1, IndexType id2)
{
  bool         found = false;
  unsigned int j = 0;
  while (j < m_IndexesList.size() && !found)
    {
    if (m_IndexesList[j].first == id1 || m_IndexesList[j].second == id2)
      {
      found = true;
      itkExceptionMacro(
        << "At most one of the 2 given index " << id1 << " or " << id2 << " already appears in the list.");
      }
    j++;
    }

  IndexCoupleType paire(id1, id2);
  m_IndexesList.push_back(paire);
}

void
HomologousPointExtractionModuleModel
::RemovePointFromList(unsigned int id)
{
  if (id >= m_IndexesList.size()) itkExceptionMacro(
      << "Impossible to erase the " << id << " element. Out of vector size (" << m_IndexesList.size() << ").");

  m_IndexesList.erase(m_IndexesList.begin() + id);
}

void
HomologousPointExtractionModuleModel
::ConvertList(PointSetPointerType fix, PointSetPointerType mov)
{
  if (m_IndexesList.size() == 0) itkExceptionMacro(<< "No point selected...");

  PointType fixedPoint;
  PointType movingPoint;
  PointType firstOrig = m_FirstInputImage->GetOrigin();
  PointType secondOrig = m_SecondInputImage->GetOrigin();

  for (unsigned int i = 0; i < m_IndexesList.size(); i++)
    {
    IndexType idFix, idMov;
    idFix = m_IndexesList[i].first;
    idFix[0] += static_cast<int>(firstOrig[0]);
    idFix[1] += static_cast<int>(firstOrig[1]);
    m_FirstInputImage->TransformIndexToPhysicalPoint(idFix, fixedPoint);
    fix->SetPoint(i, fixedPoint);

    idMov = m_IndexesList[i].second;
    idMov[0] += static_cast<int>(secondOrig[0]);
    idMov[1] += static_cast<int>(secondOrig[1]);
    m_SecondInputImage->TransformIndexToPhysicalPoint(idMov, movingPoint);
    mov->SetPoint(i, movingPoint);
    }
}

void
HomologousPointExtractionModuleModel
::ComputeTransform(TransformEnumType transformType)
{
  switch (transformType)
    {
    case otb::TRANSLATION:
      {
      ScalesType scales(2);
      scales.Fill(0.01);
      this->GenericRegistration<TranslationTransformType>(scales);
      break;
      }
    case otb::AFFINE:
      {
      this->AffineLeastSquareRegistration();
      break;
      }
    case otb::SIMILARITY2D:
      {
      ScalesType scales(4);
      // Scaling scale
      scales[0] = 1.;
      // Rotation scale
      scales[1] = 1.;
      // Translation scale
      scales[2] = 1000;
      scales[3] = 1000;

      this->GenericRegistration<Similarity2DTransformType>(scales);
      break;
      }
    default:
      {
      itkExceptionMacro(<< "Not supported transform type " << transformType);
      return;
      }
    }

  return;
}

template<typename T>
void
HomologousPointExtractionModuleModel
::GenericRegistration(const ScalesType& scales)
{
  // Create the point set.
  PointSetPointerType fixPointSet = PointSetType::New();
  PointSetPointerType movingPointSet = PointSetType::New();
  this->ConvertList(fixPointSet, movingPointSet);
  typedef PointSetRegister<T> PointSetRegisterType;
  typename PointSetRegisterType::Pointer reg = PointSetRegisterType::New();
  reg->SetFixPointSet(fixPointSet);
  reg->SetMovingPointSet(movingPointSet);
  reg->SetScales(scales);
  reg->ComputeTransform();
  m_TransformParameters = reg->GetTransformParameters();
  typename T::Pointer transform = T::New();
  transform->SetParameters(m_TransformParameters);
  m_Resampler->SetTransform(transform);

/** This code is commented out because it only handles the Rigid2DTransform

  typedef itk::LandmarkBasedTransformInitializer<T,VectorImageType,VectorImageType> TransformInitializerType;
  typename TransformInitializerType::Pointer ti = TransformInitializerType::New();
  typename TransformInitializerType::LandmarkPointContainer fixedLandmarks, movingLandmarks;
  typename TransformInitializerType::LandmarkPointType fixedPoint,movingPoint;

  for(IndexesListType::const_iterator it = m_IndexesList.begin(); it!=m_IndexesList.end();++it)
    {
    m_FirstInputImage->TransformIndexToPhysicalPoint(it->first,fixedPoint);
    m_SecondInputImage->TransformIndexToPhysicalPoint(it->second,movingPoint);

    fixedLandmarks.push_back(fixedPoint);
    movingLandmarks.push_back(movingPoint);
    }
  ti->SetFixedLandmarks(fixedLandmarks);
  ti->SetMovingLandmarks(movingLandmarks);
  typename T::Pointer transform = T::New();
  ti->SetTransform(transform);
  ti->InitializeTransform();
  m_TransformParameters = transform->GetParameters();
  m_Resampler->SetTransform(transform);
*/
}

void
HomologousPointExtractionModuleModel
::AffineLeastSquareRegistration()
{
  unsigned int nbPoints = m_IndexesList.size();

  if (nbPoints == 0) itkExceptionMacro(<< "No point selected...");

  vnl_sparse_matrix<double> ax(nbPoints, 3), ay(nbPoints, 3);
  vnl_vector<double>        bx(nbPoints), by(nbPoints);

  PointType firstOrig = m_FirstInputImage->GetOrigin();
  PointType secondOrig = m_SecondInputImage->GetOrigin();

  for (unsigned int i = 0; i < nbPoints; i++)
    {

    ax(i, 0) = m_IndexesList[i].first[0] + static_cast<int>(firstOrig[0]);
    ax(i, 1) = m_IndexesList[i].first[1] + static_cast<int>(firstOrig[1]);
    ax(i, 2) = 1.;

    ay(i, 0) = m_IndexesList[i].first[0] + static_cast<int>(firstOrig[0]);
    ay(i, 1) = m_IndexesList[i].first[1] + static_cast<int>(firstOrig[1]);
    ay(i, 2) = 1.;

    bx[i] = m_IndexesList[i].second[0] + static_cast<int>(secondOrig[0]);
    by[i] = m_IndexesList[i].second[1] + static_cast<int>(secondOrig[1]);
    }

  vnl_sparse_matrix_linear_system<double> linearSystemX(ax, bx), linearSystemY(ay, by);
  vnl_vector<double>                      bestXParams(3), bestYParams(3);

  vnl_lsqr linearSystemSolverX(linearSystemX), linearSystemSolverY(linearSystemY);

  linearSystemSolverX.minimize(bestXParams);

  otbGenericMsgDebugMacro(<< "Number of unknown: " << linearSystemX.get_number_of_unknowns());
  otbGenericMsgDebugMacro(<< "RMS error: " << linearSystemX.get_rms_error(bestXParams));
  otbGenericMsgDebugMacro(<< "Relative residuals: " << linearSystemX.get_relative_residual(bestXParams));

  linearSystemSolverY.minimize(bestYParams);

  otbGenericMsgDebugMacro(<< "Number of unknown: " << linearSystemY.get_number_of_unknowns());
  otbGenericMsgDebugMacro(<< "RMS error: " << linearSystemY.get_rms_error(bestYParams));
  otbGenericMsgDebugMacro(<< "Relative residuals: " << linearSystemY.get_relative_residual(bestYParams));

  otbGenericMsgDebugMacro(<< "Done.");

  itk::Vector<double, 2>    translation;
  itk::Matrix<double, 2, 2> mat;

  m_TransformParameters.SetSize(6);

  m_TransformParameters[0] = bestXParams[0];
  m_TransformParameters[1] = bestXParams[1];
  m_TransformParameters[2] = bestYParams[0];
  m_TransformParameters[3] = bestYParams[1];
  m_TransformParameters[4] = bestXParams[2];
  m_TransformParameters[5] = bestYParams[2];

  AffineTransformType::Pointer transform = AffineTransformType::New();
  transform->SetParameters(m_TransformParameters);
  m_Resampler->SetTransform(transform);

  otbGenericMsgDebugMacro(<< "Best params:" << m_TransformParameters);
}

HomologousPointExtractionModuleModel::ContinuousIndexType
HomologousPointExtractionModuleModel
::TransformPoint(TransformEnumType transformType, IndexType id)
{
  ContinuousIndexType out;
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
    case otb::SIMILARITY2D:
      {
      out = this->GenericTransformPoint<Similarity2DTransformType>(id);
      break;
      }
    default:
      {
      itkExceptionMacro(<< "Not supported transform type " << transformType);
      }
    }

  return out;
}

template<typename T>
HomologousPointExtractionModuleModel::ContinuousIndexType
HomologousPointExtractionModuleModel
::GenericTransformPoint(IndexType index)
{
  typename T::Pointer transform = T::New();
  transform->SetParameters(m_TransformParameters);

  //OutPointListType            out;
  typename T::InputPointType inPoint;
  typename T::OutputPointType outPoint;
  ContinuousIndexType idOut;

  PointType firstOrig = m_FirstInputImage->GetOrigin();
  PointType secondOrig = m_SecondInputImage->GetOrigin();
  index[0] += static_cast<int>(firstOrig[0]);
  index[1] += static_cast<int>(firstOrig[1]);

  m_FirstInputImage->TransformIndexToPhysicalPoint(index, inPoint);
  outPoint = transform->TransformPoint(inPoint);
  m_SecondInputImage->TransformPhysicalPointToContinuousIndex(outPoint, idOut);

  idOut[0] -= static_cast<int>(secondOrig[0]);
  idOut[1] -= static_cast<int>(secondOrig[1]);

  return idOut;
}

HomologousPointExtractionModuleModel::ContinuousIndexListType
HomologousPointExtractionModuleModel
::TransformPoints(TransformEnumType transformType)
{
  IndexListType           inList;
  ContinuousIndexListType outList;
  for (unsigned int i = 0; i < m_IndexesList.size(); i++)
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
    case otb::SIMILARITY2D:
      {
      outList = this->GenericTransformPoints<Similarity2DTransformType>(inList);
      break;
      }
    default:
      {
      itkExceptionMacro(<< "Not supported transform type " << transformType);
      }
    }
  return outList;
}

template<typename T>
HomologousPointExtractionModuleModel::ContinuousIndexListType
HomologousPointExtractionModuleModel
::GenericTransformPoints(IndexListType inList)
{
  typename T::Pointer transform = T::New();
  transform->SetParameters(m_TransformParameters);

  ContinuousIndexListType outList;
  typename T::InputPointType inPoint;
  typename T::OutputPointType outPoint;
  ContinuousIndexType idOut;
  IndexType           idStarted;

  PointType firstOrig = m_FirstInputImage->GetOrigin();
  PointType secondOrig = m_SecondInputImage->GetOrigin();

  for (unsigned int i = 0; i < inList.size(); i++)
    {
    idStarted[0] = inList[i][0] + static_cast<int>(firstOrig[0]);
    idStarted[1] = inList[i][1] + static_cast<int>(firstOrig[1]);

    m_FirstInputImage->TransformIndexToPhysicalPoint(inList[i], inPoint);
    outPoint = transform->TransformPoint(inPoint);
    m_SecondInputImage->TransformPhysicalPointToContinuousIndex(outPoint, idOut);

    idOut[0] -= static_cast<int>(secondOrig[0]);
    idOut[1] -= static_cast<int>(secondOrig[1]);

    outList.push_back(idOut);
    }

  return outList;
}

void
HomologousPointExtractionModuleModel
::OK()
{
  // Import parameters from fixed image
  m_Resampler->SetOutputSize(m_FirstInputImage->GetLargestPossibleRegion().GetSize());
  m_Resampler->SetOutputSpacing(m_FirstInputImage->GetSpacing());
  m_Resampler->SetOutputOrigin(m_SecondInputImage->GetOrigin());
  m_Resampler->UpdateOutputInformation();
  m_PerBander->SetInput(m_SecondInputImage);
  m_PerBander->SetFilter(m_Resampler);
  m_PerBander->UpdateOutputInformation();
  m_Output = m_PerBander->GetOutput();

  m_OutputChanged = true;
  this->NotifyAll();
}

} // namespace otb
