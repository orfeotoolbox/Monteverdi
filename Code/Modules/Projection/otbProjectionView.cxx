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
#include "otbProjectionModule.h"

#include <FL/Fl_Text_Buffer.H>
#include <FL/fl_ask.H>
#include <FL/Enumerations.H>

#include "otbMacro.h"
#include "otbPerBandVectorImageFilter.h"
#include "itkExceptionObject.h"


#include "elevation/ossimElevManager.h"
#include "base/ossimFilename.h"
#include "base/ossimDirectory.h"

// Maps include
#include "otbMapProjections.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "otbWindowedSincInterpolateImageBlackmanFunction.h"
#include "otbWindowedSincInterpolateImageHammingFunction.h"
#include "otbWindowedSincInterpolateImageCosineFunction.h"
#include "otbWindowedSincInterpolateImageWelchFunction.h"
#include "otbWindowedSincInterpolateImageLanczosFunction.h"
#include "otbBSplineInterpolateImageFunction.h"
#include "otbMsgReporter.h"

#include "itkContinuousIndex.h"



namespace otb
{
/**
 * Constructor
 */
ProjectionView::ProjectionView()
{
  // Build the Gui
  this->CreateGUI();
  
  // Initiliaze the GUI values
  guiUTMZone->value("31");
  guiTRANSMERCATOREast->value("0");
  guiTRANSMERCATORNorth->value("0");
  guiTRANSMERCATORScale->value("1");
  
  // m_InterpType = LINEAR_;
  m_MapType = MAP_UTM;
  // m_HasOutput = false;
}

/**
 * Handle the notification of the model
 */
void
ProjectionView::Notify()
{
  if(m_Controller->GetModel()->GetTransformChanged())
    {
      // Update the cartographic parameters
      this->UpdateMapParam();
      // Update the output 
      this->UpdateOutputParameters();
    }
}

/**
 *
 */
void
ProjectionView
::Show()
{
  // Notify the controller that the GUI is going to be shown
  // Note that call is useful for transform initialization and paramters initiaization
  // Get the projection initial parameters
  
  int utmZone = atoi(guiUTMZone->value());
  bool north = guiUTMNorth->value();
  m_Controller->UpdateUTMTransform(utmZone,north);
  
  try
  {
    this->InitializeAction();
    // test if the good parameters are available
    // int resCheckImageParameters = this->CheckImageParameters();
    int resCheckImageParameters = 0;
    
    if (resCheckImageParameters == 1)
      {
	itkExceptionMacro(<<"Invalid image parameters");
      }
    
    guiMainWindow->show();
  }
  catch (itk::ExceptionObject & err)
    {
      MsgReporter::GetInstance()->SendError(err.GetDescription());
    }
}

/**
 *
 */
void
ProjectionView
::Quit()
{
  guiMainWindow->hide();
}

/**
 *
 */
void
ProjectionView
::OK()
{
  // tell the model to project the region edited in the GUI
  unsigned int sizeX    = atoi(guiSizeX->value());  
  unsigned int sizeY    = atoi(guiSizeY->value());
  double       spacingX = atof(guiSpacingX->value());
  double       spacingY = atof(guiSpacingY->value());
  double       originX =  atof(guiLongSelection->value());
  double       originY =  atof(guiLatSelection->value());
  bool         isUl    =  guiCenterPixel->value();
  
  std::cout << "View : Spacing  [" <<spacingX << "," << spacingY << "]" << std::endl;
  m_Controller->ProjectRegion(sizeX,sizeY,spacingX,spacingY,originX,originY,isUl);

  // Project the image in the selected map projection
  m_Controller->ReprojectImage();
}

/**
 *
 */
void
ProjectionView
::UpdateMapParam()
{
  ModelType::OutputPointType      geoPoint, newCartoPoint;
  // Get the new values of Long/Lat from the gui

  geoPoint[0] = atof(guiLongSelection->value());
  geoPoint[1] = atof(guiLatSelection->value());

  // Project the new geo Point 
  TransformType::Pointer                  rsTransform        = m_Controller->GetModel()->GetTransform();
  
  itk::OStringStream oss;
  oss<<setiosflags(std::ios_base::fixed);
  oss.str("");

  switch (this->GetMapType())
    {
    case MAP_UTM:
      {
	newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
	oss.str("");
	oss<<newCartoPoint[1];
	guiUTMNorthSelection->value(oss.str().c_str());
	oss.str("");
	oss<<newCartoPoint[0];
	guiUTMEastSelection->value(oss.str().c_str());
	break;
      }
    case MAP_LAMBERT2:
      {
	newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
	oss.str("");
	oss<<newCartoPoint[1];
	guiLambertNorthSelection->value(oss.str().c_str());
	oss.str("");
	oss<<newCartoPoint[0];
	guiLambertEastSelection->value(oss.str().c_str());
	break;
      }
    case MAP_TRANSMERCATOR:
      {
	newCartoPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
	oss.str("");
	oss<<newCartoPoint[1];
	guiTransmercatorNorthSelection->value(oss.str().c_str());
	oss.str("");
	oss<<newCartoPoint[0];
	guiTransmercatorEastSelection->value(oss.str().c_str());
	break;
      }
    default:
      fl_alert("Problem with map projection type, please contact developpers");
      break;
    }
}


/**
 * 
 */
void
ProjectionView
::UpdateLongLat()
{
  IndexType index;
  PointType point,geoPoint;
  
  index.Fill(0);
  // Get the transform from the model
  TransformType::Pointer rsTransform = m_Controller->GetModel()->GetTransform();
  
  // Apply the transform to the middle point of the image
  if(guiCenterPixel->value() == 1 && guiULPixel->value() == 0)
    {
      index[0] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[0]/2 + 1;
      index[1] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[1]/2 + 1;
      
    }
  else if(guiULPixel->value() == 1 && guiCenterPixel->value() == 0)
    {
      index = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetIndex();
    }
  
  // From index to Physical Point
  m_Controller->GetModel()->GetInputImage()->TransformIndexToPhysicalPoint(index,point);
  
  // Transform to geo and to Choosen projection
  geoPoint    = rsTransform->GetTransform()->GetFirstTransform()->TransformPoint(point);
  
  std::cout << std::setprecision(10) << "View::UpdateLatLong:: index " << index << " point " << point << " -->outputPoint " <<rsTransform->TransformPoint(point) << std::endl;
  
  // Fill the datas in the GUI
  itk::OStringStream oss;
  oss<<setiosflags(std::ios_base::fixed);
  oss.str("");
  oss<<geoPoint[0];
  guiLongSelection->value(oss.str().c_str());
  oss.str("");
  oss<<geoPoint[1];
  guiLatSelection->value(oss.str().c_str());
  
  //Update the Map parameters
  this->UpdateMapParam();
}

/**
 * 
 */

void
ProjectionView
::UpdateOutputParameters()
{
  itk::OStringStream oss;
  oss.str("");
  oss<<m_Controller->GetModel()->GetOutputSpacing()[0];
  guiSpacingX->value(oss.str().c_str());
  oss.str("");
  oss<<m_Controller->GetModel()->GetOutputSpacing()[1];
  guiSpacingY->value(oss.str().c_str());
  
  oss.str("");
  oss<<m_Controller->GetModel()->GetOutputSize()[0];
  guiSizeX->value(oss.str().c_str());
  oss.str("");
  oss<<m_Controller->GetModel()->GetOutputSize()[1];
  guiSizeY->value(oss.str().c_str());
}


int
ProjectionView::CheckImageParameters()
{

//   if (strcmp("",guiSizeX->value()) == 0 || strcmp("",guiSizeY->value()) == 0)
//   {
//     fl_alert("Please, set Size values...");
//     return 1;
//   }
//   if (strcmp("",guiSpacingX->value()) == 0 || strcmp("",guiSpacingY->value()) == 0)
//   {
//     fl_alert("Please, set Spacing values...");
//     return 1;
//   }

//   m_OutputSize[0] = atoi(guiSizeX->value());
//   m_OutputSize[1] = atoi(guiSizeY->value());
//   m_OutputSpacing[0] = atof(guiSpacingX->value());
//   m_OutputSpacing[1] = atof(guiSpacingY->value());

//   ForwardSensorInputPointType longLatPoint;
//   longLatPoint[0] = strtod(guiLongSelection->value(), NULL);
//   longLatPoint[1] = strtod(guiLatSelection->value(), NULL);
//   DoubleVectorType cartoPoint = LongLatPointToCartoInOriginRef/*LongLatPointToCarto*/(longLatPoint);

//   // upper left
//   if ( guiCenterPixel->value()==1 )
//   {
//     m_OutputOrigin[0] = cartoPoint[0]-m_OutputSpacing[0]*m_OutputSize[0]/2;
//     m_OutputOrigin[1] = cartoPoint[1]-m_OutputSpacing[1]*m_OutputSize[1]/2;
//   }
//   else
//   {
//     if ( guiULPixel->value()==1 )
//     {
//       m_OutputOrigin[0] = cartoPoint[0];
//       m_OutputOrigin[1] = cartoPoint[1];
//     }
//   }

   return 0;
}


int
ProjectionView
::CheckMapParameters()
{
//   int res = 0;
//   switch (this->GetMapType())
//   {
//   case UTM:
//   {
//     if (strcmp("",guiUTMZone->value()) )
//     {
//       char hem = 'N';
//       if ( guiUTMSouth->value() == 1 )
//         hem = 'S';
//       typedef UtmInverseProjection UtmProjectionType;
//       UtmProjectionType::Pointer utmProjection = UtmProjectionType::New();
//       utmProjection->SetZone(atoi(guiUTMZone->value()));
//       utmProjection->SetHemisphere(hem);
//       res = this->CreateOutput<UtmProjectionType>(utmProjection);
//     }
//     else
//     {
//       itk::OStringStream oss;
//       oss.str("Invalid Zone parameter: ");
//       oss<<guiUTMZone->value();
//       fl_alert(oss.str().c_str());
//     }
//     break;
//   }
//   case LAMBERT2:
//   {
//     typedef Lambert2EtenduInverseProjection Lambert2Type;
//     Lambert2Type::Pointer lambert2Projection = Lambert2Type::New();
//     res = this->CreateOutput<Lambert2Type>(lambert2Projection);

//     break;
//   }
//   case TRANSMERCATOR:
//   {
//     if (strcmp("",guiTRANSMERCATOREast->value()) != 0 && strcmp("",guiTRANSMERCATORNorth->value()) != 0 )
//     {
//       if (strcmp("",guiTRANSMERCATORScale->value()) != 0 )

//       {
//         typedef otb::TransMercatorInverseProjection TransMercatorProjectionType;
//         TransMercatorProjectionType::Pointer transMercatorProjection = TransMercatorProjectionType::New();

//         transMercatorProjection->SetParameters(strtod(guiTRANSMERCATOREast->value(), NULL),
//                                                strtod(guiTRANSMERCATORNorth->value(), NULL),
//                                                strtod(guiTRANSMERCATORScale->value(), NULL));
//         res = this->CreateOutput<TransMercatorProjectionType>(transMercatorProjection);
//       }
//       else
//       {
//         fl_alert("Invalid Scale Factor...");
//       }
//     }
//     else
//     {
//       fl_alert("Please, set Easting and Northing values...");
//     }
//     break;
//   }
//   default:
//     fl_alert("Problem with map projection type, please contact developpers");
//     res = 1;
//     break;
//   }
  return 0;
}



int
ProjectionView
::UpdateInterpolator()
{
  int res = 0;

  switch (this->GetInterpolatorType())
  {
  case MAP_LINEAR_:
  {
    typedef itk::LinearInterpolateImageFunction<SingleImageType, double> LinearType;
    LinearType::Pointer interp = LinearType::New();
    m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
    break;
  }
  case MAP_NEAREST:
  {
    typedef itk::NearestNeighborInterpolateImageFunction<SingleImageType, double> NearestType;
    NearestType::Pointer interp = NearestType::New();
    m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
    break;
  }
  case MAP_SINC:
  {
    itk::OStringStream oss;
    oss.str("");
    oss<<guiSincRadius->value();
    if ( strcmp("",oss.str().c_str()) != 0 )
    {
      oss.str("");
      oss<<guiSincWindow->value();
      switch ( guiSincWindow->value() )
      {
      case 0:
      {
        typedef WindowedSincInterpolateImageBlackmanFunction<SingleImageType> BlackmanType;
        BlackmanType::Pointer interp = BlackmanType::New();
        interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()) );
	interp->Initialize();
        m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
        break;
      }
      case 1:
      {

        typedef WindowedSincInterpolateImageCosineFunction<SingleImageType> CosineType;
        CosineType::Pointer interp = CosineType::New();
        interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
	interp->Initialize();
        m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
        break;
      }
      case 2:
      {
        typedef WindowedSincInterpolateImageGaussianFunction<SingleImageType> GaussianType;
        GaussianType::Pointer interp = GaussianType::New();
        interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
	interp->Initialize();
        m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
        break;
      }
      case 3:
      {
        typedef WindowedSincInterpolateImageHammingFunction<SingleImageType> HammingType;
        HammingType::Pointer interp = HammingType::New();
        interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
	//interp->Initialize();
        m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
        break;
      }
      case 4:
      {
        typedef WindowedSincInterpolateImageLanczosFunction<SingleImageType> LanczosType;
        LanczosType::Pointer interp = LanczosType::New();
        interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
	//interp->Initialize();
        m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
        break;
      }
      case 5:
      {
        typedef WindowedSincInterpolateImageWelchFunction<SingleImageType> WelchType;
        WelchType::Pointer interp = WelchType::New();
        interp->SetRadius(static_cast<unsigned int>(guiSincRadius->value()));
	//interp->Initialize();
        m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);
        break;
      }
      default:
        fl_alert("Problem with interpolator type, please contact developpers");
        break;
      }
    }
    else
    {
      fl_alert("Invalid ionterpolator Radius...");
    }
    break;
  }
  case MAP_SPLINES:
  {
    typedef otb::BSplineInterpolateImageFunction<SingleImageType, double, double> SplineType;
    SplineType::Pointer interp = SplineType::New();
    interp->SetSplineOrder(static_cast<unsigned int>(guiSplineOrder->value()) );
    ////interp->Initialize();
    m_Controller->GetModel()->GetResampler()->SetInterpolator(interp);

    break;
  }
  default:
    fl_alert("Problem with map projection type, please contact developpers");
    res = 1;
    break;
  }
   return 0;
}


