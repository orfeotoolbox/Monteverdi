/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Copyright (c) CS Systemes d'information. All rights reserved.
  See CSCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbPixelSpectrumModel_h
#define __otbPixelSpectrumModel_h

#include "otbVectorImage.h"
#include "otbMVCModel.h"
#include "otbLayerBasedModel.h"
#include "otbPixelSpectrumModelListener.h"
#include "otbImageLayerBase.h"
#include "otbCurve2D.h"

#include "itkRGBPixel.h"
#include "otbTypeManager.h"

#include <vector>

namespace otb
{
/** \class PixelSpectrumModel
*   \brief This class is a layer based model reporting pixel  information
*
*   \sa ImageLayer
*
*  \ingroup Visualization
 */

class PixelSpectrumModel
: public MVCModel<PixelSpectrumModelListener>,
public LayerBasedModel<ImageLayerBase <otb::Image <itk::RGBPixel <unsigned char>, 2> > >
{

public:
  /** Standard class typedefs */
  typedef PixelSpectrumModel Self;
  typedef LayerBasedModel
  <ImageLayerBase <otb::Image <itk::RGBPixel<unsigned char>, 2> > >
  Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Runtime information */
  itkTypeMacro(PixelSpectrumModel, LayerBasedModel);

  /** New macro */
  itkNewMacro(Self);

  /** Layer typedef */
  typedef Superclass::LayerType LayerType;

  /** We assume that TLayer is has an IndexType definition */
  typedef LayerType::IndexType      IndexType;
  typedef LayerType::LayerValueType LayerValueType;

  /** Listener typedef */
  typedef PixelSpectrumModelListener ListenerType;

  typedef TypeManager::Floating_Point_Precision PixelType;
  typedef itk::VariableLengthVector<PixelType>  SpectrumVectorType;

  struct SpectrumAndIdVectorType {
    SpectrumVectorType sprectrum;
    int id;
    IndexType coord;
    bool visible;
  };

  static unsigned int NoneID;

  typedef std::vector<SpectrumAndIdVectorType> SeveralSpectrumsDataType;

  /** Get/Set the viewer name */
  itkGetStringMacro(PixelSpectrum);
  itkGetConstReferenceMacro(CurrentPixelSpectrum, SpectrumVectorType);

  SeveralSpectrumsDataType GetSeveralSpectrumData();

  IndexType GetSpectrumIndexByID(int id);

  SpectrumVectorType GetSpectrumByID(int id);

  itkGetMacro(AxisLengthY, float);
  itkSetMacro(AxisLengthY, float);
  itkGetMacro(AxisLengthX, float);
  itkSetMacro(AxisLengthX, float);

  itkGetMacro(CurentCurveAlpha, float);
  itkSetMacro(CurentCurveAlpha, float);

  /* set/Get the extremum values of/within pixels */
  itkGetMacro(MinValue, double);
  itkSetMacro(MinValue, double);
  itkGetMacro(MaxValue, double);
  itkSetMacro(MaxValue, double);

  /** Get/Set Id */
  itkGetMacro(Id, unsigned int);
  itkSetMacro(Id, unsigned int);

  /** Get/Set the Grid Step */
  itkGetMacro(GridStepY, double);
  itkSetMacro(GridStepY, double);
  itkGetMacro(GridStepX, double);
  itkSetMacro(GridStepX, double);

  /** Relative to the grid origin */
  itkGetMacro(GridOriginX, double);
  itkSetMacro(GridOriginX, double);
  itkGetMacro(GridOriginY, double);
  itkSetMacro(GridOriginY, double);
  itkGetMacro(GridOriginXTranslate, double);
  itkSetMacro(GridOriginXTranslate, double);
  itkGetMacro(GridOriginYTranslate, double);
  itkSetMacro(GridOriginYTranslate, double);

  double GetGridOriginXTemp()
  {
    return (m_GridOriginXTranslate + m_GridOriginX);
  }

  double GetGridOriginYTemp()
  {
    return (m_GridOriginYTranslate + m_GridOriginY);
  }

  /** Get/Set the Grid Step */
  itkGetMacro(AutomaticGridStep, bool);
  itkSetMacro(AutomaticGridStep, bool);

  int GetIdFromPosition(int i);

  /** Clear the pixel description */
  void ClearPixelSpectrum();

  void UpdateCurrentPixelSpectrum(const IndexType& index);
  unsigned int AddAPixelSpectrum(const IndexType& index);
  int RemoveAPixelSpectrumById(int i);
  void RemoveAllSpectrums();
  void ViewerInteraction(int dataToChange, float value);
  void DetermineMinMaxPixelValues();
  void SetYAxisLenghtControl(float value, bool direct);
  int IsVisibleById(int id);
  void SetVisibleById(int id, int v);
  bool CheckIfAlreadyExistingSprectrum(const IndexType& index);

protected:
  /** Constructor */
  PixelSpectrumModel();
  /** Destructor */
  ~PixelSpectrumModel();

  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Notify a registered listener */
  void        NotifyListener(ListenerType * listener);

private:
  PixelSpectrumModel(const Self &); // purposely not implemented
  void operator = (const Self &);   // purposely not implemented

  /** The pixel description */
  std::string        m_PixelSpectrum;
  SpectrumVectorType m_CurrentPixelSpectrum;

  SeveralSpectrumsDataType m_SeveralSpectrumData;

  float m_AxisLengthX;
  float m_AxisLengthY;

  float m_CurentCurveAlpha;

  /** extremum values for a pixel */
  double m_MinValue;
  double m_MaxValue;

  /** grid step over y and x axis */
  double m_GridStepX;
  double m_GridStepY;

  double m_GridOriginX;
  double m_GridOriginY;

  double m_GridOriginXTranslate;
  double m_GridOriginYTranslate;

  bool m_AutomaticGridStep;

  /** last Id of a curve in the vector, incremented when a curve is added*/
  unsigned int m_Id;

}; // end class

} // end namespace otb

#endif
