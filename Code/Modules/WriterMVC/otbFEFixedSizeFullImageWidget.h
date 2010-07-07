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
#ifndef __otbFEFixedSizeFullImageWidget_h
#define __otbFEFixedSizeFullImageWidget_h

#include "otbFixedSizeFullImageWidget.h"
#include "otbImageWidgetBoxForm.h"

namespace otb
{
/** \class FEFixedSizeFullImageWidget
 *
 */
template <class TPixel, class TController, class TLabel = double>
class ITK_EXPORT FEFixedSizeFullImageWidget : public FixedSizeFullImageWidget<TPixel>
{
public:
  /** Standard class typedefs */
  typedef FEFixedSizeFullImageWidget       Self;
  typedef FixedSizeFullImageWidget<TPixel> Superclass;
  typedef itk::SmartPointer<Self>          Pointer;
  typedef itk::SmartPointer<const Self>    ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(FEFixedSizeFullImageWidget, FixedSizeFullImageWidget);

  typedef TController                      ControllerType;
  typedef typename ControllerType::Pointer ControllerPointerType;
  typedef typename Superclass::IndexType   IndexType;
  typedef typename Superclass::SizeType    SizeType;
  typedef ImageWidgetBoxForm               BoxType;

  itkSetObjectMacro(Controller, ControllerType);
  itkGetObjectMacro(Controller, ControllerType);

  itkSetMacro(CustomHandle, bool);
  itkSetMacro(PixelSelection, bool);

  itkSetMacro(ClickIndex, IndexType);
  itkGetMacro(ClickIndex, IndexType);

  itkSetMacro(SelectedPixel, IndexType);
  itkGetMacro(SelectedPixel, IndexType);

  /** Default mode handling, without ROI selection */
  virtual int handle(int event)
  {
    if (m_Controller.IsNotNull())
      {
      switch (event)
        {
        case FL_RELEASE:
          {
          if (m_CustomHandle)
            {
            // in case of mouse click, change the point of view
            int x = Fl::event_x();
            int y = Fl::event_y();
            int w = this->GetInput()->GetLargestPossibleRegion().GetSize()[0];
            int h = this->GetInput()->GetLargestPossibleRegion().GetSize()[1];
            m_BoxSize[0] = this->w() - 2;
            m_BoxSize[1] = this->h() - 2;

            m_ClickIndex[0] = x;
            m_ClickIndex[1] = y;

            m_ClickIndex = this->WindowToImageCoordinates(m_ClickIndex);

            int ulx = static_cast<int>(static_cast<double>(m_BoxSize[0]) / 2 - 1);
            int uly = static_cast<int>(static_cast<double>(m_BoxSize[1]) / 2 - 1);

            if (m_ClickIndex[0] - ulx < 0) m_ClickIndex[0] = 0;
            else if ((m_ClickIndex[0] + ulx) > w) m_ClickIndex[0] = w - m_BoxSize[0];
            else m_ClickIndex[0] -= ulx;

            if (m_ClickIndex[1] - uly < 0) m_ClickIndex[1] = 0;
            else if ((m_ClickIndex[1] + uly) > h) m_ClickIndex[1] = h - m_BoxSize[1];
            else m_ClickIndex[1] -= uly;

            m_Box->SetIndex(m_ClickIndex);
            m_Box->SetSize(m_BoxSize);

            this->redraw();

            m_Controller->SetExtractInput(m_ClickIndex, m_BoxSize);
            }
          else if (m_PixelSelection)
            {
            m_SelectedPixel[0] = Fl::event_x();
            m_SelectedPixel[1] = Fl::event_y();
            m_SelectedPixel = this->WindowToImageCoordinates(m_SelectedPixel);
            IndexType id;
            id[0] = m_SelectedPixel[0] - 1;
            id[1] = m_SelectedPixel[1] - 1;
            SizeType size;
            size[0] = 3;
            size[1] = 3;
            m_Box->SetIndex(id);
            m_Box->SetSize(size);

            this->redraw();

            m_Controller->ChangePixelSelection();
            }
          break;
          }
        }
      }

    return Superclass::handle(event);
  }

protected:
  /**
   * Constructor.
   */
  FEFixedSizeFullImageWidget()
  {
    m_CustomHandle = false;
    m_PixelSelection = false;
    m_ClickIndex[0] = 0;
    m_ClickIndex[1] = 0;
    m_SelectedPixel[0] = 0;
    m_SelectedPixel[1] = 0;
    m_BoxSize[0] = 0;
    m_BoxSize[1] = 0;
    m_Box = BoxType::New();
    m_Box->SetSize(m_BoxSize);
    m_Box->SetIndex(m_ClickIndex);
    this->GetFormList()->PushBack(m_Box);
    this->SetFormOverlayVisible(true);
  }
  /**
   * Destructor.
   */
  virtual ~FEFixedSizeFullImageWidget() {}

private:
  bool             m_CustomHandle;
  bool             m_PixelSelection;
  IndexType        m_ClickIndex;
  IndexType        m_SelectedPixel;
  SizeType         m_BoxSize;
  BoxType::Pointer m_Box;
// Pointer to the controller
  ControllerPointerType m_Controller;
};

} // end namespace otb
#endif
