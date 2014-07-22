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
#ifndef __otbEdgeExtractionViewGUI_h
#define __otbEdgeExtractionViewGUI_h

#include "otbFeatureExtractionBaseViewGUI.h"
#include "otbEdgeExtractionViewGroup.h"

namespace otb
{
/** \class EdgeExtractionViewGUI
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT EdgeExtractionViewGUI
  : public FeatureExtractionBaseViewGUI //, public EdgeExtractionViewGroup
{
public:
  /** Standard class typedefs */
  typedef EdgeExtractionViewGUI        Self;
  typedef FeatureExtractionBaseViewGUI        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(EdgeExtractionViewGUI, FeatureExtractionBaseViewGUI);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** Constructor */
  EdgeExtractionViewGUI();
  
  /** Update feature information with Radiometric indices features */
  virtual void UpdateFeatureInfo(FeatureType feat);
  
  /** Specific GUI for menu items and parameters widgets */
  EdgeExtractionViewGroup* m_SpecificGUI;

protected:
  /** Destructor */
  virtual ~EdgeExtractionViewGUI();
  
  /** Register parameters widgets */
  virtual void InitParameterGroupList();

private:

  /** Stores pairs : menu index - panel index (used for callbacks) */
  int m_FeatureTable[60];
  
  /** Generic callback for radiometric indices items */
  void GenericMenuItemCallback_i(Fl_Menu_* o, void* v);
  static void GenericMenuItemCallback(Fl_Menu_* o, void* v);

};

}

#endif
