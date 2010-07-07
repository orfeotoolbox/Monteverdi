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
#ifndef __otbClass_h
#define __otbClass_h

#include "itkObject.h"
#include "itkSmartPointer.h"
#include "otbMacro.h"

namespace otb
{

template <class TLabel, class TColor>
class Class
  : public itk::Object
{
public:
  /// Standard class typedefs
  typedef Class                         Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef std::vector<unsigned int>     ROIIndexVectorType;

  /// Standard macros
  itkNewMacro(Self);
  itkTypeMacro(Class, Object);

  /// Label type
  typedef TLabel LabelType;
  /// Color type
  typedef TColor ColorType;

  itkGetMacro(Name, std::string);
  itkSetMacro(Name, std::string);
  itkGetMacro(Id, LabelType);
  itkSetMacro(Id, LabelType);
  itkGetMacro(Color, ColorType);
  itkSetMacro(Color, ColorType);

protected:
  /// Constructor
  Class() {}
  /// Destructor
  virtual ~Class() {}

private:
  Class(const Self&); // purposely not implemented
  void operator =(const Self&); // purposely not implemented

  /// Name of the class
  std::string m_Name;
  /// Class id
  LabelType m_Id;
  /// Color
  ColorType m_Color;
};

} // end namespace otb

#endif
