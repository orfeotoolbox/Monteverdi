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
#ifndef __otbTypeManager_h
#define __otbTypeManager_h

#include "ConfigureMonteverdi.h"

#include "otbImage.h"
#include "otbVectorImage.h"
#include "otbVectorData.h"
#include "otbImageWithQuicklook.h"
#include "itkFixedArray.h"

#include "itkObject.h"
#include "itkObjectFactory.h"

namespace otb
{
/** \class TypeManager
 *  \brief This class helps to manage coherently the type name in Monteverdi.
 *
 *  Due to its authoritative nature, this class has a unique instance
 *  which maybe accessed through the GetInstance() method (accordingly
 *  to the Singleton pattern).
 *
 *  The two main functionality are to register a new type using the
 *  template method RegisterType() and to get the name associated with
 *  a type using the GetTypeName() template method.
 */

class TypeManager
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef TypeManager                   Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  // Pixel Types
  typedef Monteverdi_FLOATING_TYPE                     Floating_Point_Precision;
  typedef unsigned short                               Label_Short_Precision;
  typedef unsigned char                                Label_Char_Precision;
  typedef std::complex<Floating_Point_Precision>       Floating_Point_Complex;
  typedef itk::FixedArray<Floating_Point_Precision, 2> Deformation_Value_Type;

  // Single Band Image Types
  typedef otb::Image<Floating_Point_Precision, 2> Floating_Point_Image;
  typedef otb::Image<Label_Short_Precision, 2>    Labeled_Short_Image;
  typedef otb::Image<Label_Char_Precision, 2>     Labeled_Char_Image;
  typedef otb::Image<Floating_Point_Complex, 2>   Floating_Point_Complex_Image;
  typedef otb::Image<Deformation_Value_Type, 2>   Deformation_Field_Type;

  // Vector Image Types
  typedef otb::VectorImage<Floating_Point_Precision, 2> Floating_Point_VectorImage;
  typedef otb::VectorImage<Floating_Point_Complex, 2>   Floating_Point_Complex_VectorImage;
  typedef otb::VectorImage<Label_Char_Precision, 2>     Labeled_Char_VectorImage;
  typedef otb::VectorImage<Label_Short_Precision, 2>    Labeled_Short_VectorImage;

  // Vector Data Types
  typedef otb::VectorData<double>                        Vector_Data;
  typedef otb::VectorData<double, 2, short unsigned int> Labeled_Vector_Data;

  typedef otb::ImageWithQuicklook<Label_Short_Precision, 2> UInt16ImageWithQuicklook;

  /** Standard type macro */
  itkTypeMacro(TypeManager, itk::Object);

  /** Get the unique instance of type manager */
  static TypeManager * GetInstance();

  /** Register a new type */
  template <typename T> void RegisterType(const std::string& name)
  {
    // Retrieve typeId
    std::string typeId = GetTypeId<T>();

    // Check if that type is allready registered
    if (m_IdToNameMap.count(typeId) == 0)
      {
      // If not, check if a type is already registered with this name
      IdToNameMapType::const_iterator mIt = m_IdToNameMap.begin();
      bool                            found = false;
      while (!found && mIt != m_IdToNameMap.end())
        {
        found = (mIt->second == name);
        ++mIt;
        }
      // If not, register the type under the given name
      if (!found)
        {
        m_IdToNameMap[typeId] = name;
        }
      else
        {
        // If name is already in use, use the typeid instead
        m_IdToNameMap[typeId] = typeId;
        }
      }
    // If typeid is already registered, do nothing
  }

  /** Get the pretty name associated with a type if it is registered */
  template<typename T> std::string GetTypeName()
  {
    // Retrieve typeId
    std::string typeId = GetTypeId<T>();

    // Check if that type is  registered
    if (m_IdToNameMap.count(typeId) > 0)
      {
      // If it is, return its name
      return m_IdToNameMap[typeId];
      }
    else
      {
      // If no name is available, return the typeId
      return typeId;
      }
  }

protected:
  /** Constructor */
  TypeManager();

  /** Destructor */
  virtual ~TypeManager();

  /** PrintSelf */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** Retrieve type id */
  template <typename T> static std::string GetTypeId()
  {
    std::string name = typeid(T).name();
    return name;
  }

private:
  /** New is private due to the Singleton pattern */
  itkNewMacro(Self);

  /** Type of the id to name map */
  typedef std::map<std::string, std::string> IdToNameMapType;

  /** The id to name map */
  IdToNameMapType m_IdToNameMap;

  /** The unique TypeManager instance */
  static Pointer Instance;
};

} // End namespace otb

#endif
