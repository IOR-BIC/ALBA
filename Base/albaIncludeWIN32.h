/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaIncludeWIN32
 Authors: Based on itkWindows.h (www.itk.org)
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/** This file is used to create the smallest windows.h possible.
 * Also it removes a few annoying #define's in windows.h. */
#include "albaConfigure.h"

#ifndef __albaIncludeWIN32_h
#define __albaIncludeWIN32_h

// include first wxWidgets if present
#ifdef ALBA_USE_WX
#include "albaIncludeWX.h"
#endif
  
#ifndef NOMINMAX
  #define NOMINMAX
#endif

#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <winbase.h>
#endif

#endif
