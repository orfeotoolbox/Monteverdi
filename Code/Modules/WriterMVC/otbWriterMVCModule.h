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
#ifndef __otbWriterMVCModule_h
#define __otbWriterMVCModule_h

// include the base class
#include "otbModule.h"

// include the OTB elements
#include "otbVectorImage.h"
#include "otbImageFileWriter.h"

// #include "otbVectorData.h"
// #include "otbVectorDataFileWriter.h"

#include "otbImage.h"
#include "otbImageList.h"
#include "otbVectorImageToImageListFilter.h"
#include "otbImageListToVectorImageFilter.h"

#include "otbWriterController.h"
#include "otbWriterModel.h"
#include "otbWriterViewGUI.h"

// #include "otbListenerBase.h"
#include "otbEventsListener.h"

namespace otb
{
/** \class WriterMVCModule
   *  \brief
   *
   *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

  class ITK_EXPORT WriterMVCModule
  : public Module, public EventsListener<std::string>
  {
    public:
      /** Standard class typedefs */
      typedef WriterMVCModule                   Self;
      typedef Module                            Superclass;
      typedef itk::SmartPointer<Self>           Pointer;
      typedef itk::SmartPointer<const Self>     ConstPointer;

      /** New macro */
      itkNewMacro(Self);

      /** Type macro */
      itkTypeMacro(WriterMVCModule,Module);

      /** OTB typedefs */
      /// Dataset
      typedef WriterModel::InputImageType       FloatingVectorImageType;
      typedef FloatingVectorImageType::Pointer  FPVImagePointerType;
    
      itkGetObjectMacro(View,WriterViewGUI);

    protected:
      /** Constructor */
      WriterMVCModule();
      /** Destructor */
      virtual ~WriterMVCModule();
      /** PrintSelf method */
      virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;
      
      virtual void ThreadedRun();
      virtual void ThreadedWatch();
      
      // Update the progress bar
      void UpdateProgress();
      
      /** The custom run command */
      virtual void Run();
      
      /** Notify Monteverdi application that Writer has a result */
      void Notify(const std::string & event);
    private:
      WriterMVCModule(const Self&); //purposely not implemented
      void operator=(const Self&); //purposely not implemented
      
      // The view
      WriterViewGUI::Pointer        m_View;
      // The controller
      WriterController::Pointer  m_Controller;
      // The model
      WriterModel::Pointer       m_Model;
      
       // Pointer to the process object
      itk::ProcessObject::Pointer m_ProcessObject;
  };


} // End namespace otb

#endif
