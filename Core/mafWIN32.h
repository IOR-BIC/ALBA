/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafWIN32.h,v $
  Language:  C++
  Date:      $Date: 2004-11-25 11:29:36 $
  Version:   $Revision: 1.1 $
  Authors:   Based on itkWindows.h (www.itk.org)
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
/** This file is used to create the smallest windows.h possible.
 * Also it removes a few annoying #define's in windows.h. */
#ifndef __mafWIN32_h
#define __mafWIN32_h
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#endif

