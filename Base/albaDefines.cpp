/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDefines
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"
#include "albaMutexLock.h"

#include "wx/string.h"
#include "albaIncludeWIN32.h"

#include <stdio.h>
#include <math.h>

#ifndef WIN32
#include <unistd.h>  
#endif

#include <iostream>
using namespace std;

#define BUFFER_DIMENSION 4096

static albaMutexLock albaLogMutex; 
static char albaLogBuffer[BUFFER_DIMENSION];

//------------------------------------------------------------------------------
void albaLogMessage(const char *format, ...)
{
  albaLogMutex.Lock();

	ALBA_PRINT_MACRO(format,albaLogBuffer,sizeof(albaLogBuffer));
	
	wxString logStr = albaLogBuffer;
	
	//The print macro removes the double %% so we need to reintroduce them before call wxLogMessage
	logStr.Replace("%", "%%");
	logStr << "\n";
  wxLogMessage(logStr);
  
  albaLogMutex.Unlock();
}
//------------------------------------------------------------------------------
// open a warning dialog and write a message
void albaWarningMessage(const char *format, ...)
{
  albaLogMutex.Lock();
  ALBA_PRINT_MACRO(format,albaLogBuffer,sizeof(albaLogBuffer));

#ifdef ALBA_USE_WX
  wxMessageBox(albaLogBuffer,"Warning Message",wxOK|wxICON_WARNING);
#else
  cerr << "Warning: " << albaLogBuffer;    
#endif
  
  albaLogMutex.Unlock();
}

//------------------------------------------------------------------------------
// open an error dialog and write a message
void albaErrorMessage(const char *format, ...)
{
  albaLogMutex.Lock();
  ALBA_PRINT_MACRO(format,albaLogBuffer,sizeof(albaLogBuffer));

#ifdef ALBA_USE_WX
  wxMessageBox(albaLogBuffer,"Error Message",wxOK|wxICON_ERROR);
#else
  cerr << "Error:" << albaLogBuffer;
#endif
  
  albaLogMutex.Unlock();
}

//------------------------------------------------------------------------------
// open a message dialog and write a message
void albaMessage(const char *format, ...)
{
  albaLogMutex.Lock();
  ALBA_PRINT_MACRO(format,albaLogBuffer,sizeof(albaLogBuffer));

#ifdef ALBA_USE_WX
  wxMessageBox(albaLogBuffer,"Information",wxOK|wxICON_INFORMATION);
#else
  cerr << albaLogBuffer;
#endif
  
  albaLogMutex.Unlock();
}


//------------------------------------------------------------------------------
bool albaEquals(double x, double y)
{
   double diff=fabs(x - y);
   double max_err=fabs(x / pow((double)10,(double)15));
   if (diff > max_err)
     return false;
   return  ( diff <= max_err);
}

//------------------------------------------------------------------------------
bool albaFloatEquals(float x, float y)
{
   float diff=fabs(x - y);
   float max_err=fabs(x / pow((double)10,(double)7));
   return  ( diff <= max_err);
}
//------------------------------------------------------------------------------
double albaRoundToPrecision(double val, unsigned prec)
{
  double k = pow((double)10,(double)prec);
  return floor(val*k+0.5)/k; 
} 
//------------------------------------------------------------------------------
void albaSleep(int msec)
{
#ifdef WIN32
  Sleep(msec);
#else

  usleep(msec*1000);
#endif
}
//------------------------------------------------------------------------------
bool albaIsLittleEndian(void)
{ 
  /* 
   * Variables definition 
   */ 
  short magic, test; 
  char * ptr;    
  magic = 0xABCD;                     /* endianity test number */ 
  ptr = (char *) &magic;               
  test = (ptr[1]<<8) + (ptr[0]&0xFF); /* build value byte by byte */ 
  return (magic == test);             /* if the same is little endian */  
}


