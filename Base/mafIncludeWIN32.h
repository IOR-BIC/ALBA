/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIncludeWIN32.h,v $
  Language:  C++
  Date:      $Date: 2005-02-20 23:33:15 $
  Version:   $Revision: 1.2 $
  Authors:   Based on itkWindows.h (www.itk.org)
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
/** This file is used to create the smallest windows.h possible.
 * Also it removes a few annoying #define's in windows.h. */
#include "mafConfigure.h"

#ifndef __mafIncludeWIN32_h
#define __mafIncludeWIN32_h

// include first wxWidgets if present
#ifdef MAF_USE_WX
#include "mafIncludeWX.h"
#endif
  
#ifndef NOMINMAX
  #define NOMINMAX
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>

#endif