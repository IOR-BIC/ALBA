/*=========================================================================

 Program: MAF2
 Module: mafDynamicLoaderTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include "mafDynamicLoaderTest.h"
#include "mafDynamicLoader.h"
#include "mafString.h"

#include <iostream>
#define TEST_RESULT CPPUNIT_ASSERT(result);

#ifdef _WIN64
#define DLL_FILENAME "DLLTest64.dll"
#else
#define DLL_FILENAME "DLLTest.dll"
#endif

//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestOpenLibrary()
//----------------------------------------------------------------------------
{
#ifdef __hpux
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define MAFDYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	mafString dllPath = MAF_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = mafDynamicLoader::OpenLibrary(dllPath);

	result = lHandle != NULL;
	TEST_RESULT;

	mafDynamicLoader::CloseLibrary(lHandle);
#endif

#ifndef MAFDYNAMICLOADER_DEFINED_TEST
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif
  
}
//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestCloseLibrary()
//----------------------------------------------------------------------------
{
#ifdef __hpux
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define MAFDYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	mafString dllPath = MAF_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = mafDynamicLoader::OpenLibrary(dllPath);

	int exit = mafDynamicLoader::CloseLibrary(lHandle);

	/*
	Return Value
	If the function succeeds, the return value is nonzero.
	*/

	result = exit;
	TEST_RESULT;
#endif

#ifndef MAFDYNAMICLOADER_DEFINED_TEST
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif
}
//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestGetSymbolAddress()
//----------------------------------------------------------------------------
{
#ifdef __hpux
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define MAFDYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	mafString dllPath = MAF_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = mafDynamicLoader::OpenLibrary(dllPath);

	void *pointer = NULL;
	pointer = (void *)mafDynamicLoader::GetSymbolAddress(lHandle, "AddNums");

	result = pointer != NULL;
	printf("\nADDRESS : %d\n", pointer);
	TEST_RESULT;

	mafDynamicLoader::CloseLibrary(lHandle);
#endif

#ifndef MAFDYNAMICLOADER_DEFINED_TEST
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif
}

//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestCallFunction()
{
#ifdef __hpux
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef __APPLE__
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif

#ifdef _WIN32
#define MAFDYNAMICLOADER_DEFINED_TEST 1
	LibHandle lHandle = NULL;
	mafString dllPath = MAF_DATA_ROOT;
	dllPath << "/Test_DynamicLoader/" << DLL_FILENAME;
	lHandle = mafDynamicLoader::OpenLibrary(dllPath);

	typedef int(*SUMFUNC)(int, int);
	SUMFUNC pointer = NULL;
	pointer = (SUMFUNC) mafDynamicLoader::GetSymbolAddress(lHandle, "AddNums");

	CPPUNIT_ASSERT(pointer != NULL);
	
	int res;
	res = pointer(5, 3);

	CPPUNIT_ASSERT(res == 8);
	
	mafDynamicLoader::CloseLibrary(lHandle);
#endif

#ifndef MAFDYNAMICLOADER_DEFINED_TEST
#define MAFDYNAMICLOADER_DEFINED_TEST 1

#endif
}

//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestLibPrefix()
//----------------------------------------------------------------------------
{
  
	#ifdef __hpux
	#define MAFDYNAMICLOADER_DEFINED_TEST 1
	result = mafString(mafDynamicLoader::LibPrefix()).Equals("lib");
	TEST_RESULT;
	#endif

	#ifdef __APPLE__
	#define MAFDYNAMICLOADER_DEFINED_TEST 1
	result = mafString(mafDynamicLoader::LibPrefix()).Equals("");
	TEST_RESULT;
	#endif

	#ifdef _WIN32
	#define MAFDYNAMICLOADER_DEFINED_TEST 1
	result = mafString(mafDynamicLoader::LibPrefix()).Equals("");
	TEST_RESULT;
	#endif

	#ifndef MAFDYNAMICLOADER_DEFINED_TEST
	#define MAFDYNAMICLOADER_DEFINED_TEST 1
	result = mafString(mafDynamicLoader::LibPrefix()).Equals("lib");
	TEST_RESULT;
	#endif
}
//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestLibExtension()
//----------------------------------------------------------------------------
{
	#ifdef __hpux
  #define MAFDYNAMICLOADER_DEFINED_TEST 1
	result = mafString(mafDynamicLoader::LibExtension()).Equals(".sl");
	TEST_RESULT;
  #endif

	#ifdef __APPLE__
	#define MAFDYNAMICLOADER_DEFINED_TEST 1
	result = mafString(mafDynamicLoader::LibExtension()).Equals(".dylib");
	TEST_RESULT;
	#endif
	
  #ifdef _WIN32
	#define MAFDYNAMICLOADER_DEFINED_TEST 1
	result = mafString(mafDynamicLoader::LibExtension()).Equals(".dll");
	TEST_RESULT;
  #endif

  #ifndef MAFDYNAMICLOADER_DEFINED_TEST
	result = mafString(mafDynamicLoader::LibExtension()).Equals(".so");
	TEST_RESULT;
	#endif
}
//----------------------------------------------------------------------------
void mafDynamicLoaderTest::TestLastError()
//----------------------------------------------------------------------------
{
  const char *str = mafDynamicLoader::LastError();
	printf("\nTest Only for Log printing:\n%s",str);
  delete str;
}
