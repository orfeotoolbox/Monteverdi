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
#ifndef __otbSpectralSignatureCurve_h
#define __otbSpectralSignatureCurve_h

#include "otbCurve2D.h"
#include "itkContinuousIndex.h"
#include "itkIndex.h"

namespace otb
{
/** \class SpectralSignatureCurve
*   \brief Class for spectralCurve rendering.
*
*   \sa ImageViewerModel
*  \ingroup Visualization
 */

template <class TSpectralSignature>
class SpectralSignatureCurve
: public Curve2D
{
public:
  /** Standard class typedefs */
  typedef SpectralSignatureCurve        Self;
  typedef Curve2D                       Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  // Standard class macros
  itkNewMacro(Self);
  itkTypeMacro(SpectralSignatureCurve, Curve2D);

  /// Superclass typedefs
  typedef typename Superclass::AffineTransformType AffineTransformType;
  typedef typename Superclass::RegionType          RegionType;
  typedef typename Superclass::PointType           PointType;
  typedef typename Superclass::VectorType          VectorType;
  typedef typename Superclass::ColorType           ColorType;
  typedef itk::ContinuousIndex<double, 2>          ContinuousIndexType;
  typedef itk::Index<2>                            IndexType;

  /// spectralCurve typedef
  typedef TSpectralSignature                     SpectralSignatureCurveType;
  typedef typename TSpectralSignature::ValueType InternPxlType;

  /// Render the curve according to display extent and axis characteristics
  virtual void  Render(const RegionType& extent, const AffineTransformType * space2ScreenTransform);

  /// Pre-computation
  virtual void BeforeRendering();

  /// Get the min for each axis from the data available
  virtual PointType GetMinimum();

  /// Get the max for each axis from the data available
  virtual PointType GetMaximum();

  void SetTransparency(float t);

  /** Set/Get the spectralCurve */
  itkSetMacro(SpectralCurve, SpectralSignatureCurveType);
  itkGetConstReferenceMacro(SpectralCurve, SpectralSignatureCurveType);

  /** Set/Get the spectralCurve color */
  itkSetMacro(SpectralCurveColor, ColorType);
  itkGetMacro(SpectralCurveColor, ColorType);

  /** Set/Get the relative coordinates (x y in the image) */
  itkSetMacro(Coord, IndexType /*PointType*/);
  itkGetMacro(Coord, IndexType /*PointType*/);

protected:
  /** Constructor */
  SpectralSignatureCurve();
  /** Destructor */
  virtual ~SpectralSignatureCurve();
  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

private:
  SpectralSignatureCurve(const Self &);     // purposely not implemented
  void                               operator = (const Self &); // purposely not implemented

  // The spectralCurve to render
  SpectralSignatureCurveType m_SpectralCurve;

  // The spectralCurve color
  ColorType m_SpectralCurveColor;

  PointType m_Minimum;
  PointType m_Maximum;
  IndexType m_Coord;
}; // end class
} // end namespace otb

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbSpectralSignatureCurve.txx"
#endif

#endif
