/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafIncludeWIN32.h,v $
  Language:  C++
  Date:      $Date: 2005-01-14 18:24:21 $
  Version:   $Revision: 1.1 $
  Authors:   Based on itkWindows.h (www.itk.org)
==========================================================================
  Copyright (c) 2002/2004 
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