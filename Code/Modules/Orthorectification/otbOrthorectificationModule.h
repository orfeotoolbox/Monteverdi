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
#ifndef __otbOrthorectificationModule_h
#define __otbOrthorectificationModule_h

#include "otbModule.h"

// Orthorectification Application
#include "otbOrthorectification.h"
#include "otbImageToVectorImageCastFilter.h"

namespace otb
{
/** \class OrthorectificationModule
 *  \brief
 *
 *  \sa DataObjectWrapper, InputDataDescriptor, OutputDataDescriptor
 */

class ITK_EXPORT OrthorectificationModule
  : public Module, public ListenerBase
{
public:
  /** Standard class typedefs */
  typedef OrthorectificationModule             Self;
  typedef Module                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(OrthorectificationModule,Module);
  

  /** Accessor to the Orthorectification Instance */
  itkGetObjectMacro(Orthorectification,Orthorectification);
  
  /** Input typedef */
  typedef Orthorectification::ImageType       ImageType;
  typedef Orthorectification::SingleImageType SingleImageType;

  /** Cast SingleImage to ImageType (VectorImage) */
  typedef ImageToVectorImageCastFilter<SingleImageType,ImageType>     CastSingleImageFilter;

protected:
  /** Constructor */
  OrthorectificationModule();

  /** Destructor */
  virtual ~OrthorectificationModule();

  /** PrintSelf method */
  virtual void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** The custom run command */
  virtual void Run();

  /** Show the Module GUI */
  virtual bool CanShow(){return true;};
  
  /** Show the Module GUI */
  virtual void Show()
  {
    m_Orthorectification->guiMainWindow->show();
  };

  /** Show the Module GUI */
  virtual void Hide()
  {
    m_Orthorectification->guiShowDEM->hide();
    m_Orthorectification->guiMainWindow->hide();
  };

  /** Notify Monteverdi application that orthorectification has a result */
  void Notify();

private:
  OrthorectificationModule(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // The view
  Orthorectification::Pointer        m_Orthorectification;

  // Cast Filter
  CastSingleImageFilter::Pointer     m_CastFilter;

};


} // End namespace otb

#endif
