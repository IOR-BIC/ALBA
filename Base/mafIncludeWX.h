/*=========================================================================

 Program: MAF2
 Module: mafIncludeWX
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/** This file is used to include wx.h */

#include "mafConfigure.h"   

#ifndef __mafIncludeWX_h
#define __mafIncludeWX_h

#ifdef MAF_USE_WX
  #ifdef WX_PRECOMP
      #include "wx/wxprec.h"  //originally was the first line of any lal cpp file 
  #else
      #include "wx/wx.h"      //originally was the first line of any lal header file 
  #endif
#endif

#endif