void
ProjectionView::InitializeAction()
{
  IndexType index;
  PointType middlePoint,geoPoint,outputPoint, outputPoint1;
  
  // Get the transform from the model
  TransformType::Pointer rsTransform = m_Controller->GetModel()->GetTransform();
  
  // Apply the transform to the middle point of the image
  index[0] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[0]/2 + 1;
  index[1] = m_Controller->GetModel()->GetInputImage()->GetLargestPossibleRegion().GetSize()[1]/2 + 1;
  
  // From index to Physical Point
  m_Controller->GetModel()->GetInputImage()->TransformIndexToPhysicalPoint(index,middlePoint);
  
  // Transform to geo and to Choosen projection
  //outputPoint = rsTransform->TransformPoint(middlePoint);
  geoPoint    = rsTransform->GetTransform()->GetFirstTransform()->TransformPoint(middlePoint);
  outputPoint = rsTransform->GetTransform()->GetSecondTransform()->TransformPoint(geoPoint);
  
  std::cout << std::setprecision(10) << middlePoint << " -->Hole Transform outputPoint " << outputPoint << " Step By Step Transform " << outputPoint1<< std::endl;

  // Fill the datas in the GUI
  itk::OStringStream oss;
  oss<<setiosflags(std::ios_base::fixed);
  oss.str("");
  oss<<geoPoint[0];
  guiLongSelection->value(oss.str().c_str());
  oss.str("");
  oss<<geoPoint[1];
  guiLatSelection->value(oss.str().c_str());

  // Update map parameters
  guiUTMZone->value("31");
  guiUTMNorth->value(1);
  guiUTMSouth->value(0);
  oss.str("");
  oss<<outputPoint[0];
  guiUTMEastSelection->value(oss.str().c_str());
  oss.str("");
  oss<<outputPoint[1];
  guiUTMNorthSelection->value(oss.str().c_str());

  // Update the output parameters
  this->UpdateOutputParameters();
}


