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
#ifndef __otbMagicMain_H
#define __otbMagicMain_H


#include <iostream>
//#include "otbAsynchronousModel.h"


namespace otb
{
namespace Process
{

class MagicMain
{
 public:

 MagicMain(){};
  virtual ~MagicMain(){};
  /* 
 void SetModel(AsynchronousModel * model)
  {
    m_Model = model;
    m_Model->Register(this);
  };

  AsynchronousModel * GetModel()
  {
    return m_Model;
  };
  */
  virtual void Notify()
  {
    std::cout<<"Notification done"<<std::endl;
  }; 
 
 protected: 
 

 private:
  // AsynchronousModel * m_Model;
};

} // namespace Process
} // namespace otb

#endif
