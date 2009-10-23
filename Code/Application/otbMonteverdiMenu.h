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
#ifndef __otbMonteverdiMenu_h
#define __otbMonteverdiMenu_h

#include "otbMonteverdiEnum.h"
#include "itkObject.h"
#include <FL/Fl_Menu.H>
#include <FL/Fl_Menu_Bar.H>


namespace otb
{

class ITK_EXPORT MonteverdiMenu : public Fl_Menu_Bar
{
 public:
  /** Standard class typedefs */
  typedef MonteverdiMenu Self;
  typedef Fl_Menu_Bar    Superclass;

  //typedef MonteverdiEnumenum{ NO_OUTPUT, SHOW, RENAME } ModuleMenuOutputEnumType;

  /** Constructor */
  MonteverdiMenu(int x, int y, int w, int h) : Fl_Menu_Bar(x, y, w, h)
    {
      // this will contains the style of the popup menu
      this->box(FL_PLASTIC_DOWN_BOX);
      this->color((Fl_Color)23);
      this->labelcolor((Fl_Color)186);

      m_ModuleMenuOutput = NO_MODULE_OUTPUT;
      m_OutputMenuOutput = NO_OUTPUT;
    };
  
  /** Destructor */
  virtual ~MonteverdiMenu() {};
  
  /** Module menu callbacks */
  static void handle_menu_module_show(Fl_Widget *w, void *v)
    {
      MonteverdiMenu * pThis = (MonteverdiMenu *)(w);
      pThis->SetModuleMenuOutput(SHOW_MODULE);
    }
  static void handle_menu_module_rename(Fl_Widget *w, void *v)
    {
      MonteverdiMenu * pThis = (MonteverdiMenu *)(w);
      pThis->SetModuleMenuOutput(RENAME_MODULE);
    }
//     static void handle_menu_module_open(Fl_Widget *w, void *v)
//     {
//       std::cout << "Open module...remove this" << std::endl;
//       
//     }
  /** Output menu callbacks */
  static void handle_menu_output_rename(Fl_Widget *w, void *v)
    {
      MonteverdiMenu * pThis = (MonteverdiMenu *)(w);
      pThis->SetOutputMenuOutput(RENAME_OUTPUT);
    }

   static void handle_menu_output_display(Fl_Widget *w, void *v)
    {
      MonteverdiMenu * pThis = (MonteverdiMenu *)(w);
      pThis->SetOutputMenuOutput(DISPLAY_OUTPUT);
    }

  static void handle_menu_output_cache(Fl_Widget *w, void *v)
  {
    MonteverdiMenu * pThis = (MonteverdiMenu *)(w);
    pThis->SetOutputMenuOutput(CACHE_OUTPUT);
  }

  static void handle_menu_output_write(Fl_Widget *w, void *v)
  {
    MonteverdiMenu * pThis = (MonteverdiMenu *)(w);
    pThis->SetOutputMenuOutput(WRITE_OUTPUT);
  }
  

  void LaunchModuleMenu(bool show = true)
    {
      const unsigned menuSize = 1 + show;
      Fl_Menu_Item popup_menu[menuSize];
      
      popup_menu[0] = (Fl_Menu_Item){ "Rename",  0, this->handle_menu_module_rename, 0, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK };	
      
      unsigned int idx = 1;
      //Add Re Open menu
      if (show)
      {
        popup_menu[idx] = (Fl_Menu_Item) { "Show Module",  0, this->handle_menu_module_show, 0, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK };	
        ++idx;
      }
      
      popup_menu[idx] = (Fl_Menu_Item){0};
       
      int x, y;
      Fl::get_mouse(x, y);
      const Fl_Menu_Item *m = popup_menu->popup(x, y, 0, 0, this);
      if ( m ) 
	m->do_callback(this, m->user_data());
      
      return;
    }

  void LaunchOutputMenu(bool display = false, bool cache = false, bool write = false)
    {
      const unsigned menuSize = 2 + display + cache + write;
      Fl_Menu_Item popup_menu[menuSize];
      
      popup_menu[0] = (Fl_Menu_Item){ "Rename", 0, this->handle_menu_output_rename, 0, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK };
      
      unsigned int idx = 1;

      if(display)
	{
	popup_menu[idx] = (Fl_Menu_Item){ "Display in viewer", 0, this->handle_menu_output_display, 0, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK };
	++idx;
	}
      if(cache)
	{
	popup_menu[idx] = (Fl_Menu_Item){ "Export dataset", 0, this->handle_menu_output_write, 0, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK };
	++idx;
	}
      if(write)
	{
	popup_menu[idx] = (Fl_Menu_Item){ "Cache dataset", 0, this->handle_menu_output_cache, 0, 0, FL_NORMAL_LABEL, FL_HELVETICA, 12, FL_BLACK };
	++idx;
	}
      popup_menu[idx] = (Fl_Menu_Item){0};
 
      int x, y;
      Fl::get_mouse(x, y);
      const Fl_Menu_Item *m = popup_menu->popup(x, y, 0, 0, this);
      if ( m ) 
	m->do_callback(this, m->user_data());
      
      return;
    }
  
  /** Set/Get m_ModuleMenuOutput */
  ModuleMenuOutputEnumType GetModuleMenuOutput() const {
      return m_ModuleMenuOutput; }
  void SetModuleMenuOutput(ModuleMenuOutputEnumType val) {
      m_ModuleMenuOutput = val; }

    /** Set/Get m_OutputModuleMenuOutput */
  OutputMenuOutputEnumType GetOutputMenuOutput() const {
      return m_OutputMenuOutput; }
  void SetOutputMenuOutput(OutputMenuOutputEnumType val) {
    m_OutputMenuOutput = val;  }

protected:

private:
  ModuleMenuOutputEnumType m_ModuleMenuOutput;
  OutputMenuOutputEnumType m_OutputMenuOutput;
};

}
#endif


