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

// this file defines the otbCommonTest for the test driver
// and all it expects is that you have a function called RegisterTests


#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(otbImageWidgetBaseNew);
  REGISTER_TEST(otbFixedSizeFullImageWidgetNew);
  REGISTER_TEST(otbFixedSizeFullImageWidget);
  REGISTER_TEST(otbFullResolutionImageWidgetNew);
  REGISTER_TEST(otbFullResolutionImageWidget);
  REGISTER_TEST(otbZoomableImageWidgetNew);
  REGISTER_TEST(otbZoomableImageWidget);
  REGISTER_TEST(otbImageViewer);
  REGISTER_TEST(otbImageViewerWithOtbImage);
  REGISTER_TEST(otbImageViewerNew);

}
