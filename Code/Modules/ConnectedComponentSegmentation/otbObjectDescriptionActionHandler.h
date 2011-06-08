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
#ifndef __otbObjectDescriptionActionHandler_h
#define __otbObjectDescriptionActionHandler_h

#include "otbImageWidgetActionHandler.h"
#include <otbAttributesMapLabelObject.h>
#include <otbLabelImageToLabelMapWithAdjacencyFilter.h>

namespace otb
{
/** \class ObjectDescriptionActionHandler
*   \brief Implements Object reporting capabilities.
*
*   \sa ImageWidgetController
*   \sa ImageWidgetActionHandler
*  \ingroup Visualization
 */

template <class TModel, class TView>
class ObjectDescriptionActionHandler
  : public ImageWidgetActionHandler
{
public:
  /** Standard class typedefs */
  typedef ObjectDescriptionActionHandler Self;
  typedef ImageWidgetActionHandler Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory */
  itkNewMacro(Self)
  ;

  /** Runtime information */
  itkTypeMacro(ObjectDescriptionActionHandler, ImageWidgetActionHandler)
  ;

  /** Model typedefs */
  typedef TModel ModelType;
  typedef typename ModelType::Pointer ModelPointerType;
  typedef typename ModelType::IndexType IndexType;

  typedef typename ModelType::LayerType LayerType;
  typedef otb::ObjectList<LayerType> LayerListType;

  typedef typename LayerListType::ConstIterator LayerConstIteratorType;

  /** View typedef */
  typedef TView ViewType;
  typedef typename ViewType::Pointer ViewPointerType;
  typedef typename ViewType::OffsetType OffsetType;
  typedef typename ViewType::ImageWidgetPointerType WidgetPointerType;

  typedef otb::AttributesMapLabelObject<unsigned int, 2, double> AttributesMapLabelObjectType;
  typedef otb::LabelMapWithAdjacency<AttributesMapLabelObjectType> AttributesLabelMapType;
  typedef typename AttributesLabelMapType::Pointer AttributesLabelMapPointerType;

  typedef typename AttributesLabelMapType::LabelObjectType LabelObjectType;
  typedef typename LabelObjectType::LabelType LabelType;

  /** object description widget typedef */
  typedef Fl_Multiline_Output ObjectDescriptionWidgetType;

  /** Get the pixel description widget */
  ObjectDescriptionWidgetType * GetObjectDescriptionWidget()
  {
    return m_ObjectDescriptionWidget;
  }

  /** Get the pixel description widget */
  AttributesLabelMapType * GetAttributesLabelMap()
  {
    return m_AttributesLabelMap;
  }

  /** Get the pixel description widget */
  void SetAttributesLabelMap(AttributesLabelMapType *attributesLabelMap)
  {
    m_AttributesLabelMap = attributesLabelMap;
  }

  /** Handle widget event
   */
  virtual bool HandleWidgetEvent(const std::string& widgetId, int event)
  {
    if (m_Model.IsNotNull() && m_View.IsNotNull() && this->GetIsActive())
      {
      // Find the source widget
      WidgetPointerType sourceWidget;
      bool handle = false;
      if (widgetId == m_View->GetScrollWidget()->GetIdentifier())
        {
        sourceWidget = m_View->GetScrollWidget();
        handle = true;
        }
      else
        if (widgetId == m_View->GetFullWidget()->GetIdentifier())
          {
          sourceWidget = m_View->GetFullWidget();
          handle = true;
          }
        else
          if (widgetId == m_View->GetZoomWidget()->GetIdentifier())
            {
            sourceWidget = m_View->GetZoomWidget();
            handle = true;
            }

      if (handle)
        {

        if (Fl::event_button() == FL_RIGHT_MOUSE && event == FL_PUSH)
          {

          //  Get the clicked index
          typename ViewType::ImageWidgetType::PointType screenPoint, imagePoint;
          screenPoint = sourceWidget->GetMousePosition();

          //  Transform to image point
          imagePoint = sourceWidget->GetScreenToImageTransform()->TransformPoint(screenPoint);

          // Transform to index
          IndexType index;

          index[0] = static_cast<long int> (imagePoint[0]);
          index[1] = static_cast<long int> (imagePoint[1]);

          std::ostringstream oss;
          oss << otbGetTextMacro("Index") << ": " << imagePoint << std::endl;
          // Report pixel info for each visible layer
          for (LayerConstIteratorType it = m_Model->GetLayers()->Begin(); it != m_Model->GetLayers()->End(); ++it)
            {
            // If the layer is visible
            if (it.Get()->GetVisible())
              {
              // Get the pixel description
              //oss << it.Get()->GetPixelDescription(index) << std::endl;

              oss << otbGetTextMacro("Value") << ": " <<it.Get()->GetValueAtIndex(index) << std::endl;

              // we test if the filter output is selected
              std::string layerName = it.Get()->GetName();
              if (!(layerName.compare("OBIA Opening Label")) && m_AttributesLabelMap != NULL)
                {
                //display label object info.
                itk::VariableLengthVector<double> pixel = it.Get()->GetValueAtIndex(index);
                LabelType label = static_cast<LabelType> (pixel[0]);
                if (m_AttributesLabelMap->GetBackgroundValue() == label)
                  {
                  oss << "Background value " << std::endl;
                  }
                else
                  {
                  oss << "Object Properties : " << std::endl<<std::endl;
                  // select object
                  if (m_AttributesLabelMap->HasLabel(label))
                    {
                    // get attributes
                    LabelObjectType *object = m_AttributesLabelMap->GetLabelObject(label);

                    unsigned int nbOfAttributes = object->GetNumberOfAttributes();
                    std::vector<std::string> listOfAttributesName;
                    listOfAttributesName.resize(nbOfAttributes, "");
                    listOfAttributesName = object->GetAvailableAttributes();
                    for (unsigned int i = 0; i < nbOfAttributes; i++)
                      {
                      std::string attributeName = listOfAttributesName.at(i);
                      oss << attributeName << " " << object->GetAttribute(attributeName.c_str()) << std::endl;
                      }

                    }
                  else
                    {
                    oss << "No object with label " << label << std::endl;
                    }

                  }
                }
              else
              {
                oss << "No object properties available " << std::endl;
                oss << "Please select ""Filter output""  and click on ""Update"" Button " << std::endl;
              }

              }
            }

          m_ObjectDescriptionWidget->value(oss.str().c_str());
          m_ObjectDescriptionWidget->redraw();

          return true;
          }
        }

      return false;
      }
    return false;
  }

  /** Set/Get the pointer to the model */
  itkSetObjectMacro(Model, ModelType)
  ;itkGetObjectMacro(Model, ModelType)
  ;

  /** Set/Get the pointer to the view */
  itkSetObjectMacro(View, ViewType)
  ;itkGetObjectMacro(View, ViewType)
  ;

protected:
  /** Constructor */
  ObjectDescriptionActionHandler() :
    m_View(), m_Model(), m_ObjectDescriptionWidget(NULL), m_AttributesLabelMap(NULL)
  {

    // build a widget with a default size
    m_ObjectDescriptionWidget = new Fl_Multiline_Output(0, 0, 500, 80);
    m_ObjectDescriptionWidget->textsize(10);
    m_ObjectDescriptionWidget->textfont(FL_SCREEN);
    //m_ObjectDescriptionWidget->deactivate();

  }

  /** Destructor */
  virtual ~ObjectDescriptionActionHandler()
  {
  }
  /** Printself method */
  void PrintSelf(std::ostream& os, itk::Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

private:
  ObjectDescriptionActionHandler(const Self&); // purposely not implemented
  void operator =(const Self&); // purposely not implemented

  // Pointer to the view
  ViewPointerType m_View;

  // Pointer to the model
  ModelPointerType m_Model;

  ObjectDescriptionWidgetType * m_ObjectDescriptionWidget;

  AttributesLabelMapType * m_AttributesLabelMap;

}; // end class
} // end namespace otb
#endif

