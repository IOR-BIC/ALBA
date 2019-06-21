/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDynamicLoaderTest
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "albaDynamicLoaderTest.h"
#include "albaDynamicLoader.h"
#include "albaString.h"

#include <iostream>
#define TEST_RESULT CPPUNIT_ASSERT(result);

#ifdef _WIN64
#define DLL_FILENAME "DLLTest64.dll"
#else
#define DLL_FILENAME "DLLTest.dll"
#endif

//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestOpenLibrary()
//----------------------------------------------------------------------------
{
#ifdef __hpux
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define ALBADYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	albaString dllPath = ALBA_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = albaDynamicLoader::OpenLibrary(dllPath);

	result = lHandle != NULL;
	TEST_RESULT;

	albaDynamicLoader::CloseLibrary(lHandle);
#endif

#ifndef ALBADYNAMICLOADER_DEFINED_TEST
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif
  
}
//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestCloseLibrary()
//----------------------------------------------------------------------------
{
#ifdef __hpux
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define ALBADYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	albaString dllPath = ALBA_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = albaDynamicLoader::OpenLibrary(dllPath);

	int exit = albaDynamicLoader::CloseLibrary(lHandle);

	/*
	Return Value
	If the function succeeds, the return value is nonzero.
	*/

	result = exit;
	TEST_RESULT;
#endif

#ifndef ALBADYNAMICLOADER_DEFINED_TEST
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif
}
//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestGetSymbolAddress()
//----------------------------------------------------------------------------
{
#ifdef __hpux
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define ALBADYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	albaString dllPath = ALBA_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = albaDynamicLoader::OpenLibrary(dllPath);

	void *pointer = NULL;
	pointer = (void *)albaDynamicLoader::GetSymbolAddress(lHandle, "AddNums");

	result = pointer != NULL;
	printf("\nADDRESS : %d\n", pointer);
	TEST_RESULT;

	albaDynamicLoader::CloseLibrary(lHandle);
#endif

#ifndef ALBADYNAMICLOADER_DEFINED_TEST
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif
}

//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestCallFunction()
{
#ifdef __hpux
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define ALBADYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	albaString dllPath = ALBA_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = albaDynamicLoader::OpenLibrary(dllPath);

	typedef int(*SUMFUNC)(int, int);
	SUMFUNC pointer = NULL;
	pointer = (SUMFUNC) albaDynamicLoader::GetSymbolAddress(lHandle, "AddNums");

	CPPUNIT_ASSERT(pointer != NULL);
	
	int res;
	res = pointer(5, 3);

	CPPUNIT_ASSERT(res == 8);
	
	albaDynamicLoader::CloseLibrary(lHandle);
#endif

#ifndef ALBADYNAMICLOADER_DEFINED_TEST
#define ALBADYNAMICLOADER_DEFINED_TEST 1

#endif
}

//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestLibPrefix()
//----------------------------------------------------------------------------
{
  
	#ifdef __hpux
	#define ALBADYNAMICLOADER_DEFINED_TEST 1
	result = albaString(albaDynamicLoader::LibPrefix()).Equals("lib");
	TEST_RESULT;
	#endif

	#ifdef __APPLE__
	#define ALBADYNAMICLOADER_DEFINED_TEST 1
	result = albaString(albaDynamicLoader::LibPrefix()).Equals("");
	TEST_RESULT;
	#endif

	#ifdef _WIN32
	#define ALBADYNAMICLOADER_DEFINED_TEST 1
	result = albaString(albaDynamicLoader::LibPrefix()).Equals("");
	TEST_RESULT;
	#endif

	#ifndef ALBADYNAMICLOADER_DEFINED_TEST
	#define ALBADYNAMICLOADER_DEFINED_TEST 1
	result = albaString(albaDynamicLoader::LibPrefix()).Equals("lib");
	TEST_RESULT;
	#endif
}
//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestLibExtension()
//----------------------------------------------------------------------------
{
	#ifdef __hpux
  #define ALBADYNAMICLOADER_DEFINED_TEST 1
	result = albaString(albaDynamicLoader::LibExtension()).Equals(".sl");
	TEST_RESULT;
  #endif

	#ifdef __APPLE__
	#define ALBADYNAMICLOADER_DEFINED_TEST 1
	result = albaString(albaDynamicLoader::LibExtension()).Equals(".dylib");
	TEST_RESULT;
	#endif
	
  #ifdef _WIN32
	#define ALBADYNAMICLOADER_DEFINED_TEST 1
	result = albaString(albaDynamicLoader::LibExtension()).Equals(".dll");
	TEST_RESULT;
  #endif

  #ifndef ALBADYNAMICLOADER_DEFINED_TEST
	result = albaString(albaDynamicLoader::LibExtension()).Equals(".so");
	TEST_RESULT;
	#endif
}
//----------------------------------------------------------------------------
void albaDynamicLoaderTest::TestLastError()
//----------------------------------------------------------------------------
{
  const char *str = albaDynamicLoader::LastError();
	printf("\nTest Only for Log printing:\n%s",str);
  delete str;
}
