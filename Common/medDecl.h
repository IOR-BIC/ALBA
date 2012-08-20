/*=========================================================================

 Program: MAF2Medical
 Module: medDecl
 Authors: Matteo Giacomoni
 
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


enum MED_MAIN_EVENT_ID
{
  ID_VME_BEHAVIOR_UPDATE = EVT_USER_START,
  MED_EVT_USER_START,
};
enum MED_MENU_EVENT_ID
{
  MED_MENU_USER_START = MENU_USER_START,
};

#endif