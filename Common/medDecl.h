/*=========================================================================

 Program: MAF2Medical
 Module: medDecl
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medDecl_H__
#define __medDecl_H__

#include "mafDefines.h"
#include "mafDecl.h"

const int MAXWIZARD = 50; 

enum MED_MAIN_EVENT_ID
{
  ID_VME_BEHAVIOR_UPDATE = EVT_USER_START,
  WIZARD_RUN_STARTING,
  WIZARD_RUN_TERMINATED,
  WIZARD_REQUIRED_VIEW,
  WIZARD_RUN_OP,
  WIZARD_UPDATE_WINDOW_TITLE,
  WIZARD_PAUSE,
  WIZARD_OP_DELETE,
  WIZARD_OP_NEW,
  WIZARD_SWITCH,
  WIZARD_INFORMATION_BOX_SHOW_SET,
  WIZARD_INFORMATION_BOX_SHOW_GET,
  WIZARD_START,
  WIZARD_END=WIZARD_START+MAXWIZARD,
  MED_EVT_USER_START,
};
enum MED_MENU_EVENT_ID
{
   MENU_WIZARD = MENU_USER_START,
   MED_MENU_USER_START,
};

#endif