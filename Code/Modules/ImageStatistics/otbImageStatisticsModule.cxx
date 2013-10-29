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
#include "otbImageStatisticsModule.h"

#include "otbFltkFilterWatcher.h"


namespace otb
{
/** Constructor */
ImageStatisticsModule::ImageStatisticsModule()
{
  // First, do constructor stuffs

  // Then, describe inputs needed by the module

  // Add a new input
  this->AddInputDescriptor<VectorImageType>("InputImage", otbGetTextMacro("Statistics for image"));
  this->AddTypeToInputDescriptor<ImageType>("InputImage");

  m_ImageToVectorImageFilter = ImageToVectorImageCastFilterType::New();
  m_StatisticsFilter = StatisticsImageFilterType::New();
  m_MinMaxFilter = MinMaxImageFilterType::New();
  m_InputImage = VectorImageType::New();
  m_FullStat = false;
}

/** Destructor */
ImageStatisticsModule::~ImageStatisticsModule()
{}

/** PrintSelf method */
void ImageStatisticsModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os, indent);
}

/** The custom run command */
void ImageStatisticsModule::Run()
{
       // While the viewer is shown, it is busy
    this->BusyOn();

        unsigned int nbOfChannels(1);
       SizeType imSize;
       imSize.Fill(0);

       // Init the list of inputs image
       m_InputImage = this->GetInputData<VectorImageType>("InputImage");
       if (m_InputImage.IsNull())
       {
              // Try to get a single image
        // If the input image is an otb::Image instead of VectorImage then
        // cast it in Vector Image and continue the processing
        ImageType::Pointer image = this->GetInputData<ImageType>("InputImage");

              if(image.IsNotNull() == true)
              {
                     m_ImageToVectorImageFilter = ImageToVectorImageCastFilterType::New();
                     // Cast image into vectorImage
                     m_ImageToVectorImageFilter->SetInput( image );
                     m_ImageToVectorImageFilter->UpdateOutputInformation();

                     m_InputImage = m_ImageToVectorImageFilter->GetOutput();
                     
                     // Compute nb of channels
                     nbOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();
                     // Compute image size
                     imSize = m_InputImage->GetLargestPossibleRegion().GetSize();
              }
              else
              {
                     this->Quit();
                     itkExceptionMacro(<< "The input image number " << 0 << " is Null.");
              }
       }

       m_InputImage->UpdateOutputInformation();
       // Compute nb of channels
       nbOfChannels = m_InputImage->GetNumberOfComponentsPerPixel();
       // Compute image size
       imSize = m_InputImage->GetLargestPossibleRegion().GetSize();

       m_MinMaxFilter = MinMaxImageFilterType::New();
       // Cast image into vectorImage
       m_MinMaxFilter->SetInput(m_InputImage);
       
       // Create GUI
       this->BuildGUI();
       pBusyBar->minimum(0);
       pBusyBar->maximum(1);

       this->Show();
       Fl::check();
       this->pBusyBar->value(1);
       this->pBusyBar->show();
       Fl::check();

       // Update interface
       this->vSizeX->value(imSize[0]);
       this->vSizeY->value(imSize[1]);
       this->vNbChannels->value(nbOfChannels);
       this->vNbPix->value(imSize[0]*imSize[1]);
       
       // Init list of channel
       vChannelId->range(1, nbOfChannels);
       this->vChannelId->value(1);

       // Compute statistics
       m_MinMaxFilter->Update();

       // Update inteface
       std::ostringstream oss;
       oss << m_MinMaxFilter->GetMinimum();
       this->vMin->value(oss.str().c_str());

       oss.str("");
       oss << m_MinMaxFilter->GetMaximum();
       this->vMax->value(oss.str().c_str());

       this->UpdateChannelStat();
       
       this->pBusyBar->value(0);
       this->pBusyBar->hide();
       Fl::check();
}

  void ImageStatisticsModule::UpdateChannelStat()
  {
       const unsigned int chId = static_cast<unsigned int>(vChannelId->value()-1);
       
       if(m_FullStat == true)
       {
              this->vMinCh->value(m_StatisticsFilter->GetMinimum()[chId]);
              this->vMaxCh->value(m_StatisticsFilter->GetMaximum()[chId]);
              this->vMeanCh->value(m_StatisticsFilter->GetMean()[chId]);
              this->vSumCh->value(m_StatisticsFilter->GetSum()[chId]);
       }
       else
       {
              this->vMinCh->value(m_MinMaxFilter->GetMinimum()[chId]);
              this->vMaxCh->value(m_MinMaxFilter->GetMaximum()[chId]);
       }
  }

  void ImageStatisticsModule::ComputeFullStat()
  {
       if(m_FullStat == false)
       {
              if (m_InputImage.IsNull())
              {
                     this->Quit();
                     itkExceptionMacro(<< "The input image number " << 0 << " is Null.");
              }
       
       m_StatisticsFilter = StatisticsImageFilterType::New();
       // Cast image into vectorImage
       m_StatisticsFilter->SetInput(m_InputImage);
       
       // Busy bar
       this->pBusyBar->value(1);
       this->pBusyBar->show();
       Fl::check();

       // Compute statistics
       m_StatisticsFilter->Update();

       // Update inteface
       std::ostringstream oss;
       oss << m_StatisticsFilter->GetMinimum();
       this->vMin->value(oss.str().c_str());

       oss.str("");
       oss << m_StatisticsFilter->GetMaximum();
       this->vMax->value(oss.str().c_str());

       oss.str("");
       oss << m_StatisticsFilter->GetMean();
       this->vMean->value(oss.str().c_str());
       this->vMean->activate();

       oss.str("");
       oss << m_StatisticsFilter->GetSum();
       this->vSum->value(oss.str().c_str());
       this->vSum->activate();

       this->vMeanCh->activate();
       this->vSumCh->activate();
       this->bCovMatrix->activate();

       m_FullStat = true;
       this->UpdateChannelStat();
       this->UpdateCovarianceText();
       
       // Hide busy bar
       this->pBusyBar->value(0);
       this->pBusyBar->hide();
       Fl::check();

       // deactivate the button -> can't compute twice
       this->bFullStat->deactivate();
       }
  }

  void ImageStatisticsModule::Quit()
  {
              // 1First, clear any previous output
              this->ClearOutputDescriptors();
              this->Hide();
              // Once module is closed, it is no longer busy
              this->BusyOff();
  }
  

 void ImageStatisticsModule::UpdateCovarianceText()
{
  std::ostringstream oss;
  
  const MatrixType & covMatrix = m_StatisticsFilter->GetCovariance();

  unsigned int start = 5;
  unsigned int height = 20;
  unsigned int width = 70;
  this->sCov->begin();
  for (unsigned int r=0; r<=covMatrix.Rows(); r++)
    {
              for (unsigned int c=0; c<=covMatrix.Cols(); c++)
              {
                     if( (r==0) && (c==0) )
                     {
                            Fl_Output * temp = new Fl_Output(start, start, height, height);
                            temp->box(FL_NO_BOX);
                            temp->labelcolor((Fl_Color)186);
                            temp->align(FL_ALIGN_CENTER);
                            temp->value("");
                     }
                     else if( (r==0) && (c!=0) )
                     {
                            oss.str("");
                            oss << c;
                            Fl_Output * temp = new Fl_Output(start+c*width-(width-height)-c, start, width, height);
                            temp->box(FL_NO_BOX);
                            temp->labelcolor((Fl_Color)186);
                            temp->align(FL_ALIGN_CENTER);
                            temp->value(oss.str().c_str());
                     }
                     else if( (r!=0) && (c==0) )
                     {
                            oss.str("");
                            oss << r;
                            Fl_Output * temp = new Fl_Output(start, start+r*height-r, height, height);
                            temp->box(FL_NO_BOX);
                            temp->labelcolor((Fl_Color)186);
                            temp->align(FL_ALIGN_CENTER);
                            temp->value(oss.str().c_str());
                     }
                     else if( (r!=0) && (c==1) )
                     {
                            oss.str("");
                            oss << covMatrix(r-1, c-1);
                            Fl_Output * temp = new Fl_Output(start+c*height-c, start+r*height-r, width, height);
                            temp->box(FL_BORDER_BOX);
                            temp->labelcolor((Fl_Color)186);
                            temp->align(FL_ALIGN_CENTER);
                            temp->value(oss.str().c_str());
                     }
                     else
                     {
                            oss.str("");
                            oss << covMatrix(r-1, c-1);
                            Fl_Output * temp = new Fl_Output(start+c*width-(width-height)-c, start+r*height-r, width, height);
                            temp->box(FL_BORDER_BOX);
                            temp->labelcolor((Fl_Color)186);
                            temp->align(FL_ALIGN_CENTER);
                            temp->value(oss.str().c_str());
                     }
              }
    }
 
       this->sCov->end();
}


} // End namespace otb
