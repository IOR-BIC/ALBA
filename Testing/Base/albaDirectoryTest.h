/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDirectoryTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_albaDirectoryTest_H__
#define __CPP_UNIT_albaDirectoryTest_H__

#include "albaTest.h"
class albaDirectory;

class albaDirectoryTest : public albaTest
{
public:

	CPPUNIT_TEST_SUITE( albaDirectoryTest );
	CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestLoad );
	CPPUNIT_TEST( TestGetFile );
	CPPUNIT_TEST( TestGetNumberOfFiles );
	CPPUNIT_TEST_SUITE_END();

	albaDirectory *m_Directory;

protected:
	void TestFixture();
	void TestStaticAllocation();
	void TestDynamicAllocation();
	void TestLoad();
	void TestGetNumberOfFiles();
	void TestGetFile();
};



#endif
