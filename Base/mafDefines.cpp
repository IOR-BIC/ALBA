/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.cpp,v $
  Language:  C++
  Date:      $Date: 2005-01-11 17:34:56 $
  Version:   $Revision: 1.7 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#ifdef MAF_USE_WX
  #include "wx/wx.h"
  #include "wx/string.h"
#endif

#include "mafMutexLock.h"

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
#else
  cerr << mafLogBuffer;    
#endif
  
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
#else
  cerr << "Warning: " << mafLogBuffer;    
#endif
  
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
#else
  cerr << "Error:" << mafLogBuffer;
#endif
  
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
#else
  cerr << mafLogBuffer;
#endif
  
  mafLogMutex.Unlock();
}
