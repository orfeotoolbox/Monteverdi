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

#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "otbVectorImage.h"


#include "otbAsynchronousProcess.h"

namespace otb
{
namespace Process
{


/** Constructor */
AsynchronousProcess
::AsynchronousProcess( std::string file ) : m_FileName(file)
{
  std::cout<<"AsynchronousProcess: Constructor"<<std::endl;
  std::cout<<this<<std::endl;
}


void
AsynchronousProcess
::Run(void *t)
{
  std::cout<<"RUN"<<std::endl;
  std::cout<<"RUN"<<(AsynchronousProcess*)(t)<<std::endl;
  AsynchronousProcess * lThis = (AsynchronousProcess*)(t);
  long int stop = 1e7;
  int valStat = lThis->GetProcessStatus();
  while( valStat<100 )
    {
      long int curVal = 0;
      while( curVal<stop)
	{
	  curVal++;
	}
      valStat++;
      lThis->SetProcessStatus(valStat);
    }
}

} // namespace Process
} //namespace otb
