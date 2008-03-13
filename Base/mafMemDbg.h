//////////////////////////////////////////////////////////////////////////
// This file enables memory leaking detection
// Include it in your .CPP or .C or .CXX files.
// Note: If you include this file before including headers that
// redefine new operators, you will not be able to compile the project

#ifndef __MemDbg_h__
#define __MemDbg_h__


#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW 
#endif

#endif