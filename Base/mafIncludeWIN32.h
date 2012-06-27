/*=========================================================================

 Program: MAF2
 Module: mafIncludeWIN32
 Authors: Based on itkWindows.h (www.itk.org)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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

#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <winbase.h>
#endif

#endif
