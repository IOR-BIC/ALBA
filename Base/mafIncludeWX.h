/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIncludeWX.h,v $
  Language:  C++
  Date:      $Date: 2005-03-23 18:01:52 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
