/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.cxx,v $
  Language:  C++
  Date:      $Date: 2004-11-09 15:31:00 $
  Version:   $Revision: 1.2 $
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
void mafLogMessage(const wxChar *format, ...)
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
void mafWarningMessage(const wxChar *format, ...)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_WX

#endif MAF_USE_WX
}

//------------------------------------------------------------------------------
// open an error dialog and write a message
void mafErrorMessage(const wxChar *format, ...)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_WX

#endif MAF_USE_WX
}

//------------------------------------------------------------------------------
// open a message dialog and write a message
void mafMessage(const wxChar *format, ...)
//------------------------------------------------------------------------------
{
#ifdef MAF_USE_WX

#endif MAF_USE_WX
}