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
#ifndef __otbObjectLabelingApplicationControllerInterface_h
#define __otbObjectLabelingApplicationControllerInterface_h

#include "itkObject.h"
#include "otbObjectLabelingApplicationModel.h"

namespace otb
{
class ObjectLabelingApplicationControllerInterface
      : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef ObjectLabelingApplicationControllerInterface Self;
  typedef itk::Object                                  Superclass;
  typedef itk::SmartPointer<Self>                      Pointer;
  typedef itk::SmartPointer<const Self>                ConstPointer;
  typedef ObjectLabelingApplicationModel::ColorType    ColorType;
  typedef ObjectLabelingApplicationModel::LabelType    LabelType;

  /** Standard type macros */
  itkTypeMacro(ObjectLabelingApplicationControllerInterface,Superclass);

  /** Users actions */
  virtual void AddClass() = 0;
  virtual void RemoveClass(unsigned int classIndex) = 0;
  virtual void ClearClasses()=0;
  virtual void ChangeClassColor(unsigned int classIndex, const ColorType & color) = 0;
  virtual void ChangeClassName(unsigned int classIndex,  const char * name) = 0;
  virtual void ChangeClassLabel(unsigned int classIndex, const LabelType& label) = 0;
  virtual void ClassSelected(unsigned int classIndex)=0;
  virtual void ClearSelectedClass() = 0;
  virtual void RemoveObject(unsigned int objectIndex)=0;
  virtual void SelectObject(unsigned int objectIndex)=0;
  virtual void ClearObjects() = 0;
  //virtual void LoadImages(const char * image, const char * label)=0;
  virtual void SaveSamplesToXMLFile(const char * fname) = 0;
  virtual void SaveClassificationParametersToXMLFile(const char * fname) = 0;
  virtual void LoadSamplesFromXMLFile(const char * fname) = 0;
  virtual void SaveColorsToAsciiFile(const char * fname) = 0;
  virtual void SaveClassification(const char * fname) = 0;
  virtual void SaveClassificationGraph(const char * fname) = 0;
  virtual void ExportClassificationToGIS(const GISExportInfo& exportInfo) = 0;
  virtual void SampleMargin() = 0;
  virtual void ChangeKernelType(int kernel) = 0;
  virtual void ChangeNumberOfCrossValidationFolders(unsigned int nb) = 0;
  virtual void ChangeParametersOptimisation(bool value) = 0;
  virtual void ChangeNumberOfCoarseSteps(unsigned int nb) = 0;
  virtual void ChangeNumberOfFineSteps(unsigned int nb) = 0;
  virtual void ChangeNumberOfMarginSamples(unsigned int nb) = 0;
  virtual void ChangeMarginColor(const ColorType & color) = 0;
  virtual void FocusOnMarginSample(unsigned int sample) = 0;
  virtual void ClearMarginSamples() = 0;
  virtual void Classify() = 0;
  virtual void ClearClassification() = 0;
  virtual void ChangeFeatureState(const std::string& fname,bool state) = 0;
  virtual void SetUseContext(bool context) = 0;
  virtual void ChangeClassificationOpacity(double value) = 0;
  virtual void UpdateViewerDisplay() =0;
  virtual void SetModel(ObjectLabelingApplicationModel * model)
  {
    m_Model = model;
  }

protected:
  /** Constructor */
  ObjectLabelingApplicationControllerInterface()
  {
    m_Model = ObjectLabelingApplicationModel::GetInstance();
  }
  /** Destructor */
  ~ObjectLabelingApplicationControllerInterface() {}

  /** The instance of the model (for commodity) */
  ObjectLabelingApplicationModel * m_Model;

private:
  ObjectLabelingApplicationControllerInterface(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};
} // end namespace otb

#endif