void
ProjectionView::SetMapType(ProjectionMapType map)
{
  m_MapType = map;
  this->Modified();
}


ProjectionMapType
ProjectionView::GetMapType()
{
  return m_MapType;
}


void
ProjectionView::SetInterpolatorType(ProjectionInterpolatorType interp)
{
  m_InterpType = interp;
  this->UpdateInterpolator();
  this->Modified();
}


ProjectionInterpolatorType
ProjectionView::GetInterpolatorType()
{
  return m_InterpType;
}

void 
ProjectionView::UpdateUTMTransform()
{
  //Get the projection parameters
  int utmZone = atoi(guiUTMZone->value());
  bool north = guiUTMNorth->value();
  
  // Update the transform 
  m_Controller->UpdateUTMTransform(utmZone,north);
  

}


/**
 * Tell the model to compute the LambertII transform
 * Done in the module cause the user is not allowed to modify this
 */
void 
ProjectionView::UpdateLambertIITransform()
{
  m_Controller->InitializeLambertIITransform();

}


/**
 * 
 */
void 
ProjectionView::UpdateTMTransform()
{
  m_Controller->UpdateTMTransform(atof(guiTRANSMERCATORScale->value()),
				  atof(guiTRANSMERCATOREast->value()),
				  atof(guiTRANSMERCATORNorth->value()));
}

} // End namespace otb

