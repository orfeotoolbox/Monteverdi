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
#ifndef __otbEnum_h
#define __otbEnum_h

typedef enum {
  OTB_FLU_HILIGHTED,
  OTB_FLU_UNHILIGHTED,
  OTB_FLU_SELECTED,
  OTB_FLU_UNSELECTED,
  OTB_FLU_OPENED,
  OTB_FLU_CLOSED,
  OTB_FLU_DOUBLE_CLICK,
  OTB_FLU_WIDGET_CALLBACK,
  OTB_FLU_MOVED_NODE,
  OTB_FLU_NEW_NODE,
  OTB_FLU_NOTHING,
  OTB_FLU_RIGHT_LEFT_RELEASED,
  OTB_FLU_RIGHT_LEFT_PUSHED,
  OTB_FLU_RIGHT_MOUSE_RELEASED,
  OTB_FLU_RIGHT_MOUSE_PUSHED
} TreeCallBackEnumType;

typedef enum{ 
    NO_MODULE_OUTPUT, 
    SHOW_MODULE, 
    RENAME_MODULE } ModuleMenuOutputEnumType;

typedef enum{ 
  NO_OUTPUT,  
  RENAME_OUTPUT,
  DISPLAY_OUTPUT,
  CACHE_OUTPUT,
  WRITE_OUTPUT} OutputMenuOutputEnumType;

#endif
