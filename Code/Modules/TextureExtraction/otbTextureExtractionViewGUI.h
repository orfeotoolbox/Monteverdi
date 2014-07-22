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
#ifndef __otbTextureExtractionViewGUI_h
#define __otbTextureExtractionViewGUI_h

#include "otbFeatureExtractionBaseViewGUI.h"
#include "otbTextureExtractionViewGroup.h"

namespace otb
{
/** \class TextureExtractionViewGUI
 *  \brief
 *
 *  \sa DataObjectWrapper, DataDescriptor, DataDescriptor
 */

class ITK_ABI_EXPORT TextureExtractionViewGUI
  : public FeatureExtractionBaseViewGUI //, public TextureExtractionViewGroup
{
public:
  /** Standard class typedefs */
  typedef TextureExtractionViewGUI        Self;
  typedef FeatureExtractionBaseViewGUI        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard type macro */
  itkTypeMacro(TextureExtractionViewGUI, FeatureExtractionBaseViewGUI);
  
  /** Standard type macro */
  itkNewMacro(Self);
  
  /** Constructor */
  TextureExtractionViewGUI();
  
  /** Update feature information with Radiometric indices features */
  virtual void UpdateFeatureInfo(FeatureType feat);
  
  /** Update channels as well as min/max values*/
  virtual void UpdateChannels();
  
  virtual void InitWidgets();
  
  /** Specific GUI for menu items and parameters widgets */
  TextureExtractionViewGroup* m_SpecificGUI;

protected:
  /** Destructor */
  virtual ~TextureExtractionViewGUI();
  
  /** Register parameters widgets */
  virtual void InitParameterGroupList();
  
  void InitTextureLists();
  
  /** Update Haralick min/max parameters */
  virtual void UpdateHarMinMax();
  
  /** Update Advanced Haralick min/max parameters */
  virtual void UpdateAdvHarMinMax();

private:

  /** Stores pairs : menu index - panel index (used for callbacks) */
  int m_FeatureTable[60];
  
  /** Generic callback for radiometric indices items */
  void GenericMenuItemCallback_i(Fl_Menu_* o, void* v);
  static void GenericMenuItemCallback(Fl_Menu_* o, void* v);
  
  /** Callbacks for Haralick min/max paramters */
  void HarCkCallback_i(Fl_Check_Button* o, void* v);
  static void HarCkCallback(Fl_Check_Button* o, void* v);
  
  void HarMinCallback_i(Fl_Value_Input* o, void* v);
  static void HarMinCallback(Fl_Value_Input* o, void* v);
  
  void HarMaxCallback_i(Fl_Value_Input* o, void* v);
  static void HarMaxCallback(Fl_Value_Input* o, void* v);
  
  /** Callbacks for Advanced Haralick min/max paramters */
  void AdvHarCkCallback_i(Fl_Check_Button* o, void* v);
  static void AdvHarCkCallback(Fl_Check_Button* o, void* v);
  
  void AdvHarMinCallback_i(Fl_Value_Input* o, void* v);
  static void AdvHarMinCallback(Fl_Value_Input* o, void* v);
  
  void AdvHarMaxCallback_i(Fl_Value_Input* o, void* v);
  static void AdvHarMaxCallback(Fl_Value_Input* o, void* v);
  
};

}

#endif
