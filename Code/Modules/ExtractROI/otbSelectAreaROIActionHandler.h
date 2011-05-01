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
#ifndef __selectAreaActionHandler_h
#define __selectAreaActionHandler_h

#include "otbImageWidgetActionHandler.h"
#include "otbRegionGlComponent.h"
#include "itkImageRegion.h"

#include <FL/Fl_Value_Input.H>

/** \class SelectAreaROIActionHandler
 *   \brief Draw a rectangle on ROI.
 *
 *   \sa ImageWidgetController
 *   \sa ImageWidgetActionHandler
 *  \ingroup Visualization
 */
namespace otb
{
template <class TModel, class TWidget>
class SelectAreaROIActionHandler : public ImageWidgetActionHandler
{
public:
  /** Standard class typedefs */
  typedef SelectAreaROIActionHandler    Self;
  typedef ImageWidgetActionHandler      Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self);

  /** Runtime information */
  itkTypeMacro(SelectAreaROIActionHandler, ImageWidgetActionHandler);

  /** Model typedefs */
  typedef TModel                         ModelType;
  typedef typename ModelType::Pointer    ModelPointerType;
  typedef typename ModelType::RegionType RegionType;

  /** View typedefs */
  typedef TWidget                        WidgetType;
  typedef typename WidgetType::Pointer   WidgetPointerType;
  typedef typename WidgetType::IndexType IndexType;

  /** RegionGlComponent View */
  typedef RegionGlComponent                       RegionGlComponentType;
  typedef typename RegionGlComponentType::Pointer RegionGlComponentPointerType;

  /** Region */
  typedef itk::ImageRegion<2>    RegionGlType;
  typedef RegionGlType::SizeType SizeType;
  typedef Fl_Value_Input         FlValueInputType;

  /** Handle widget event
   * \param widgetId The id of the moved widget
   * \param event The event
   * \return The handling return code
   */
  virtual bool HandleWidgetEvent(std::string widgetId, int event)
  {
    // if left mouse button pressed
    if (Fl::event_button() != FL_LEFT_MOUSE)
      {
      return false;
      }

    if (m_Model.IsNotNull() && this->GetIsActive())
      {
      if (widgetId == m_Widget->GetIdentifier())
        {
        // Min and max
        int maxX, minX, maxY, minY;

        // Get the clicked index
        typename WidgetType::PointType screenPoint, imagePoint;    /*, lScreenSizePoint, lImageSizePoint; */
        screenPoint = m_Widget->GetMousePosition();

        // Transform to image point
        imagePoint = m_Widget->GetScreenToImageTransform()->TransformPoint(screenPoint);

        // Transform to index
        IndexType lIndex, lIndex2, lIndexLargestMin, lIndexLargestMax;
        lIndex[0] = static_cast<long int>(imagePoint[0]);
        lIndex[1] = static_cast<long int>(imagePoint[1]);

        SizeType lSize, lSizeLargest;

        lIndexLargestMin = m_LargestRegion.GetIndex();
        lSizeLargest = m_LargestRegion.GetSize();
        lIndexLargestMax[0] = lSizeLargest[0] - lIndexLargestMin[0];
        lIndexLargestMax[1] = lSizeLargest[1] - lIndexLargestMin[1];

        switch (event)
          {
          case FL_PUSH:
            {
            m_FirstPush = false;
            m_StartIndex = lIndex;

            if ((m_StartIndex[0] >= lIndexLargestMin[0]) && (m_StartIndex[1] >= lIndexLargestMin[1]) &&
                (m_StartIndex[0] < lIndexLargestMax[0]) && (m_StartIndex[1] < lIndexLargestMax[1]))
              {
              m_Region.SetIndex(m_StartIndex);
              m_Region.Crop(m_LargestRegion);
              m_RegionGlComponent->SetVisible(false);

              m_StartX->value(m_StartIndex[0]);
              m_StartY->value(m_StartIndex[1]);
              m_SizeX->value(1);
              m_SizeY->value(1);

              /** Call Update Region in otbExtractROIModule.cxx */
              m_StartY->do_callback();
              }
            break;
            }
          case FL_RELEASE:
            {
            if (m_StartIndex[0] != lIndex[0] && m_StartIndex[1] != lIndex[1])
              {
              if ((m_StartIndex[0] >= lIndexLargestMin[0]) && (m_StartIndex[1] >= lIndexLargestMin[1]) &&
                  (m_StartIndex[0] < lIndexLargestMax[0]) && (m_StartIndex[1] < lIndexLargestMax[1]))
                {
                m_StopIndex = lIndex;

                minX = std::min(m_StartIndex[0], m_StopIndex[0]);
                minY = std::min(m_StartIndex[1], m_StopIndex[1]);
                maxX = std::max(m_StartIndex[0], m_StopIndex[0]);
                maxY = std::max(m_StartIndex[1], m_StopIndex[1]);

                lIndex2[0] = minX;
                lIndex2[1] = minY;

                lSize[0] = maxX - minX;
                lSize[1] = maxY - minY;

                m_Region.SetIndex(lIndex2);
                m_Region.SetSize(lSize);
                m_Region.Crop(m_LargestRegion);

                m_RegionGlComponent->SetRegion(m_Region);

                lIndex2 = m_Region.GetIndex();
                lSize = m_Region.GetSize();

                m_StartX->value(lIndex2[0]);
                m_StartY->value(lIndex2[1]);
                m_SizeX->value(lSize[0]);
                m_SizeY->value(lSize[1]);

                /** Call Update Region in otbExtractROIModule.cxx */
                m_SizeY->do_callback();
                }
              }
            break;
            }
          case FL_DRAG:
            {
            if ((m_StartIndex[0] >= lIndexLargestMin[0]) && (m_StartIndex[1] >= lIndexLargestMin[1]) &&
                (m_StartIndex[0] < lIndexLargestMax[0]) && (m_StartIndex[1] < lIndexLargestMax[1]))
              {
              // only redraw the red box in the widget
              m_StopIndex = lIndex;

              minX = std::min(m_StartIndex[0], m_StopIndex[0]);
              minY = std::min(m_StartIndex[1], m_StopIndex[1]);
              maxX = std::max(m_StartIndex[0], m_StopIndex[0]);
              maxY = std::max(m_StartIndex[1], m_StopIndex[1]);

              lIndex2[0] = minX;
              lIndex2[1] = minY;

              lSize[0] = maxX - minX;
              lSize[1] = maxY - minY;

              m_Region.SetIndex(lIndex2);
              m_Region.SetSize(lSize);
              m_Region.Crop(m_LargestRegion);

              m_RegionGlComponent->SetRegion(m_Region);
              m_RegionGlComponent->SetVisible(true);

              lIndex2 = m_Region.GetIndex();
              lSize = m_Region.GetSize();

              m_StartX->value(lIndex2[0]);
              m_StartY->value(lIndex2[1]);
              m_SizeX->value(lSize[0]);
              m_SizeY->value(lSize[1]);

              /** Call Update Region in otbExtractROIModule.cxx */
              m_SizeY->do_callback();

              m_Widget->redraw();
              }
            break;
            }
          default:
            {
            }
          }
        return true;
        }
      }
    return false;
  }

