/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIncludeWX.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:33:15 $
  Version:   $Revision: 1.3 $
  Authors:   Based on itkWindows.h (www.itk.org)
==========================================================================
  Copyright (c) 2001/2005 
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
