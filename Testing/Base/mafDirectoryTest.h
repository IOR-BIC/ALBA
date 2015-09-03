/*=========================================================================

 Program: MAF2
 Module: mafDirectoryTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_mafDirectoryTest_H__
#define __CPP_UNIT_mafDirectoryTest_H__

#include "mafTest.h"
class mafDirectory;

class mafDirectoryTest : public mafTest
{
public:

	CPPUNIT_TEST_SUITE( mafDirectoryTest );
	CPPUNIT_TEST( TestFixture ); // just to test that the fixture has no leaks
	CPPUNIT_TEST( TestDynamicAllocation );
	CPPUNIT_TEST( TestStaticAllocation );
	CPPUNIT_TEST( TestLoad );
	CPPUNIT_TEST( TestGetFile );
	CPPUNIT_TEST( TestGetNumberOfFiles );
	CPPUNIT_TEST_SUITE_END();

	mafDirectory *m_Directory;

protected:
	void TestFixture();
	void TestStaticAllocation();
	void TestDynamicAllocation();
	void TestLoad();
	void TestGetNumberOfFiles();
	void TestGetFile();
};



#endif
