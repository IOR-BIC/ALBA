/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafCurlUtilityTest.cpp,v $
Language:  C++
Date:      $Date: 2010-12-03 12:25:13 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include "mafCurlUtilityTest.h"
#include "mafCurlUtility.h"

//----------------------------------------------------------------------------
void mafCurlUtilityTest::setUp()
//----------------------------------------------------------------------------
{
  
}
//----------------------------------------------------------------------------
void mafCurlUtilityTest::tearDown()
//----------------------------------------------------------------------------
{
 
}
//----------------------------------------------------------------------------
void mafCurlUtilityTest::TestWriteMemoryCallback()
//----------------------------------------------------------------------------
{
  //first branch of safe realloc if(ptr)
	 char *array = "1234567890";
   void *pointer = (void *)array;
   int number = 1;

   msfTreeSearchReult *d = new msfTreeSearchReult();
   d->memory = NULL;
   d->size = 10;

   int resultRealSize = WriteMemoryCallback(pointer, sizeof(char)*10, number, d);
   CPPUNIT_ASSERT(resultRealSize = 10);

   delete d;

   //other branch of SafeRealloc
   char *array2 = new char[10];
   int i=0 , size = 10;
   array2[0] = '0';array2[1] = '1';array2[2] = '2';array2[3] = '3';array2[4] = '4';
   array2[5] = '5';array2[6] = '6';array2[7] = '7';array2[8] = '8';array2[9] = '9';
   
   void *pointer2 = (void *)array2;
   int number2 = 1;

   msfTreeSearchReult *d2 = new msfTreeSearchReult();
   d2->memory = array2;
   d2->size = 10;

   int resultRealSize2 = WriteMemoryCallback(pointer2, sizeof(char)*10, number2, d2);
   CPPUNIT_ASSERT(resultRealSize2 = 10);

   delete d2;
}