  void UpdateGlRegion()
  {
    IndexType lIndex;
    SizeType  lSize;

    lIndex[0] = static_cast<unsigned long>(m_StartX->value());
    lIndex[1] = static_cast<unsigned long>(m_StartY->value());

    lSize[0] = static_cast<unsigned long>(m_SizeX->value());
    lSize[1] = static_cast<unsigned long>(m_SizeY->value());

    m_Region.SetIndex(lIndex);
    m_Region.SetSize(lSize);
    m_Region.Crop(m_LargestRegion);

    m_RegionGlComponent->SetRegion(m_Region);
    m_RegionGlComponent->SetVisible(true);

    lIndex = m_Region.GetIndex();
    lSize = m_Region.GetSize();

    m_StartX->value(lIndex[0]);
    m_StartY->value(lIndex[1]);
    m_SizeX->value(lSize[0]);
    m_SizeY->value(lSize[1]);

    m_Widget->redraw();
  }

  /** Set/Get the pointer to the view */
  itkSetObjectMacro(Widget, WidgetType);
  itkGetObjectMacro(Widget, WidgetType);

  /** Set/Get the pointer to the model */
  itkSetObjectMacro(Model, ModelType);
  itkGetObjectMacro(Model, ModelType);

  /** Set/Get the pointer to the RegionGlComponent */
  itkSetObjectMacro(RegionGlComponent, RegionGlComponentType);
  itkGetObjectMacro(RegionGlComponent, RegionGlComponentType);

  itkSetMacro(LargestRegion, RegionType);
//      itkGetObjectMacro(LargestRegion, RegionType);

  /** Set/Get the pointer to the input */
  void SetStartX(FlValueInputType *fvi)
  {
    m_StartX = fvi;
  }
  void SetStartY(FlValueInputType *fvi)
  {
    m_StartY = fvi;
  }
  void SetSizeX(FlValueInputType *fvi)
  {
    m_SizeX = fvi;
  }
  void SetSizeY(FlValueInputType *fvi)
  {
    m_SizeY = fvi;
  }
  void SetLatitute1(FlValueInputType *fvi)
  {
    m_Lat1 = fvi;
  }
  void SetLatitute2(FlValueInputType *fvi)
  {
    m_Lat2 = fvi;
  }
  void SetLongitude1(FlValueInputType *fvi)
  {
    m_Long1 = fvi;
  }
  void SetLongitude2(FlValueInputType *fvi)
  {
    m_Long2 = fvi;
  }

protected:
  /** Constructor */
  SelectAreaROIActionHandler() : m_Widget(), m_Model(), m_RegionGlComponent()
  {
    m_FirstPush = true;
    m_StartIndex.Fill(0);
    m_StopIndex.Fill(0);
  }

  /** Destructor */
  virtual ~SelectAreaROIActionHandler(){}
  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

private:
  SelectAreaROIActionHandler(const Self&);        // purposely not implemented
  void operator =(const Self&);    // purposely not implemented

  // Pointer to the widget
  WidgetPointerType m_Widget;
  // Pointer to the model
  ModelPointerType m_Model;
  // Pointer to the GlComponent
  RegionGlComponentPointerType m_RegionGlComponent;
  // Pointer to the Region of RegionGlComponent
  RegionGlType m_Region;
  RegionGlType m_LargestRegion;

  bool m_FirstPush;

  // Area selection indexes
  IndexType m_StartIndex;
  IndexType m_StopIndex;

  // Input value
  FlValueInputType *m_StartX;
  FlValueInputType *m_StartY;
  FlValueInputType *m_SizeX;
  FlValueInputType *m_SizeY;
  FlValueInputType *m_Long1;
  FlValueInputType *m_Lat1;
  FlValueInputType *m_Long2;
  FlValueInputType *m_Lat2;

};     // end class
} // otb namespace

#endif
