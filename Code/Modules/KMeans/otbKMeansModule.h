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
#ifndef __otbKMeansModule_h
#define __otbKMeansModule_h

// include the base class
#include "otbModule.h"
// include the GUI
#include "otbKMeansModuleGUI.h"

#include "otbTypeManager.h"

// KMeans
#include "itkEuclideanDistance.h"
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkListSample.h"
#include "itkUnaryFunctorImageFilter.h"
#include "otbStreamingShrinkImageFilter.h"
#include "otbChangeLabelImageFilter.h"

namespace otb
{

namespace Functor
{
template <class TSample, class TLabel> class KMeansFunctor
{
public:
  /** operator */
  TLabel operator ()(const TSample& sample) const
  {
    typename CentroidMapType::const_iterator it = m_CentroidsMap.begin();

    if (it == m_CentroidsMap.end())
      {
      return 0;
      }

    TLabel resp = it->first;
    double minDist = m_Distance->Evaluate(sample, it->second);
    ++it;

    while (it != m_CentroidsMap.end())
      {
      double dist = m_Distance->Evaluate(sample, it->second);

      if (dist < minDist)
        {
        resp = it->first;
        minDist = dist;
        }
      ++it;
      }
    return resp;
  }

  /** Add a new centroid */
  void AddCentroid(const TLabel& label, const TSample& centroid)
  {
    m_CentroidsMap[label] = centroid;
  }

  /** Constructor */
  KMeansFunctor() : m_CentroidsMap(), m_Distance()
  {
    m_Distance = DistanceType::New();
  }

  bool operator !=(const KMeansFunctor& other) const
  {
    return m_CentroidsMap != other.m_CentroidsMap;
  }

private:
  typedef std::map<TLabel, TSample>                   CentroidMapType;
  typedef itk::Statistics::EuclideanDistance<TSample> DistanceType;

  CentroidMapType m_CentroidsMap;
  typename DistanceType::Pointer m_Distance;
};
}

/** \class KMeansModule
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT KMeansModule
  : public Module, public KMeansModuleGUI
{
public:
  /** Standard class typedefs */
  typedef KMeansModule                  Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(KMeansModule, Module);

  typedef TypeManager::Floating_Point_Precision   PrecisionType;
  typedef TypeManager::Floating_Point_VectorImage FloatingVectorImageType;

  typedef TypeManager::Label_Short_Precision LabelType;
  typedef TypeManager::Labeled_Short_Image   LabeledImageType;

  typedef FloatingVectorImageType::PixelType                               SampleType;
  typedef itk::Statistics::ListSample<SampleType>                          ListSampleType;
  typedef itk::Statistics::WeightedCentroidKdTreeGenerator<ListSampleType> TreeGeneratorType;
  typedef TreeGeneratorType::KdTreeType                                    TreeType;
  typedef itk::Statistics::KdTreeBasedKmeansEstimator<TreeType>            EstimatorType;
  typedef otb::StreamingShrinkImageFilter<FloatingVectorImageType,
      FloatingVectorImageType>              SamplingFilterType;
  typedef Functor::KMeansFunctor<SampleType, LabelType> KMeansFunctorType;
  typedef itk::UnaryFunctorImageFilter<FloatingVectorImageType,
      LabeledImageType, KMeansFunctorType>     KMeansFilterType;
  typedef otb::ChangeLabelImageFilter<LabeledImageType, FloatingVectorImageType> ChangeLabelFilterType;

  /** Show the Module GUI */
  virtual bool CanShow(){return true; }

  /** Show the Module GUI */
  virtual void Show()
  {
    pProgressBar->minimum(0);
    pProgressBar->maximum(1);
    wKMeansWindow->show();
    pProgressBar->value(0.);
    pProgressBar->hide();
    this->UpdateNumberOfSamples();
  }
protected:
  /** Constructor */
  KMeansModule();
  /** Destructor */
  virtual ~KMeansModule();
  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Callbacks */
  virtual void Ok();
  virtual void Cancel();
  virtual void UpdateNumberOfSamples();
  virtual void ThreadedRun();
  virtual void ThreadedWatch();

  // Update the progress bar
  void UpdateProgress();

  // Hide the window
  void HideWindow();

  // Update the number of samples knowing available RAM
  void UpdateNumberOfSamplesFromAvailableRAM();

private:
  // Callback to update the window label
  static void UpdateProgressCallback(void * data);

  // Callback to hide window
  static void HideWindowCallback(void * data);

  // Callback to Error reporter window
  static void SendErrorCallback(void * data);

  KMeansModule(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  // KMeans estimator
  EstimatorType::Pointer m_Estimator;

  // KMeans filter
  KMeansFilterType::Pointer m_KMeansFilter;

  // Change label filter
  ChangeLabelFilterType::Pointer m_ChangeLabelFilter;

  // Pointer to the process object
  itk::ProcessObject::Pointer m_ProcessObject;

  //error msg
  std::string m_ErrorMsg;

  // number of samples computed from RAM
  double     m_NumberOfSampleKnowingRAM;
};

} // End namespace otb

#endif
