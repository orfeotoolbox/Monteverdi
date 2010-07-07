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
#ifndef __otbMonteverdiEvent_h
#define __otbMonteverdiEvent_h

namespace otb
{

class ITK_EXPORT MonteverdiEvent
{
public:
  /** Constructor */
  MonteverdiEvent() : m_Type("Unknown"), m_InstanceId("Unknown") {}
  MonteverdiEvent(const std::string& type, const std::string& id) : m_Type(type), m_InstanceId(id) {}
  virtual ~MonteverdiEvent(){}

  void SetType(const std::string& type)
  {
    m_Type = type;
  }

  const std::string& GetType() const
  {
    return m_Type;
  }

  void SetInstanceId(const std::string& id)
  {
    m_InstanceId = id;
  }

  const std::string& GetInstanceId() const
  {
    return m_InstanceId;
  }

private:
  /// Type
  std::string m_Type;

  /// Instance Id
  std::string m_InstanceId;
};

}
#endif
