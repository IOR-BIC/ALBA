/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-10 06:59:16 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#include <stdio.h>
#include <stdarg.h>
#include <varargs.h>

#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
void mafLogMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
  MAF_PRINT_MACRO(format);

#ifdef MAF_USE_WX
  wxLogMessage(msg);
#else MAF_USE_WX
  cerr<<"mafGlobalStringBuffer"    
#endif MAF_USE_WX
}
//------------------------------------------------------------------------------
// open a warning dialog and write a message
void mafWarningMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_WX

#endif MAF_USE_WX
}

//------------------------------------------------------------------------------
// open an error dialog and write a message
void mafErrorMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_WX

#endif MAF_USE_WX
}

//------------------------------------------------------------------------------
// open a message dialog and write a message
void mafMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_WX

#endif MAF_USE_WX
}