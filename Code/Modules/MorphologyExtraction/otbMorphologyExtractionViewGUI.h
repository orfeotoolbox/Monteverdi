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
#ifndef __otbMorphologyExtractionViewGUI_h
#define __otbMorphologyExtractionViewGUI_h

#include "otbFeatureExtractionBaseViewGUI.h"
#include "otbMorphologyExtractionViewGroup.h"

namespace otb
{
/** \class MorphologyExtractionViewGUI
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT MorphologyExtractionViewGUI
  : public FeatureExtractionBaseViewGUI //, public MorphologyExtractionViewGroup
{
public:
  /** Standard class typedefs */
  typedef MorphologyExtractionViewGUI        Self;
  typedef FeatureExtractionBaseViewGUI        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(MorphologyExtractionViewGUI, FeatureExtractionBaseViewGUI);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** Constructor */
  MorphologyExtractionViewGUI();
  
  /** Update feature information with Radiometric indices features */
  virtual void UpdateFeatureInfo(FeatureType feat);
  
  /** Specific GUI for menu items and parameters widgets */
  MorphologyExtractionViewGroup* m_SpecificGUI;

protected:
  /** Destructor */
  virtual ~MorphologyExtractionViewGUI();
  
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
