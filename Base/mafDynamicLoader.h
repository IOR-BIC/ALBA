/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDynamicLoader.h,v $
  Language:  C++
  Date:      $Date: 2004-11-18 22:35:25 $
  Version:   $Revision: 1.3 $
  Authors:   Based on DynamicLoader (www.itk.org), adapted by Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafDynamicLoader_h
#define __mafDynamicLoader_h

#include "mafConfigure.h"

// Ugly stuff for library handles.
// They are different on several different OS's
#if defined(__hpux)

# include <dl.h>
typedef shl_t LibHandle;

#elif defined(_WIN32)

#include "mafWIN32.h"
typedef HMODULE LibHandle;

#elif defined(__powerpc)

typedef ConnectionID LibHandle;

#else

typedef void* LibHandle;

#endif

/** Portable loading of dynamic libraries or dll's.
  mafDynamicLoader provides a portable interface to loading dynamic 
  libraries or dll's into a process. Beware this class include mafWIN32.h 
  and thus should be included before any other not including this header file,
  since MS VS6.0 seems to have a bug which make macros and inline functions 
  defined before of this class to be mis-compiled.
  
 */
class MAF_EXPORT mafDynamicLoader
{
public:
  /** Load a dynamic library into the current process.
   * The returned LibHandle can be used to access the symbols in the 
   * library. */
  static LibHandle OpenLibrary(const char*);

  /** Attempt to detach a dynamic library from the
   * process.  A value of true is returned if it is sucessful. */
  static int CloseLibrary(LibHandle);
  
  /** Find the address of the symbol in the given library. */
  static void* GetSymbolAddress(LibHandle, const char*);

  /** Return the library prefix for the given architecture */
  static const char* LibPrefix();

  /** Return the library extension for the given architecture. */
  static const char* LibExtension();

  /** Return the last error produced from a calls made on this class. */
  static const char* LastError();
  
protected:
  mafDynamicLoader() {};
  ~mafDynamicLoader() {};
};
  
#endif
