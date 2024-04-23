/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDynamicLoader
 Authors: Based on DynamicLoader (www.itk.org), adapted by Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDynamicLoader.h"

// This file is actually 3 different implementations.
// 1. HP machines which uses shl_load
// 2. Power PC MAC which uses GetSharedLibrary
// 3. Windows which uses LoadLibrary
// 4. Most unix systems which use dlopen (default )
// Each part of the ifdef contains a complete implementation for
// the static methods of albaDynamicLoader.  


//----------------------------------------------------------------------------
// 1. Implementation for HPUX  machines
#ifdef __hpux
#define ALBADYNAMICLOADER_DEFINED 1
#include <dl.h>

//----------------------------------------------------------------------------
LibHandle albaDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  return shl_load(libname, BIND_DEFERRED | DYNAMIC_PATH, 0L);
}

//----------------------------------------------------------------------------
int albaDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  return 0;
}

//----------------------------------------------------------------------------
void* albaDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  void* addr;
  int status;
  
  status = shl_findsym (&lib, sym, TYPE_PROCEDURE, &addr);
  return (status < 0) ? (void*)0 : addr;
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "lib";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".sl";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  return 0;
}

#endif


//----------------------------------------------------------------------------
// 2. Implementation for the Power PC (MAC)
#ifdef __APPLE__
#define ALBADYNAMICLOADER_DEFINED 
#include <mach-o/dyld.h>

//----------------------------------------------------------------------------
LibHandle albaDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  NSObjectFileImageReturnCode rc;
  NSObjectFileImage image;
  
  rc = NSCreateObjectFileImageFromFile(libname, &image);
  return NSLinkModule(image, libname, true);
}

//----------------------------------------------------------------------------
int albaDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  return 0;
}

//----------------------------------------------------------------------------
void* albaDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  void *result=0;
  if(NSIsSymbolNameDefined(sym))
    {
    NSSymbol symbol= NSLookupAndBindSymbol(sym);
    if(symbol)
      {
      result = NSAddressOfSymbol(symbol);
      }
    }
  return result;
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".dylib";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  return 0;
}

#endif

//----------------------------------------------------------------------------
// 3. Implementation for Windows win32 code
#ifdef _WIN32
#include "albaIncludeWIN32.h"
#define ALBADYNAMICLOADER_DEFINED 1
  
//----------------------------------------------------------------------------
LibHandle albaDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  return LoadLibraryA(libname);
}

//----------------------------------------------------------------------------
int albaDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  return (int)FreeLibrary(lib);
}

//----------------------------------------------------------------------------
void* albaDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  return (void *)GetProcAddress(lib, sym);
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".dll";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  LPVOID lpMsgBuf;

  FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL 
    );
  
  static char* str = 0;
  delete [] str;
  str = strcpy(new char[strlen((char*)lpMsgBuf)+1], (char*)lpMsgBuf);
  // Free the buffer.
  LocalFree( lpMsgBuf );
  return str;
}

#endif

// ---------------------------------------------------------------
// 4. Implementation for default UNIX machines.
// if nothing has been defined then use this
#ifndef ALBADYNAMICLOADER_DEFINED
#define ALBADYNAMICLOADER_DEFINED
// Setup for most unix machines
#include <dlfcn.h>
  
//----------------------------------------------------------------------------
LibHandle albaDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  return dlopen(libname, RTLD_LAZY);
}

//----------------------------------------------------------------------------
int albaDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  if (lib)
  {
    return (int)dlclose(lib);
  }
  else
  {
    return 0;
  }
}

//----------------------------------------------------------------------------
void* albaDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  return dlsym(lib, sym);
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "lib";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".so";
}

//----------------------------------------------------------------------------
const char* albaDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  return dlerror(); 
}

#endif
