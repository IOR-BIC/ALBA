/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIncludeWX.h,v $
  Language:  C++
  Date:      $Date: 2005-01-14 18:24:21 $
  Version:   $Revision: 1.1 $
  Authors:   Based on itkWindows.h (www.itk.org)
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
/** This file is used to include wx.h */

#include "mafConfigure.h"

#ifndef __mafIncludeWX_h
#define __mafIncludeWX_h

#ifdef MAF_USE_WX
//  #ifndef WX_PRECOMP
    #include "wx/wx.h"
//  #endif
#endif

#endif 