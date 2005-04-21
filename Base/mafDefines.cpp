/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDefines.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:55:14 $
  Version:   $Revision: 1.12 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafDefines.h"
#include "mafMutexLock.h"

#include "mafIncludeWX.h"
#include "wx/string.h"

#include <math.h>

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
  wxMessageBox(mafLogBuffer,"Warning Message",wxOK|wxICON_WARNING);
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
  wxMessageBox(mafLogBuffer,"Error Message",wxOK|wxICON_ERROR);
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
  wxMessageBox(mafLogBuffer,"Information",wxOK|wxICON_INFORMATION);
#else
  cerr << mafLogBuffer;
#endif
  
  mafLogMutex.Unlock();
}


//------------------------------------------------------------------------------
bool mafEquals(double x, double y)
//------------------------------------------------------------------------------
{
   double diff=fabs(x - y);
   double max_err=fabs(x / pow((double)10,(double)15));
   if (diff > max_err)
     return false;
   return  ( diff <= max_err);
}

//------------------------------------------------------------------------------
bool mafFloatEquals(float x, float y)
//------------------------------------------------------------------------------
{
   float diff=fabs(x - y);
   float max_err=fabs(x / pow((double)10,(double)7));
   return  ( diff <= max_err);
}
