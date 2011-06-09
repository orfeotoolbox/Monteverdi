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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
#include <iostream>
#include "otbTestMain.h"

#include "otbConfigure.h"

void RegisterTests()
{
#ifdef OTB_USE_CURL
  REGISTER_TEST(otbTileMapImportModuleTest);
#endif
  REGISTER_TEST(otbVectorizationModuleTest);
  REGISTER_TEST(otbObjectLabelingOpenImage);
  REGISTER_TEST(otbObjectLabelingColoredOutput);
  REGISTER_TEST(otbFineCorrelationModuleTest);
  REGISTER_TEST(otbMeanShiftModuleTest);
  REGISTER_TEST(otbVectorizationClassification);
  REGISTER_TEST(otbPolarimetricSynthesisTest);
  REGISTER_TEST(otbColorMappingModuleTest);
  REGISTER_TEST(otbDEMToImageGeneratorModuleTest);
  REGISTER_TEST(otbImageStatisticsModuleTest);
  REGISTER_TEST(otbConnectedComponentSegmentationModuleTest);
  REGISTER_TEST(otbVectorDataTransformTest);

#ifdef OTB_USE_MAPNIK
  REGISTER_TEST(otbRasterizationModuleTest);
#endif
}
