/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.cpp,v $
  Language:  C++
  Date:      $Date: 2004-11-18 22:35:24 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"
#include "mafMutexLock.h"

#ifdef MAF_USE_WX
  #include "wx/wx.h"
  #include "wx/string.h"
#endif

#include <iostream>
using namespace std;

static mafMutexLock mafLogMutex; 
static char mafLogBuffer[2048];

//------------------------------------------------------------------------------
void mafLogMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
  
  mafLogMutex.Lock();
  MAF_PRINT_MACRO(format,mafLogBuffer,sizeof(mafLogBuffer));

#ifdef MAF_USE_WX
  wxLogMessage(mafLogBuffer);
#else MAF_USE_WX
  cerr << mafLogBuffer;    
#endif MAF_USE_WX
  
  mafLogMutex.Unlock();
}
//------------------------------------------------------------------------------
// open a warning dialog and write a message
void mafWarningMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
  mafLogMutex.Lock();
  MAF_PRINT_MACRO(format,mafLogBuffer,sizeof(mafLogBuffer));

#ifdef MAF_USE_WX
  //wxMessage(mafLogBuffer);
#else MAF_USE_WX
  cerr << "Warning: " << mafLogBuffer;    
#endif MAF_USE_WX
  
  mafLogMutex.Unlock();
}

//------------------------------------------------------------------------------
// open an error dialog and write a message
void mafErrorMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
  mafLogMutex.Lock();
  MAF_PRINT_MACRO(format,mafLogBuffer,sizeof(mafLogBuffer));

#ifdef MAF_USE_WX
  //wxLogMessage(mafLogBuffer);
#else MAF_USE_WX
  cerr << "Error:" << mafLogBuffer;
#endif MAF_USE_WX
  
  mafLogMutex.Unlock();
}

//------------------------------------------------------------------------------
// open a message dialog and write a message
void mafMessage(const char *format, ...)
//------------------------------------------------------------------------------
{
  mafLogMutex.Lock();
  MAF_PRINT_MACRO(format,mafLogBuffer,sizeof(mafLogBuffer));

#ifdef MAF_USE_WX
  //wxLogMessage(mafLogBuffer);
#else MAF_USE_WX
  cerr << mafLogBuffer;
#endif MAF_USE_WX
  
  mafLogMutex.Unlock();
}