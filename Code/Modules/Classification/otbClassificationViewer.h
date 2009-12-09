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
#ifndef __otbClassificationViewer_h
#define __otbClassificationViewer_h

#include "otbClass.h"
#include "otbImageViewerBase.h"


namespace otb
{

template <class TPixel, class TLabel = double>
class ClassificationViewer
      : public otb::ImageViewerBase<TPixel,TLabel>
{
public:
  /// Standard class typedefs
  typedef ClassificationViewer                Self;
  typedef otb::ImageViewerBase<TPixel,TLabel> Superclass;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::SmartPointer<const Self>       ConstPointer;

  /// Standard macros
  itkNewMacro(Self);
  itkTypeMacro(ClassificationViewer,ImageViewerBase);

  typedef TPixel                                         PixelType;
  typedef TLabel                                         LabelType;
  typedef typename Superclass::IndexType                 IndexType;
  typedef typename Superclass::FullWidgetType::ColorType ColorType;
  typedef otb::Class<LabelType,ColorType>                ClassType;
  typedef typename ClassType::Pointer                    ClassPointerType;
  typedef std::vector<ClassPointerType>                  ClassesMapType;
  typedef otb::Image<LabelType,2>                        LabeledImageType;
  typedef typename LabeledImageType::Pointer             LabeledImagePointerType;

  itkSetObjectMacro(LabeledImage,LabeledImageType);
  itkGetObjectMacro(LabeledImage,LabeledImageType);
  itkSetMacro(ShowClass,bool);
  itkGetMacro(ShowClass,bool);

  void SetClassesMap(ClassesMapType & classesMap)
  {
    m_ClassesMap = classesMap;
  }

  ClassesMapType & GetClassesMap(void)
  {
    return m_ClassesMap;
  }

protected:
  /// Constructor
  ClassificationViewer()
  {
    m_LabeledImage = NULL;
    m_ShowClass = false;
  };
  /// Destructor
  virtual ~ClassificationViewer() {};

  virtual void ReportPixel(IndexType index)
  {
    Superclass::ReportPixel(index);
    if (!m_ShowClass || this->GetPixLocOutput() == NULL)
    {
      return;
    }
    itk::OStringStream oss;
    oss<<this->GetPixLocOutput()->value();
    if (m_LabeledImage->GetBufferedRegion().IsInside(index))
    {

      LabelType label = m_LabeledImage->GetPixel(index);
      bool found = false;
      typename ClassesMapType::iterator it = m_ClassesMap.begin();

      while (!found && it!=m_ClassesMap.end())
      {
        if ((*it)->GetId() == label)
        {
          found = true;
          oss<<(*it)->GetName()<<std::endl;
        }
        ++it;
      }
    }

    if (oss.good())
    {
      //this->ClearPixLocVal();
      this->GetPixLocOutput()->value(oss.str().c_str());
      this->GetPixLocOutput()->redraw();
      Fl::check();
    }
  }

private:
  ClassificationViewer(const Self&); // purposely not implemented
  void operator=(const Self&); // purposely not implemented

/// Labeled Image
  LabeledImagePointerType m_LabeledImage;
  bool m_ShowClass;
  ClassesMapType m_ClassesMap;
};

} // end namespace otb

#endif
