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

void RegisterTests()
{
REGISTER_TEST(otbChangeDetectionModuleTest);
REGISTER_TEST(otbGCPToSensorModelModuleTest);
REGISTER_TEST(otbThresholdModuleTestNew);
REGISTER_TEST(otbThresholdModuleTest);
REGISTER_TEST(otbOpticalCalibrationModuleTest);
REGISTER_TEST(otbSarCalibrationModuleTest);
REGISTER_TEST(otbSupervisedClassificationModelTest);
REGISTER_TEST(otbSupervisedClassificationModelSetImageTest);
REGISTER_TEST(otbSupervisedClassificationModelSetLabeledImageTest);
REGISTER_TEST(otbSupervisedClassificationModelSetVectorDataTest);
REGISTER_TEST(otbSupervisedClassificationModelTrainTest);
REGISTER_TEST(otbSertitProcessingTest);
REGISTER_TEST(otbSertitProcessingTestNonGeoProduct);
REGISTER_TEST(otbSertitProcessingTestNonGeoProductXT);
}
