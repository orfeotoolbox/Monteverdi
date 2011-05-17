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
#ifndef __otbCachingPathManager_h
#define __otbCachingPathManager_h

#include "itkObjectFactory.h"
#include <string>
#include "itkObject.h"
#include "itkMacro.h"
#include "otbMacro.h"

namespace otb
{
/** \class CachingPathManager
 * \brief This class implements an message reporter windows. It's used by alls graphicals applications wants to report message
 *
 * Use FLTK gui.
 *
 */
class CachingPathManager
  : public itk::Object
{
public:
  /** Standard class typedefs */
  typedef CachingPathManager                   Self;
  typedef itk::Object                   Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(CachingPathManager, Object);

  /** Get the unique instance of the model */
  static Pointer GetInstance();

  /** Set the caching path */
  virtual void SetCachingPath(const std::string& path);
  /** Get the caching path */
  virtual std::string GetCachingPath();
  /** Get the full caching path */
  virtual std::string GetFullCachingPath();
  /** If the caching path is not valid, will open a selection path window and return the valid path. */
  virtual std::string GetAValidCachingPath();
  /** Open a window to select a path. Will loop until the path is valid. */
  virtual void ChangeCachingPath();
  /** Chaeck caching path validity (not an existing file and a writable dir) */
  virtual bool IsValidCachingPath(const std::string& path);
  /** Call IsValidCachingPath(const std::string& path) with path = m_CachingPath */
  virtual bool IsValidCachingPath();
  /** Create the caching path dir. */
 virtual  bool CreateCachingDirectory();
 /** Does the caching dir exists */
 virtual bool CachingDirExists();

protected:
  // Constructor and destructor
  CachingPathManager();
  virtual ~CachingPathManager() {}

private:
  CachingPathManager(const Self&); //purposely not implemented
  void operator =(const Self&); //purposely not implemented

  /** The instance singleton */
  static Pointer m_Instance;
  /** Caching path. */
  std::string m_CachingPath;
  /** Does the validity of the Caching path was checked. */
  bool m_CachingPathWasChecked;
  /** Does the caching path is valid. */
  bool m_CachingPathIsOK;

};

} // end namespace otb

#endif
