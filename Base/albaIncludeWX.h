/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaIncludeWX
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/** This file is used to include wx.h */

#include "albaConfigure.h"   

#ifndef __albaIncludeWX_h
#define __albaIncludeWX_h

#ifdef ALBA_USE_WX
  #ifdef WX_PRECOMP
      #include "wx/wxprec.h"  //originally was the first line of any lal cpp file 
  #else
      #include "wx/wx.h"      //originally was the first line of any lal header file 
  #endif
#endif

#endif
