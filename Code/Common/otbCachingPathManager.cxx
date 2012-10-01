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

#include "otbCachingPathManager.h"
#include "FLU/Flu_File_Chooser.h"
#include "otbI18n.h"
#include "otbMsgReporter.h"

#include "itksys/SystemTools.hxx"


namespace otb
{
/** Initialize the singleton */
CachingPathManager::Pointer CachingPathManager::m_Instance = NULL;

/// Constructor
CachingPathManager
::CachingPathManager()
{
  m_CachingPath = "Caching/";
  m_CachingPathWasChecked = false;
  m_CachingPathIsOK = false;
}

/** Manage the singleton */
CachingPathManager::Pointer
CachingPathManager
::GetInstance()
{
  if (!m_Instance)
    {
    m_Instance = CachingPathManager::New();
    }
  return m_Instance;
}

//Set title
void
CachingPathManager
::SetCachingPath(const std::string& path)
{
  m_CachingPath = itksys::SystemTools::GetRealPath( m_CachingPath.c_str() );
  m_CachingPathWasChecked = false;
  m_CachingPathIsOK = false;
}


//Get path
std::string
CachingPathManager
::GetCachingPath()
{
  // Pay attention, "Caching/" give "" with GetFilenameName
  std::string res = itksys::SystemTools::GetFilenameName( m_CachingPath.c_str() );
  if( res == "" )
    {
      res = m_CachingPath;
    }

  return res;
}

//Get path
std::string
CachingPathManager
::GetFullCachingPath()
{
  return itksys::SystemTools::GetRealPath( m_CachingPath.c_str() );
}


std::string
CachingPathManager
::GetAValidCachingPath()
{
  bool isOK = false;
  if( m_CachingPathWasChecked == true )
    {
      if( m_CachingPathIsOK == true )
        {
          isOK = true;
        }
    }
  else
    {
      this->IsValidCachingPath(m_CachingPath);
      if( m_CachingPathIsOK == true )
        {
          isOK = true;
        }
    }

  
  if(isOK == false)
    {
      this->ChangeCachingPath();
    }

  return this->GetCachingPath();
}


void
CachingPathManager
::ChangeCachingPath()
{
  const char * filename = NULL;
  filename = flu_dir_chooser(otbGetTextMacro("Choose a caching direstory..."), "");
  
  if (filename == NULL)
    {
      return;
    }

  if( this->IsValidCachingPath(filename) == false )
    {
      itk::OStringStream oss;
      oss<<"Invalid directory ";
      oss<<filename;
      oss<<" as caching dir."<<std::endl;
        
      MsgReporter::GetInstance()->SendError(oss.str());
      this->ChangeCachingPath();
    }
  else
    {
      m_CachingPath = filename;
    }
}

bool
CachingPathManager
::IsValidCachingPath( const std::string& path )
{
  bool res = true;
  // if existing dir, check writable
  if ( itksys::SystemTools::FileIsDirectory( path.c_str() ) )
    {
      itk::OStringStream oss;
      oss<<path<<"/foo";
      if( itksys::SystemTools::Touch( oss.str().c_str(), true ) == false )
        {
          res = false;
        }
      else
        {
          itksys::SystemTools::RemoveFile( oss.str().c_str() );
        }
    }
  // if existing file
  else if( itksys::SystemTools::FileExists(path.c_str()) )
    {
      res = false;
    }
  // don't exist, try to create it
  else if( itksys::SystemTools::MakeDirectory( path.c_str() ) ==false )
    {
      res = false;
    }
  else
    {
      itksys::SystemTools::RemoveADirectory( path.c_str() );
    }

  m_CachingPathWasChecked = true;
  m_CachingPathIsOK = res;

  return res;
}

bool
CachingPathManager
::IsValidCachingPath()
{
  return this->IsValidCachingPath(m_CachingPath);
}


// Path validity
bool
CachingPathManager
::CreateCachingDirectory()
{
  bool res = itksys::SystemTools::MakeDirectory( m_CachingPath.c_str() );
  
  if( res == false )
    {
      itk::OStringStream oss;
      oss<<"Not enough permission for write caching in ";
      oss<<itksys::SystemTools::GetRealPath(".")<<".";
      MsgReporter::GetInstance()->SendError(oss.str());
    }

  return res;
}

// Path validity
bool
CachingPathManager
::CachingDirExists()
{
  return itksys::SystemTools::FileExists( m_CachingPath.c_str() );
}

}
