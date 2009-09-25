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
#ifndef __otbWriterController_h
#define __otbWriterController_h

#include "otbWriterControllerInterface.h"
#include "otbWriterView.h"

namespace otb
{
/** \class WriterController
 *  \brief 
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_EXPORT WriterController
  : public WriterControllerInterface
{
public:
  /** Standard class typedefs */
  typedef WriterController                 Self;
  typedef WriterControllerInterface                        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** New macro */
  itkNewMacro(Self);

  /** Type macro */
  itkTypeMacro(WriterController,Module);

  typedef WriterView          ViewType;
  typedef ViewType::Pointer                 ViewPointerType;

  virtual void OpenInputImage(const char * filename);
//   virtual void SaveOutput();
//   virtual void InitInput();
//   virtual void ClearFeatures();
//   virtual void CreateFeature(FeatureType featureType);
//   virtual void ClearSelectedChannels();
//   virtual void SetOutputFileName( const char * name );
//   virtual void AddInputChannels(std::vector<unsigned int> list);
//   virtual void ChangeFilterStatus(int id);
//   virtual void ViewedRegionChanged();
//   virtual void AddToOutputListOrder(int id);
//   virtual void RemoveFromOutputListOrder(int id);
//   virtual void ExchangeOutputListOrder( int direction );
  
  void SetView(ViewPointerType pView)
  {
    m_View = pView;
  }
protected:
  /** Constructor */
  WriterController();
  /** Destructor */
  virtual ~WriterController();
  

  
  
private:
  WriterController(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** Pointer to the view */
  ViewPointerType m_View;
};


} // End namespace otb

#endif
