/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
See OTBCopyright.txt for details.


    This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE,  See the above copyright notices for more information.

=========================================================================*/

#include "otbObjectCountingController.h"
#include "otbMsgReporter.h"
#include "otbFltkFilterWatcher.h"
#include <FL/fl_ask.H>

namespace otb
{

ObjectCountingController
::ObjectCountingController()
{
  m_IsExtractRunning = false;
  m_HasDoubleClicked = true;
}

ObjectCountingController
::~ObjectCountingController()
{
  MsgReporter::GetInstance()->Hide();
}

void
ObjectCountingController
::OpenImage()
{
  FltkFilterWatcher watcher(m_Model->GetShrinker(),0,0,200,20, "Loading image ...");
  try
  {
    SizeType qlSize;
    qlSize[0] = m_View->gScroll->w();
    qlSize[1] = m_View->gScroll->h();
    m_Model->SetQuicklookSize( qlSize );
    m_Model->OpenImage();
    std::cout << "Controller openImage continue..." << std::endl;
    ViewType::RegionType region;
    IndexType newIndex;
    newIndex.Fill(0);
    ViewType::SizeType newSize;
    newSize[0]=m_View->gImageViewer->w();
    newSize[1]=m_View->gImageViewer->h();
    region.SetIndex(newIndex);
    region.SetSize(newSize);
    std::cout << "Will set the current region" << std::endl;
    m_Model->SetCurrentRegion(region);
    m_IsExtractRunning = true;
    std::cout << "Image extraction" << std::endl;
    m_Model->RunImageExtraction();
    m_IsExtractRunning = false;
    m_HasDoubleClicked = true;
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
ObjectCountingController
::IndexClicked(const IndexType & index)
{
  if ( m_IsExtractRunning==false )
  {
    m_IsExtractRunning = true;
    ViewType::RegionType region = m_View->GetFullImageWidget()->GetBufferedRegion();
    IndexType newIndex;
    newIndex[0]=index[0]*m_Model->GetShrinkFactor()-region.GetSize()[0]/2;
    newIndex[1]=index[1]*m_Model->GetShrinkFactor()-region.GetSize()[1]/2;
    region.SetIndex(newIndex);
    m_Model->SetCurrentRegion(region);
    m_Model->RunImageExtraction();
    m_IsExtractRunning = false;
  }
  m_HasDoubleClicked = true;
}


void
ObjectCountingController
::SaveResult(const char * cfname)
{
  try
  {
    FltkFilterWatcher watcher(m_Model->GetRelabelFilter(),0,0,300,20, "Run over Full Image ...");
    m_Model->RunChain( cfname );
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}


void
ObjectCountingController
::SavePolygon(const char * cfname)
{
  try
  {
    m_Model->SavePolygon( cfname );
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
ObjectCountingController
::SimpleLeftClick(const IndexType & index)
{
  if ( m_HasDoubleClicked == true )
  {
    m_Model->NewPolygon(index);
    m_HasDoubleClicked = false;
  }
  else
    m_Model->AddPointToCurrentPolygon(index);
}



void
ObjectCountingController
::DoubleLeftClick(const IndexType & index)
{
  m_Model->AddPointToCurrentPolygon(index);
  // update refrence pixel value
  if ( m_View->cbUseSpectralAngle->value() == 1 )
    m_Model->ComputeReferencePixel();

  m_HasDoubleClicked = true;
}

void
ObjectCountingController
::DoubleRightClick(const IndexType & index)
{
  try
  {
    m_Model->ErasePolygon(index);
    m_HasDoubleClicked = true;
    // update refrence pixel value
    if ( m_View->cbUseSpectralAngle->value() == 1 )
    {
      m_Model->ComputeReferencePixel();
    }
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}



ObjectCountingController::IndexType
ObjectCountingController
::TransformIndex( const IndexType & index)
{
  ViewType::RegionType region = m_View->GetFullImageWidget()->GetBufferedRegion();
  IndexType newIndex;
  newIndex[0]=index[0]+m_Model->GetCurrentRegion().GetIndex()[0];
  newIndex[1]=index[1]+m_Model->GetCurrentRegion().GetIndex()[1];

  return newIndex;
}

void
ObjectCountingController
::UseSVM(bool b)
{
  m_Model->SetUseSVM(b);
  if (b==false)
  {
    // refenrece pixel
    m_Model->ComputeReferencePixel();
  }
}

void
ObjectCountingController
::UpdateThresholdValue( double val )
{
  m_Model->SetThresholdValue( static_cast<PixelType>(val) );
}


void
ObjectCountingController
::ComputeOverExtract()
{


  try
  {
    FltkFilterWatcher watcher(m_Model->GetRelabelFilter(),0,0,300,20, "Run over Extracted Image ...");
    m_Model->RunOverExtract();
  }
  catch (itk::ExceptionObject & err)
  {
    MsgReporter::GetInstance()->SendError(err.GetDescription());
  }
}

void
ObjectCountingController
::UpdateSpatialRadius(int val)
{
  m_Model->SetSpatialRadius(val);
}

void
ObjectCountingController
::UpdateRangeRadius(int val)
{
  m_Model->SetRangeRadius(val);
}

void
ObjectCountingController
::UpdateScale(int val)
{
  m_Model->SetScale(val);
}

void
ObjectCountingController
::UpdateNuParameter(double val)
{
  m_Model->SetNuParameter(val);
}

void
ObjectCountingController
::UpdateMinRegionSize(int val)
{
  m_Model->SetMinRegionSize(val);
}

void
ObjectCountingController
::SetOutputImageName(const char * name)
{
  m_Model->SetOutputImageName(name);
}

void
ObjectCountingController
::SetOutputVectorFileName(const char * name)
{
  m_Model->SetOutputVectorFileName(name);
}

void
ObjectCountingController
::UseSmoothing(bool b)
{
  m_Model->SetUseSmoothing(b);
  if (b == false)
  {
    m_Model->ComputeReferencePixel();
  }

}

} // end namespace otb
