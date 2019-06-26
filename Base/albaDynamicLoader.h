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
#ifndef __albaDynamicLoader_h
#define __albaDynamicLoader_h
#include "albaBase.h"
#include "albaConfigure.h"

// Ugly stuff for library handles.
// They are different on several different OS's
#if defined(__hpux)

# include <dl.h>
typedef shl_t LibHandle;

#elif defined(_WIN32)

#include "albaIncludeWIN32.h"
typedef HINSTANCE LibHandle;

#elif defined(__powerpc)

typedef ConnectionID LibHandle;

#else

typedef void* LibHandle;

#endif

/** Portable loading of dynamic libraries or dll's.
  albaDynamicLoader provides a portable interface to loading dynamic 
  libraries or dll's into a process. Beware this class include albaWIN32.h 
  and thus should be included before any other not including this header file,
  since MS VS6.0 seems to have a bug which make macros and inline functions 
  defined before of this class to be mis-compiled.
  
 */
class ALBA_EXPORT albaDynamicLoader : public albaBase
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
  albaDynamicLoader() {};
  ~albaDynamicLoader() {};
};
  
#endif

