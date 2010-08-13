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
#include "otbObjectLabelingModule.h"

namespace otb
{
/** Constructor */
ObjectLabelingModule::ObjectLabelingModule()
{
  // This module needs pipeline locking
  this->NeedsPipelineLockingOff();

  // First, do constructor stuffs
  m_Controller = ControllerType::New();
  m_View = ViewType::New();
  m_Model = ModelType::New();

  m_Caster = CasterFilterType::New();

  m_View->SetController(m_Controller);
  m_View->SetModel(m_Model);
  m_View->SetWidgetsController(m_Controller->GetWidgetsController());

  m_Controller->SetModel(m_Model);
  m_Controller->SetView(m_View);

  m_Model->RegisterListener(this);

  // Then, describe inputs needed by the module
  this->AddInputDescriptor<VectorImageType>("InputImage","The image to classify (huge image not supported yet)");
  this->AddInputDescriptor<ImageType>("LabeledImage","The segmentation of the image (huge image not supported yet)");
  this->AddTypeToInputDescriptor<LabeledImageType>("LabeledImage");


}

/** Destructor */
ObjectLabelingModule::~ObjectLabelingModule()
{}

/** PrintSelf method */
void ObjectLabelingModule::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  // Call superclass implementation
  Superclass::PrintSelf(os,indent);
}


/** The custom run command */
void ObjectLabelingModule::Run()
{
  // Untill window closing, module will be busy
  this->BusyOn();
  
   VectorImageType::Pointer fpvImage = this->GetInputData<VectorImageType>("InputImage");
   ImageType::Pointer lImage;
   LabeledImageType::Pointer lImageLab = this->GetInputData<LabeledImageType>("LabeledImage");
   
   if(lImageLab.IsNotNull())
     {
       m_Caster->SetInput( lImageLab );
       lImage = m_Caster->GetOutput();
     }
   else
     {
       lImage = this->GetInputData<ImageType>("LabeledImage");
     }

 
   if(fpvImage.IsNotNull() && lImage.IsNotNull())
     {
     // Process the input as an FloatingVectorImageType
    m_View->Build();
     fpvImage->UpdateOutputInformation();
     lImage->UpdateOutputInformation();
     m_Controller->OpenImage(fpvImage,lImage);
     m_View->UpdateViewerSetup();
     }
   else
     {
       itkExceptionMacro(<<"One of the input image is null");
     }
}

/** The Notify */
void ObjectLabelingModule::Notify(const std::string & event)
{

   if (event == "OutputsUpdated")
     {
     this->ClearOutputDescriptors();
     this->AddOutputDescriptor(m_Model->GetLabeledOutput(),"LabeledOutput","The classes labels image");
     this->AddOutputDescriptor(m_Model->GetColoredOutput(),"ColoredOutput","The classes colors image");
     this->NotifyOutputsChange();
     }
   else if(event == "Quit")
     {
     // Once module is closed, it is no longer busy
     this->BusyOff();
     }
}
} // End namespace otb

